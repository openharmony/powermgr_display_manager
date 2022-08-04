/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "screen_controller.h"

#include "delayed_sp_singleton.h"
#include "refbase.h"
#include "display_power_mgr_service.h"
#include "display_param_helper.h"
#include "power_setting_helper.h"
#include "system_ability_definition.h"
#include "display_common.h"
#include "display_log.h"

using namespace std;
using namespace OHOS::PowerMgr;

namespace OHOS {
namespace DisplayPowerMgr {
namespace {
sptr<PowerSettingObserver> g_brightnessObserver;
}

ScreenController::ScreenController(uint32_t displayId, const shared_ptr<DisplayEventHandler>& handler)
    : handler_(handler)
{
    DISPLAY_HILOGI(COMP_SVC, "ScreenController created for displayId=%{public}u", displayId);
    action_ = make_shared<ScreenAction>(displayId);
    state_ = action_->GetDisplayState();

    string name = "BrightnessController_" + to_string(displayId);
    shared_ptr<AnimateCallback> animateCallback = make_shared<AnimateCallbackImpl>(action_);
    animator_ = make_shared<GradualAnimator>(name, animateCallback);

    DisplayEventHandler::EventCallback cancelBoostCallback = bind([&]() { CancelBoostBrightness(); });
    handler_->EmplaceCallBack(DisplayEventHandler::Event::EVENT_CANCEL_BOOST_BRIGHTNESS, cancelBoostCallback);
    DisplayEventHandler::EventCallback setSettingBrightnessCallback = bind([&]() { SetSettingBrightness(); });
    handler->EmplaceCallBack(DisplayEventHandler::Event::EVENT_SET_SETTING_BRIGHTNESS, setSettingBrightnessCallback);
}

ScreenController::AnimateCallbackImpl::AnimateCallbackImpl(const shared_ptr <ScreenAction>& action)
    : action_(action)
{}

void ScreenController::AnimateCallbackImpl::OnStart()
{
    DISPLAY_HILOGD(FEAT_BRIGHTNESS, "ScreenAnimatorCallback onStart");
}

void ScreenController::AnimateCallbackImpl::OnChanged(uint32_t currentValue)
{
    bool ret = action_->SetBrightness(currentValue);
    if (ret) {
        DISPLAY_HILOGD(FEAT_BRIGHTNESS, "Update brightness, brightness=%{public}u", currentValue);
    } else {
        DISPLAY_HILOGD(FEAT_BRIGHTNESS, "Update brightness failed, brightness=%{public}d", currentValue);
    }
}

void ScreenController::AnimateCallbackImpl::OnEnd()
{
    DISPLAY_HILOGD(FEAT_BRIGHTNESS, "ScreenAnimatorCallback OnEnd");
}

DisplayState ScreenController::GetState()
{
    return state_;
}

bool ScreenController::UpdateState(DisplayState state, uint32_t reason)
{
    DISPLAY_HILOGI(FEAT_STATE, "UpdateState, state=%{public}u, current state=%{public}u",
                   static_cast<uint32_t>(state), static_cast<uint32_t>(state_));
    RETURN_IF_WITH_RET(state == state_, true);

    switch (state) {
        case DisplayState::DISPLAY_ON:
        case DisplayState::DISPLAY_OFF: {
            function<void(DisplayState)> callback = bind(&ScreenController::OnStateChanged, this, placeholders::_1);
            bool ret = action_->SetDisplayState(state, callback);
            if (!ret) {
                DISPLAY_HILOGW(FEAT_STATE, "SetDisplayState failed state=%{public}d", state);
                return ret;
            }
            break;
        }
        case DisplayState::DISPLAY_DIM:
        case DisplayState::DISPLAY_SUSPEND: {
            bool ret = action_->SetDisplayPower(state, stateChangeReason_);
            if (!ret) {
                DISPLAY_HILOGW(FEAT_STATE, "SetDisplayPower failed state=%{public}d", state);
                return ret;
            }
            break;
        }
        default:
            break;
    }

    lock_guard lock(mutexState_);
    state_ = state;
    stateChangeReason_ = reason;

    DISPLAY_HILOGI(FEAT_STATE, "Update screen state to %{public}u", state);
    return true;
}

bool ScreenController::IsScreenOn()
{
    lock_guard lock(mutexState_);
    return (state_ == DisplayState::DISPLAY_ON || state_ == DisplayState::DISPLAY_DIM);
}

bool ScreenController::SetBrightness(uint32_t value, uint32_t gradualDuration)
{
    if (!CanSetBrightness()) {
        DISPLAY_HILOGW(FEAT_BRIGHTNESS, "Cannot set brightness, ignore the change");
        return false;
    }
    DISPLAY_HILOGI(FEAT_BRIGHTNESS, "Set brightness, value=%{public}u", value);
    cachedSettingBrightness_ = GetBrightness();
    bool ret = UpdateBrightness(value, gradualDuration);
    if (ret) {
        handler_->SendEvent(DisplayEventHandler::Event::EVENT_SET_SETTING_BRIGHTNESS);
    }
    return ret;
}

uint32_t ScreenController::GetBrightness()
{
    return static_cast<uint32_t>(GetDeviceBrightness() / discount_);
}

bool ScreenController::DiscountBrightness(double discount, uint32_t gradualDuration)
{
    if (!CanDiscountBrightness()) {
        DISPLAY_HILOGW(FEAT_BRIGHTNESS, "Cannot discount brightness, ignore the change");
        return false;
    }
    uint32_t origianlBrightness = GetBrightness();
    DISPLAY_HILOGI(FEAT_BRIGHTNESS, "Discount brightness, discount=%{public}lf", discount);
    discount_ = discount;
    return UpdateBrightness(origianlBrightness, gradualDuration);
}

bool ScreenController::OverrideBrightness(uint32_t value, uint32_t gradualDuration)
{
    if (!CanOverrideBrightness()) {
        DISPLAY_HILOGW(FEAT_BRIGHTNESS, "Cannot override brightness, ignore the change");
        return false;
    }
    DISPLAY_HILOGI(FEAT_BRIGHTNESS, "Override brightness, value=%{public}u", value);
    if (!isBrightnessOverridden_) {
        isBrightnessOverridden_ = true;
        cachedSettingBrightness_ = GetBrightness();
    }
    overriddenBrightness_ = value;
    return UpdateBrightness(value, gradualDuration);
}

bool ScreenController::RestoreBrightness(uint32_t gradualDuration)
{
    if (!IsBrightnessOverridden()) {
        DISPLAY_HILOGD(FEAT_BRIGHTNESS, "Brightness is not override, no need to restore");
        return false;
    }
    isBrightnessOverridden_ = false;
    uint32_t settingBrightness = GetSettingBrightness();
    return UpdateBrightness(settingBrightness, gradualDuration);
}

bool ScreenController::IsBrightnessOverridden() const
{
    return isBrightnessOverridden_;
}

bool ScreenController::BoostBrightness(uint32_t timeoutMs, uint32_t gradualDuration)
{
    if (!CanBoostBrightness()) {
        DISPLAY_HILOGW(FEAT_BRIGHTNESS, "Cannot boost brightness, ignore the change");
        return false;
    }
    bool ret = true;
    if (!isBrightnessBoosted_) {
        uint32_t maxBrightness = DisplayParamHelper::GetInstance().GetMaxBrightness();
        DISPLAY_HILOGI(FEAT_BRIGHTNESS, "Boost brightness, maxBrightness: %{public}d", maxBrightness);
        isBrightnessBoosted_ = true;
        cachedSettingBrightness_ = GetBrightness();
        ret = UpdateBrightness(maxBrightness, gradualDuration);
    }

    // If boost multi-times, we will resend the cancel boost event.
    handler_->RemoveEvent(DisplayEventHandler::Event::EVENT_CANCEL_BOOST_BRIGHTNESS);
    handler_->SendEvent(DisplayEventHandler::Event::EVENT_CANCEL_BOOST_BRIGHTNESS, timeoutMs);
    DISPLAY_HILOGD(FEAT_BRIGHTNESS, "BoostBrightness update timeout=%{public}u, ret=%{public}d", timeoutMs, ret);
    return ret;
}

bool ScreenController::CancelBoostBrightness(uint32_t gradualDuration)
{
    DISPLAY_HILOGD(FEAT_BRIGHTNESS, "Cancel boost brightness");
    if (!IsBrightnessBoosted()) {
        DISPLAY_HILOGD(FEAT_BRIGHTNESS, "Brightness is not boost, no need to restore");
        return false;
    }
    handler_->RemoveEvent(DisplayEventHandler::Event::EVENT_CANCEL_BOOST_BRIGHTNESS);
    isBrightnessBoosted_ = false;
    uint32_t settingBrightness = GetSettingBrightness();
    return UpdateBrightness(settingBrightness, gradualDuration);
}

bool ScreenController::IsBrightnessBoosted() const
{
    return isBrightnessBoosted_;
}

void ScreenController::OnStateChanged(DisplayState state)
{
    auto pms = DelayedSpSingleton<DisplayPowerMgrService>::GetInstance();
    if (pms == nullptr) {
        DISPLAY_HILOGW(FEAT_STATE, "pms is nullptr");
        return;
    }

    bool ret = action_->SetDisplayPower(state, stateChangeReason_);
    if (state == DisplayState::DISPLAY_ON) {
        // Restore the brightness before screen off
        uint32_t screenOnBrightness = GetScreenOnBrightness();
        UpdateBrightness(screenOnBrightness);
    }

    if (ret) {
        pms->NotifyStateChangeCallback(action_->GetDisplayId(), state);
    }
}

bool ScreenController::CanSetBrightness()
{
    return IsScreenOn() && !IsBrightnessOverridden() && !IsBrightnessBoosted();
}

bool ScreenController::CanDiscountBrightness()
{
    return IsScreenOn();
}

bool ScreenController::CanOverrideBrightness()
{
    return IsScreenOn() && !IsBrightnessBoosted();
}

bool ScreenController::CanBoostBrightness()
{
    return IsScreenOn() && !IsBrightnessOverridden();
}

bool ScreenController::UpdateBrightness(uint32_t value, uint32_t gradualDuration)
{
    DISPLAY_HILOGI(FEAT_BRIGHTNESS, "Update brightness, value=%{public}u, discount=%{public}lf,"\
                   " duration=%{public}u", value, discount_, gradualDuration);
    auto brightness = DisplayPowerMgrService::GetSafeBrightness(static_cast<uint32_t>(value * discount_));

    if (animator_->IsAnimating()) {
        animator_->StopAnimation();
    }
    if (gradualDuration > 0) {
        DISPLAY_HILOGI(FEAT_BRIGHTNESS, "Update brightness gradually");
        animator_->StartAnimation(GetDeviceBrightness(), brightness, gradualDuration);
        return true;
    }
    bool isSucc = action_->SetBrightness(brightness);
    DISPLAY_HILOGD(FEAT_BRIGHTNESS, "Updated brightness is %{public}s, brightness: %{public}u",
                   isSucc ? "succ" : "failed", brightness);
    return isSucc;
}

uint32_t ScreenController::GetSettingBrightness(const std::string& key) const
{
    PowerSettingHelper& helper = PowerSettingHelper::GetInstance(DISPLAY_MANAGER_SERVICE_ID);
    int32_t value;
    ErrCode ret = helper.GetIntValue(key, value);
    if (ret != ERR_OK) {
        DISPLAY_HILOGW(FEAT_BRIGHTNESS, "get setting brightness failed, return cachedBrightness_=%{public}u,"\
                       " key=%{public}s, ret=%{public}d", cachedSettingBrightness_, key.c_str(), ret);
        return cachedSettingBrightness_;
    }
    return static_cast<uint32_t>(value);
}

void ScreenController::SetSettingBrightness()
{
    uint32_t brightness = GetBrightness();
    uint32_t settingBrightness = GetSettingBrightness();
    if (brightness == static_cast<uint32_t>(settingBrightness)) {
        DISPLAY_HILOGD(FEAT_BRIGHTNESS, "no need to set setting brightness");
        return;
    }
    PowerSettingHelper& helper = PowerSettingHelper::GetInstance(DISPLAY_MANAGER_SERVICE_ID);
    ErrCode ret = helper.PutIntValue(SETTING_BRIGHTNESS_KEY, static_cast<int32_t>(brightness));
    if (ret != ERR_OK) {
        DISPLAY_HILOGW(FEAT_BRIGHTNESS, "set setting brightness failed, ret=%{public}d", ret);
    }
}

uint32_t ScreenController::GetScreenOnBrightness() const
{
    if (IsBrightnessBoosted()) {
        DISPLAY_HILOGD(FEAT_BRIGHTNESS, "Brightness is boosted, return max brightness");
        return DisplayParamHelper::GetInstance().GetMaxBrightness();
    } else if (IsBrightnessOverridden()) {
        DISPLAY_HILOGD(FEAT_BRIGHTNESS, "Brightness is overridden, return overridden brightness=%{public}u",
                       overriddenBrightness_);
        return overriddenBrightness_;
    } else {
        return GetSettingBrightness();
    }
}

void ScreenController::RegisterSettingBrightnessObserver()
{
    if (g_brightnessObserver) {
        DISPLAY_HILOGD(FEAT_BRIGHTNESS, "setting brightness observer is already registered");
        return;
    }
    PowerSettingHelper& helper = PowerSettingHelper::GetInstance(DISPLAY_MANAGER_SERVICE_ID);
    PowerSettingObserver::UpdateFunc updateFunc = bind(&ScreenController::BrightnessSettingUpdateFunc, this,
                                                       placeholders::_1);
    g_brightnessObserver = helper.CreateObserver(SETTING_BRIGHTNESS_KEY, updateFunc);
    ErrCode ret = helper.RegisterObserver(g_brightnessObserver);
    if (ret != ERR_OK) {
        DISPLAY_HILOGW(FEAT_BRIGHTNESS, "register setting brightness observer failed, ret=%{public}d", ret);
        g_brightnessObserver = nullptr;
    }
}

void ScreenController::BrightnessSettingUpdateFunc(const string& key)
{
    uint32_t brightness = GetBrightness();
    uint32_t settingBrightness = GetSettingBrightness(key);
    if (brightness == static_cast<uint32_t>(settingBrightness)) {
        DISPLAY_HILOGD(FEAT_BRIGHTNESS, "no need to set setting brightness");
        return;
    }
    DISPLAY_HILOGD(FEAT_BRIGHTNESS, "setting brightness updated, brightness %{public}u -> %{public}u",
                   brightness, settingBrightness);
    UpdateBrightness(settingBrightness);
}

void ScreenController::UnregisterSettingBrightnessObserver()
{
    if (g_brightnessObserver == nullptr) {
        DISPLAY_HILOGD(FEAT_BRIGHTNESS, "g_brightnessObserver is nullptr, no need to unregister");
        return;
    }
    PowerSettingHelper& helper = PowerSettingHelper::GetInstance(DISPLAY_MANAGER_SERVICE_ID);
    ErrCode ret = helper.UnregisterObserver(g_brightnessObserver);
    if (ret != ERR_OK) {
        DISPLAY_HILOGW(FEAT_BRIGHTNESS, "unregister setting brightness observer failed, ret=%{public}d", ret);
    }
    g_brightnessObserver = nullptr;
}

double ScreenController::GetDiscount() const
{
    return discount_;
}

uint32_t ScreenController::GetDeviceBrightness()
{
    return action_->GetBrightness();
}
} // namespace DisplayPowerMgr
} // namespace OHOS

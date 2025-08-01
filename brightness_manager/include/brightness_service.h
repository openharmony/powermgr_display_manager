/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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

#ifndef BRIGHTNESS_CONTROLLER_H
#define BRIGHTNESS_CONTROLLER_H

#include <atomic>
#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "brightness_action.h"
#include "brightness_dimming.h"
#include "brightness_base.h"
#include "brightness_param_helper.h"
#include "calculation_manager.h"
#include "display_common.h"
#include "display_power_info.h"
#include "display_manager_lite.h"
#include "dm_common.h"
#include "iremote_object.h"
#include "idisplay_brightness_callback.h"
#include "light_lux_manager.h"
#include "refbase.h"
#include "brightness_ffrt.h"
#ifdef ENABLE_SENSOR_PART
#include "sensor_agent_type.h"
#endif

#include <sys/types.h>

namespace OHOS {
namespace DisplayPowerMgr {
class BrightnessService {
public:
    class DimmingCallbackImpl : public BrightnessDimmingCallback {
    public:
        DimmingCallbackImpl(const std::shared_ptr<BrightnessAction>& action,
            std::function<void(uint32_t)> callback);
        ~DimmingCallbackImpl() override = default;
        DimmingCallbackImpl(const DimmingCallbackImpl&) = delete;
        DimmingCallbackImpl& operator=(const DimmingCallbackImpl&) = delete;
        DimmingCallbackImpl(DimmingCallbackImpl&&) = delete;
        DimmingCallbackImpl& operator=(DimmingCallbackImpl&&) = delete;

        void OnStart() override;
        void OnChanged(uint32_t currentValue) override;
        void OnEnd() override;
        void DiscountBrightness(double discount) override;

    private:
        const std::shared_ptr<BrightnessAction> mAction{};
        std::function<void(uint32_t)> mCallback{};
        double mDiscount{1.0};
    };

    class FoldStatusLisener : public Rosen::DisplayManagerLite::IFoldStatusListener {
    public:
        FoldStatusLisener() = default;
        virtual ~FoldStatusLisener() = default;

        FoldStatusLisener(const FoldStatusLisener&) = delete;
        FoldStatusLisener& operator=(const FoldStatusLisener&) = delete;
        FoldStatusLisener(FoldStatusLisener&&) = delete;
        FoldStatusLisener& operator=(FoldStatusLisener&&) = delete;

        /**
        * @param FoldStatus uint32_t; UNKNOWN = 0, EXPAND = 1,  FOLDED = 2,  HALF_FOLD = 3;
        */
        void OnFoldStatusChanged(Rosen::FoldStatus foldStatus) override;

    private:
        Rosen::FoldStatus mLastFoldStatus = Rosen::FoldStatus::UNKNOWN;
    };

    static constexpr const char* SETTING_AUTO_ADJUST_BRIGHTNESS_KEY {"settings.display.auto_screen_brightness"};
    static const int LUX_LEVEL_LENGTH = 23;

    BrightnessService(const BrightnessService&) = delete;
    BrightnessService& operator=(const BrightnessService&) = delete;
    BrightnessService(BrightnessService&&) = delete;
    BrightnessService& operator=(BrightnessService&&) = delete;

    static BrightnessService& Get();
    static uint32_t GetMappingBrightnessLevel(uint32_t level);
    static uint32_t GetOrigBrightnessLevel(uint32_t level);
    static void SetSettingAutoBrightness(bool enable);
    static bool GetSettingAutoBrightness(const std::string& key = SETTING_AUTO_ADJUST_BRIGHTNESS_KEY);

    void Init(uint32_t defaultMax, uint32_t defaultMin);
    void DeInit();
    void SetDisplayState(uint32_t id, DisplayState state);
    DisplayState GetDisplayState();
    bool IsScreenOnState(DisplayState state);
    bool IsSupportLightSensor(void);
    bool IsAutoAdjustBrightness(void);
    bool AutoAdjustBrightness(bool enable);
    bool StateChangedSetAutoBrightness(bool enable);
    void ProcessLightLux(float lux);
    void RegisterSettingBrightnessObserver();
    void UnregisterSettingBrightnessObserver();
    uint32_t GetBrightness();
    uint32_t GetDeviceBrightness();
    uint32_t GetCachedSettingBrightness();
    uint32_t GetScreenOnBrightness(bool isUpdateTarget);
    uint32_t GetBrightnessLevel(float lux);
    uint32_t GetBrightnessHighLevel(uint32_t level);
    uint32_t GetMappingBrightnessNit(uint32_t level);
    uint32_t GetBrightnessLevelFromNit(uint32_t nit);
    uint32_t GetMappingHighBrightnessLevel(uint32_t level);
    bool SetBrightness(uint32_t value, uint32_t gradualDuration = 0, bool continuous = false);
    void SetScreenOnBrightness();
    bool OverrideBrightness(uint32_t value, uint32_t gradualDuration = 0);
    bool RestoreBrightness(uint32_t gradualDuration = 0);
    bool IsBrightnessOverridden();
    bool BoostBrightness(uint32_t timeoutMs, uint32_t gradualDuration = 0);
    bool CancelBoostBrightness(uint32_t gradualDuration = 0);
    bool IsBrightnessBoosted();
    bool DiscountBrightness(double discount, uint32_t gradualDuration = 0);
    double GetDiscount() const;
    uint32_t GetDimmingUpdateTime() const;
    void ClearOffset();
    void UpdateBrightnessSceneMode(BrightnessSceneMode mode);
    uint32_t GetDisplayId();
    void SetDisplayId(uint32_t displayId);
    uint32_t SetLightBrightnessThreshold(std::vector<int32_t> threshold, sptr<IDisplayBrightnessCallback> callback);
    uint32_t GetCurrentDisplayId(uint32_t defaultId);
    bool IsDimming();
    void ReportBrightnessBigData(uint32_t brightness);
    bool IsSleepStatus();
    void SetSleepBrightness();
    int GetDisplayIdWithFoldstatus(Rosen::FoldStatus foldStatus);
    int GetSensorIdWithFoldstatus(Rosen::FoldStatus foldStatus);
    int GetDisplayIdWithDisplayMode(Rosen::FoldDisplayMode mode);
    int GetSensorIdWithDisplayMode(Rosen::FoldDisplayMode mode);
    uint32_t GetCurrentSensorId();
    void SetCurrentSensorId(uint32_t sensorId);
    int NotifyScreenPowerStatus(uint32_t displayId, uint32_t status);

    static uint32_t GetSafeBrightness(uint32_t value);
    bool SetMaxBrightness(double value);
    bool SetMaxBrightnessNit(uint32_t maxNit);
private:
    static const constexpr char* SETTING_BRIGHTNESS_KEY{"settings.display.screen_brightness_status"};
    static const uint32_t SAMPLING_RATE = 100000000;
    static constexpr uint32_t DEFAULT_DISPLAY_ID = 0;
    static constexpr uint32_t SECOND_DISPLAY_ID = 1;
    static constexpr uint32_t DEFAULT_BRIGHTNESS = 50;
    static constexpr const double DISCOUNT_MIN = 0.01;
    static constexpr const double DISCOUNT_MAX = 1.00;
    static const uint32_t AMBIENT_LUX_LEVELS[LUX_LEVEL_LENGTH];
    static const uint32_t WAIT_FOR_FIRST_LUX_MAX_TIME = 200;
    static const uint32_t WAIT_FOR_FIRST_LUX_STEP = 10;
    static uint32_t brightnessValueMin;
    static uint32_t brightnessValueMax;

    BrightnessService();
    virtual ~BrightnessService() = default;

    static uint32_t GetSettingBrightness(const std::string& key = SETTING_BRIGHTNESS_KEY);
    bool mIsLuxActiveWithLog{true};
#ifdef ENABLE_SENSOR_PART
    static void AmbientLightCallback(SensorEvent* event);
    void InitSensors();
    void ActivateAmbientSensor();
    void DeactivateAmbientSensor();
    void ActivateAmbientSensor1();
    void DeactivateAmbientSensor1();
    void ActivateValidAmbientSensor();
    void DeactivateValidAmbientSensor();
    void DeactivateAllAmbientSensor();
    bool mIsSupportLightSensor{false};
    SensorUser mSensorUser{};
    SensorUser mSensorUser1{};
#endif
    bool mIsLightSensorEnabled{false};
    bool mIsLightSensor1Enabled{false};

    void UpdateCurrentBrightnessLevel(float lux, bool isFastDuration);
    void SetBrightnessLevel(uint32_t value, uint32_t duration);
    bool IsScreenOn();
    bool CanSetBrightness();
    bool CanDiscountBrightness();
    bool CanOverrideBrightness();
    bool CanBoostBrightness();
    bool UpdateBrightness(uint32_t value, uint32_t gradualDuration = 0, bool updateSetting = false);
    void SetSettingBrightness(uint32_t value);
    void UpdateBrightnessSettingFunc(const std::string& key);
    void RegisterFoldStatusListener();
    void UnRegisterFoldStatusListener();
    std::string GetReason();
    bool GetIsSupportLightSensor();
    bool IsCurrentSensorEnable();

    bool mIsFoldDevice{false};
    bool mIsAutoBrightnessEnabled{false};
    DisplayState mState{DisplayState::DISPLAY_UNKNOWN};
    uint32_t mBrightnessLevel{0};
    std::atomic<uint32_t> mBrightnessTarget{0};
    uint32_t mDisplayId{0};
    uint32_t mCurrentSensorId{5};
    int mLuxLevel{-1};
    double mDiscount{1.0f};
    std::atomic<bool> mIsBrightnessOverridden{false};
    std::atomic<bool> mIsBrightnessBoosted{false};
    uint32_t mCachedSettingBrightness{DEFAULT_BRIGHTNESS};
    uint32_t mOverriddenBrightness{DEFAULT_BRIGHTNESS};
    uint32_t mBeforeOverriddenBrightness{DEFAULT_BRIGHTNESS};
    std::shared_ptr<BrightnessAction> mAction{nullptr};
    std::shared_ptr<BrightnessDimmingCallback> mDimmingCallback{nullptr};
    std::shared_ptr<BrightnessDimming> mDimming;
    LightLuxManager mLightLuxManager{};
    BrightnessCalculationManager mBrightnessCalculationManager{};
    sptr<Rosen::DisplayManagerLite::IFoldStatusListener> mFoldStatusistener;
    std::shared_ptr<PowerMgr::FFRTQueue> queue_;
    bool mIsUserMode{false};
    std::atomic<bool> mIsSleepStatus{false};
    std::atomic<bool> mIsDisplayOnWhenFirstLuxReport{false};
    std::atomic<bool> mWaitForFirstLux{false};
    std::atomic<uint32_t> mCurrentBrightness{DEFAULT_BRIGHTNESS};
    std::once_flag mInitCallFlag;
    std::mutex mBoostHandleLock{};
    std::mutex mFirstLuxHandleLock{};
};
} // namespace DisplayPowerMgr
} // namespace OHOS
#endif // BRIGHTNESS_CONTROLLER_H
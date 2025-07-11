/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "display_mock_parcel_test.h"
#include "display_log.h"
#include "display_mgr_errors.h"
#include "display_power_mgr_client.h"
#include "ipc_object_stub.h"
#include "power_state_machine_info.h"
#ifdef HAS_HIVIEWDFX_HISYSEVENT_PART
#include <hisysevent.h>
#endif
#include <memory>
#include <unistd.h>

using namespace testing::ext;
#ifdef HAS_HIVIEWDFX_HISYSEVENT_PART
using namespace OHOS::HiviewDFX;
#endif
using namespace OHOS::PowerMgr;
using namespace OHOS::DisplayPowerMgr;
using namespace OHOS;
using namespace std;

namespace {
static constexpr uint32_t BRIGHTNESS_SETTING_VALUE = 50;
static constexpr uint32_t BRIGHTNESS_OVERRIDE_VALUE = 100;
static constexpr uint32_t BRIGHTNESS_DURATION = 0;
static constexpr uint32_t BRIGHTNESS_DEFAULT_VALUE = 102;
static constexpr uint32_t BRIGHTNESS_MIN_VALUE = 5;
static constexpr uint32_t BRIGHTNESS_MAX_VALUE = 255;
static constexpr uint32_t BRIGHTNESS_NONE_VALUE = 0;
static constexpr uint32_t CODE_VALUE = 100;
static constexpr uint32_t DISPLAY_ID = 0;
static constexpr uint32_t MAIN_ID_PROXY = 0;
static constexpr uint32_t REASON = 0;
static constexpr int32_t BRIGHTNESS_ADJUST_VALUE = 150;
static constexpr int32_t MAIN_ID_CLIENT = 0;
static constexpr int32_t TIMEOUT_MS = 500;
static constexpr double DISCOUNT_VALUE = 0.30;
static constexpr uint32_t DEFAULT_DURATION = 500;
static constexpr uint32_t ERR_OK = 0;
} // namespace

void DisplayMockParcelTest::DisplayProxyBrightnessTestFunc(std::shared_ptr<DisplayPowerMgrProxy>& sptrDisplayProxy)
{
    bool ret = false;
    int32_t errCode = 0;
    EXPECT_NE(sptrDisplayProxy->SetBrightness(BRIGHTNESS_SETTING_VALUE, DISPLAY_ID, false, ret, errCode), ERR_OK);
    EXPECT_FALSE(ret);
    EXPECT_NE(sptrDisplayProxy->DiscountBrightness(DISCOUNT_VALUE, DISPLAY_ID, ret), ERR_OK);
    EXPECT_FALSE(ret);
    EXPECT_NE(sptrDisplayProxy->OverrideBrightness(BRIGHTNESS_OVERRIDE_VALUE, DISPLAY_ID, DEFAULT_DURATION, ret),
        ERR_OK);
    EXPECT_FALSE(ret);
    EXPECT_NE(sptrDisplayProxy->RestoreBrightness(DISPLAY_ID, DEFAULT_DURATION, ret), ERR_OK);
    EXPECT_FALSE(ret);
    uint32_t brightness = 0;
    EXPECT_NE(sptrDisplayProxy->GetBrightness(DISPLAY_ID, brightness), ERR_OK);
    EXPECT_EQ(BRIGHTNESS_NONE_VALUE, brightness);
    EXPECT_NE(sptrDisplayProxy->GetDefaultBrightness(brightness), ERR_OK);
    EXPECT_EQ(BRIGHTNESS_NONE_VALUE, brightness);
    EXPECT_NE(sptrDisplayProxy->GetMaxBrightness(brightness), ERR_OK);
    EXPECT_EQ(BRIGHTNESS_NONE_VALUE, brightness);
    EXPECT_NE(sptrDisplayProxy->GetMinBrightness(brightness), ERR_OK);
    EXPECT_EQ(BRIGHTNESS_NONE_VALUE, brightness);
    EXPECT_NE(sptrDisplayProxy->AdjustBrightness(DISPLAY_ID, BRIGHTNESS_ADJUST_VALUE, BRIGHTNESS_DURATION, ret),
        ERR_OK);
    EXPECT_FALSE(ret);
    EXPECT_NE(sptrDisplayProxy->AutoAdjustBrightness(true, ret), ERR_OK);
    EXPECT_FALSE(ret);
    EXPECT_NE(sptrDisplayProxy->AutoAdjustBrightness(false, ret), ERR_OK);
    EXPECT_FALSE(ret);
    EXPECT_NE(sptrDisplayProxy->IsAutoAdjustBrightness(ret), ERR_OK);
    EXPECT_FALSE(ret);
    EXPECT_NE(sptrDisplayProxy->BoostBrightness(TIMEOUT_MS, DISPLAY_ID, ret), ERR_OK);
    EXPECT_FALSE(ret);
    EXPECT_NE(sptrDisplayProxy->CancelBoostBrightness(DISPLAY_ID, ret), ERR_OK);
    EXPECT_FALSE(ret);
}

void DisplayMockParcelTest::DisplayProxyTestFunc(std::shared_ptr<DisplayPowerMgrProxy>& sptrDisplayProxy)
{
    std::vector<uint32_t> result;
    result.push_back(DISPLAY_ID);
    bool ret = false;
    EXPECT_NE(sptrDisplayProxy->SetDisplayState(DISPLAY_ID,
        static_cast<uint32_t>(DisplayPowerMgr::DisplayState::DISPLAY_ON), REASON, ret), ERR_OK);
    EXPECT_FALSE(ret);
    int32_t state = 0;
    EXPECT_NE(sptrDisplayProxy->GetDisplayState(DISPLAY_ID, state), ERR_OK);
    EXPECT_NE(DisplayPowerMgr::DisplayState::DISPLAY_ON, DisplayPowerMgr::DisplayState(state));
    std::vector<uint32_t> ids;
    EXPECT_NE(sptrDisplayProxy->GetDisplayIds(ids), ERR_OK);
    EXPECT_NE(result, ids);
    uint32_t mainId = MAIN_ID_PROXY;
    EXPECT_NE(sptrDisplayProxy->GetMainDisplayId(mainId), ERR_OK);
    EXPECT_EQ(MAIN_ID_PROXY, mainId);
    DisplayProxyBrightnessTestFunc(sptrDisplayProxy);
}

void DisplayMockParcelTest::DisplayPowerMgrTestCallback::OnDisplayStateChanged(
    uint32_t displayId, DisplayPowerMgr::DisplayState state, uint32_t reason)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrTestCallback::OnDisplayStateChangedStub");
}

namespace {
/**
 * @tc.name: DisplayMockParcelTest_001
 * @tc.desc: test DisplayPowerMgrClient function with mock parcel
 * @tc.type: FUNC
 * @tc.require: issueI5YZQR
 */
HWTEST_F(DisplayMockParcelTest, DisplayMockParcelTest_001, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayMockParcelTest_001 function start!");
    auto& client = DisplayPowerMgr::DisplayPowerMgrClient::GetInstance();
    std::vector<uint32_t> result;
    result.push_back(DISPLAY_ID);
    EXPECT_FALSE(client.SetDisplayState(DisplayPowerMgr::DisplayState::DISPLAY_ON));
    EXPECT_NE(DisplayPowerMgr::DisplayState::DISPLAY_ON, client.GetDisplayState(DISPLAY_ID));
    EXPECT_NE(result, client.GetDisplayIds());
    EXPECT_NE(MAIN_ID_CLIENT, client.GetMainDisplayId());
    EXPECT_FALSE(client.SetBrightness(BRIGHTNESS_SETTING_VALUE, DISPLAY_ID));
    EXPECT_FALSE(client.DiscountBrightness(DISCOUNT_VALUE, DISPLAY_ID));
    EXPECT_FALSE(client.OverrideBrightness(BRIGHTNESS_OVERRIDE_VALUE, DISPLAY_ID));
    EXPECT_FALSE(client.RestoreBrightness(DISPLAY_ID));
    EXPECT_NE(BRIGHTNESS_SETTING_VALUE, client.GetBrightness(DISPLAY_ID));
    EXPECT_EQ(BRIGHTNESS_DEFAULT_VALUE, client.GetDefaultBrightness());
    EXPECT_EQ(BRIGHTNESS_MAX_VALUE, client.GetMaxBrightness());
    EXPECT_NE(BRIGHTNESS_MIN_VALUE, client.GetMinBrightness());
    EXPECT_FALSE(client.AutoAdjustBrightness(true));
    EXPECT_FALSE(client.AdjustBrightness(BRIGHTNESS_ADJUST_VALUE, BRIGHTNESS_DURATION, DISPLAY_ID));
    EXPECT_FALSE(client.AutoAdjustBrightness(false));
    EXPECT_FALSE(client.IsAutoAdjustBrightness());
    EXPECT_FALSE(client.BoostBrightness(TIMEOUT_MS, DISPLAY_ID));
    EXPECT_FALSE(client.CancelBoostBrightness(DISPLAY_ID));
    EXPECT_EQ(BRIGHTNESS_NONE_VALUE, client.GetDeviceBrightness(DISPLAY_ID));
    DISPLAY_HILOGI(LABEL_TEST, "DisplayMockParcelTest_001 function end!");
}

/**
 * @tc.name: DisplayMockParcelTest_002
 * @tc.desc: test DisplayPowerMgrProxy function(IRemoteObject is null, IDisplayPowerCallback is null)
 * @tc.type: FUNC
 * @tc.require: issueI5YZQR
 */
HWTEST_F(DisplayMockParcelTest, DisplayMockParcelTest_002, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayMockParcelTest_002 function start!");
    auto sptrDisplayProxy = std::make_shared<DisplayPowerMgr::DisplayPowerMgrProxy>(nullptr);
    sptr<IDisplayPowerCallback> callbackPtr = nullptr;

    DisplayMockParcelTest::DisplayProxyTestFunc(sptrDisplayProxy);
    uint32_t brightness = BRIGHTNESS_NONE_VALUE;
    EXPECT_NE(sptrDisplayProxy->GetDeviceBrightness(DISPLAY_ID, brightness), ERR_OK);
    EXPECT_EQ(BRIGHTNESS_NONE_VALUE, brightness);

    bool result = false;
    EXPECT_NE(sptrDisplayProxy->RegisterCallback(callbackPtr, result), ERR_OK);
    EXPECT_FALSE(result);
    DISPLAY_HILOGI(LABEL_TEST, "DisplayMockParcelTest_002 function is end");
}

/**
 * @tc.name: DisplayMockParcelTest_003
 * @tc.desc: test DisplayPowerMgrProxy function(IRemoteObject is null, IDisplayPowerCallback is not null)
 * @tc.type: FUNC
 * @tc.require: issueI5YZQR
 */
HWTEST_F(DisplayMockParcelTest, DisplayMockParcelTest_003, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayMockParcelTest_003 function start!");
    auto sptrDisplayProxy = std::make_shared<DisplayPowerMgr::DisplayPowerMgrProxy>(nullptr);
    sptr<IDisplayPowerCallback> callbackPtr = new DisplayMockParcelTest::DisplayPowerMgrTestCallback();
    bool result = false;
    EXPECT_NE(sptrDisplayProxy->RegisterCallback(callbackPtr, result), ERR_OK);
    EXPECT_FALSE(result);
    DISPLAY_HILOGI(LABEL_TEST, "DisplayMockParcelTest_003 function end");
}

/**
 * @tc.name: DisplayMockParcelTest_004
 * @tc.desc: test DisplayPowerMgrProxy function(IRemoteObject and IDisplayPowerCallback is not null)
 * @tc.type: FUNC
 * @tc.require: issueI5YZQR
 */
HWTEST_F(DisplayMockParcelTest, DisplayMockParcelTest_004, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayMockParcelTest_004 function start!");
    sptr<IPCObjectStub> sptrRemoteObj = new IPCObjectStub();
    auto sptrDisplayProxy = std::make_shared<DisplayPowerMgr::DisplayPowerMgrProxy>(sptrRemoteObj);

    sptr<IDisplayPowerCallback> callbackPtr = new DisplayMockParcelTest::DisplayPowerMgrTestCallback();

    DisplayMockParcelTest::DisplayProxyTestFunc(sptrDisplayProxy);
    uint32_t brightness = 0;
    EXPECT_NE(sptrDisplayProxy->GetDeviceBrightness(DISPLAY_ID, brightness), ERR_OK);
    EXPECT_EQ(BRIGHTNESS_NONE_VALUE, brightness);

    bool result = false;
    EXPECT_NE(sptrDisplayProxy->RegisterCallback(callbackPtr, result), ERR_OK);
    EXPECT_FALSE(result);
    DISPLAY_HILOGI(LABEL_TEST, "DisplayMockParcelTest_004 function end");
}

/**
 * @tc.name: DisplayMockParcelTest_005
 * @tc.desc: test DisplayPowerCallbackStub function
 * @tc.type: FUNC
 * @tc.require: issueI5YZQR
 */
HWTEST_F(DisplayMockParcelTest, DisplayMockParcelTest_005, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayMockParcelTest_005 function start!");
    sptr<DisplayPowerCallbackStub> callbackPtr = new DisplayMockParcelTest::DisplayPowerMgrTestCallback();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    EXPECT_NE(ERR_OK, callbackPtr->OnRemoteRequest(CODE_VALUE, data, reply, option));
    DISPLAY_HILOGI(LABEL_TEST, "DisplayMockParcelTest_005 function end!");
}
} // namespace

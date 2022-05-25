/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#include "display_power_mgr_client.h"
#include "display_log.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::DisplayPowerMgr;

class DisplayPowerMgrBrightnessTest : public Test {
public:
    void SetUp()
    {
        DisplayPowerMgrClient::GetInstance().SetDisplayState(DisplayState::DISPLAY_ON);
    }

    void TearDown()
    {
        DisplayPowerMgrClient::GetInstance().RestoreBrightness();
        sleep(1); // wait for gradual animation
        DisplayPowerMgrClient::GetInstance().CancelBoostBrightness();
        sleep(1); // wait for gradual animation
    }
};

namespace {
/**
 * @tc.name: DisplayPowerMgrBrightness001
 * @tc.desc: Test SetBrightness
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrBrightness001, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrBrightness001: fun is start");
    bool ret = DisplayPowerMgrClient::GetInstance().SetBrightness(100);
    EXPECT_TRUE(ret);
    sleep(1); // wait for gradual animation
    uint32_t value = DisplayPowerMgrClient::GetInstance().GetBrightness();
    EXPECT_EQ(value, 100);
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrBrightness001: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrBrightness002
 * @tc.desc: Test OverrideBrightness
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrBrightness002, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrBrightness002: fun is start");
    bool ret = DisplayPowerMgrClient::GetInstance().OverrideBrightness(255);
    EXPECT_TRUE(ret);
    sleep(1); // wait for gradual animation
    uint32_t value = DisplayPowerMgrClient::GetInstance().GetBrightness();
    EXPECT_EQ(value, 255);
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrBrightness002: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrBrightness003
 * @tc.desc: Test RestoreBrightness after OverrideBrightness
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrBrightness003, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrBrightness003: fun is start");
    bool ret = DisplayPowerMgrClient::GetInstance().SetBrightness(100);
    EXPECT_TRUE(ret);
    sleep(1); // wait for gradual animation
    ret = DisplayPowerMgrClient::GetInstance().OverrideBrightness(255);
    EXPECT_TRUE(ret);
    sleep(1); // wait for gradual animation
    uint32_t value = DisplayPowerMgrClient::GetInstance().GetBrightness();
    EXPECT_EQ(value, 255);
    ret = DisplayPowerMgrClient::GetInstance().RestoreBrightness(); // restore
    EXPECT_TRUE(ret);
    sleep(1); // wait for gradual animation
    value = DisplayPowerMgrClient::GetInstance().GetBrightness();
    EXPECT_EQ(value, 100);
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrBrightness003: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrBrightness004
 * @tc.desc: Test SetBrightness after OverrideBrightness
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrBrightness004, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrBrightness004: fun is start");
    bool ret = DisplayPowerMgrClient::GetInstance().OverrideBrightness(255);
    EXPECT_TRUE(ret);
    sleep(1); // wait for gradual animation
    ret = DisplayPowerMgrClient::GetInstance().SetBrightness(100);
    EXPECT_FALSE(ret);
    sleep(1); // wait for gradual animation
    uint32_t value = DisplayPowerMgrClient::GetInstance().GetBrightness();
    EXPECT_EQ(value, 255);
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrBrightness004: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrBrightness005
 * @tc.desc: Test RestoreBrightness after not override
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrBrightness005, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrBrightness005: fun is start");
    bool ret = DisplayPowerMgrClient::GetInstance().SetBrightness(100);
    EXPECT_TRUE(ret);
    sleep(1); // wait for gradual animation
    uint32_t value = DisplayPowerMgrClient::GetInstance().GetBrightness();
    EXPECT_EQ(value, 100);
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrBrightness005: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrBrightness006
 * @tc.desc: Test OverrideBrightness off Disable the call
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrBrightness006, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrBrightness006: fun is start");
    DisplayPowerMgrClient::GetInstance().SetDisplayState(DisplayState::DISPLAY_OFF);
    bool isBoost = DisplayPowerMgrClient::GetInstance().OverrideBrightness(100);
    EXPECT_FALSE(isBoost);
    bool isRestore = DisplayPowerMgrClient::GetInstance().RestoreBrightness();
    EXPECT_FALSE(isRestore);
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrBrightness006: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrMaxBrightness001
 * @tc.desc: Test GetMaxBrightness less equals 255
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrMaxBrightness001, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrMaxBrightness001: fun is start");
    uint32_t value = DisplayPowerMgrClient::GetInstance().GetMaxBrightness();
    EXPECT_LE(value, 255);
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrMaxBrightness001: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrMinBrightness001
 * @tc.desc: Test GetMinBrightness greater equals 0
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrMinBrightness001, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrMinBrightness001: fun is start");
    uint32_t value = DisplayPowerMgrClient::GetInstance().GetMinBrightness();
    EXPECT_GE(value, 0);
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrMinBrightness001: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrDefaultBrightness001
 * @tc.desc: Test GetDefaultnBrightness greater equals 0 and less equals 255
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrDefaultBrightness001, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrDefaultBrightness001: fun is start");
    uint32_t value = DisplayPowerMgrClient::GetInstance().GetDefaultBrightness();
    EXPECT_GE(value, 0);
    EXPECT_LE(value, 255);
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrDefaultBrightness001: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrBoostBrightnessNormal
 * @tc.desc: Test BoostBrightness the normal test
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrBoostBrightnessNormal, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrBoostBrightnessNormal: fun is start");
    bool isSucc = DisplayPowerMgrClient::GetInstance().BoostBrightness(1000);
    EXPECT_TRUE(isSucc);
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrBoostBrightnessNormal: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrBoostBrightnessAbnormal
 * @tc.desc: Test BoostBrightness the abnormal value
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrBoostBrightnessAbnormal, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrBoostBrightnessAbnormal: fun is start");
    bool isSucc = DisplayPowerMgrClient::GetInstance().BoostBrightness(-1);
    EXPECT_FALSE(isSucc);
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrBoostBrightnessAbnormal: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrCancelBoostBrightnessNormal
 * @tc.desc: Test CancelBoostBrightness the normal test
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrCancelBoostBrightnessNormal, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrCancelBoostBrightnessNormal: fun is start");
    bool isSucc = DisplayPowerMgrClient::GetInstance().BoostBrightness(50000);
    EXPECT_TRUE(isSucc);
    sleep(1);
    bool isCancel = DisplayPowerMgrClient::GetInstance().CancelBoostBrightness();
    EXPECT_TRUE(isCancel);
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrCancelBoostBrightnessNormal: fun is end");
}

/**
 * @tc.name: BoostAndOverrideMutuallyExclusive1
 * @tc.desc: Test BoostBrightness and OverrideBrightness are mutually exclusive
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, BoostAndOverrideMutuallyExclusive1, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "BoostAndOverrideMutuallyExclusive: fun is start");
    bool isBoost = DisplayPowerMgrClient::GetInstance().BoostBrightness(50000);
    EXPECT_TRUE(isBoost);
    sleep(1);
    bool isOverride = DisplayPowerMgrClient::GetInstance().OverrideBrightness(100);
    EXPECT_FALSE(isOverride);
    bool isRestore = DisplayPowerMgrClient::GetInstance().RestoreBrightness();
    EXPECT_FALSE(isRestore);
    DISPLAY_HILOGI(LABEL_TEST, "BoostAndOverrideMutuallyExclusive: fun is end");
}

/**
 * @tc.name: BoostAndOverrideMutuallyExclusive2
 * @tc.desc: Test BoostBrightness and OverrideBrightness are mutually exclusive
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, BoostAndOverrideMutuallyExclusive2, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "BoostAndOverrideMutuallyExclusive2: fun is start");
    bool isOverride = DisplayPowerMgrClient::GetInstance().OverrideBrightness(255);
    EXPECT_TRUE(isOverride);
    sleep(1);
    bool isBoost = DisplayPowerMgrClient::GetInstance().BoostBrightness(50000);
    EXPECT_FALSE(isBoost);
    bool isCancel = DisplayPowerMgrClient::GetInstance().CancelBoostBrightness();
    EXPECT_FALSE(isCancel);
    DISPLAY_HILOGI(LABEL_TEST, "BoostAndOverrideMutuallyExclusive2: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrBoostBrightnessTimeout1
 * @tc.desc: Test BoostBrightness timeout restore system brightness
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrBoostBrightnessTimeout1, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrBoostBrightnessTimeout1: fun is start");
    uint32_t beforeValue = DisplayPowerMgrClient::GetInstance().GetBrightness();
    bool isBoost = DisplayPowerMgrClient::GetInstance().BoostBrightness(1000);
    EXPECT_TRUE(isBoost);
    sleep(2);
    uint32_t currentValue = DisplayPowerMgrClient::GetInstance().GetBrightness();
    EXPECT_EQ(beforeValue, currentValue);
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrBoostBrightnessTimeout1: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrBoostBrightnessTimeout2
 * @tc.desc: Test BoostBrightness timeout brightness adjustment can be called
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrBoostBrightnessTimeout2, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrBoostBrightnessTimeout2: fun is start");
    bool isBoost = DisplayPowerMgrClient::GetInstance().BoostBrightness(1000);
    EXPECT_TRUE(isBoost);
    sleep(2);
    bool isSet = DisplayPowerMgrClient::GetInstance().SetBrightness(100);
    EXPECT_TRUE(isSet);
    uint32_t currentValue = DisplayPowerMgrClient::GetInstance().GetBrightness();
    EXPECT_EQ(100, currentValue);
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrBoostBrightnessTimeout2: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrBoostBrightnessCancel1
 * @tc.desc: Test BoostBrightness Cancel restore system brightness
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrBoostBrightnessCancel1, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrBoostBrightnessCancel1: fun is start");
    uint32_t beforeValue = DisplayPowerMgrClient::GetInstance().GetBrightness();
    bool isBoost = DisplayPowerMgrClient::GetInstance().BoostBrightness(10000);
    EXPECT_TRUE(isBoost);
    sleep(1);
    bool isCancel = DisplayPowerMgrClient::GetInstance().CancelBoostBrightness();
    EXPECT_TRUE(isCancel);
    sleep(1);
    uint32_t currentValue = DisplayPowerMgrClient::GetInstance().GetBrightness();
    EXPECT_EQ(beforeValue, currentValue) << "beforeValue: " << beforeValue << " currentVal: " << currentValue;
    DISPLAY_HILOGI(LABEL_TEST, "BoostBrightnessCancel1: beforeValue: %{public}d, currentVal: %{public}d",
        beforeValue, currentValue);
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrBoostBrightnessCancel1: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrBoostBrightnessCancel2
 * @tc.desc: Test BoostBrightness Cancel brightness adjustment can be called
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrBoostBrightnessCancel2, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrBoostBrightnessCancel2: fun is start");
    bool isBoost = DisplayPowerMgrClient::GetInstance().BoostBrightness(10000);
    EXPECT_TRUE(isBoost);
    sleep(1);
    bool isCancel = DisplayPowerMgrClient::GetInstance().CancelBoostBrightness();
    EXPECT_TRUE(isCancel);
    bool isSet = DisplayPowerMgrClient::GetInstance().SetBrightness(111);
    EXPECT_TRUE(isSet);
    uint32_t currentValue = DisplayPowerMgrClient::GetInstance().GetBrightness();
    EXPECT_EQ(111, currentValue);
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrBoostBrightnessCancel2: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrBoostBrightnessNotAdjust
 * @tc.desc: Test BoostBrightness do not adjust brightness
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrBoostBrightnessNotAdjust, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrBoostBrightnessNotAdjust: fun is start");
    bool isBoost = DisplayPowerMgrClient::GetInstance().BoostBrightness(10000);
    EXPECT_TRUE(isBoost);
    sleep(1);
    bool isSet = DisplayPowerMgrClient::GetInstance().SetBrightness(100);
    EXPECT_FALSE(isSet);
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrBoostBrightnessNotAdjust: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrBoostBrightnessScreenOff
 * @tc.desc: Test BoostBrightness off Disable the call
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrBoostBrightnessScreenOff, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrBoostBrightnessScreenOff: fun is start");
    DisplayPowerMgrClient::GetInstance().SetDisplayState(DisplayState::DISPLAY_OFF);
    bool isBoost = DisplayPowerMgrClient::GetInstance().BoostBrightness(10000);
    EXPECT_FALSE(isBoost);
    bool isCancel = DisplayPowerMgrClient::GetInstance().CancelBoostBrightness();
    EXPECT_FALSE(isCancel);
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrBoostBrightnessScreenOff: fun is end");
}

/**
 * @tc.name: DisplayPowerMgrBoostBrightnessMultipleCalls
 * @tc.desc: Test BoostBrightness multiple calls
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerMgrBrightnessTest, DisplayPowerMgrBoostBrightnessMultipleCalls, TestSize.Level0)
{
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrBoostBrightnessMultipleCalls: fun is start");
    const uint32_t COUNT = 3;
    const uint32_t TIMEOUT = 1000;
    for (uint32_t i = 0; i < COUNT; ++i) {
        bool isBoost = DisplayPowerMgrClient::GetInstance().BoostBrightness(TIMEOUT);
        EXPECT_TRUE(isBoost);
    }
    // Brightness adjustment is not allowed
    bool isSet = DisplayPowerMgrClient::GetInstance().SetBrightness(123);
    EXPECT_FALSE(isSet);
    // Wait for a timeout
    sleep(COUNT + 1);
    bool isCancel = DisplayPowerMgrClient::GetInstance().CancelBoostBrightness();
    EXPECT_FALSE(isCancel);
    isSet = DisplayPowerMgrClient::GetInstance().SetBrightness(222);
    EXPECT_TRUE(isSet);
    uint32_t currentVal = DisplayPowerMgrClient::GetInstance().GetBrightness();
    EXPECT_EQ(222, currentVal);
    DISPLAY_HILOGI(LABEL_TEST, "DisplayPowerMgrBoostBrightnessMultipleCalls: fun is end");
}
}
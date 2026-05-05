/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef MOCK_DISPLAY_POWER_MGR_CLIENT_H
#define MOCK_DISPLAY_POWER_MGR_CLIENT_H

#include <gmock/gmock.h>
#include "display_power_info.h"
#include "display_mgr_errors.h"

namespace OHOS {
namespace DisplayPowerMgr {

class MockDisplayPowerMgrClient {
public:
    static MockDisplayPowerMgrClient& GetInstance()
    {
        static MockDisplayPowerMgrClient instance;
        return instance;
    }

    MOCK_METHOD(bool, SetBrightness, (uint32_t value, uint32_t displayId, bool continuous));
    MOCK_METHOD(DisplayErrors, GetError, ());
};

}  // namespace DisplayPowerMgr
}  // namespace OHOS

#endif  // MOCK_DISPLAY_POWER_MGR_CLIENT_H

/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef POWERMGR_IDISPLAY_BRIGHTNESS_CALLBACK_H
#define POWERMGR_IDISPLAY_BRIGHTNESS_CALLBACK_H

#include <iremote_broker.h>
#include <iremote_object.h>

namespace OHOS {
namespace DisplayPowerMgr {
class IDisplayBrightnessCallback : public IRemoteBroker {
public:
    virtual void OnNotifyApsLightBrightnessChange(uint32_t type, int32_t state) = 0;

    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.powermgr.IDisplayBrightnessCallback");
};
} // namespace DisplayPowerMgr
} // namespace OHOS
#endif // POWERMGR_IDISPLAY_POWER_CALLBACK_H
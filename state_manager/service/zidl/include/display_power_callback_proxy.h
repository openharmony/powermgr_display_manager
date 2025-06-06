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

#ifndef DISPLAYMGR_DISPLAY_POWER_CALLBACK_PROXY_H
#define DISPLAYMGR_DISPLAY_POWER_CALLBACK_PROXY_H

#include <cstdint>
#include <functional>
#include <iremote_proxy.h>
#include "iremote_broker.h"
#include "iremote_object.h"
#include "refbase.h"
#include "display_power_info.h"
#include "idisplay_power_callback.h"

namespace OHOS {
namespace DisplayPowerMgr {
class DisplayPowerCallbackProxy : public IRemoteProxy<IDisplayPowerCallback> {
public:
    explicit DisplayPowerCallbackProxy(const sptr<IRemoteObject>& impl)
        : IRemoteProxy<IDisplayPowerCallback>(impl) {}
    ~DisplayPowerCallbackProxy() override = default;
    virtual void OnDisplayStateChanged(uint32_t displayId, DisplayState state, uint32_t reason) override;

private:
    static inline BrokerDelegator<DisplayPowerCallbackProxy> delegator_;
};
} // namespace DisplayPowerMgr
} // namespace OHOS

#endif // DISPLAYMGR_DISPLAY_POWER_CALLBACK_PROXY_H
/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ohos.brightness.ani.hpp"
#include "display_log.h"
using namespace OHOS::DisplayPowerMgr;
ANI_EXPORT ani_status ANI_Constructor(ani_vm *vm, uint32_t *result)
{
    ani_env *env;
    if (ANI_OK != vm->GetEnv(ANI_VERSION_1, &env)) {
        DISPLAY_HILOGE(COMP_FWK, "Error from GetEnv");
        return ANI_ERROR;
    }
    if (ANI_OK != ohos::brightness::ANIRegister(env)) {
        DISPLAY_HILOGE(COMP_FWK, "Error from ohos::brightness::ANIRegister");
        return ANI_ERROR;
    }
    *result = ANI_VERSION_1;
    return ANI_OK;
}

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

#ifndef OHOS_DISPLAY_MANAGER_CLI_PARSE_BRIGHTNESS_U32_H
#define OHOS_DISPLAY_MANAGER_CLI_PARSE_BRIGHTNESS_U32_H

#include <charconv>
#include <cstdint>
#include <string_view>
#include <system_error>

namespace OHOS {
namespace DisplayPowerMgr {
inline bool ParseBrightnessU32(std::string_view text, uint32_t &out)
{
    if (text.empty()) {
        return false;
    }
    uint32_t value = 0;
    auto result = std::from_chars(text.data(), text.data() + text.size(), value, 10);
    if (result.ec != std::errc() || result.ptr != text.data() + text.size()) {
        return false;
    }
    out = value;
    return true;
}
} // namespace DisplayPowerMgr
} // namespace OHOS
#endif // OHOS_DISPLAY_MANAGER_CLI_PARSE_BRIGHTNESS_U32_H

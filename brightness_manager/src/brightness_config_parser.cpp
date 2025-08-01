/*
 * Copyright (c) 2024-2024 Huawei Device Co., Ltd.
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

#include "brightness_config_parser.h"

#include <unistd.h>
#include <cJSON.h>

#include "config_parser_base.h"
#include "display_log.h"

namespace OHOS {
namespace DisplayPowerMgr {
namespace {
const std::string CONFIG_NAME = "brightness_config";
} // namespace

using namespace OHOS::DisplayPowerMgr;

bool BrightnessConfigParser::ParseConfig(BrightnessConfig::Data& data)
{
    DISPLAY_HILOGI(FEAT_BRIGHTNESS, "parse BrightnessConfigParser start!");
    const std::string fileContent = ConfigParserBase::Get().LoadConfigRoot(0, CONFIG_NAME);
    if (!ParseConfigJsonRoot(fileContent, data)) {
        DISPLAY_HILOGI(FEAT_BRIGHTNESS, "parse BrightnessConfigParser error!");
        return false;
    }
    DISPLAY_HILOGI(FEAT_BRIGHTNESS, "parse BrightnessConfigParser over!");
    return true;
}

bool BrightnessConfigParser::ParseConfigJsonRoot(const std::string& fileContent, BrightnessConfig::Data& data)
{
    const cJSON* root = cJSON_Parse(fileContent.c_str());
    if (!root) {
        DISPLAY_HILOGE(FEAT_BRIGHTNESS, "Parse file %{public}s failure.", fileContent.c_str());
        return false;
    }
    if (!cJSON_IsObject(root)) {
        DISPLAY_HILOGE(FEAT_BRIGHTNESS, "root is not an object!");
        cJSON_Delete(const_cast<cJSON*>(root));
        return false;
    }

    ConfigParserBase::Get().ParseScreenData(root, "displayModeData", data.displayModeMap, "displayMode");
    ConfigParserBase::Get().ParseScreenData(root, "foldStatus", data.foldStatusModeMap, "foldStatus");

    cJSON_Delete(const_cast<cJSON*>(root));
    return true;
}

void BrightnessConfigParser::PrintConfig(const BrightnessConfig::Data& data)
{
    std::string text = "";
    text.append("screenData: ");
    text.append(ConfigParserBase::Get().ScreenDataToString("displayModeData", data.displayModeMap, "displayMode"));
    text.append(ConfigParserBase::Get().ScreenDataToString("foldStatus", data.foldStatusModeMap, "foldStatus"));
    DISPLAY_HILOGI(FEAT_BRIGHTNESS, "%{public}s", text.c_str());
}
} // namespace DisplayPowerMgr
} // namespace OHOS

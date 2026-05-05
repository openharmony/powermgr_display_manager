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

#include "cli_command.h"

#include <iostream>
#include <string>
#include <unordered_map>
#include <functional>
#include <cstring>
#include <cstdlib>
#include "cJSON.h"

#include "display_mgr_errors.h"

#ifdef CLI_UT_TEST
#include "mock_display_power_mgr_client.h"
#else
#include "display_power_mgr_client.h"
#endif

using CommandHandler = std::function<int(int, char**)>;

namespace OHOS {
namespace DisplayPowerMgr {

static const uint32_t BRIGHTNESS_MIN_VALUE = 0;
static const uint32_t BRIGHTNESS_MAX_VALUE = 255;

int OutputSuccess(const std::string& jsonData)
{
    cJSON* response = cJSON_CreateObject();
    cJSON_AddStringToObject(response, "status", "success");
    cJSON* data = cJSON_Parse(jsonData.c_str());
    if (data != nullptr) {
        cJSON_AddItemToObject(response, "data", data);
    }
    char* out = cJSON_PrintUnformatted(response);
    std::cout << out << std::endl;
    cJSON_free(out);
    cJSON_Delete(response);
    return 0;
}

int OutputError(const std::string& code, const std::string& message)
{
    cJSON* response = cJSON_CreateObject();
    cJSON_AddStringToObject(response, "status", "error");
    cJSON_AddStringToObject(response, "errCode", code.c_str());
    cJSON_AddStringToObject(response, "errMsg", message.c_str());
    char* out = cJSON_PrintUnformatted(response);
    std::cout << out << std::endl;
    cJSON_free(out);
    cJSON_Delete(response);
    return 1;
}

SetBrightnessArgs ParseSetBrightnessArgs(int argc, char** argv)
{
    SetBrightnessArgs args;
    const uint32_t temp = 10;
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "--value") == 0 && i + 1 < argc) {
            args.hasValue = true;
            args.value = static_cast<uint32_t>(std::strtoul(argv[i + 1], nullptr, temp));
            i++;
        } else if (strcmp(argv[i], "--continuous") == 0) {
            args.continuous = true;
        }
    }
    return args;
}

int CmdSetBrightness(int argc, char** argv)
{
    SetBrightnessArgs args = ParseSetBrightnessArgs(argc, argv);

    if (!args.hasValue) {
        return OutputError("ERR_ARG_MISSING",
            "Missing required parameter: --value is required to specify brightness level."
                " Example: ohos-displayManager set-brightness --value 128");
    }

    if (args.value < BRIGHTNESS_MIN_VALUE || args.value > BRIGHTNESS_MAX_VALUE) {
        return OutputError("ERR_ARG_OUT_OF_RANGE",
            "Brightness value " + std::to_string(args.value) + " is out of range, must be"
                " between 0 and 255. Example: --value 128");
    }

#ifdef CLI_UT_TEST
    auto& client = MockDisplayPowerMgrClient::GetInstance();
#else
    auto& client = DisplayPowerMgrClient::GetInstance();
#endif
    bool result = client.SetBrightness(args.value, 0, args.continuous);

    if (!result) {
        DisplayErrors err = client.GetError();
        std::string errCodeStr;
        std::string errMsg;

        switch (err) {
            case DisplayErrors::ERR_PERMISSION_DENIED:
                errCodeStr = "ERR_PERMISSION_DENIED";
                errMsg = "Permission denied: system application identity is required to set brightness.";
                break;
            case DisplayErrors::ERR_SYSTEM_API_DENIED:
                errCodeStr = "ERR_SYSTEM_API_DENIED";
                errMsg = "System API access denied: system-level permission is required.";
                break;
            case DisplayErrors::ERR_PARAM_INVALID:
                errCodeStr = "ERR_ARG_INVALID";
                errMsg = "Invalid parameter: brightness value is not valid. Please check the brightness value (0-255).";
                break;
            case DisplayErrors::ERR_CONNECTION_FAIL:
                errCodeStr = "ERR_INTERNAL_ERROR";
                errMsg = "Failed to connect to display manager service."
                    " Please check if the service is running and try again later.";
                break;
            default:
                errCodeStr = "ERR_INTERNAL_ERROR";
                errMsg = "Set brightness failed with internal error code: "
                    + std::to_string(static_cast<int32_t>(err)) +
                    ". Please check system logs for details or try again later.";
                break;
        }
        return OutputError(errCodeStr, errMsg);
    }

    cJSON* data = cJSON_CreateObject();
    cJSON_AddNumberToObject(data, "value", args.value);
    cJSON_AddBoolToObject(data, "continuous", args.continuous);
    cJSON_AddStringToObject(data, "message", "Brightness set successfully");
    char* dataStr = cJSON_PrintUnformatted(data);
    std::string dataJson(dataStr);
    cJSON_free(dataStr);
    cJSON_Delete(data);
    return OutputSuccess(dataJson);
}

void PrintHelp(const char* prog)
{
    std::cerr << prog << " - Display brightness management CLI tool for adjusting screen brightness" << std::endl;
    std::cerr << std::endl;
    std::cerr << "Usage:" << std::endl;
    std::cerr << "  " << prog << " <command> [options]" << std::endl;
    std::cerr << std::endl;
    std::cerr << "Parameters:" << std::endl;
    std::cerr << "  --help                  Display this help message" << std::endl;
    std::cerr << std::endl;
    std::cerr << "Commands:" << std::endl;
    std::cerr << "  set-brightness          Set display screen brightness" << std::endl;
}

void PrintSubCommandHelp(const char* prog)
{
    std::cerr << prog << " set-brightness - Set the brightness of a display device" << std::endl;
    std::cerr << std::endl;
    std::cerr << "Usage:" << std::endl;
    std::cerr << "  " << prog << " set-brightness --value <brightness> [--continuous]" << std::endl;
    std::cerr << std::endl;
    std::cerr << "Parameters:" << std::endl;
    std::cerr << "  --value <brightness>    Brightness value (required, range: 0-255)" << std::endl;
    std::cerr << "  --continuous            Continuous adjustment mode (optional, default: false)" << std::endl;
    std::cerr << "  --help                  Display this help message" << std::endl;
}

int DispatchCommand(int argc, char** argv)
{
    const uint32_t ARGC_MAX = 2;
    if (argc < ARGC_MAX) {
        PrintHelp(argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "--help") == 0) {
        PrintHelp(argv[0]);
        return 0;
    }

    static std::unordered_map<std::string, CommandHandler> commands = {
        {"set-brightness", CmdSetBrightness},
    };

    std::string cmdName = argv[1];

    // Subcommand --help support: <cli-name> <subcommand> --help
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            if (cmdName == "set-brightness") {
                PrintSubCommandHelp(argv[0]);
                return 0;
            }
            PrintHelp(argv[0]);
            return 0;
        }
    }

    auto it = commands.find(cmdName);
    if (it == commands.end()) {
        std::cerr << "[ERROR] Unknown command: " << cmdName << std::endl;
        PrintHelp(argv[0]);
        return 1;
    }

    const uint32_t ARGC_MAX = 2;
    return it->second(argc - ARGC_MAX, argv + ARGC_MAX);
}

}  // namespace DisplayPowerMgr
}  // namespace OHOS

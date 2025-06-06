/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "display_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "message_parcel.h"
#include "securec.h"

using namespace OHOS::DisplayPowerMgr;
using namespace OHOS;

namespace {
const int32_t REWIND_READ_DATA = 0;
const int32_t SLEEP_TIME = 5;
} // namespace

DisplayFuzzerTest::DisplayFuzzerTest()
{
    service_ = DelayedSpSingleton<DisplayPowerMgrService>::GetInstance();
    service_->Init();
}

DisplayFuzzerTest::~DisplayFuzzerTest()
{
    if (service_ != nullptr) {
        sleep(SLEEP_TIME);
        service_->Deinit();
        service_->Reset();
    }
    service_ = nullptr;
}

void DisplayFuzzerTest::TestDisplayServiceStub(const uint32_t code, const uint8_t* data, size_t size)
{
    MessageParcel datas;
    datas.WriteInterfaceToken(DisplayPowerMgrService::GetDescriptor());
    datas.WriteBuffer(data, size);
    datas.RewindRead(REWIND_READ_DATA);
    MessageParcel reply;
    MessageOption option;
    service_->OnRemoteRequest(code, datas, reply, option);
}

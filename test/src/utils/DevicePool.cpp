/////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Tencent is pleased to support the open source community by making tgfx available.
//
//  Copyright (C) 2023 THL A29 Limited, a Tencent company. All rights reserved.
//
//  Licensed under the BSD 3-Clause License (the "License"); you may not use this file except
//  in compliance with the License. You may obtain a copy of the License at
//
//      https://opensource.org/licenses/BSD-3-Clause
//
//  unless required by applicable law or agreed to in writing, software distributed under the
//  license is distributed on an "as is" basis, without warranties or conditions of any kind,
//  either express or implied. see the license for the specific language governing permissions
//  and limitations under the license.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "DevicePool.h"
#include <thread>

namespace tgfx {
static std::mutex threadCacheLocker = {};
static std::unordered_map<std::thread::id, std::shared_ptr<tgfx::GLDevice>> threadCacheMap = {};

std::shared_ptr<tgfx::GLDevice> DevicePool::Make() {
  std::lock_guard<std::mutex> autoLock(threadCacheLocker);
  auto threadID = std::this_thread::get_id();
  auto result = threadCacheMap.find(threadID);
  if (result != threadCacheMap.end()) {
    return result->second;
  }
  auto device = tgfx::GLDevice::Make();
  if (device != nullptr) {
    threadCacheMap[threadID] = device;
  }
  return device;
}

void DevicePool::CleanAll() {
  std::lock_guard<std::mutex> autoLock(threadCacheLocker);
  threadCacheMap = {};
}
}  // namespace tgfx

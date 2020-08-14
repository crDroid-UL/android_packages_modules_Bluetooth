/*
 * Copyright 2019 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <fstream>
#include <iostream>
#include <mutex>
#include <string>

#include "hal/hci_hal.h"
#include "module.h"

namespace bluetooth {
namespace hal {

class SnoopLogger : public ::bluetooth::Module {
 public:
  static const ModuleFactory Factory;

  static const std::string kBtSnoopLogModeDisabled;
  static const std::string kBtSnoopLogModeFiltered;
  static const std::string kBtSnoopLogModeFull;

  static const std::string kBtSnoopMaxPacketsPerFileProperty;
  static const std::string kIsDebuggableProperty;
  static const std::string kBtSnoopLogModeProperty;
  static const std::string kBtSnoopDefaultLogModeProperty;

  // Put in header for test
  struct PacketHeaderType {
    uint32_t length_original;
    uint32_t length_captured;
    uint32_t flags;
    uint32_t dropped_packets;
    uint64_t timestamp;
    uint8_t type;
  } __attribute__((__packed__));

  // Put in header for test
  struct FileHeaderType {
    uint8_t identification_pattern[8];
    uint32_t version_number;
    uint32_t datalink_type;
  } __attribute__((__packed__));

  // Set File Path before module is started to ensure all packets are written to the right file
  static void SetFilePath(std::string filename);

  // Returns the maximum number of packets per file
  // Changes to this value is only effective after restarting Bluetooth
  static size_t GetMaxPacketsPerFile();

  // Get snoop logger mode based on current system setup
  // Changes to this values is only effective after restarting Bluetooth
  static std::string GetBtSnoopMode();

  // Get snoop log path based on current system configuration
  static std::string GetLogPath();

  // Has to be defined from 1 to 4 per btsnoop format
  enum PacketType {
    CMD = 1,
    ACL = 2,
    SCO = 3,
    EVT = 4,
    ISO = 5,
  };

  enum Direction {
    INCOMING,
    OUTGOING,
  };

  void Capture(const HciPacket& packet, Direction direction, PacketType type);

 protected:
  void ListDependencies(ModuleList* list) override;
  void Start() override;
  void Stop() override;

  // Visible for testing
  SnoopLogger(std::string log_path, size_t max_packets_per_file, const std::string& btsnoop_mode);
  void CloseCurrentSnoopLogFile();
  void OpenNextSnoopLogFile();

 private:
  static std::string user_file_path_;
  std::string file_path_;
  std::ofstream btsnoop_ostream_;
  bool is_enabled_ = false;
  bool is_filtered_ = false;
  size_t max_packets_per_file_;
  size_t packet_counter_ = 0;
  std::recursive_mutex file_mutex_;
};

}  // namespace hal
}  // namespace bluetooth

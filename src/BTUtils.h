// Copyright 2018 AntorFR
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.



#ifndef _BTUTILS_H_
#define _BTUTILS_H_
#include "sdkconfig.h"
#if defined(CONFIG_BT_ENABLED)
#include <esp_gap_bt_api.h> // ESP32 BLE
#include <string>

/**
 * @brief A set of general %BLE utilities.
 */
class BTUtils {
public:
	static const char*        eirTypeToString(uint8_t advType);
	static char*              buildHexData(uint8_t* target, uint8_t* source, uint8_t length);
	static std::string        buildPrintData(uint8_t* source, size_t length);
	
	static const char*        devTypeToString(esp_bt_dev_type_t type);
	static void dumpGapEvent(
		esp_bt_gap_cb_event_t  event,
		esp_bt_gap_cb_param_t* param);
	
	static const char* gapEventToString(uint32_t eventType);
};

#endif // CONFIG_BT_ENABLED
#endif /* _BTUTILS_H_ */
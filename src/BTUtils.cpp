// Copyright 2018 AntorFR
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


/*
 * BTUtils.cpp
 *
 *  Created on: Mar 25, 2017
 *      Author: kolban
 */
#include "sdkconfig.h"
#if defined(CONFIG_BT_ENABLED)
#include "BTAddress.h"
#include "BTUtils.h"
#include "BTUUID.h"
#include "GeneralUtils.h"

#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <esp_bt.h>              // ESP32 BLE
#include <esp_bt_main.h>     // ESP32 BLE
#include <esp_gap_bt_api.h> // ESP32 BLE
#include "esp_gap_bt_api.h"
#include "esp_bt_device.h"
#include <esp_err.h>         // ESP32 ESP-IDF
#include <esp_log.h>         // ESP32 ESP-IDF
#include <map>               // Part of C++ STL
#include <sstream>
#include <iomanip>

#ifdef ARDUINO_ARCH_ESP32
#include "esp32-hal-log.h"
#endif



/**
 * @brief Given an eir type, return a string representation of the type.
 *
 * For details see ...
 * https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/bluetooth/esp_gap_bt.html#_CPPv217esp_bt_eir_type_t
 *
 * @return A string representation of the type.
 */

const char* BTUtils::eirTypeToString(uint8_t eirType) {
	switch(eirType) {
		case ESP_BT_EIR_TYPE_FLAGS:                   // 0x01
			return "ESP_BT_EIR_TYPE_FLAGS";

		case ESP_BT_EIR_TYPE_INCMPL_16BITS_UUID:             // 0x02
			return "ESP_BT_EIR_TYPE_INCMPL_16BITS_UUID";

		case ESP_BT_EIR_TYPE_CMPL_16BITS_UUID:             // 0x03
			return "ESP_BT_EIR_TYPE_CMPL_16BITS_UUID";

		case ESP_BT_EIR_TYPE_INCMPL_32BITS_UUID:             // 0x04
			return "ESP_BT_EIR_TYPE_INCMPL_32BITS_UUID";

		case ESP_BT_EIR_TYPE_CMPL_32BITS_UUID:             // 0x05
			return "ESP_BT_EIR_TYPE_CMPL_32BITS_UUID";

		case ESP_BT_EIR_TYPE_INCMPL_128BITS_UUID:            // 0x06
			return "ESP_BT_EIR_TYPE_INCMPL_128BITS_UUID";

		case ESP_BT_EIR_TYPE_CMPL_128BITS_UUID:            // 0x07
			return "ESP_BT_EIR_TYPE_CMPL_128BITS_UUID";

		case ESP_BT_EIR_TYPE_SHORT_LOCAL_NAME:             // 0x08
			return "ESP_BT_EIR_TYPE_SHORT_LOCAL_NAME";

		case ESP_BT_EIR_TYPE_CMPL_LOCAL_NAME:              // 0x09
			return "ESP_BT_EIR_TYPE_CMPL_LOCAL_NAME";

		case ESP_BT_EIR_TYPE_TX_POWER_LEVEL:                 // 0x0a
			return "ESP_BT_EIR_TYPE_TX_POWER_LEVEL";

		case ESP_BT_EIR_TYPE_MANU_SPECIFIC:  // 0xff
			return "ESP_BT_EIR_TYPE_MANU_SPECIFIC";

		default:
			log_d(" eir data type: 0x%x", eirType);
			return "";
	} // End switch
} // advTypeToString


/**
 * @brief Create a hex representation of data.
 *
 * @param [in] target Where to write the hex string.  If this is null, we malloc storage.
 * @param [in] source The start of the binary data.
 * @param [in] length The length of the data to convert.
 * @return A pointer to the formatted buffer.
 */
char* BTUtils::buildHexData(uint8_t *target, uint8_t *source, uint8_t length) {
// Guard against too much data.
	if (length > 100) {
		length = 100;
	}

	if (target == nullptr) {
		target = (uint8_t *)malloc(length * 2 + 1);
		if (target == nullptr) {
			log_e("buildHexData: malloc failed");
			return nullptr;
		}
	}
	char *startOfData = (char *)target;

	int i;
	for (i=0; i<length; i++) {
		sprintf((char *)target, "%.2x", (char)*source);
		source++;
		target +=2;
	}

// Handle the special case where there was no data.
	if (length == 0) {
		*startOfData = 0;
	}

	return startOfData;
} // buildHexData


/**
 * @brief Build a printable string of memory range.
 * Create a string representation of a piece of memory. Only printable characters will be included
 * while those that are not printable will be replaced with '.'.
 * @param [in] source Start of memory.
 * @param [in] length Length of memory.
 * @return A string representation of a piece of memory.
 */
std::string BTUtils::buildPrintData(uint8_t* source, size_t length) {
	std::ostringstream ss;
	for (int i=0; i<length; i++) {
		char c = *source;
		if (isprint(c)) {
			ss << c;
		} else {
			ss << '.';
		}
		source++;
	}
	return ss.str();
} // buildPrintData


/**
 * @brief Convert a BLE device type to a string.
 * @param [in] type The device type.
 */
const char* BTUtils::devTypeToString(esp_bt_dev_type_t type) {
	switch(type) {
	case ESP_BT_DEVICE_TYPE_BREDR:
		return "ESP_BT_DEVICE_TYPE_BREDR";
	case ESP_BT_DEVICE_TYPE_BLE:
		return "ESP_BT_DEVICE_TYPE_BLE";
	case ESP_BT_DEVICE_TYPE_DUMO:
		return "ESP_BT_DEVICE_TYPE_DUMO";
	default:
		return "Unknown";
	}
} // devTypeToString


/**
 * @brief Dump the GAP event to the log.
 */
void BTUtils::dumpGapEvent(
	esp_bt_gap_cb_event_t  event,
	esp_bt_gap_cb_param_t* param) {
	log_d("Received a GAP event: %s", gapEventToString(event));

} // dumpGapEvent


/**
 * @brief Convert a BT GAP event type to a string representation.
 * @param [in] eventType The type of event.
 * @return A string representation of the event type.
 */
const char* BTUtils::gapEventToString(uint32_t eventType) {
	switch(eventType) {
		case ESP_BT_GAP_DISC_RES_EVT:			return "ESP_BT_GAP_DISC_RES_EVT";
		case ESP_BT_GAP_DISC_STATE_CHANGED_EVT:	return "ESP_BT_GAP_DISC_STATE_CHANGED_EVT";
		case ESP_BT_GAP_RMT_SRVCS_EVT:			return "ESP_BT_GAP_RMT_SRVCS_EVT";
		case ESP_BT_GAP_RMT_SRVC_REC_EVT:		return "ESP_BT_GAP_RMT_SRVC_REC_EVT";
		case ESP_BT_GAP_AUTH_CMPL_EVT:       	return "ESP_BT_GAP_AUTH_CMPL_EVT";
		//case ESP_BT_GAP_CFM_REQ_EVT:			return "ESP_BT_GAP_CFM_REQ_EVT";
		//case ESP_BT_GAP_KEY_NOTIF_EVT:			return "ESP_BT_GAP_KEY_NOTIF_EVT";
		//case ESP_BT_GAP_KEY_REQ_EVT:            return "ESP_BT_GAP_KEY_REQ_EVT";
		case ESP_BT_GAP_READ_RSSI_DELTA_EVT:    return "ESP_BT_GAP_READ_RSSI_DELTA_EVT";
		case ESP_BT_GAP_EVT_MAX:                return "ESP_BT_GAP_EVT_MAX";

		default:
			log_d("gapEventToString: Unknown event type %d 0x%.2x", eventType, eventType);
			return "Unknown event type";
	}
} // gapEventToString



#endif // CONFIG_BT_ENABLED
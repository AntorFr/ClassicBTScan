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

#include "sdkconfig.h"
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_bt_device.h"
#include <esp_log.h>

#if defined(CONFIG_BT_ENABLED) && defined(CONFIG_BLUEDROID_ENABLED)

#ifdef ARDUINO_ARCH_ESP32
#include "esp32-hal-log.h"
#include "esp32-hal-bt.h"
#endif

#include "BTScan.h"

/*
static char *uuid2str(esp_bt_uuid_t *uuid, char *str, size_t size){
    if (uuid == NULL || str == NULL) {
        return NULL;
    }

    if (uuid->len == 2 && size >= 5) {
        sprintf(str, "%04x", uuid->uuid.uuid16);
    } else if (uuid->len == 4 && size >= 9) {
        sprintf(str, "%08x", uuid->uuid.uuid32);
    } else if (uuid->len == 16 && size >= 37) {
        uint8_t *p = uuid->uuid.uuid128;
        sprintf(str, "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                p[15], p[14], p[13], p[12], p[11], p[10], p[9], p[8],
                p[7], p[6], p[5], p[4], p[3], p[2], p[1], p[0]);
    } else {
        return NULL;
    }

    return str;
}
*/

 BTScan::BTScan() {
	m_pAdvertisedDeviceCallbacks     = nullptr;
	m_stopped                        = true;
	m_wantDuplicates                 = false;
} // BLEScan


BTScan::~BTScan(void) {
    stop_bt();
}

void BTScan::handleGAPEvent( esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t* param) {
	switch(event) {
        case ESP_BT_GAP_DISC_RES_EVT:{
            if (m_stopped) { // If we are not scanning, nothing to do with the extra results.
			    break;
			}

            // Examine our list of previously scanned addresses and, if we found this one already,
            // ignore it.
            BTAddress advertisedAddress(param->disc_res.bda);
            bool found = false;

            for (int i=0; i<m_scanResults.getCount(); i++) {
                if (m_scanResults.getDevice(i).getAddress().equals(advertisedAddress)) {
                    found = true;
                    break;
                }
            }
            if (found && !m_wantDuplicates) {  // If we found a previous entry AND we don't want duplicates, then we are done.
                log_d("Ignoring %s, already seen it.", advertisedAddress.toString().c_str());
                break;
            }

            // We now construct a model of the advertised device that we have just found for the first
            // time.
            BTAdvertisedDevice advertisedDevice;
            log_d( "Device found: %s", advertisedAddress.toString().c_str());

            advertisedDevice.setAddress(advertisedAddress);
            advertisedDevice.parseAdvertisement(&param->disc_res);
            advertisedDevice.setScan(this);

            if (m_pAdvertisedDeviceCallbacks) {
                m_pAdvertisedDeviceCallbacks->onResult(advertisedDevice);
            }

            if (!found) {   // If we have previously seen this device, don't record it again.
                m_scanResults.m_vectorAdvertisedDevices.push_back(advertisedDevice);
            }


            break;
        }
		case ESP_BT_GAP_DISC_STATE_CHANGED_EVT: {
			switch(param->disc_st_chg.state) {
				//
				// ESP_GAP_SEARCH_INQ_CMPL_EVT
				//
				// Event that indicates that the duration allowed for the search has completed or that we have been
				// asked to stop.
				case ESP_BT_GAP_DISCOVERY_STOPPED: {
					m_stopped = true;
					if (m_scanCompleteCB != nullptr) {
						m_scanCompleteCB(m_scanResults);
					}
					m_semaphoreScanEnd.give();
					break;
				} // ESP_BT_GAP_DISC_STATE_CHANGED_EVT
				case ESP_BT_GAP_DISCOVERY_STARTED: {
					 log_i("Discovery started.");
					break;
				}
				default: {
					break;
				}
			} // switch - search_evt
			break;
		} // ESP_GAP_BLE_SCAN_RESULT_EVT
		default: {
			break;
		} // default
	} // End switch
} // gapEventHandler

void BTScan::setAdvertisedDeviceCallbacks(BTAdvertisedDeviceCallbacks* pAdvertisedDeviceCallbacks, bool wantDuplicates) {
	m_wantDuplicates = wantDuplicates;
	m_pAdvertisedDeviceCallbacks = pAdvertisedDeviceCallbacks;
} // setAdvertisedDeviceCallbacks


bool BTScan::stop_bt() {
    if (btStarted()){
        esp_bluedroid_disable();
        esp_bluedroid_deinit();
        btStop();
    }
}


bool BTScan::start(uint32_t duration, void (*scanCompleteCB)(BTScanResults)) {
	log_d(">> start(duration=%d)", duration);

	m_semaphoreScanEnd.take(std::string("start"));
	m_scanCompleteCB = scanCompleteCB;                  // Save the callback to be invoked when the scan completes.

	m_scanResults.m_vectorAdvertisedDevices.clear();

    m_stopped = false;

    /* set discoverable and connectable mode, wait to be connected */
    esp_bt_gap_set_scan_mode(ESP_BT_SCAN_MODE_CONNECTABLE_DISCOVERABLE);

    /* start to discover nearby Bluetooth devices */
    log_i("start to discover nearby Bluetooth devices");
    esp_bt_gap_start_discovery(ESP_BT_INQ_MODE_GENERAL_INQUIRY, 10, 0);

	log_d("<< start()");
	return true;
} // start


/**
 * @brief Start scanning and block until scanning has been completed.
 * @param [in] duration The duration in seconds for which to scan.
 * @return The BLEScanResults.
 */
BTScanResults BTScan::start(uint32_t duration) {
	if(start(duration, nullptr)) {
		m_semaphoreScanEnd.wait("start");   // Wait for the semaphore to release.
	}
	return m_scanResults;
} // start


/**
 * @brief Stop an in progress scan.
 * @return N/A.
 */
void BTScan::stop() {
	log_d(">> stop()");

    esp_bt_gap_cancel_discovery();

	m_stopped = true;

	m_semaphoreScanEnd.give();

	log_d("<< stop()");
} // stop


/**
 * @brief Dump the scan results to the log.
 */
void BTScanResults::dump() {
	log_d(">> Dump scan results:");
	for (int i=0; i<getCount(); i++) {
		log_d("- %s", getDevice(i).toString().c_str());
	}
} // dump


/**
 * @brief Return the count of devices found in the last scan.
 * @return The number of devices found in the last scan.
 */
int BTScanResults::getCount() {
	return m_vectorAdvertisedDevices.size();
} // getCount


/**
 * @brief Return the specified device at the given index.
 * The index should be between 0 and getCount()-1.
 * @param [in] i The index of the device.
 * @return The device at the specified index.
 */
BTAdvertisedDevice BTScanResults::getDevice(uint32_t i) {
	return m_vectorAdvertisedDevices.at(i);
}


#endif
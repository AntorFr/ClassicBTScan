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
 * BLE.cpp
 *
 *  Created on: Mar 16, 2017
 *      Author: kolban
 */
#include "sdkconfig.h"
#if defined(CONFIG_BT_ENABLED)
#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <freertos/task.h>
#include <esp_err.h>
#include <nvs_flash.h>
#include <esp_bt.h>            // ESP32 BLE
#include <esp_bt_device.h>     // ESP32 BLE
#include <esp_bt_main.h>       // ESP32 BLE
#include <esp_gap_bt_api.h>
#include <esp_gatts_api.h>     // ESP32 BLE
#include <esp_gattc_api.h>     // ESP32 BLE
#include <esp_gatt_common_api.h>// ESP32 BLE
#include <esp_err.h>           // ESP32 ESP-IDF
#include <esp_log.h>           // ESP32 ESP-IDF
#include <map>                 // Part of C++ Standard library
#include <sstream>             // Part of C++ Standard library
#include <iomanip>             // Part of C++ Standard library

#include "BTDevice.h"
#include "GeneralUtils.h"
#ifdef ARDUINO_ARCH_ESP32
#include "esp32-hal-log.h"
#include "esp32-hal-bt.h"
#endif


/**
 * Singletons for the BLEDevice.
 */
//BLEServer* BLEDevice::m_pServer = nullptr;
BTScan*   BTDevice::m_pScan   = nullptr;
//BLEClient* BLEDevice::m_pClient = nullptr;
bool       BT_initialized          = false;   // Have we been initialized?
//esp_ble_sec_act_t 	BLEDevice::m_securityLevel = (esp_ble_sec_act_t)0;
//BLESecurityCallbacks* BLEDevice::m_securityCallbacks = nullptr;
//uint16_t   BLEDevice::m_localMTU = 23;

/**
 * @brief Create a new instance of a client.
 * @return A new instance of the client.
 */
/* STATIC */ /* BLEClient* BLEDevice::createClient() {
	log_d(">> createClient");
#ifndef CONFIG_GATTC_ENABLE  // Check that BLE GATTC is enabled in make menuconfig
	log_e("BLE GATTC is not enabled - CONFIG_GATTC_ENABLE not defined");
	abort();
#endif  // CONFIG_GATTC_ENABLE
	m_pClient = new BLEClient();
	log_d("<< createClient");
	return m_pClient;
} // createClient
*/


/**
 * @brief Create a new instance of a server.
 * @return A new instance of the server.
 
/* STATIC */ /* BLEServer* BLEDevice::createServer() {
	log_d(">> createServer");
#ifndef CONFIG_GATTS_ENABLE  // Check that BLE GATTS is enabled in make menuconfig
	log_e("BLE GATTS is not enabled - CONFIG_GATTS_ENABLE not defined");
	abort();
#endif // CONFIG_GATTS_ENABLE
	m_pServer = new BLEServer();
	m_pServer->createApp(0);
	log_d("<< createServer");
	return m_pServer;
} // createServer

*/

/**
 * @brief Handle GATT server events.
 *
 * @param [in] event The event that has been newly received.
 * @param [in] gatts_if The connection to the GATT interface.
 * @param [in] param Parameters for the event.
 */
/* STATIC */ /* void BLEDevice::gattServerEventHandler(
   esp_gatts_cb_event_t      event,
   esp_gatt_if_t             gatts_if,
   esp_ble_gatts_cb_param_t* param
) {
	log_d("gattServerEventHandler [esp_gatt_if: %d] ... %s",
		gatts_if,
		BLEUtils::gattServerEventTypeToString(event).c_str());

	BLEUtils::dumpGattServerEvent(event, gatts_if, param);

	switch(event) {
		case ESP_GATTS_CONNECT_EVT: {
			BLEDevice::m_localMTU = 23;
#ifdef CONFIG_BLE_SMP_ENABLE   // Check that BLE SMP (security) is configured in make menuconfig
			if(BLEDevice::m_securityLevel){
				esp_ble_set_encryption(param->connect.remote_bda, BLEDevice::m_securityLevel);
			}
#endif	// CONFIG_BLE_SMP_ENABLE
			break;
		} // ESP_GATTS_CONNECT_EVT

		case ESP_GATTS_MTU_EVT: {
			BLEDevice::m_localMTU = param->mtu.mtu;
	        ESP_LOGI(LOG_TAG, "ESP_GATTS_MTU_EVT, MTU %d", BLEDevice::m_localMTU);
	        break;
		}
		default: {
			break;
		}
	} // switch


	if (BLEDevice::m_pServer != nullptr) {
		BLEDevice::m_pServer->handleGATTServerEvent(event, gatts_if, param);
	}
} // gattServerEventHandler
 */

/**
 * @brief Handle GATT client events.
 *
 * Handler for the GATT client events.
 *
 * @param [in] event
 * @param [in] gattc_if
 * @param [in] param
 */
/* STATIC */ /* void BLEDevice::gattClientEventHandler(
	esp_gattc_cb_event_t      event,
	esp_gatt_if_t             gattc_if,
	esp_ble_gattc_cb_param_t* param) {

	log_d("gattClientEventHandler [esp_gatt_if: %d] ... %s",
		gattc_if, BLEUtils::gattClientEventTypeToString(event).c_str());
	BLEUtils::dumpGattClientEvent(event, gattc_if, param);

	switch(event) {
		case ESP_GATTC_CONNECT_EVT: {
			if(BLEDevice::getMTU() != 23){
				esp_err_t errRc = esp_ble_gattc_send_mtu_req(gattc_if, param->connect.conn_id);
				if (errRc != ESP_OK) {
					log_e("esp_ble_gattc_send_mtu_req: rc=%d %s", errRc, GeneralUtils::errorToString(errRc));
				}
			}
#ifdef CONFIG_BLE_SMP_ENABLE   // Check that BLE SMP (security) is configured in make menuconfig
			if(BLEDevice::m_securityLevel){
				esp_ble_set_encryption(param->connect.remote_bda, BLEDevice::m_securityLevel);
			}
#endif	// CONFIG_BLE_SMP_ENABLE
			break;
		} // ESP_GATTC_CONNECT_EVT

		default: {
			break;
		}
	} // switch


	// If we have a client registered, call it.
	if (BLEDevice::m_pClient != nullptr) {
		BLEDevice::m_pClient->gattClientEventHandler(event, gattc_if, param);
	}

} // gattClientEventHandler

*/

/**
 * @brief Handle GAP events.
 */
/* STATIC */ void BTDevice::gapEventHandler(
	esp_bt_gap_cb_event_t event,
	esp_bt_gap_cb_param_t *param) {

    /*

	if (BLEDevice::m_pServer != nullptr) {
		BLEDevice::m_pServer->handleGAPEvent(event, param);
	}

	if (BLEDevice::m_pClient != nullptr) {
		BLEDevice::m_pClient->handleGAPEvent(event, param);
	}
    */

	if (BTDevice::m_pScan != nullptr) {
		BTDevice::getScan()->handleGAPEvent(event, param);
	}

	/*
	 * Security events:
	 */


} // gapEventHandler


/**
 * @brief Get the BLE device address.
 * @return The BLE device address.
 */
/* STATIC*/ BTAddress BTDevice::getAddress() {
	const uint8_t* bdAddr = esp_bt_dev_get_address();
	esp_bd_addr_t addr;
	memcpy(addr, bdAddr, sizeof(addr));
	return BTAddress(addr);
} // getAddress


/**
 * @brief Retrieve the Scan object that we use for scanning.
 * @return The scanning object reference.  This is a singleton object.  The caller should not
 * try and release/delete it.
 */
/* STATIC */ BTScan* BTDevice::getScan() {
	//log_d(">> getScan");
	if (m_pScan == nullptr) {
		m_pScan = new BTScan();
		//log_d(" - creating a new scan object");
	}
	//log_d("<< getScan: Returning object at 0x%x", (uint32_t)m_pScan);
	return m_pScan;
} // getScan


/**
 * @brief Get the value of a characteristic of a service on a remote device.
 * @param [in] bdAddress
 * @param [in] serviceUUID
 * @param [in] characteristicUUID
 */
/* STATIC */ /* std::string BLEDevice::getValue(BLEAddress bdAddress, BLEUUID serviceUUID, BLEUUID characteristicUUID) {
	log_d(">> getValue: bdAddress: %s, serviceUUID: %s, characteristicUUID: %s", bdAddress.toString().c_str(), serviceUUID.toString().c_str(), characteristicUUID.toString().c_str());
	BLEClient *pClient = createClient();
	pClient->connect(bdAddress);
	std::string ret = pClient->getValue(serviceUUID, characteristicUUID);
	pClient->disconnect();
	log_d("<< getValue");
	return ret;
} // getValue

*/

/**
 * @brief Initialize the %BLE environment.
 * @param deviceName The device name of the device.
 */
/* STATIC */ void BTDevice::init(std::string deviceName) {
	if(!BT_initialized){
		BT_initialized = true;   // Set the initialization flag to ensure we are only initialized once.

        esp_err_t errRc = ESP_OK;

        if (!btStarted() && !btStart()){
            log_e("initialize controller failed");
        }
        
        esp_bluedroid_status_t bt_state = esp_bluedroid_get_status();
        if (bt_state == ESP_BLUEDROID_STATUS_UNINITIALIZED){
            errRc = esp_bluedroid_init();
            if (errRc != ESP_OK) {
                log_e("esp_bluedroid_init: rc=%d %s", errRc, GeneralUtils::errorToString(errRc));
                return;
            }
        }
        
        if (bt_state != ESP_BLUEDROID_STATUS_ENABLED){
            errRc = esp_bluedroid_enable();
            if (errRc != ESP_OK) {
                log_e("esp_bluedroid_enable: rc=%d %s", errRc, GeneralUtils::errorToString(errRc));
                return;
            }
        }

        errRc = esp_bt_gap_register_callback(BTDevice::gapEventHandler);
        if (errRc != ESP_OK) {
            log_e("esp_ble_gap_register_callback: rc=%d %s", errRc, GeneralUtils::errorToString(errRc));
            return;
        }

        errRc = ::esp_bt_dev_set_device_name(deviceName.c_str());
        if (errRc != ESP_OK) {
            log_e("esp_ble_gap_set_device_name: rc=%d %s", errRc, GeneralUtils::errorToString(errRc));
            return;
        };
    }
	vTaskDelay(200/portTICK_PERIOD_MS); // Delay for 200 msecs as a workaround to an apparent Arduino environment issue.
} // init


/**
 * @brief Set the transmission power.
 * The power level can be one of:
 * * ESP_PWR_LVL_N14
 * * ESP_PWR_LVL_N11
 * * ESP_PWR_LVL_N8
 * * ESP_PWR_LVL_N5
 * * ESP_PWR_LVL_N2
 * * ESP_PWR_LVL_P1
 * * ESP_PWR_LVL_P4
 * * ESP_PWR_LVL_P7
 * @param [in] powerLevel.
 */
/* STATIC */ /* void BLEDevice::setPower(esp_power_level_t powerLevel) {
	log_d(">> setPower: %d", powerLevel);
	esp_err_t errRc = ::esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_DEFAULT, powerLevel);
	if (errRc != ESP_OK) {
		log_e("esp_ble_tx_power_set: rc=%d %s", errRc, GeneralUtils::errorToString(errRc));
	};
	log_d("<< setPower");
} // setPower */


/**
 * @brief Set the value of a characteristic of a service on a remote device.
 * @param [in] bdAddress
 * @param [in] serviceUUID
 * @param [in] characteristicUUID
 */
/* STATIC */ /*void BLEDevice::setValue(BLEAddress bdAddress, BLEUUID serviceUUID, BLEUUID characteristicUUID, std::string value) {
	log_d(">> setValue: bdAddress: %s, serviceUUID: %s, characteristicUUID: %s", bdAddress.toString().c_str(), serviceUUID.toString().c_str(), characteristicUUID.toString().c_str());
	BLEClient *pClient = createClient();
	pClient->connect(bdAddress);
	pClient->setValue(serviceUUID, characteristicUUID, value);
	pClient->disconnect();
} // setValue */


/**
 * @brief Return a string representation of the nature of this device.
 * @return A string representation of the nature of this device.
 */
/* STATIC */ std::string BTDevice::toString() {
	std::ostringstream oss;
	oss << "BD Address: " << getAddress().toString();
	return oss.str();
} // toString


/**
 * @brief Add an entry to the BLE white list.
 * @param [in] address The address to add to the white list.
 */
void BTDevice::whiteListAdd(BTAddress address) {
    /*
	log_d(">> whiteListAdd: %s", address.toString().c_str());
	esp_err_t errRc = esp_ble_gap_update_whitelist(true, *address.getNative());  // True to add an entry.
	if (errRc != ESP_OK) {
		log_e("esp_ble_gap_update_whitelist: rc=%d %s", errRc, GeneralUtils::errorToString(errRc));
	}
	log_d("<< whiteListAdd");
    */
} // whiteListAdd


/**
 * @brief Remove an entry from the BLE white list.
 * @param [in] address The address to remove from the white list.
 */
void BTDevice::whiteListRemove(BTAddress address) {
    /*
	log_d(">> whiteListRemove: %s", address.toString().c_str());
	esp_err_t errRc = esp_ble_gap_update_whitelist(false, *address.getNative());  // False to remove an entry.
	if (errRc != ESP_OK) {
		log_e("esp_ble_gap_update_whitelist: rc=%d %s", errRc, GeneralUtils::errorToString(errRc));
	}
	log_d("<< whiteListRemove");
    */
} // whiteListRemove

/*
 * @brief Set encryption level that will be negotiated with peer device durng connection
 * @param [in] level Requested encryption level
 */
/*
void BLEDevice::setEncryptionLevel(esp_ble_sec_act_t level) {
	BLEDevice::m_securityLevel = level;
}
*/

/*
 * @brief Set callbacks that will be used to handle encryption negotiation events and authentication events
 * @param [in] cllbacks Pointer to BLESecurityCallbacks class callback
 */
/*
void BLEDevice::setSecurityCallbacks(BLESecurityCallbacks* callbacks) {
	BLEDevice::m_securityCallbacks = callbacks;
}
*/

/*
 * @brief Setup local mtu that will be used to negotiate mtu during request from client peer
 * @param [in] mtu Value to set local mtu, should be larger than 23 and lower or equal to 517
 */
/*
esp_err_t BLEDevice::setMTU(uint16_t mtu) {
	log_d(">> setLocalMTU: %d", mtu);
	esp_err_t err = esp_ble_gatt_set_local_mtu(mtu);
	if(err == ESP_OK){
		m_localMTU = mtu;
	} else {
		log_e("can't set local mtu value: %d", mtu);
	}
	log_d("<< setLocalMTU");
	return err;
} */

/*
 * @brief Get local MTU value set during mtu request or default value
 */ /*
uint16_t BLEDevice::getMTU() {
	return m_localMTU;
} */

bool BTDevice::getInitialized() {
	return BT_initialized;
}
#endif // CONFIG_BT_ENABLED
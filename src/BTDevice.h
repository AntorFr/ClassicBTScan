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

#ifndef MAIN_BT_Device_H_
#define MAIN_BT_Device_H_
#include "sdkconfig.h"
#if defined(CONFIG_BT_ENABLED)

#include <map>               // Part of C++ STL
#include <string>
#include <esp_bt.h>
#include "esp_bt_main.h"

#include "esp_gap_bt_api.h"


#include "BTAddress.h"
#include "BTAdvertisedDevice.h"
#include "BTScan.h"

class BTAdvertisedDevice;
class BTAdvertisedDeviceCallbacks;
class BTScan;


/**
 * @brief %BLE functions.
 */
class BTDevice {
public:

//	static BLEClient*  createClient();    // Create a new BLE client.
//	static BLEServer*  createServer();    // Cretae a new BLE server.
	static BTAddress    getAddress();      // Retrieve our own local BD address.
	static BTScan*      getScan();         // Get the scan object
//	static std::string getValue(BLEAddress bdAddress, BLEUUID serviceUUID, BLEUUID characteristicUUID);	  // Get the value of a characteristic of a service on a server.
	static void        init(std::string deviceName);   // Initialize the local BLE environment.
//	static void        setPower(esp_power_level_t powerLevel);  // Set our power level.
//	static void        setValue(BLEAddress bdAddress, BLEUUID serviceUUID, BLEUUID characteristicUUID, std::string value);   // Set the value of a characteristic on a service on a server.
	static std::string toString();        // Return a string representation of our device.
	static void        whiteListAdd(BTAddress address);    // Add an entry to the BLE white list.
	static void        whiteListRemove(BTAddress address); // Remove an entry from the BLE white list.
//	static void		   setEncryptionLevel(esp_ble_sec_act_t level);
//	static void		   setSecurityCallbacks(BLESecurityCallbacks* pCallbacks);
//	static esp_err_t   setMTU(uint16_t mtu);
//	static uint16_t	   getMTU();
	static bool        getInitialized(); // Returns the state of the device, is it initialized or not?

private:
//	static BLEServer *m_pServer;
	static BTScan   *m_pScan;
//	static BLEClient *m_pClient;
//	static esp_ble_sec_act_t 	m_securityLevel;
//	static BLESecurityCallbacks* m_securityCallbacks;
//	static uint16_t		m_localMTU;

//	static esp_gatt_if_t getGattcIF();

/*
	static void gattClientEventHandler(
		esp_gattc_cb_event_t      event,
		esp_gatt_if_t             gattc_if,
		esp_ble_gattc_cb_param_t* param);

	static void gattServerEventHandler(
	   esp_gatts_cb_event_t      event,
	   esp_gatt_if_t             gatts_if,
	   esp_ble_gatts_cb_param_t* param);

*/
	static void gapEventHandler(
		esp_bt_gap_cb_event_t  event,
		esp_bt_gap_cb_param_t* param);


}; // class BLE

#endif // CONFIG_BT_ENABLED
#endif /* MAIN_BT_Device_H_ */
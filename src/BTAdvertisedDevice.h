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

#ifndef _BT_ADVERTISED_DEVICE_H_
#define _BT_ADVERTISED_DEVICE_H_

#include "sdkconfig.h"
#if defined(CONFIG_BT_ENABLED)
#include "esp_gap_bt_api.h"

#include <map>

#include "BTScan.h"
#include "BTAddress.h"

class BTAdvertisedDevice;
class BTAdvertisedDeviceCallbacks;
class BTClient;
class BTScan;
/**
 * @brief A representation of a %BLE advertised device found by a scan.
 *
 * When we perform a %BLE scan, the result will be a set of devices that are advertising.  This
 * class provides a model of a detected device.
 */
class BTAdvertisedDevice {
public:
	BTAdvertisedDevice();

	BTAddress  getAddress();
	uint16_t    getAppearance();
	std::string getManufacturerData();
	std::string getName();
	uint32_t 	getCod();
	std::string getServiceType();
	std::string getDeviceType();
	int         getRSSI();
	BTScan*     getScan();
	std::string getServiceData();
	//BTUUID      getServiceDataUUID();
	//BTUUID      getServiceUUID();
	int8_t      getTXPower();


	//bool		isAdvertisingService(BLEUUID uuid);
	bool        haveAppearance();
	bool        haveManufacturerData();
	bool        haveName();
	bool        haveRSSI();
	bool		haveCod();
	bool        haveServiceData();
	bool        haveServiceUUID();
	bool        haveTXPower();

	std::string toString();

private:
	friend class BTScan;
	void parseAdvertisement(esp_bt_gap_cb_param_t::disc_res_param*  disc_res);
	void setAddress(BTAddress address);
	void setAdFlag(uint8_t adFlag);
	void setAppearance(uint16_t appearance);
	//void setManufacturerData(std::string manufacturerData);
	void setName(std::string name);
	void setRSSI(int8_t rssi);
	void setScan(BTScan* pScan);
	void setServiceData(std::string data);
	void setTXPower(int8_t txPower);
	void setCod(uint32_t cod);

	bool get_name_from_eir(uint8_t *eir, uint8_t *bdname, uint8_t *bdname_len);

	std::string deviceType(uint32_t major_cod);
	std::string serviceType(uint32_t service_cod);


	bool m_haveAppearance;
	bool m_haveManufacturerData;
	bool m_haveName;
	bool m_haveRSSI;
	bool m_haveServiceData;
	bool m_haveServiceUUID;
	bool m_haveTXPower;
	bool m_haveCod;


	BTAddress   m_address = BTAddress((uint8_t*)"\0\0\0\0\0\0");
	uint8_t     m_adFlag;
	uint16_t    m_appearance;
	std::string m_manufacturerData;
	std::string m_name;
	BTScan*     m_pScan;
	uint8_t 	m_bdname_len;
    uint8_t 	m_eir_len;
    int 		m_rssi;
    uint32_t 	m_cod;
    uint8_t 	m_eir[ESP_BT_GAP_EIR_DATA_LEN];
    uint8_t 	m_bdname[ESP_BT_GAP_MAX_BDNAME_LEN + 1];
	int8_t      m_txPower;
	std::string m_serviceData;
	std::string m_deviceType;
	std::string m_serviceType;

};

/**
 * @brief A callback handler for callbacks associated device scanning.
 *
 * When we are performing a scan as a %BLE client, we may wish to know when a new device that is advertising
 * has been found.  This class can be sub-classed and registered such that when a scan is performed and
 * a new advertised device has been found, we will be called back to be notified.
 */
class BTAdvertisedDeviceCallbacks {
public:
	virtual ~BTAdvertisedDeviceCallbacks() {}
	/**
	 * @brief Called when a new scan result is detected.
	 *
	 * As we are scanning, we will find new devices.  When found, this call back is invoked with a reference to the
	 * device that was found.  During any individual scan, a device will only be detected one time.
	 */
	virtual void onResult(BTAdvertisedDevice advertisedDevice) = 0;
};

#endif /* CONFIG_BT_ENABLED */
#endif /* _BT_ADVERTISED_DEVICE_H_ */
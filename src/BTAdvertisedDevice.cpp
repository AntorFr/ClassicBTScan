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

/*
 * BLEAdvertisedDevice.cpp
 *
 * During the scanning procedure, we will be finding advertised BLE devices.  This class
 * models a found device.
 *
 *
 * See also:
 * https://www.bluetooth.com/specifications/assigned-numbers/generic-access-profile
 *
 *  Created on: Jul 3, 2017
 *      Author: kolban
 */
#include "sdkconfig.h"
#if defined(CONFIG_BT_ENABLED)
#include <esp_log.h>

#include <string>
#include <sstream>
#include <iomanip>
#include <string.h>
#include <stdio.h>


#include "BTAdvertisedDevice.h"
//#include "BTUtils.h"
#ifdef ARDUINO_ARCH_ESP32
#include "esp32-hal-log.h"
#endif




BTAdvertisedDevice::BTAdvertisedDevice() {
	m_adFlag           = 0;
	m_manufacturerData = "";
	m_name             = "";
	m_rssi             = -9999;
	m_serviceData      = "";
	m_deviceType	   = "";
	m_serviceType	   = "";
	m_txPower          = 0;
	m_pScan            = nullptr;

	m_haveManufacturerData = false;
	m_haveName             = false;
	m_haveRSSI             = false;
	m_haveServiceData      = false;
	m_haveServiceUUID      = false;
	m_haveTXPower          = false;
	m_haveCod			   = false;

} // BLEAdvertisedDevice


/**
 * @brief Get the address.
 *
 * Every %BLE device exposes an address that is used to identify it and subsequently connect to it.
 * Call this function to obtain the address of the advertised device.
 *
 * @return The address of the advertised device.
 */
BTAddress BTAdvertisedDevice::getAddress() {
	return m_address;
} // getAddress


/**
 * @brief Get the manufacturer data.
 * @return The manufacturer data of the advertised device.
 */
std::string BTAdvertisedDevice::getManufacturerData() {
	return m_manufacturerData;
} // getManufacturerData


/**
 * @brief Get the name.
 * @return The name of the advertised device.
 */
std::string BTAdvertisedDevice::getName() {
	return m_name;
} // getName


/**
 * @brief Get the RSSI.
 * @return The RSSI of the advertised device.
 */
int BTAdvertisedDevice::getRSSI() {
	return m_rssi;
} // getRSSI


/**
 * @brief Get the scan object that created this advertisement.
 * @return The scan object.
 */
BTScan* BTAdvertisedDevice::getScan() {
	return m_pScan;
} // getScan


/**
 * @brief Get the service data.
 * @return The ServiceData of the advertised device.
 */
std::string BTAdvertisedDevice::getServiceData() {
	return m_serviceData;
} //getServiceData


/**
 * @brief Get the service data UUID.
 * @return The service data UUID.
*/
BTUUID BTAdvertisedDevice::getServiceDataUUID() {
	return m_serviceDataUUID;
} // getServiceDataUUID




/**
 * @brief Check advertised serviced for existence required UUID
 * @return Return true if service is advertised
 */
bool BTAdvertisedDevice::isAdvertisingService(BTUUID uuid){
	for (int i = 0; i < m_serviceUUIDs.size(); ++i) {
		if(m_serviceUUIDs[i].equals(uuid))
			return true;
	}
	return false;
}

/**
 * @brief Get the TX Power.
 * @return The TX Power of the advertised device.
 */
int8_t BTAdvertisedDevice::getTXPower() {
	return m_txPower;
} // getTXPower

uint32_t BTAdvertisedDevice::getCod() {
	return m_cod;
} //getCod


std::string BTAdvertisedDevice::getServiceType(){
	return m_serviceType;
}

std::string BTAdvertisedDevice::getDeviceType(){
	return m_deviceType;
}

/**
 * @brief Does this advertisement have manufacturer data?
 * @return True if there is manufacturer data present.
 */
bool BTAdvertisedDevice::haveManufacturerData() {
	return m_haveManufacturerData;
} // haveManufacturerData


/**
 * @brief Does this advertisement have a name value?
 * @return True if there is a name value present.
 */
bool BTAdvertisedDevice::haveName() {
	return m_haveName;
} // haveName


/**
 * @brief Does this advertisement have a signal strength value?
 * @return True if there is a signal strength value present.
 */
bool BTAdvertisedDevice::haveRSSI() {
	return m_haveRSSI;
} // haveRSSI


/**
 * @brief Does this advertisement have a service data value?
 * @return True if there is a service data value present.
 */
bool BTAdvertisedDevice::haveServiceData() {
	return m_haveServiceData;
} // haveServiceData


/**
 * @brief Does this advertisement have a service UUID value?
 * @return True if there is a service UUID value present.
 */
bool BTAdvertisedDevice::haveServiceUUID() {
	return m_haveServiceUUID;
} // haveServiceUUID


/**
 * @brief Does this advertisement have a transmission power value?
 * @return True if there is a transmission power value present.
 */
bool BTAdvertisedDevice::haveTXPower() {
	return m_haveTXPower;
} // haveTXPower

bool BTAdvertisedDevice::haveCod() {
	return m_haveCod;
} // haveCod


/**
 * @brief Parse the advertising pay load.
 *
 * The pay load is a buffer of bytes that is either 31 bytes long or terminated by
 * a 0 length value.  Each entry in the buffer has the format:
 * [length][type][data...]
 *
 * The length does not include itself but does include everything after it until the next record.  A record
 * with a length value of 0 indicates a terminator.
 *
 * https://www.bluetooth.com/specifications/assigned-numbers/generic-access-profile
 */
void BTAdvertisedDevice::parseDiscResult(esp_bt_gap_cb_param_t::disc_res_param* disc_res) {

    esp_bt_gap_dev_prop_t *p;

	uint8_t bdname_len = 0;
	uint8_t eir_len = 0;
	uint32_t cod = 0;
	int32_t rssi = -129;
	uint8_t eir[ESP_BT_GAP_EIR_DATA_LEN];
	uint8_t bdname[ESP_BT_GAP_MAX_BDNAME_LEN + 1];

	BTAddress advertisedAddress(disc_res->bda);
	log_i("Device adresse %s", advertisedAddress.toString().c_str());

    for (int i = 0; i < disc_res->num_prop; i++) {
        p = disc_res->prop + i;
        switch (p->type) {
			case ESP_BT_GAP_DEV_PROP_COD: {
				cod = *(uint32_t *)(p->val);
				setCod(cod);
				break;
			}
			case ESP_BT_GAP_DEV_PROP_RSSI:{
				rssi = *(int8_t *)(p->val);
				setRSSI(rssi);
				break;
			}
			case ESP_BT_GAP_DEV_PROP_BDNAME: {
				uint8_t bdname_len = (p->len > ESP_BT_GAP_MAX_BDNAME_LEN) ? ESP_BT_GAP_MAX_BDNAME_LEN :
							(uint8_t)p->len;
				memcpy(bdname, (uint8_t *)(p->val), bdname_len);
				bdname[bdname_len] = '\0';

				setName(std::string(reinterpret_cast<char*>(bdname)));
			}
			case ESP_BT_GAP_DEV_PROP_EIR: {
				memcpy(eir, (uint8_t *)(p->val), p->len);
				eir_len = p->len;
				
				//#ifndef WiFi_h //wait for Arduino IDF update to properly work with wifi > https://github.com/espressif/arduino-esp32/issues/1997
				parseEir(eir);
				//#endif
				

				break;
			}
			default:{
				break;
			}
        } // switch
    } //for

	

	if (eir && !haveName()) {
		log_v("Get name from eir");
    	if(get_name_from_eir(eir, bdname, &bdname_len))
			setName(std::string(reinterpret_cast<char*>(bdname)));
    }

	//log_i("Device name %s", p_dev.bdname);

	//std::string bdname((char *) p_dev.bdname);
	//log_i("Device name %s", bdname);


	




} // parseDiscResult

void BTAdvertisedDevice::parseEir(uint8_t* payload) {
	uint8_t length;
	uint8_t eir_type;
	uint8_t sizeConsumed = 0;
	bool finished = false;
	setPayload(payload);

	while(!finished) {
		length = *payload;          // Retrieve the length of the record.
		payload++;                  // Skip to type
		sizeConsumed += 1 + length; // increase the size consumed.

		if (length != 0) { // A length of 0 indicates that we have reached the end.
			eir_type = *payload;
			payload++;
			length--;

			char* pHex = BTUtils::buildHexData(nullptr, payload, length);
			log_d("Type: 0x%.2x (%s), length: %d, data: %s",
					eir_type, BTUtils::eirTypeToString(eir_type), length, pHex);
			free(pHex);

			switch(eir_type) {
				case ESP_BT_EIR_TYPE_SHORT_LOCAL_NAME: {   // eir Data Type: 0x08
					setName(std::string(reinterpret_cast<char*>(payload), length));
					break;
				} // ESP_BT_EIR_TYPE_SHORT_LOCAL_NAME

				case ESP_BT_EIR_TYPE_CMPL_LOCAL_NAME: {   // eir Data Type: 0x09
					setName(std::string(reinterpret_cast<char*>(payload), length));
					break;
				} // ESP_BT_EIR_TYPE_CMPL_LOCAL_NAME

				case ESP_BT_EIR_TYPE_TX_POWER_LEVEL: {      
					setTXPower(*payload);
					break;
				} // ESP_BLE_AD_TYPE_TX_PWR

				case ESP_BT_EIR_TYPE_INCMPL_16BITS_UUID :
				case ESP_BT_EIR_TYPE_CMPL_16BITS_UUID: {   
					for (int var = 0; var < length/2; ++var) {
						setServiceUUID(BTUUID(*reinterpret_cast<uint16_t*>(payload+var*2)));
					}
					break;
				} // ESP_BLE_AD_TYPE_16SRV_PART

				case ESP_BT_EIR_TYPE_INCMPL_32BITS_UUID:
				case ESP_BT_EIR_TYPE_CMPL_32BITS_UUID: {  
					for (int var = 0; var < length/4; ++var) {
						setServiceUUID(BTUUID(*reinterpret_cast<uint32_t*>(payload+var*4)));
					}
					break;
				} // ESP_BLE_AD_TYPE_32SRV_PART

				case ESP_BT_EIR_TYPE_INCMPL_128BITS_UUID: { // Adv Data Type: 0x07
					setServiceUUID(BTUUID(payload, 16, false));
					break;
				} // ESP_BT_EIR_TYPE_INCMPL_128BITS_UUID

				case ESP_BT_EIR_TYPE_CMPL_128BITS_UUID: { // Adv Data Type: 0x06
					setServiceUUID(BTUUID(payload, 16, false));
					break;
				} // ESP_BT_EIR_TYPE_CMPL_128BITS_UUID

				// See CSS Part A 1.4 Manufacturer Specific Data
				case ESP_BT_EIR_TYPE_MANU_SPECIFIC: {
					//setManufacturerData(std::string(reinterpret_cast<char*>(payload), length));
					break;
				} // ESP_BT_EIR_TYPE_MANU_SPECIFIC

				
				default: {
					log_d("Unhandled type: eirType: %d - 0x%.2x", eir_type, eir_type);
					break;
				}
			} // switch
			payload += length;
		} // Length <> 0


		if (sizeConsumed >=31 || length == 0) {
			finished = true;
		}
	} // !finished
} // parseAdvertisement

bool BTAdvertisedDevice::get_name_from_eir(uint8_t *eir, uint8_t *bdname, uint8_t *bdname_len)
{
    uint8_t *rmt_bdname = NULL;
    uint8_t rmt_bdname_len = 0;

    if (!eir) {
        return false;
    }

    rmt_bdname = esp_bt_gap_resolve_eir_data(eir, ESP_BT_EIR_TYPE_CMPL_LOCAL_NAME, &rmt_bdname_len);
    if (!rmt_bdname) {
        rmt_bdname = esp_bt_gap_resolve_eir_data(eir, ESP_BT_EIR_TYPE_SHORT_LOCAL_NAME, &rmt_bdname_len);
    }

    if (rmt_bdname) {
        if (rmt_bdname_len > ESP_BT_GAP_MAX_BDNAME_LEN) {
            rmt_bdname_len = ESP_BT_GAP_MAX_BDNAME_LEN;
        }

        if (bdname) {
            memcpy(bdname, rmt_bdname, rmt_bdname_len);
            bdname[rmt_bdname_len] = '\0';
        }
        if (bdname_len) {
            *bdname_len = rmt_bdname_len;
        }
        return true;
    }

    return false;
}


/**
 * @brief Set the address of the advertised device.
 * @param [in] address The address of the advertised device.
 */
void BTAdvertisedDevice::setAddress(BTAddress address) {
	m_address = address;
} // setAddress

/**
 * @brief Set the adFlag for this device.
 * @param [in] The discovered adFlag.
 */
void BTAdvertisedDevice::setAdFlag(uint8_t adFlag) {
	m_adFlag = adFlag;
} // setAdFlag


/**
 * @brief Set the manufacturer data for this device.
 * @param [in] The discovered manufacturer data.
 */
/*
void BTAdvertisedDevice::setManufacturerData(std::string manufacturerData) {
	m_manufacturerData     = manufacturerData;
	m_haveManufacturerData = true;
	char* pHex = BTUtils::buildHexData(nullptr, (uint8_t*)m_manufacturerData.data(), (uint8_t)m_manufacturerData.length());
	log_d("- manufacturer data: %s", pHex);
	free(pHex);
} // setManufacturerData
*/

/**
 * @brief Set the name for this device.
 * @param [in] name The discovered name.
 */
void BTAdvertisedDevice::setName(std::string name) {
	m_name     = name;
	m_haveName = true;
	log_d("- setName(): name: %s", m_name.c_str());
} // setName


/**
 * @brief Set the RSSI for this device.
 * @param [in] rssi The discovered RSSI.
 */
void BTAdvertisedDevice::setRSSI(int8_t rssi) {
	m_rssi     = rssi;
	m_haveRSSI = true;
	log_d("- setRSSI(): rssi: %d", m_rssi);
} // setRSSI


/**
 * @brief Set the Scan that created this advertised device.
 * @param pScan The Scan that created this advertised device.
 */
void BTAdvertisedDevice::setScan(BTScan* pScan) {
	m_pScan = pScan;
} // setScan


void BTAdvertisedDevice::setCod(uint32_t cod) {
	if(esp_bt_gap_is_valid_cod(cod)){
		m_cod = cod;
		m_haveCod = true;
		log_d("- setCod(): cod: %d", m_cod);
		
		m_deviceType = deviceType(esp_bt_gap_get_cod_major_dev(cod));
		log_d("- setCod(): deviceType: %s", m_deviceType.c_str());
		m_serviceType = serviceType(esp_bt_gap_get_cod_srvc(cod));
		log_d("- setCod(): serviceType: %s", m_serviceType.c_str());
	}

} // setScan


/**
 * @brief Set the Service UUID for this device.
 * @param [in] serviceUUID The discovered serviceUUID
 */
void BTAdvertisedDevice::setServiceUUID(const char* serviceUUID) {
	return setServiceUUID(BTUUID(serviceUUID));
} // setServiceUUID


/**
 * @brief Set the Service UUID for this device.
 * @param [in] serviceUUID The discovered serviceUUID
 */
void BTAdvertisedDevice::setServiceUUID(BTUUID serviceUUID) {
	m_serviceUUIDs.push_back(serviceUUID);
	m_haveServiceUUID = true;
	log_d("- addServiceUUID(): serviceUUID: %s", serviceUUID.toString().c_str());
} // setServiceUUID

/**
 * @brief Set the ServiceData value.
 * @param [in] data ServiceData value.
 */
void BTAdvertisedDevice::setServiceData(std::string serviceData) {
	m_haveServiceData = true;         // Set the flag that indicates we have service data.
	m_serviceData     = serviceData;  // Save the service data that we received.
} //setServiceData


/**
 * @brief Set the ServiceDataUUID value.
 * @param [in] data ServiceDataUUID value.

void BLEAdvertisedDevice::setServiceDataUUID(BTUUID uuid) {
	m_haveServiceData = true;         // Set the flag that indicates we have service data.
	m_serviceDataUUID = uuid;
} // setServiceDataUUID
*/

/**
 * @brief Set the power level for this device.
 * @param [in] txPower The discovered power level.
 */
void BTAdvertisedDevice::setTXPower(int8_t txPower) {
	m_txPower     = txPower;
	m_haveTXPower = true;
	log_d("- txPower: %d", m_txPower);
} // setTXPower

std::string BTAdvertisedDevice::deviceType(uint32_t major_cod){ // enum esp_bt_cod_major_dev_t
    switch (major_cod) {
        case ESP_BT_COD_MAJOR_DEV_MISC:         return std::string("Miscellaneous");
        case ESP_BT_COD_MAJOR_DEV_COMPUTER:     return std::string("Computer");
        case ESP_BT_COD_MAJOR_DEV_PHONE:        return std::string("Phone");
        case ESP_BT_COD_MAJOR_DEV_LAN_NAP:      return std::string("Network Access Point");
        case ESP_BT_COD_MAJOR_DEV_AV:           return std::string("Audio/Video");
        case ESP_BT_COD_MAJOR_DEV_PERIPHERAL:   return std::string("Peripheral");
        case ESP_BT_COD_MAJOR_DEV_IMAGING:      return std::string("Imaging");
        case ESP_BT_COD_MAJOR_DEV_WEARABLE:     return std::string("Wearable");
        case ESP_BT_COD_MAJOR_DEV_TOY:          return std::string("Toy");
        case ESP_BT_COD_MAJOR_DEV_HEALTH:       return std::string("Health");
        case ESP_BT_COD_MAJOR_DEV_UNCATEGORIZED:return std::string("Uncategorized");
        default:                                return std::string("Uncategorized");
    }
}
// TODO RETURN MULTIPLE RESULT
std::string BTAdvertisedDevice::serviceType(uint32_t service_cod){ // enum esp_bt_cod_srvc_t
    if (service_cod & ESP_BT_COD_SRVC_NONE)             return std::string("None indicates an invalid value");
    if (service_cod & ESP_BT_COD_SRVC_LMTD_DISCOVER)    return std::string("Limited Discoverable Mode");
    if ( service_cod & ESP_BT_COD_SRVC_POSITIONING)     return std::string("Positioning");
    if ( service_cod & ESP_BT_COD_SRVC_NETWORKING)      return std::string("Networking");
    if ( service_cod & ESP_BT_COD_SRVC_RENDERING)       return std::string("Rendering");
    if ( service_cod & ESP_BT_COD_SRVC_CAPTURING)       return std::string("Capturing");
    if ( service_cod & ESP_BT_COD_SRVC_AUDIO)           return std::string("Audio");
    if ( service_cod & ESP_BT_COD_SRVC_TELEPHONY)       return std::string("Telephony");
    if ( service_cod & ESP_BT_COD_SRVC_INFORMATION)     return std::string("Information");
    return std::string("undefined");
    
}


/**
 * @brief Create a string representation of this device.
 * @return A string representation of this device.
 */
std::string BTAdvertisedDevice::toString() {
	std::stringstream ss;
	ss << "Name: " << getName() << ", Address: " << getAddress().toString();
	if (haveManufacturerData()) {
		char *pHex = BTUtils::buildHexData(nullptr, (uint8_t*)getManufacturerData().data(), getManufacturerData().length());
		ss << ", manufacturer data: " << pHex;
		free(pHex);
	}
	if (haveServiceUUID()) {
		ss << ", serviceUUID: " << getServiceUUID().toString();
	}
	if (haveTXPower()) {
		ss << ", txPower: " << (int)getTXPower();
	}
	return ss.str();
} // toString

uint8_t* BTAdvertisedDevice::getPayload() {
	return m_payload;
}

void BTAdvertisedDevice::setPayload(uint8_t* payload) {
	m_payload = payload;
}



#endif /* CONFIG_BT_ENABLED */
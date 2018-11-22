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


#include "sdkconfig.h"
#if defined(CONFIG_BT_ENABLED)

#include <string>
#include <sstream>
#include <iomanip>
#include <string.h>
#include <stdio.h>
#ifdef ARDUINO_ARCH_ESP32
	#include "esp32-hal-log.h"
#endif

#include "BTAddress.h"



/**
 * @brief Create an address from the native ESP32 representation.
 * @param [in] address The native representation.
 */
BTAddress::BTAddress(esp_bd_addr_t address) {
	memcpy(m_address, address, ESP_BD_ADDR_LEN);
} // BTAddress

BTAddress::BTAddress(std::string stringAddress) {
	if (stringAddress.length() != 17) {
		return;
	}
	int data[6];
	sscanf(stringAddress.c_str(), "%x:%x:%x:%x:%x:%x", &data[0], &data[1], &data[2], &data[3], &data[4], &data[5]);
	m_address[0] = (uint8_t)data[0];
	m_address[1] = (uint8_t)data[1];
	m_address[2] = (uint8_t)data[2];
	m_address[3] = (uint8_t)data[3];
	m_address[4] = (uint8_t)data[4];
	m_address[5] = (uint8_t)data[5];
} // BTAddress


/**
 * @brief Determine if this address equals another.
 * @param [in] otherAddress The other address to compare against.
 * @return True if the addresses are equal.
 */
bool BTAddress::equals(BTAddress otherAddress) {
	return memcmp(otherAddress.getNative(), m_address, 6) == 0;
} // equals


/**
 * @brief Return the native representation of the address.
 * @return The native representation of the address.
 */
esp_bd_addr_t *BTAddress::getNative() {
	return &m_address;
} // getNative

std::string BTAddress::toString() {
	std::stringstream stream;
	stream << std::setfill('0') << std::setw(2) << std::hex << (int)((uint8_t *)(m_address))[0] << ':';
	stream << std::setfill('0') << std::setw(2) << std::hex << (int)((uint8_t *)(m_address))[1] << ':';
	stream << std::setfill('0') << std::setw(2) << std::hex << (int)((uint8_t *)(m_address))[2] << ':';
	stream << std::setfill('0') << std::setw(2) << std::hex << (int)((uint8_t *)(m_address))[3] << ':';
	stream << std::setfill('0') << std::setw(2) << std::hex << (int)((uint8_t *)(m_address))[4] << ':';
	stream << std::setfill('0') << std::setw(2) << std::hex << (int)((uint8_t *)(m_address))[5];
	return stream.str();
} // toString
#endif
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


#ifndef _BTUUID_H_
#define _BTUUID_H_
#include "sdkconfig.h"
#if defined(CONFIG_BT_ENABLED)
#include <esp_gatt_defs.h>
#include <string>

/**
 * @brief A model of a %BLE UUID.
 */
class BTUUID {
public:
	BTUUID(std::string uuid);
	BTUUID(uint16_t uuid);
	BTUUID(uint32_t uuid);
	BTUUID(esp_bt_uuid_t uuid);
	BTUUID(uint8_t* pData, size_t size, bool msbFirst);
	BTUUID(esp_gatt_id_t gattId);
	BTUUID();
	int            	bitSize();   // Get the number of bits in this uuid.
	bool           	equals(BTUUID uuid);
	esp_bt_uuid_t* 	getNative();
	BTUUID          to128();
	std::string    	toString();
	static BTUUID   fromString(std::string uuid);  // Create a BTUUID from a string

private:
	esp_bt_uuid_t m_uuid;       // The underlying UUID structure that this class wraps.
	bool          m_valueSet;   // Is there a value set for this instance.
}; // BTUUID
#endif /* CONFIG_BT_ENABLED */
#endif /* _BTUUID_H_ */
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
 * BLEAddress.h
 *
 *  Created on: Jul 2, 2017
 *      Author: kolban
 */

#ifndef _BT_ADDRESS_H_ 
#define _BT_ADDRESS_H_ 

#include "sdkconfig.h"
#if defined(CONFIG_BT_ENABLED)
#include "esp_bt_defs.h"
#include "esp_gap_bt_api.h"
#include <string>


class BTAddress {
	public:
		BTAddress(esp_bd_addr_t address);
		BTAddress(std::string stringAddress);
		bool           equals(BTAddress otherAddress);
		esp_bd_addr_t* getNative();
		std::string    toString();

	private:
		esp_bd_addr_t m_address;
};

#endif /* CONFIG_BT_ENABLED */
#endif /* _BT_ADDRESS_H_  */
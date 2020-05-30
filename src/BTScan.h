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

#ifndef _BT_SCAN_H_
#define _BT_SCAN_H_

#ifndef Scan_duration
  #define Scan_duration 10
#endif

#include "sdkconfig.h"

#if defined(CONFIG_BT_ENABLED) && defined(CONFIG_BLUEDROID_ENABLED)

//#include "Arduino.h"

#include "esp_bt.h"
#include "esp_bt_main.h"

#include "esp_gap_bt_api.h"

#include <vector>
#include "FreeRTOS.h"

#include "BTAddress.h"
#include "BTAdvertisedDevice.h"

class BTAdvertisedDevice;
class BTAdvertisedDeviceCallbacks;
class BTScan;


class BTScanResults {
public:
	void                dump();
	int                 getCount();
	BTAdvertisedDevice  getDevice(uint32_t i);

private:
	friend class BTScan;
	std::vector<BTAdvertisedDevice> m_vectorAdvertisedDevices;
};

class BTScan
{
  public:
    void           setAdvertisedDeviceCallbacks(
                      BTAdvertisedDeviceCallbacks* pAdvertisedDeviceCallbacks,
                      bool wantDuplicates = false);
    bool           start(uint32_t duration, void (*scanCompleteCB)(BTScanResults));
    BTScanResults  start(uint32_t duration);
    void           stop();
    BTScanResults getResults();
    void			clearResults();

  private:
    BTScan();
    ~BTScan(void);
    friend class BTDevice;
    void handleGAPEvent(esp_bt_gap_cb_event_t  event, esp_bt_gap_cb_param_t* param);
    //void parseAdvertisement(BLEClient* pRemoteDevice, uint8_t *payload);

    BTAdvertisedDeviceCallbacks*  m_pAdvertisedDeviceCallbacks;
    bool                          m_stopped;
    FreeRTOS::Semaphore           m_semaphoreScanEnd = FreeRTOS::Semaphore("ScanEnd");
    BTScanResults                 m_scanResults;
    bool                          m_wantDuplicates;
    void                        (*m_scanCompleteCB)(BTScanResults scanResults);
    bool                          stop_bt();


};

#endif
#endif
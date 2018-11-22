#include <BTDevice.h>
 #include <BTScan.h>


#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BTDevice BTDevice;

void setup() {
  Serial.begin(115200);
  
  BTDevice::init("");
  BTScan* pBLEScan = BTDevice::getScan(); //create new scan

  pBLEScan->start(Scan_duration);
  

  Serial.println("The device started, now you can pair it with bluetooth!");
}

void loop() {

  delay(10000);
}

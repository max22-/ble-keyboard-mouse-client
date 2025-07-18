#include "ble_hid_client.h"

void BLEHIDClientScanCallbacks::onResult(const NimBLEAdvertisedDevice* advertisedDevice) {
    BLE_HID_DEBUG("device found: %s (appearance = 0x%x)\n", advertisedDevice->toString().c_str(), advertisedDevice->getAppearance());
}

void BLEHIDClientScanCallbacks::onScanEnd(const NimBLEScanResults& results, int reason) {
    BLE_HID_DEBUG("scan end");
    this->results = &results;
}

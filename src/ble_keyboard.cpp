#include "ble_hid_client.h"

void BLEKeyboard::handle_report(uint8_t *report, size_t len) {
    Serial.print("keyboard : ");
    for(size_t i = 0; i < len; i++) {
        Serial.printf("0x%02x ", report[i]);
    }
    Serial.println();
}

bool BLEKeyboard::connect(const NimBLEAdvertisedDevice* advDevice) {
    if(!BLEHIDDevice::connect(advDevice))
        return false;
    NimBLERemoteCharacteristic *protocol_mode_chr = pSvc->getCharacteristic(NimBLEUUID("2a4e"));
    if(protocol_mode_chr) {
        if(protocol_mode_chr->writeValue<uint8_t>(0)) {
            BLE_HID_DEBUG("protocol mode set to 'boot'");
        }
        else {
            BLE_HID_DEBUG("failed to set protocol to 'boot'");
            return false;
        }
    } else {
        BLE_HID_DEBUG("protocol mode chararacteristic not found");
        return false;
    }
    return true;
}
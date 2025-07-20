#include "ble_keyboard_mouse_client.h"

void BLEMouse::handle_report(uint8_t *report, size_t len) {
    Serial.print("mouse : ");
    for(size_t i = 0; i < len; i++) {
        Serial.printf("0x%02x ", report[i]);
    }
    Serial.println();
}

bool BLEMouse::connect(const NimBLEAdvertisedDevice* advDevice) {
    if(!BLEHIDDevice::connect(advDevice))
        return false;
    NimBLERemoteCharacteristic *report_map_chr = pSvc->getCharacteristic(NimBLEUUID("2a4b"));
    if(report_map_chr) {
        BLE_HID_DEBUG("report map characteristic found");
        NimBLEAttValue val = report_map_chr->readValue();
        parse_report_descriptor(val.data(), val.length());
    } else {
        BLE_HID_DEBUG("report map characteristic not found");
    }
    return true;
}

void BLEMouse::parse_report_descriptor(const uint8_t *desc, size_t len) {
    // TODO
    Serial.println("Report map:");
    for(size_t i = 0; i < len; i++)
        Serial.printf("0x%02x ", desc[i]);
    Serial.println();
}
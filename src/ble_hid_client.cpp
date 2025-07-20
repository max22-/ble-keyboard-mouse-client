#include "ble_keyboard_mouse_client.h"

void BLEHIDClient::begin(const char *device_name, bool keyboard_enabled, bool mouse_enabled) {
    NimBLEDevice::init(device_name);
    NimBLEDevice::setSecurityAuth(true, true, true);
    NimBLEDevice::setSecurityPasskey(123456);
    NimBLEDevice::setSecurityIOCap(BLE_HS_IO_KEYBOARD_ONLY);
    this->keyboard_enabled = keyboard_enabled;
    this->mouse_enabled = mouse_enabled;
    pScan = NimBLEDevice::getScan();
    pScan->setScanCallbacks(&scan_callbacks, false);
    pScan->setInterval(100);
    pScan->setWindow(100);
    pScan->setActiveScan(true);
    if(this->keyboard_enabled || this->mouse_enabled)
        start_scan();
}

void BLEHIDClient::loop() {
    if(!(keyboard_enabled || mouse_enabled)) return;
    const NimBLEScanResults *results = scan_callbacks.results;
    scan_callbacks.results = nullptr;
    if(results && results->getCount() > 0) {
        BLE_HID_DEBUG("found devices:");
        for(auto device: *results) {
            BLE_HID_DEBUG("* address: %s", device->getAddress().toString().c_str());
            bool is_bonded = NimBLEDevice::isBonded(device->getAddress());
            if(is_bonded)
                BLE_HID_DEBUG("    the device is bonded");
            else
                BLE_HID_DEBUG("    the device is not bonded");
            if(device->isAdvertisingService(NimBLEUUID("1812")) || is_bonded) {
                uint16_t appearance = device->getAppearance();
                if(appearance == 0x3c1 && keyboard_enabled && !keyboard.is_connected()) {
                    BLE_HID_DEBUG("    type: keyboard");
                    keyboard.connect(device);
                } else if(appearance == 0x3c2 && mouse_enabled && !mouse.is_connected()) {
                    BLE_HID_DEBUG("    type: mouse");
                    mouse.connect(device);
                }
            }
        }
    } else if((keyboard_enabled && !keyboard.is_connected()) || (mouse_enabled && !mouse.is_connected())) {
        start_scan();
    }
}

void BLEHIDClient::start_scan() {
    if(last_scan == 0 || (millis() - last_scan >= BLE_HID_SCAN_PERIOD)) {
        BLE_HID_DEBUG("starting scan");
        last_scan = millis();
        pScan->start(BLE_HID_SCAN_DURATION);
    }
}

void BLEHIDClient::enable_keyboard() {
    keyboard_enabled = true;
}

void BLEHIDClient::disable_keyboard() {
    keyboard_enabled = false;
}

BLEKeyboard& BLEHIDClient::get_keyboard() {
    return keyboard;
}

void BLEHIDClient::enable_mouse() {
    mouse_enabled = true;
}

void BLEHIDClient::disable_mouse() {
    mouse_enabled = false;
}

BLEMouse& BLEHIDClient::get_mouse() {
    return mouse;
}


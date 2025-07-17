#ifndef BLE_KEYBOARD_CLIENT_H
#define BLE_KEYBOARD_CLIENT_H

#include <NimBLEDevice.h>

class BLEKeyboardClientScanCallbacks : public NimBLEScanCallbacks {
public:
    const NimBLEAdvertisedDevice* get_device();
private:
    void onResult(const NimBLEAdvertisedDevice* advertisedDevice) override;
    void onScanEnd(const NimBLEScanResults& results, int reason) override;
    const NimBLEAdvertisedDevice* device = nullptr;
};

class BLEKeyboardClient {
public:
    void begin(const char *device_name);
    bool start_scan(uint32_t duration);
    bool is_scanning();
    bool keyboard_found();
    const char *keyboard_name();
    bool connect();

protected:
    NimBLEScan *pScan = nullptr;
    BLEKeyboardClientScanCallbacks scan_callbacks;
    NimBLEClient *pClient = nullptr;
    NimBLERemoteService *pSvc = nullptr;
    NimBLERemoteCharacteristic *pChr = nullptr;
    NimBLERemoteDescriptor *pDsc = nullptr;
};

#endif
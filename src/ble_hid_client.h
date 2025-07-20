#ifndef BLE_HID_CLIENT_H
#define BLE_HID_CLIENT_H

#include <NimBLEDevice.h>

#define BLE_HID_SCAN_DURATION 5000
#define BLE_HID_SCAN_PERIOD 10000

class BLEHIDClient;

class BLEHIDClientScanCallbacks : public NimBLEScanCallbacks {
public:
    const NimBLEScanResults* results;
private:
    void onResult(const NimBLEAdvertisedDevice* advertisedDevice) override;
    void onScanEnd(const NimBLEScanResults& results, int reason) override;
};

class BLEHIDClientCallbacks : public NimBLEClientCallbacks {
public:
    bool connected = false;
private:
    void onConnect(NimBLEClient* pClient) override;
    void onDisconnect(NimBLEClient* pClient, int reason) override;
    void onAuthenticationComplete(NimBLEConnInfo& connInfo) override;
};


class BLEHIDDevice {
protected:
    virtual void handle_report(uint8_t *report, size_t len) = 0;
    virtual bool connect(const NimBLEAdvertisedDevice* advDevice);
    NimBLERemoteService* pSvc = nullptr;
private:
    bool is_connected();
    NimBLEAdvertisedDevice *device = nullptr;
    NimBLEClient *pClient = nullptr;
    bool enabled = false;
    friend class BLEHIDClient;
    BLEHIDClientCallbacks callbacks;
};

class BLEKeyboard : public BLEHIDDevice {
public:
    void on_key_pressed(std::function<void (uint8_t)>);
    void on_key_released(std::function<void (uint8_t)>);
    void debug();
private:
    void handle_report(uint8_t *report, size_t len) override;
    bool connect(const NimBLEAdvertisedDevice* advDevice) override;
    std::function<void (uint8_t)> key_pressed_callback;
    std::function<void (uint8_t)> key_released_callback;
    bool keys_states[256] = {false};
    friend class BLEHIDClient;
};

class BLEMouse : public BLEHIDDevice {
private:
    void handle_report(uint8_t *report, size_t len) override;
    bool connect(const NimBLEAdvertisedDevice* advDevice) override;
    void parse_report_descriptor(const uint8_t *desc, size_t len);
    friend class BLEHIDClient;
};

class BLEHIDClient {
public:
    void begin(const char *device_name, bool keyboard_enabled, bool mouse_enabled);
    void loop();

    void enable_keyboard();
    void disable_keyboard();
    BLEKeyboard& get_keyboard();

    void enable_mouse();
    void disable_mouse();
    BLEMouse& get_mouse();

private:
    NimBLEScan *pScan = nullptr;
    BLEHIDClientScanCallbacks scan_callbacks;
    void start_scan();
    unsigned long last_scan = 0;

    bool keyboard_enabled = false;
    BLEKeyboard keyboard;

    bool mouse_enabled = false;
    BLEMouse mouse;
};

#define BLE_HID_DEBUG(...) \
  do { \
    Serial.printf("%s:%d: ", __FILE__, __LINE__); \
    Serial.printf(__VA_ARGS__); \
    Serial.println(); \
  } while(0)

#endif
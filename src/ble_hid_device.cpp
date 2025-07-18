#include "ble_hid_client.h"

bool BLEHIDDevice::connect(const NimBLEAdvertisedDevice* advDevice) {
    NimBLERemoteService* pSvc = nullptr;
    bool characteristic_found = false;

    pClient = nullptr;

    if(NimBLEDevice::getCreatedClientCount()) {
        BLE_HID_DEBUG("we should reuse a client");
        pClient = NimBLEDevice::getClientByPeerAddress(advDevice->getAddress());
        if(pClient) {
            BLE_HID_DEBUG("we already know the client");
            if(!pClient->connect(advDevice, false)) {
                BLE_HID_DEBUG("failed to reconnect");
                return false;
            }
        } else {
            BLE_HID_DEBUG("reusing a disconnected client");
            pClient = NimBLEDevice::getDisconnectedClient();
        }
    }

    if(!pClient) {
        BLE_HID_DEBUG("no client to reuse, creating a new one");
        if(NimBLEDevice::getCreatedClientCount() >= NIMBLE_MAX_CONNECTIONS) {
            BLE_HID_DEBUG("too many clients");
            return false;
        }
        pClient = NimBLEDevice::createClient();
        BLE_HID_DEBUG("new client created");
        BLE_HID_DEBUG("pClient = 0x%x", pClient);
        pClient->setClientCallbacks(&callbacks, false);
        pClient->setConnectionParams(12, 12, 0, 150);
        pClient->setConnectTimeout(5000);
        if(!pClient->connect(advDevice)) {
            BLE_HID_DEBUG("failed to connect");
            BLE_HID_DEBUG("device = 0x%x", advDevice);
            goto cleanup1;
        }
    }
    
    if(!pClient->isConnected()) {
        if(!pClient->connect(advDevice)) {
            BLE_HID_DEBUG("failed to connect");
            goto cleanup1;
        }
    }

    // TODO: check if this is really needed
    if(!pClient->secureConnection()) {
        BLE_HID_DEBUG("failed to enable secure connection");
        goto cleanup2;
    } else {
        BLE_HID_DEBUG("secure connection enabled");
    }


    pSvc = pClient->getService("1812");
    if(!pSvc) {
        BLE_HID_DEBUG("service not found");
        goto cleanup2;
    }

    for (auto* chr : pSvc->getCharacteristics(true)) {
        if(chr->getUUID() == NimBLEUUID("2a4d")) {
            BLE_HID_DEBUG("found a 2a4d characteristic");
            if(chr->canNotify())
                BLE_HID_DEBUG("this characteristic can notify");
            else
                BLE_HID_DEBUG("this characteristic can't notify");
        }
        if(chr->getUUID() == NimBLEUUID("2a4d") && chr->canNotify()) {
            BLE_HID_DEBUG("subsribing to a 0x2a4d characteristic");
            if(!chr->subscribe(true, [this](NimBLERemoteCharacteristic* pRemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
                this->handle_report(pData, length);
            })) {
                BLE_HID_DEBUG("failed to subscribe");
            }
            characteristic_found = true;
        }
    }
    if(!characteristic_found) {
        BLE_HID_DEBUG("characteristic not found");
        goto cleanup2;
    }
    BLE_HID_DEBUG("advertised device address: %s", advDevice->getAddress().toString().c_str());
    BLE_HID_DEBUG("peer address: %s", pClient->getPeerAddress().toString().c_str());
    
    return true;

cleanup2:
    pClient->disconnect();
cleanup1:
    NimBLEDevice::deleteClient(pClient);
    pClient = nullptr;
    return false;
}

bool BLEHIDDevice::is_connected() {
    return callbacks.connected;
}
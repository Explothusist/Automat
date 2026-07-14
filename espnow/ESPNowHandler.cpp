#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_ESPNOW_

#include "ESPNowHandler.h"

#include <vector>

#ifdef AUTOMAT_ESP32_ARDUINO_
#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#endif
#ifdef AUTOMAT_ESP32_ESPIDF_
#endif

namespace atmt {

    ESPNowSystem::ESPNowSystem():
        m_has_init{ false },
        m_system_mac_address{ 0 },
        m_registered_handlers{ },
        m_all_registered_mac_addresses{ }
    {

    }
    
    int ESPNowSystem::InitESPNow() {
        if (ESPNowIsInitialized()) return ESP_OK;

#ifdef AUTOMAT_ESP32_ARDUINO_
        WiFi.mode(WIFI_MODE_STA);
        WiFi.disconnect();
        WiFi.macAddress(m_system_mac_address);
        WiFi.setTxPower(WIFI_POWER_19dBm);

        int result = esp_now_init();
        if (result != ESP_OK) {
            atmt::platform_printf("ESPNow Init Failed\n");
            return result;
        }
        esp_now_register_recv_cb(ESPNowSystem::SystemReceiveCallback);
        result = AddPeer(k_ESPNowBroadcastAddress);
        if (result != ESP_OK) {
            atmt::platform_printf("Broadcast Address Add As Peer Failed\n");
            return result;
        }
#endif

        m_has_init = true;
        return ESP_OK;
    };
    bool ESPNowSystem::ESPNowIsInitialized() {
        return m_has_init;
    };

    int ESPNowSystem::AddPeer(const uint8_t* mac_address) {
        if (!ESPNowIsInitialized()) {
            atmt::platform_printf("Must Initialize ESPNow Before Adding ESPNow Peer\n");
            return ESP_FAIL;
        }

        for (const RegisteredESPNowHandler &registered : m_all_registered_mac_addresses) {
            if (memcmp(registered.mac_address, mac_address, 6) == 0) {
                return ESP_OK;
            }
        }

        esp_now_peer_info_t peer_info = {};
        memcpy(peer_info.peer_addr, mac_address, 6);
        peer_info.channel = 0;
        peer_info.encrypt = false;
        int result = esp_now_add_peer(&peer_info);
        if (result != ESP_OK) {
            atmt::platform_printf("ESPNow Add Peer Failed\n");
            return result;
        }
        RegisteredESPNowHandler new_address{ };
        new_address.handler = nullptr;
        memcpy(new_address.mac_address, mac_address, 6);
        m_all_registered_mac_addresses.push_back(new_address);
        return ESP_OK;
    }
    int ESPNowSystem::RegisterESPNowHandler(const uint8_t* mac_address, ESPNowHandler* handler, bool add_as_peer) {
        if (!ESPNowIsInitialized()) {
            atmt::platform_printf("Must Initialize ESPNow Before Registering ESPNow Handler\n");
            return ESP_FAIL;
        }

        if (add_as_peer) {
            int result = AddPeer(mac_address);
            if (result != ESP_OK) {
                atmt::platform_printf("Register ESPNow Handler Failed on Add Peer\n");
                return result;
            }
        }
        for (RegisteredESPNowHandler &registered : m_registered_handlers) {
            if (registered.handler == handler) {
                memcpy(registered.mac_address, mac_address, 6);
                return ESP_OK;
            }
        }
        RegisteredESPNowHandler new_registered{ };
        new_registered.handler = handler;
        memcpy(new_registered.mac_address, mac_address, 6);
        m_registered_handlers.push_back(new_registered);
        return ESP_OK;
    };
    int ESPNowSystem::SendMessage(const uint8_t* mac_address, const uint8_t* data, size_t length) {
        if (!ESPNowIsInitialized()) {
            atmt::platform_printf("Must Initialize ESPNow Before Sending ESPNow Message\n");
            return ESP_FAIL;
        }

        int result = esp_now_send(mac_address, data, length);
        if (result != ESP_OK) {
            atmt::platform_printf("ESPNow Send Message Failed\n");
            return result;
        }
        return ESP_OK;
    };
    void ESPNowSystem::SystemReceiveCallback(const uint8_t* mac_info, const uint8_t* incoming_data, int data_length) {
        if (!g_ESPNowSystem.ESPNowIsInitialized()) {
            atmt::platform_printf("Must Initialize ESPNow Before Receiving ESPNow Message\n");
            return;
        }

        for (const RegisteredESPNowHandler &registered : g_ESPNowSystem.m_registered_handlers) {
            if (memcmp(registered.mac_address, mac_info, 6) == 0 ||
                memcmp(registered.mac_address, k_ESPNowBroadcastAddress, 6) == 0) {
                if (!registered.handler) continue;
                registered.handler->ReceiveCallback(mac_info, incoming_data, data_length);
            }
        }
    }

    const uint8_t* ESPNowSystem::GetMACAddress() {
        if (!ESPNowIsInitialized()) {
            atmt::platform_printf("Must Initialize ESPNow Before Retrieving System MAC Address\n");
            return nullptr;
        }

        return m_system_mac_address;
    }

    ESPNowSystem g_ESPNowSystem{ };


    ESPNowHandler::ESPNowHandler(uint8_t address_code):
        packet{ address_code },
        event{ },
        m_target_mac_address{ 0 }
    {

    }
    ESPNowHandler::~ESPNowHandler() {

    }

    void ESPNowHandler::init() {
        int result = g_ESPNowSystem.InitESPNow();
        if (result != ESP_OK) {
            atmt::platform_printf("Global ESPNow Init Failed\n");
            return;
        }
        memcpy(m_target_mac_address, k_ESPNowBroadcastAddress, 6);
        result = g_ESPNowSystem.RegisterESPNowHandler(k_ESPNowBroadcastAddress, this, false);
        if (result != ESP_OK) {
            atmt::platform_printf("Failed to Register ESPNow Handler to Broadcast Address\n");
            return;
        }
    }
    void ESPNowHandler::periodic() {
        if (packet.hasRawBytesToSend()) {
            uint8_t bytes[kMaxPacketSize];
            int length = packet.peekAllRawBytesToSend(bytes, kMaxPacketSize); // Don't clear from queue yet
            int result = g_ESPNowSystem.SendMessage(m_target_mac_address, bytes, length);
            if (result != ESP_OK) {
                atmt::platform_printf("Failed to Send Pending Bytes ESPNow");
            }else {
                packet.getAllRawBytesToSend(bytes, kMaxPacketSize); // Clear from queue
            }
        }

        packet.periodic();
    }
    
#ifdef ATMT_SUBMODULE_COMMAND_BASED_
    void ESPNowHandler::systemPeriodic() {
        periodic();
    };
    void ESPNowHandler::disabledPeriodic() {};
    void ESPNowHandler::autonomousPeriodic() {};
    void ESPNowHandler::teleopPeriodic() {};
#endif
    
#ifdef ATMT_SUBMODULE_COMMAND_BASED_
    void ESPNowHandler::internal_init(RobotState* robot_state, EventHandler* event_handler) {
        event.internal_init(robot_state, event_handler);
        packet.internal_init(&event);
    };
#endif

    int ESPNowHandler::setTargetMACAddress(const uint8_t* mac_address) {
        int result = g_ESPNowSystem.RegisterESPNowHandler(mac_address, this, true);
        if (result != ESP_OK) {
            atmt::platform_printf("Failed to Set ESPNow Handler MAC Address\n");
            return result;
        }
        memcpy(m_target_mac_address, mac_address, 6);
        return ESP_OK;
    };
    void ESPNowHandler::ReceiveCallback(const uint8_t* mac_info, const uint8_t* incoming_data, int data_length) {
        packet.inputReceivedRawBytes(incoming_data, data_length);
    }
    
    const uint8_t* ESPNowHandler::GetSystemMACAddress() {
        return g_ESPNowSystem.GetMACAddress();
    };
    const uint8_t* ESPNowHandler::GetTargetMACAddress() {
        return m_target_mac_address;
    };

};

#else
// #error "Enable ATMT_SUBMODULE_ESPNOW_ in automat_submodules.h to use ESPNowHandler"
#endif
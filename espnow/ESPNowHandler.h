#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_ESPNOW_

#ifndef AUTOMAT_ESPNOW_HANDLER_
#define AUTOMAT_ESPNOW_HANDLER_

#include <cstdint>
#include "../automat_platform.h"
#include "../utils.h"
#include "../packet_handling/PacketHandler.h"
#include "../packet_handling/PacketEventHandler.h"
#ifdef ATMT_SUBMODULE_COMMAND_BASED_
#include "../command_based/Subsystem.h"
#include "../command_based/Trigger.h"
#endif

#ifdef AUTOMAT_VEX_
#error "ESPNow funcitonality is not available on VEX platforms, only ESP32 platforms"
#endif

/*
    See PacketHandler for packet format and details, dataflow outline, etc.
*/

namespace atmt {

    class ESPNowHandler;

    struct RegisteredESPNowHandler {
        uint8_t mac_address[6];
        ESPNowHandler* handler;
    };

    constexpr uint8_t k_ESPNowBroadcastAddress[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    
    class ESPNowSystem {
        public:
            ESPNowSystem();
            ~ESPNowSystem();

            int InitESPNow();
            bool ESPNowIsInitialized();

            int AddPeer(const uint8_t* mac_address);
            int RegisterESPNowHandler(const uint8_t* mac_address, ESPNowHandler* handler, bool add_as_peer);
            int SendMessage(const uint8_t* mac_address, const uint8_t* data, size_t length);
            static void SystemReceiveCallback(const uint8_t* mac_info, const uint8_t* incoming_data, int data_length);

            const uint8_t* GetMACAddress();

        private:
            bool m_has_init;
            uint8_t m_system_mac_address[6];
            std::vector<RegisteredESPNowHandler> m_registered_handlers;
            std::vector<RegisteredESPNowHandler> m_all_registered_mac_addresses;
    };

    extern ESPNowSystem g_ESPNowSystem;


#ifdef ATMT_SUBMODULE_COMMAND_BASED_
    class ESPNowHandler : public Subsystem {
#else
    class ESPNowHandler {
#endif
        public:
            ESPNowHandler(uint8_t address_code);
#ifdef ATMT_SUBMODULE_COMMAND_BASED_
            ~ESPNowHandler() override;

            void init() override;
            void systemPeriodic() override;
            void disabledPeriodic() override;
            void autonomousPeriodic() override;
            void teleopPeriodic() override;

            void internal_init(RobotState* robot_state, EventHandler* event_handler);
#else
            ~ESPNowHandler();

            void init();
#endif
            void periodic();

            int setTargetMACAddress(const uint8_t* mac_address);
            void ReceiveCallback(const uint8_t* mac_info, const uint8_t* incoming_data, int data_length);

            PacketHandler packet;
            PacketEventHandler event;
        private:
            uint8_t m_target_mac_address[6];
    };

};

#endif

#else
#error "Enable ATMT_SUBMODULE_ESPNOW_ in automat_submodules.h to use ESPNowHandler"
#endif
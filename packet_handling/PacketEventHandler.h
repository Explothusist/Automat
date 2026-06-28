#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_PACKET_HANDLING_

#ifndef AUTOMAT_PACKET_EVENT_HANDLER_
#define AUTOMAT_PACKET_EVENT_HANDLER_

#include <queue>
#include <deque>
#include <cstdint>
#include "../automat_platform.h"
#include "../utils.h"
#ifdef ATMT_SUBMODULE_COMMAND_BASED_
#include "../command_based/Trigger.h"
#endif

/*
    Handles the interactions with the event system for classes like SerialHandler and ESPNowHandler
*/

namespace atmt {

    class EventHandler;

    void SetReadSerialEvents(bool to_read);

    class PacketEventHandler {
        public:
            PacketEventHandler();
            ~PacketEventHandler();

            void init();
            void periodic();

#ifdef ATMT_SUBMODULE_COMMAND_BASED_
            void internal_init(RobotState* robot_state, EventHandler* event_handler);

            void bindToMessage(Trigger* trigger, Command* command);
            void bindAutoTrigger(Trigger* trigger);
            void bindTeleopTrigger(Trigger* trigger);

            void triggerEvent(SerialEvent event, uint8_t sender, uint8_t code[], uint8_t length, int id);
#endif
        private:
#ifdef ATMT_SUBMODULE_COMMAND_BASED_
            std::vector<Trigger_Event*> m_triggers;
            std::vector<Trigger_Event*> m_temp_triggers;
            RobotState* m_robot_state;
            // TimedRobot* m_robot;
            EventHandler* m_event_handler;
#endif
    };

}

#endif

#else
#error "Enable ATMT_SUBMODULE_SERIAL_ in automat_submodules.h to use SerialReader"
#endif
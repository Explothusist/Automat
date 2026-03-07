#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_COMMAND_BASED_

#ifndef AUTOMAT_EVENT_HANDLER_
#define AUTOMAT_EVENT_HANDLER_

#include <vector>

#include "../automat_platform.h"

#include "Trigger.h"

namespace atmt {

    class Command;
    class TimedRobot;
    class Joystick;
    class SerialReader;

    void SetReadEvents(bool to_read);

    class EventHandler {
        public:
            EventHandler();

            // void init(RobotState* robot_state, TimedRobot* robot);
            void init();

            std::vector<Command*> pollEvents();
            std::vector<int> pollEventTerminations();
            bool pollAutonomousTriggers();

            Trigger_Event* interpretTrigger(Trigger_Event* trigger, bool is_stick); // Returns a temp trigger
            int assignCommandId(Command* command);
        private:
            std::vector<Command*> m_triggered_commands;
            std::vector<int> m_command_terminations;
            bool m_autonomous_triggered;

            // bool m_master_event_enable;

            // RobotState* m_robot_state;
            // TimedRobot* m_robot;

            int m_command_id_counter;
    };

}

#endif

#else
#error "Enable ATMT_SUBMODULE_COMMAND_BASED_ in automat_submodules.h to use EventHandler"
#endif
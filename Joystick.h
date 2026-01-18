
#ifndef AUTOMAT_JOYSTICK_
#define AUTOMAT_JOYSTICK_

#include <vector>

#include "automat_platform.h"

#include "Trigger.h"

namespace atmt {

    class Command;

    void SetReadJoystickEvents(bool to_read);

    typedef enum {
        PrimaryJoystick,
        PartnerJoystick
    } JoystickType;

    class Joystick {
        public:
            Joystick();
#ifdef AUTOMAT_VEX_
            Joystick(JoystickType type);
#endif
            ~Joystick();

            std::vector<Command*> pollEvents();
            std::vector<int> pollEventTerminations();
            bool pollAutonomousTriggers();

            void bindKey(StickIndicator stick, StickEvent event, Command* command);
            void bindKey(StickIndicator stick, StickEvent event, TriggerType type, Command* command);
            void bindKey(ButtonIndicator button, ButtonEvent event, Command* command);
            void bindKey(ButtonIndicator button, ButtonEvent event, TriggerType type, Command* command);

            void bindAutoTrigger(StickIndicator stick, StickEvent event);
            void bindAutoTrigger(ButtonIndicator button, ButtonEvent event);

            void triggerEvent(StickIndicator stick, StickEvent event);
            void triggerEvent(ButtonIndicator button, ButtonEvent event);

            void interpretTrigger(Trigger* trigger, bool is_stick);

            void setAxis12(int axis1, int axis2);
            void setAxis34(int axis3, int axis4);

            StickEvent getStickState(StickIndicator stick);
            ButtonEvent getButtonState(ButtonIndicator button);
            int getRawAxis(AxisIndicator axis);
        private:
            std::vector<Trigger*> m_triggers;
            std::vector<Trigger*> m_temp_triggers;

            std::vector<Command*> m_triggered_commands;
            std::vector<int> m_command_terminations;
            bool m_autonomous_triggered;

            StickEvent m_stick_state[2];
            int m_axis_position[4];
            ButtonEvent m_button_state[12];
    };

}

#endif
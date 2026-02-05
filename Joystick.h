
#ifndef AUTOMAT_JOYSTICK_
#define AUTOMAT_JOYSTICK_

#include <vector>

#include "automat_platform.h"

#include "Trigger.h"

#ifdef AUTOMAT_ESP32_
#include <functional>
#endif

namespace atmt {

    class Command;

    void SetReadJoystickEvents(bool to_read);

#ifdef AUTOMAT_VEX_
    typedef enum {
        PrimaryJoystick,
        PartnerJoystick
    } JoystickType;
#endif

#ifdef AUTOMAT_ESP32_
    struct JoystickState {
        bool buttons[12]; // A, B, X, Y, Up, Left, Down, Right, S, s, LB, RB
        uint8_t axes[6]; // RY, RX, LY, LX, RT, LT
        uint8_t axis_range[2]; // min, max (i.e. -100, 100)
    };

    typedef enum {
        PollMode_Manual,
        PollMode_Continuous,
        PollMode_ESPNow
    } PollingMode;

    typedef enum {
        Range_Min,
        Range_Max
    } RangeComp;
#endif

    class Joystick {
        public:
            // Joystick();
#ifdef AUTOMAT_VEX_
            Joystick(JoystickType type);
#endif
#ifdef AUTOMAT_ESP32_
            Joystick(PollingMode poll_mode);
            Joystick(PollingMode poll_mode, std::function<JoystickState()> state_function); // For state = Continuous
#endif
            ~Joystick();

            void init();

            std::vector<Command*> pollEvents();
            std::vector<int> pollEventTerminations();
            bool pollAutonomousTriggers();

#ifdef AUTOMAT_ESP32_
            void updateState(JoystickState new_state);
            void setStatePollingMode(PollingMode poll_mode);
            void setStateFunction(std::function<JoystickState()> state_function); // For state = Continuous

            void runPollState();
#endif

            void bindKey(StickIndicator stick, StickEvent event, Command* command);
            void bindKey(StickIndicator stick, StickEvent event, TriggerType type, Command* command);
            void bindKey(ButtonIndicator button, ButtonEvent event, Command* command);
            void bindKey(ButtonIndicator button, ButtonEvent event, TriggerType type, Command* command);

            void bindAutoTrigger(StickIndicator stick, StickEvent event);
            void bindAutoTrigger(ButtonIndicator button, ButtonEvent event);

            void triggerRawStick(StickIndicator stick, double stick_x, double stick_y);
            void triggerRawAxis(AxisIndicator axis, double value);
            void triggerEvent(StickIndicator stick, StickEvent event);
            void triggerEvent(ButtonIndicator button, ButtonEvent event);

            void interpretTrigger(Trigger* trigger, bool is_stick);

            void setAxisRight(double axis_x, double axis_y);
            void setAxisLeft(double axis_x, double axis_y);

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
            double m_axis_position[6]; // ESP32 and VEX use different subsets, but this is all (enums)
            ButtonEvent m_button_state[14];

#ifdef AUTOMAT_ESP32_
            PollingMode m_poll_mode;
            std::function<JoystickState()> m_state_function; // For state Continuous
#endif
    };

}

#endif
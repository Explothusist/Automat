#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_COMMAND_BASED_

#ifndef AUTOMAT_JOYSTICK_
#define AUTOMAT_JOYSTICK_

#include <vector>

#include "../automat_platform.h"

#include "Trigger.h"
#include "Subsystem.h"

#ifdef AUTOMAT_ESP32_
#include <functional>
#endif

namespace atmt {

    class Command;
    // class TimedRobot;
    class EventHandler;

    void SetReadJoystickEvents(bool to_read);

#ifdef AUTOMAT_VEX_
    typedef enum {
        NotAVexJoystick,
        PrimaryJoystick,
        PartnerJoystick
    } JoystickType;
#endif

// #ifdef AUTOMAT_ESP32_
    struct JoystickState {
        bool buttons[12]; // A, B, X, Y, Up, Left, Down, Right, S, s, LB, RB
        int axes[6]; // RY, RX, LY, LX, RT, LT (0, 255)
        int axis_range[2]; // min, max (i.e. -100, 100)
    };

    typedef enum {
        PollMode_Manual,
        PollMode_Continuous//, // Different on Esp32/VEX
        // PollMode_ESPNow
    } PollingMode;

    typedef enum {
        Range_Min,
        Range_Max
    } RangeComp;
// #endif

    class Joystick : public Subsystem {
        public:
            // Joystick();
#ifdef AUTOMAT_VEX_
            Joystick(PollingMode poll_mode);
            Joystick(PollingMode poll_mode, JoystickType type);
#endif
#ifdef AUTOMAT_ESP32_
            Joystick(PollingMode poll_mode);
            Joystick(PollingMode poll_mode, std::function<JoystickState()> state_function); // For state = Continuous
#endif
            ~Joystick() override;

            void init() override;
            void systemPeriodic() override;
            void disabledPeriodic() override;
            void autonomousPeriodic() override;
            void teleopPeriodic() override;

            void internal_init(RobotState* robot_state, EventHandler* event_handler);

            void updateState(JoystickState new_state);
            void setStatePollingMode(PollingMode poll_mode);
            void runPollState();
#ifdef AUTOMAT_ESP32_
            void setStateFunction(std::function<JoystickState()> state_function); // For state = Continuous
#endif

            void bindKey(Trigger* trigger, Command* command);
            void bindAutoTrigger(Trigger* trigger);

            void triggerRawStick(StickIndicator stick, double stick_x, double stick_y);
            void triggerRawAxis(AxisIndicator axis, double value);
            void triggerEvent(StickIndicator stick, StickEvent event);
            void triggerEvent(ButtonIndicator button, ButtonEvent event);

            void setAxisRight(double axis_x, double axis_y);
            void setAxisLeft(double axis_x, double axis_y);

            StickEvent getStickState(StickIndicator stick);
            ButtonEvent getButtonState(ButtonIndicator button);
            double getRawAxis(AxisIndicator axis);
        private:
            std::vector<Trigger_Event*> m_triggers;
            std::vector<Trigger_Event*> m_temp_triggers;

            // std::vector<Command*> m_triggered_commands;
            // std::vector<int> m_command_terminations;
            // bool m_autonomous_triggered;

            PollingMode m_poll_mode;
#ifdef AUTOMAT_ESP32_
            std::function<JoystickState()> m_state_function; // For state Continuous
#endif

            StickEvent m_stick_state[3];
            double m_axis_position[7]; // ESP32 and VEX use different subsets, but this is all (enums)
            ButtonEvent m_button_state[15];

            RobotState* m_robot_state;
            // TimedRobot* m_robot;
            EventHandler* m_event_handler;
    };

}

#endif

#else
#error "Enable ATMT_SUBMODULE_COMMAND_BASED_ in automat_submodules.h to use Joystick"
#endif
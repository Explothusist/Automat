
#include "Joystick.h"

#ifdef AUTOMAT_VEX_
#include "vex.h"
#endif

#ifdef AUTOMAT_ESP32_
#include <WifiEspNow.h>
#include <WifiEspNowBroadcast.h>

#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#endif

#endif

#include <cmath>

namespace atmt {

    int global_command_id_counter = 0;

    constexpr int joystick_threshold = 0.3; // Percent

    bool m_read_joystick_events{ false };

#ifdef AUTOMAT_VEX_
    vex::controller m_controller_primary = vex::controller(vex::primary);
    vex::controller m_controller_partner = vex::controller(vex::partner);
    Joystick* current_joystick_primary{ nullptr };
    bool primary_init{ false };
    Joystick* current_joystick_partner{ nullptr };
    bool partner_init{ false };

    constexpr double joystick_range = 100;

    void buttonAPressed_Primary() {
        if (!current_joystick_primary || !m_read_joystick_events) return;
        current_joystick_primary->triggerEvent(AButton, ButtonPressed);
    };
    void buttonAReleased_Primary() {
        if (!current_joystick_primary || !m_read_joystick_events) return;
        current_joystick_primary->triggerEvent(AButton, ButtonReleased);
    };
    void buttonBPressed_Primary() {
        if (!current_joystick_primary || !m_read_joystick_events) return;
        current_joystick_primary->triggerEvent(BButton, ButtonPressed);
    };
    void buttonBReleased_Primary() {
        if (!current_joystick_primary || !m_read_joystick_events) return;
        current_joystick_primary->triggerEvent(BButton, ButtonReleased);
    };
    void buttonXPressed_Primary() {
        if (!current_joystick_primary || !m_read_joystick_events) return;
        current_joystick_primary->triggerEvent(XButton, ButtonPressed);
    };
    void buttonXReleased_Primary() {
        if (!current_joystick_primary || !m_read_joystick_events) return;
        current_joystick_primary->triggerEvent(XButton, ButtonReleased);
    };
    void buttonYPressed_Primary() {
        if (!current_joystick_primary || !m_read_joystick_events) return;
        current_joystick_primary->triggerEvent(YButton, ButtonPressed);
    };
    void buttonYReleased_Primary() {
        if (!current_joystick_primary || !m_read_joystick_events) return;
        current_joystick_primary->triggerEvent(YButton, ButtonReleased);
    };
    void buttonUpPressed_Primary() {
        if (!current_joystick_primary || !m_read_joystick_events) return;
        current_joystick_primary->triggerEvent(UpButton, ButtonPressed);
    };
    void buttonUpReleased_Primary() {
        if (!current_joystick_primary || !m_read_joystick_events) return;
        current_joystick_primary->triggerEvent(UpButton, ButtonReleased);
    };
    void buttonDownPressed_Primary() {
        if (!current_joystick_primary || !m_read_joystick_events) return;
        current_joystick_primary->triggerEvent(DownButton, ButtonPressed);
    };
    void buttonDownReleased_Primary() {
        if (!current_joystick_primary || !m_read_joystick_events) return;
        current_joystick_primary->triggerEvent(DownButton, ButtonReleased);
    };
    void buttonLeftPressed_Primary() {
        if (!current_joystick_primary || !m_read_joystick_events) return;
        current_joystick_primary->triggerEvent(LeftButton, ButtonPressed);
    };
    void buttonLeftReleased_Primary() {
        if (!current_joystick_primary || !m_read_joystick_events) return;
        current_joystick_primary->triggerEvent(LeftButton, ButtonReleased);
    };
    void buttonRightPressed_Primary() {
        if (!current_joystick_primary || !m_read_joystick_events) return;
        current_joystick_primary->triggerEvent(RightButton, ButtonPressed);
    };
    void buttonRightReleased_Primary() {
        if (!current_joystick_primary || !m_read_joystick_events) return;
        current_joystick_primary->triggerEvent(RightButton, ButtonReleased);
    };
    void buttonL1Pressed_Primary() {
        if (!current_joystick_primary || !m_read_joystick_events) return;
        current_joystick_primary->triggerEvent(L1Button, ButtonPressed);
    };
    void buttonL1Released_Primary() {
        if (!current_joystick_primary || !m_read_joystick_events) return;
        current_joystick_primary->triggerEvent(L1Button, ButtonReleased);
    };
    void buttonL2Pressed_Primary() {
        if (!current_joystick_primary || !m_read_joystick_events) return;
        current_joystick_primary->triggerEvent(L2Button, ButtonPressed);
    };
    void buttonL2Released_Primary() {
        if (!current_joystick_primary || !m_read_joystick_events) return;
        current_joystick_primary->triggerEvent(L2Button, ButtonReleased);
    };
    void buttonR1Pressed_Primary() {
        if (!current_joystick_primary || !m_read_joystick_events) return;
        current_joystick_primary->triggerEvent(R1Button, ButtonPressed);
    };
    void buttonR1Released_Primary() {
        if (!current_joystick_primary || !m_read_joystick_events) return;
        current_joystick_primary->triggerEvent(R1Button, ButtonReleased);
    };
    void buttonR2Pressed_Primary() {
        if (!current_joystick_primary || !m_read_joystick_events) return;
        current_joystick_primary->triggerEvent(R2Button, ButtonPressed);
    };
    void buttonR2Released_Primary() {
        if (!current_joystick_primary || !m_read_joystick_events) return;
        current_joystick_primary->triggerEvent(R2Button, ButtonReleased);
    };

    StickEvent leftStickState_Primary{ StickCenter };
    void leftStickMoved_Primary() {
        if (!current_joystick_primary || !m_read_joystick_events) return;
        double axis4_pos = m_controller_primary.Axis4.position(); // L/R
        double axis3_pos = m_controller_primary.Axis3.position(); // U/D
        current_joystick_primary->triggerRawStick(LeftStick, axis4_pos / joystick_range, axis3_pos / joystick_range);
        // current_joystick_primary->setAxis12(axis1_pos, axis2_pos);
        // StickEvent new_state = StickCenter;
        // if (std::abs(axis1_pos) > std::abs(axis2_pos)) {
        //     if (axis1_pos > joystick_threshold) { // PROBLEM HERE
        //         new_state = StickRight;
        //     }else if (axis1_pos < -joystick_threshold) {
        //         new_state = StickLeft;
        //     }
        // }else {
        //     if (axis2_pos > joystick_threshold) {
        //         new_state = StickUp;
        //     }else if (axis2_pos < -joystick_threshold) {
        //         new_state = StickDown;
        //     }
        // }
        // if (new_state != leftStickState_Primary) {
        //     leftStickState_Primary = new_state;
        //     current_joystick_primary->triggerEvent(LeftStick, new_state);
        // }
    };
    StickEvent rightStickState_Primary{ StickCenter };
    void rightStickMoved_Primary() {
        if (!current_joystick_primary || !m_read_joystick_events) return;
        double axis1_pos = m_controller_primary.Axis1.position(); // L/R
        double axis2_pos = m_controller_primary.Axis2.position(); // U/D
        current_joystick_primary->triggerRawStick(RightStick, axis1_pos / joystick_range, axis2_pos / joystick_range);
        // current_joystick_primary->setAxis34(axis3_pos, axis4_pos);
        // StickEvent new_state = StickCenter;
        // if (std::abs(axis4_pos) > std::abs(axis3_pos)) {
        //     if (axis4_pos > joystick_threshold) {
        //         new_state = StickRight;
        //     }else if (axis4_pos < -joystick_threshold) {
        //         new_state = StickLeft;
        //     }
        // }else {
        //     if (axis3_pos > joystick_threshold) {
        //         new_state = StickUp;
        //     }else if (axis3_pos < -joystick_threshold) {
        //         new_state = StickDown;
        //     }
        // }
        // if (new_state != rightStickState_Primary) {
        //     rightStickState_Primary = new_state;
        //     current_joystick_primary->triggerEvent(RightStick, new_state);
        // }
    };

    void buttonAPressed_Partner() {
        if (!current_joystick_partner || !m_read_joystick_events) return;
        current_joystick_partner->triggerEvent(AButton, ButtonPressed);
    };
    void buttonAReleased_Partner() {
        if (!current_joystick_partner || !m_read_joystick_events) return;
        current_joystick_partner->triggerEvent(AButton, ButtonReleased);
    };
    void buttonBPressed_Partner() {
        if (!current_joystick_partner || !m_read_joystick_events) return;
        current_joystick_partner->triggerEvent(BButton, ButtonPressed);
    };
    void buttonBReleased_Partner() {
        if (!current_joystick_partner || !m_read_joystick_events) return;
        current_joystick_partner->triggerEvent(BButton, ButtonReleased);
    };
    void buttonXPressed_Partner() {
        if (!current_joystick_partner || !m_read_joystick_events) return;
        current_joystick_partner->triggerEvent(XButton, ButtonPressed);
    };
    void buttonXReleased_Partner() {
        if (!current_joystick_partner || !m_read_joystick_events) return;
        current_joystick_partner->triggerEvent(XButton, ButtonReleased);
    };
    void buttonYPressed_Partner() {
        if (!current_joystick_partner || !m_read_joystick_events) return;
        current_joystick_partner->triggerEvent(YButton, ButtonPressed);
    };
    void buttonYReleased_Partner() {
        if (!current_joystick_partner || !m_read_joystick_events) return;
        current_joystick_partner->triggerEvent(YButton, ButtonReleased);
    };
    void buttonUpPressed_Partner() {
        if (!current_joystick_partner || !m_read_joystick_events) return;
        current_joystick_partner->triggerEvent(UpButton, ButtonPressed);
    };
    void buttonUpReleased_Partner() {
        if (!current_joystick_partner || !m_read_joystick_events) return;
        current_joystick_partner->triggerEvent(UpButton, ButtonReleased);
    };
    void buttonDownPressed_Partner() {
        if (!current_joystick_partner || !m_read_joystick_events) return;
        current_joystick_partner->triggerEvent(DownButton, ButtonPressed);
    };
    void buttonDownReleased_Partner() {
        if (!current_joystick_partner || !m_read_joystick_events) return;
        current_joystick_partner->triggerEvent(DownButton, ButtonReleased);
    };
    void buttonLeftPressed_Partner() {
        if (!current_joystick_partner || !m_read_joystick_events) return;
        current_joystick_partner->triggerEvent(LeftButton, ButtonPressed);
    };
    void buttonLeftReleased_Partner() {
        if (!current_joystick_partner || !m_read_joystick_events) return;
        current_joystick_partner->triggerEvent(LeftButton, ButtonReleased);
    };
    void buttonRightPressed_Partner() {
        if (!current_joystick_partner || !m_read_joystick_events) return;
        current_joystick_partner->triggerEvent(RightButton, ButtonPressed);
    };
    void buttonRightReleased_Partner() {
        if (!current_joystick_partner || !m_read_joystick_events) return;
        current_joystick_partner->triggerEvent(RightButton, ButtonReleased);
    };
    void buttonL1Pressed_Partner() {
        if (!current_joystick_partner || !m_read_joystick_events) return;
        current_joystick_partner->triggerEvent(L1Button, ButtonPressed);
    };
    void buttonL1Released_Partner() {
        if (!current_joystick_partner || !m_read_joystick_events) return;
        current_joystick_partner->triggerEvent(L1Button, ButtonReleased);
    };
    void buttonL2Pressed_Partner() {
        if (!current_joystick_partner || !m_read_joystick_events) return;
        current_joystick_partner->triggerEvent(L2Button, ButtonPressed);
    };
    void buttonL2Released_Partner() {
        if (!current_joystick_partner || !m_read_joystick_events) return;
        current_joystick_partner->triggerEvent(L2Button, ButtonReleased);
    };
    void buttonR1Pressed_Partner() {
        if (!current_joystick_partner || !m_read_joystick_events) return;
        current_joystick_partner->triggerEvent(R1Button, ButtonPressed);
    };
    void buttonR1Released_Partner() {
        if (!current_joystick_partner || !m_read_joystick_events) return;
        current_joystick_partner->triggerEvent(R1Button, ButtonReleased);
    };
    void buttonR2Pressed_Partner() {
        if (!current_joystick_partner || !m_read_joystick_events) return;
        current_joystick_partner->triggerEvent(R2Button, ButtonPressed);
    };
    void buttonR2Released_Partner() {
        if (!current_joystick_partner || !m_read_joystick_events) return;
        current_joystick_partner->triggerEvent(R2Button, ButtonReleased);
    };

    StickEvent leftStickState_Partner{ StickCenter };
    void leftStickMoved_Partner() {
        if (!current_joystick_partner || !m_read_joystick_events) return;
        double axis4_pos = m_controller_partner.Axis4.position(); // L/R
        double axis3_pos = m_controller_partner.Axis3.position(); // U/D
        current_joystick_partner->triggerRawStick(LeftStick, axis4_pos / joystick_range, axis3_pos / joystick_range);
        // current_joystick_partner->setAxis12(axis1_pos, axis2_pos);
        // StickEvent new_state = StickCenter;
        // if (std::abs(axis1_pos) > std::abs(axis2_pos)) {
        //     if (axis1_pos > joystick_threshold) {
        //         new_state = StickRight;
        //     }else if (axis1_pos < -joystick_threshold) {
        //         new_state = StickLeft;
        //     }
        // }else {
        //     if (axis2_pos > joystick_threshold) {
        //         new_state = StickUp;
        //     }else if (axis2_pos < -joystick_threshold) {
        //         new_state = StickDown;
        //     }
        // }
        // if (new_state != leftStickState_Partner) {
        //     leftStickState_Partner = new_state;
        //     current_joystick_partner->triggerEvent(LeftStick, new_state);
        // }
    };
    StickEvent rightStickState_Partner{ StickCenter };
    void rightStickMoved_Partner() {
        if (!current_joystick_partner || !m_read_joystick_events) return;
        double axis1_pos = m_controller_partner.Axis1.position(); // L/R
        double axis2_pos = m_controller_partner.Axis2.position(); // U/D
        current_joystick_partner->triggerRawStick(RightStick, axis1_pos / joystick_range, axis2_pos / joystick_range);
        // StickEvent new_state = StickCenter;
        // if (std::abs(axis4_pos) > std::abs(axis3_pos)) {
        //     if (axis4_pos > joystick_threshold) {
        //         new_state = StickRight;
        //     }else if (axis4_pos < -joystick_threshold) {
        //         new_state = StickLeft;
        //     }
        // }else {
        //     if (axis3_pos > joystick_threshold) {
        //         new_state = StickUp;
        //     }else if (axis3_pos < -joystick_threshold) {
        //         new_state = StickDown;
        //     }
        // }
        // if (new_state != rightStickState_Partner) {
        //     rightStickState_Partner = new_state;
        //     current_joystick_partner->triggerEvent(RightStick, new_state);
        // }
    };

    void SetAsPrimaryJoystick(Joystick* joystick) {
        current_joystick_primary = joystick;
    };
    void SetAsPartnerJoystick(Joystick* joystick) {
        current_joystick_partner = joystick;
    };
#endif

    void SetReadJoystickEvents(bool to_read) {
        m_read_joystick_events = to_read;
        
#ifdef AUTOMAT_VEX_
        if (to_read) {
            if (!primary_init) {
                primary_init = true;
                m_controller_primary.ButtonA.pressed(buttonAPressed_Primary);
                m_controller_primary.ButtonA.released(buttonAReleased_Primary);
                m_controller_primary.ButtonB.pressed(buttonBPressed_Primary);
                m_controller_primary.ButtonB.released(buttonBReleased_Primary);
                m_controller_primary.ButtonX.pressed(buttonXPressed_Primary);
                m_controller_primary.ButtonX.released(buttonXReleased_Primary);
                m_controller_primary.ButtonY.pressed(buttonYPressed_Primary);
                m_controller_primary.ButtonY.released(buttonYReleased_Primary);
                m_controller_primary.ButtonUp.pressed(buttonUpPressed_Primary);
                m_controller_primary.ButtonUp.released(buttonUpReleased_Primary);
                m_controller_primary.ButtonDown.pressed(buttonDownPressed_Primary);
                m_controller_primary.ButtonDown.released(buttonDownReleased_Primary);
                m_controller_primary.ButtonRight.pressed(buttonRightPressed_Primary);
                m_controller_primary.ButtonRight.released(buttonRightReleased_Primary);
                m_controller_primary.ButtonLeft.pressed(buttonLeftPressed_Primary);
                m_controller_primary.ButtonLeft.released(buttonLeftReleased_Primary);
                m_controller_primary.ButtonL1.pressed(buttonL1Pressed_Primary);
                m_controller_primary.ButtonL1.released(buttonL1Released_Primary);
                m_controller_primary.ButtonL2.pressed(buttonL2Pressed_Primary);
                m_controller_primary.ButtonL2.released(buttonL2Released_Primary);
                m_controller_primary.ButtonR1.pressed(buttonR1Pressed_Primary);
                m_controller_primary.ButtonR1.released(buttonR1Released_Primary);
                m_controller_primary.ButtonR2.pressed(buttonR2Pressed_Primary);
                m_controller_primary.ButtonR2.released(buttonR2Released_Primary);
                m_controller_primary.Axis1.changed(leftStickMoved_Primary);
                m_controller_primary.Axis2.changed(leftStickMoved_Primary);
                m_controller_primary.Axis3.changed(rightStickMoved_Primary);
                m_controller_primary.Axis4.changed(rightStickMoved_Primary);
            }

            if (!partner_init) {
                partner_init = true;
                m_controller_partner.ButtonA.pressed(buttonAPressed_Partner);
                m_controller_partner.ButtonA.released(buttonAReleased_Partner);
                m_controller_partner.ButtonB.pressed(buttonBPressed_Partner);
                m_controller_partner.ButtonB.released(buttonBReleased_Partner);
                m_controller_partner.ButtonX.pressed(buttonXPressed_Partner);
                m_controller_partner.ButtonX.released(buttonXReleased_Partner);
                m_controller_partner.ButtonY.pressed(buttonYPressed_Partner);
                m_controller_partner.ButtonY.released(buttonYReleased_Partner);
                m_controller_partner.ButtonUp.pressed(buttonUpPressed_Partner);
                m_controller_partner.ButtonUp.released(buttonUpReleased_Partner);
                m_controller_partner.ButtonDown.pressed(buttonDownPressed_Partner);
                m_controller_partner.ButtonDown.released(buttonDownReleased_Partner);
                m_controller_partner.ButtonRight.pressed(buttonRightPressed_Partner);
                m_controller_partner.ButtonRight.released(buttonRightReleased_Partner);
                m_controller_partner.ButtonLeft.pressed(buttonLeftPressed_Partner);
                m_controller_partner.ButtonLeft.released(buttonLeftReleased_Partner);
                m_controller_partner.ButtonL1.pressed(buttonL1Pressed_Partner);
                m_controller_partner.ButtonL1.released(buttonL1Released_Partner);
                m_controller_partner.ButtonL2.pressed(buttonL2Pressed_Partner);
                m_controller_partner.ButtonL2.released(buttonL2Released_Partner);
                m_controller_partner.ButtonR1.pressed(buttonR1Pressed_Partner);
                m_controller_partner.ButtonR1.released(buttonR1Released_Partner);
                m_controller_partner.ButtonR2.pressed(buttonR2Pressed_Partner);
                m_controller_partner.ButtonR2.released(buttonR2Released_Partner);
                m_controller_partner.Axis1.changed(leftStickMoved_Partner);
                m_controller_partner.Axis2.changed(leftStickMoved_Partner);
                m_controller_partner.Axis3.changed(rightStickMoved_Partner);
                m_controller_partner.Axis4.changed(rightStickMoved_Partner);
            }
        }
#endif
    };

#ifdef AUTOMAT_ESP32_
#endif

#ifdef AUTOMAT_VEX_
    Joystick::Joystick(JoystickType type):
#endif
#ifdef AUTOMAT_ESP32_
    Joystick::Joystick(PollingMode poll_mode, std::function<JoystickState()> state_function):
#endif
        m_triggers{ std::vector<Trigger*>() },
        m_temp_triggers{ std::vector<Trigger*>() },
        m_triggered_commands{ std::vector<Command*>() },
        m_command_terminations{ std::vector<int>() },
        m_autonomous_triggered{ false },
        m_stick_state{ StickCenter, StickCenter },
        m_axis_position{ 0, 0, 0, 0 },
        m_button_state{ ButtonReleased, ButtonReleased, ButtonReleased, ButtonReleased, ButtonReleased, ButtonReleased, ButtonReleased, ButtonReleased, ButtonReleased, ButtonReleased, ButtonReleased, ButtonReleased },
#ifdef AUTOMAT_ESP32_
        m_poll_mode{ poll_mode },
        m_state_function{ state_function }
#endif
    {
        
#ifdef AUTOMAT_VEX_
        if (type == PrimaryJoystick) {
            SetAsPrimaryJoystick(this);
        }else if (type == PartnerJoystick) {
            SetAsPartnerJoystick(this);
        }
#endif
    };
#ifdef AUTOMAT_ESP32_
    Joystick::Joystick(PollingMode poll_mode):
        Joystick(poll_mode, nullptr)
    {

    };
#endif
    Joystick::~Joystick() {
        for (Trigger* trigger : m_triggers) {
            delete trigger;
        }
        m_triggers.clear();
    };

    void Joystick::init() {

    };

    std::vector<Command*> Joystick::pollEvents() {
        std::vector<Command*> commands = m_triggered_commands;
        m_triggered_commands.clear();
        return commands;
    };
    std::vector<int> Joystick::pollEventTerminations() {
        std::vector<int> commands = m_command_terminations;
        m_command_terminations.clear();
        return commands;
    };
    bool Joystick::pollAutonomousTriggers() {
        bool triggered = m_autonomous_triggered;
        m_autonomous_triggered = false;
        return triggered;
    };

#ifdef AUTOMAT_ESP32_
    void Joystick::updateState(JoystickState new_state) {
        if (m_read_joystick_events) {
            // Button Handlers
            if (new_state.buttons[AButton] != m_button_state[AButton]) {
                triggerEvent(AButton, new_state.buttons[AButton] ? ButtonPressed : ButtonReleased);
            }
            if (new_state.buttons[BButton] != m_button_state[BButton]) {
                triggerEvent(BButton, new_state.buttons[BButton] ? ButtonPressed : ButtonReleased);
            }
            if (new_state.buttons[XButton] != m_button_state[XButton]) {
                triggerEvent(XButton, new_state.buttons[XButton] ? ButtonPressed : ButtonReleased);
            }
            if (new_state.buttons[YButton] != m_button_state[YButton]) {
                triggerEvent(YButton, new_state.buttons[YButton] ? ButtonPressed : ButtonReleased);
            }
            if (new_state.buttons[UpButton] != m_button_state[UpButton]) {
                triggerEvent(UpButton, new_state.buttons[UpButton] ? ButtonPressed : ButtonReleased);
            }
            if (new_state.buttons[LeftButton] != m_button_state[LeftButton]) {
                triggerEvent(LeftButton, new_state.buttons[LeftButton] ? ButtonPressed : ButtonReleased);
            }
            if (new_state.buttons[DownButton] != m_button_state[DownButton]) {
                triggerEvent(DownButton, new_state.buttons[DownButton] ? ButtonPressed : ButtonReleased);
            }
            if (new_state.buttons[RightButton] != m_button_state[RightButton]) {
                triggerEvent(RightButton, new_state.buttons[RightButton] ? ButtonPressed : ButtonReleased);
            }
            if (new_state.buttons[StartButton] != m_button_state[StartButton]) {
                triggerEvent(StartButton, new_state.buttons[StartButton] ? ButtonPressed : ButtonReleased);
            }
            if (new_state.buttons[SelectButton] != m_button_state[SelectButton]) {
                triggerEvent(SelectButton, new_state.buttons[SelectButton] ? ButtonPressed : ButtonReleased);
            }
            if (new_state.buttons[L1Button] != m_button_state[L1Button]) {
                triggerEvent(L1Button, new_state.buttons[L1Button] ? ButtonPressed : ButtonReleased);
            }
            if (new_state.buttons[R1Button] != m_button_state[R1Button]) {
                triggerEvent(R1Button, new_state.buttons[R1Button] ? ButtonPressed : ButtonReleased);
            }

            // Stick Handlers
            if (new_state.axes[AxisRY] != m_axis_position[AxisRY] || new_state.axes[AxisRX] != m_axis_position[AxisRX]) {
                const double range_min = new_state.axis_range[Range_Min]; // Cast to double
                const double range_max = new_state.axis_range[Range_Max];
                double axis_x = (new_state.axes[AxisRX] - range_min) / (range_max - range_min);
                double axis_y = (new_state.axes[AxisRY] - range_min) / (range_max - range_min);
                axis_x = (axis_x * 2.0) - 1.0; // 0-1 scale -> -1-1 scale
                axis_y = (axis_y * 2.0) - 1.0; // 0-1 scale -> -1-1 scale
                triggerRawStick(RightStick, axis_x, axis_y);
            }
            if (new_state.axes[AxisLY] != m_axis_position[AxisLY] || new_state.axes[AxisLX] != m_axis_position[AxisLX]) {
                const double range_min = new_state.axis_range[Range_Min]; // Cast to double
                const double range_max = new_state.axis_range[Range_Max];
                double axis_x = (new_state.axes[AxisLX] - range_min) / (range_max - range_min);
                double axis_y = (new_state.axes[AxisLY] - range_min) / (range_max - range_min);
                axis_x = (axis_x * 2.0) - 1.0; // 0-1 scale -> -1-1 scale
                axis_y = (axis_y * 2.0) - 1.0; // 0-1 scale -> -1-1 scale
                triggerRawStick(LeftStick, axis_x, axis_y);
            }

            // Trigger Handlers
            if (new_state.axes[AxisLT] != m_axis_position[AxisLT]) {
                const double range_min = new_state.axis_range[Range_Min]; // Cast to double
                const double range_max = new_state.axis_range[Range_Max];
                double axis = (new_state.axes[AxisLT] - range_min) / (range_max - range_min);
                axis = (axis * 2.0) - 1.0; // 0-1 scale -> -1-1 scale
                triggerRawAxis(AxisLT, axis);
            }
            if (new_state.axes[AxisRT] != m_axis_position[AxisRT]) {
                const double range_min = new_state.axis_range[Range_Min]; // Cast to double
                const double range_max = new_state.axis_range[Range_Max];
                double axis = (new_state.axes[AxisRT] - range_min) / (range_max - range_min);
                axis = (axis * 2.0) - 1.0; // 0-1 scale -> -1-1 scale
                triggerRawAxis(AxisRT, axis);
            }
        }
    };
    void Joystick::setStatePollingMode(PollingMode poll_mode) {
        m_poll_mode = poll_mode;
    };
    void Joystick::setStateFunction(std::function<JoystickState()> state_function) {
        m_state_function = state_function;
    };

    void Joystick::runPollState() {
        if (m_poll_mode == PollMode_Continuous) { // Otherwise manual or event triggered
            if (!m_state_function) {
                return;
            }
            updateState(m_state_function());
        }
    };
#endif

    void Joystick::triggerRawStick(StickIndicator stick, double stick_x, double stick_y) {
        switch (stick) {
            case RightStick:
                setAxisRight(stick_x, stick_y);
                break;
            case LeftStick:
                setAxisLeft(stick_x, stick_y);
                break;
        }
        StickEvent new_state = StickCenter;
        if (std::abs(stick_x) > std::abs(stick_y)) {
            if (stick_x > joystick_threshold) {
                new_state = StickRight;
            }else if (stick_x < -joystick_threshold) {
                new_state = StickLeft;
            }
        }else {
            if (stick_y > joystick_threshold) {
                new_state = StickUp;
            }else if (stick_y < -joystick_threshold) {
                new_state = StickDown;
            }
        }
        if (new_state != m_stick_state[stick]) {
            m_stick_state[stick] = new_state;
            triggerEvent(stick, new_state);
        }
    };
    void Joystick::triggerRawAxis(AxisIndicator axis, double value) {
        m_axis_position[axis] = value;
        ButtonEvent new_state = (value > joystick_threshold) ? ButtonPressed : ButtonReleased;
        switch (axis) {
            case AxisLT:
                if (new_state != m_button_state[L2Button]) {
                    triggerEvent(L2Button, new_state);
                }
                break;
            case AxisRT:
                if (new_state != m_button_state[R2Button]) {
                    triggerEvent(R2Button, new_state);
                }
                break;
        }
    };
    void Joystick::triggerEvent(StickIndicator stick, StickEvent event) {
        m_stick_state[stick] = event;
        for (int i = 0; i < static_cast<int>(m_temp_triggers.size()); i++) {
            if (m_temp_triggers[i]->matchesEvent(stick, event)) {
                interpretTrigger(m_temp_triggers[i], true);
                
                delete m_temp_triggers[i];
                m_temp_triggers.erase(m_temp_triggers.begin() + i);
                i -= 1;
            }
        }

        for (Trigger* trigger : m_triggers) {
            if (trigger->matchesEvent(stick, event)) {
                interpretTrigger(trigger, true);
            }
        }
    };
    void Joystick::triggerEvent(ButtonIndicator button, ButtonEvent event) {
        m_button_state[button] = event;
        for (int i = 0; i < static_cast<int>(m_temp_triggers.size()); i++) {
            if (m_temp_triggers[i]->matchesEvent(button, event)) {
                interpretTrigger(m_temp_triggers[i], false);

                delete m_temp_triggers[i];
                m_temp_triggers.erase(m_temp_triggers.begin() + i);
                i -= 1;
            }
        }

        for (Trigger* trigger : m_triggers) {
            if (trigger->matchesEvent(button, event)) {
                interpretTrigger(trigger, false);
            }
        }
    };

    void Joystick::interpretTrigger(Trigger* trigger, bool is_stick) {
        switch (trigger->getTriggerEffect()) {
            case StartCommand:
                {
                    Command* baseCommand = trigger->getCommand();
                    if (baseCommand != nullptr) {
                        Command* command = baseCommand->clone();
                        command->setId(global_command_id_counter);
                        global_command_id_counter += 1;
                        m_triggered_commands.push_back(command);

                        if (trigger->getTriggerType() == WhileTrigger) {
                            if (is_stick) {
                                m_temp_triggers.push_back(new StickEndingTrigger(EndCommand, static_cast<StickTrigger*>(trigger), command->getId()));
                            }else {
                                m_temp_triggers.push_back(new ButtonEndingTrigger(EndCommand, static_cast<ButtonTrigger*>(trigger), command->getId()));
                            }
                        }
                    }
                }
                break;
            case EndCommand:
                {
                    m_command_terminations.push_back(trigger->getCommandId());
                }
                break;
            case StartAutonomous:
                {
                    m_autonomous_triggered = true;
                }
                break;
            default:
                break;
        }
    };

    void Joystick::bindKey(StickIndicator stick, StickEvent event, Command* command) {
        bindKey(stick, event, OnTrigger, command);
    };
    void Joystick::bindKey(StickIndicator stick, StickEvent event, TriggerType type, Command* command) {
        m_triggers.push_back(new StickTrigger(StartCommand, stick, event, type, command));
    };
    void Joystick::bindKey(ButtonIndicator button, ButtonEvent event, Command* command) {
        bindKey(button, event, OnTrigger, command);
    };
    void Joystick::bindKey(ButtonIndicator button, ButtonEvent event, TriggerType type, Command* command) {
        m_triggers.push_back(new ButtonTrigger(StartCommand, button, event, type, command));
    };

    void Joystick::bindAutoTrigger(StickIndicator stick, StickEvent event) {
        m_temp_triggers.push_back(new StickTrigger(StartAutonomous, stick, event));
    };
    void Joystick::bindAutoTrigger(ButtonIndicator button, ButtonEvent event) {
        m_temp_triggers.push_back(new ButtonTrigger(StartAutonomous, button, event));
    };

    void Joystick::setAxisRight(double axis_x, double axis_y) {
        m_axis_position[AxisRY] = axis_y; // AxisRY
        m_axis_position[AxisRX] = axis_x; // AxisRX
    };
    void Joystick::setAxisLeft(double axis_x, double axis_y) {
        m_axis_position[AxisLY] = axis_y; // AxisLY
        m_axis_position[AxisLX] = axis_x; // AxisLX
    };

    ButtonEvent Joystick::getButtonState(ButtonIndicator button) {
        return m_button_state[button];
    };
    StickEvent Joystick::getStickState(StickIndicator stick) {
        return m_stick_state[stick];
    };
    int Joystick::getRawAxis(AxisIndicator axis) {
        return m_axis_position[axis];
    };

};
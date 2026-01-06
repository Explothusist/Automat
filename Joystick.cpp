
#include "Joystick.h"

#ifdef AUTOMAT_VEX_
#include "vex.h"
#endif

#include <cmath>

namespace atmt {

    int global_command_id_counter = 0;

    const int joystick_threshold = 30;

#ifdef AUTOMAT_VEX_
    vex::controller m_controller_primary = vex::controller(vex::primary);
    vex::controller m_controller_partner = vex::controller(vex::partner);
    Joystick* current_joystick_primary{ nullptr };
    bool primary_init{ false };
    Joystick* current_joystick_partner{ nullptr };
    bool partner_init{ false };

    void buttonAPressed_Primary() {
        current_joystick_primary->triggerEvent(AButton, ButtonPressed);
    };
    void buttonAReleased_Primary() {
        current_joystick_primary->triggerEvent(AButton, ButtonReleased);
    };
    void buttonBPressed_Primary() {
        current_joystick_primary->triggerEvent(BButton, ButtonPressed);
    };
    void buttonBReleased_Primary() {
        current_joystick_primary->triggerEvent(BButton, ButtonReleased);
    };
    void buttonXPressed_Primary() {
        current_joystick_primary->triggerEvent(XButton, ButtonPressed);
    };
    void buttonXReleased_Primary() {
        current_joystick_primary->triggerEvent(XButton, ButtonReleased);
    };
    void buttonYPressed_Primary() {
        current_joystick_primary->triggerEvent(YButton, ButtonPressed);
    };
    void buttonYReleased_Primary() {
        current_joystick_primary->triggerEvent(YButton, ButtonReleased);
    };
    void buttonUpPressed_Primary() {
        current_joystick_primary->triggerEvent(UpButton, ButtonPressed);
    };
    void buttonUpReleased_Primary() {
        current_joystick_primary->triggerEvent(UpButton, ButtonReleased);
    };
    void buttonDownPressed_Primary() {
        current_joystick_primary->triggerEvent(DownButton, ButtonPressed);
    };
    void buttonDownReleased_Primary() {
        current_joystick_primary->triggerEvent(DownButton, ButtonReleased);
    };
    void buttonLeftPressed_Primary() {
        current_joystick_primary->triggerEvent(LeftButton, ButtonPressed);
    };
    void buttonLeftReleased_Primary() {
        current_joystick_primary->triggerEvent(LeftButton, ButtonReleased);
    };
    void buttonRightPressed_Primary() {
        current_joystick_primary->triggerEvent(RightButton, ButtonPressed);
    };
    void buttonRightReleased_Primary() {
        current_joystick_primary->triggerEvent(RightButton, ButtonReleased);
    };
    void buttonL1Pressed_Primary() {
        current_joystick_primary->triggerEvent(L1Button, ButtonPressed);
    };
    void buttonL1Released_Primary() {
        current_joystick_primary->triggerEvent(L1Button, ButtonReleased);
    };
    void buttonL2Pressed_Primary() {
        current_joystick_primary->triggerEvent(L2Button, ButtonPressed);
    };
    void buttonL2Released_Primary() {
        current_joystick_primary->triggerEvent(L2Button, ButtonReleased);
    };
    void buttonR1Pressed_Primary() {
        current_joystick_primary->triggerEvent(R1Button, ButtonPressed);
    };
    void buttonR1Released_Primary() {
        current_joystick_primary->triggerEvent(R1Button, ButtonReleased);
    };
    void buttonR2Pressed_Primary() {
        current_joystick_primary->triggerEvent(R2Button, ButtonPressed);
    };
    void buttonR2Released_Primary() {
        current_joystick_primary->triggerEvent(R2Button, ButtonReleased);
    };

    StickEvent leftStickState_Primary{ StickCenter };
    void leftStickMoved_Primary() {
        int axis1_pos = m_controller_primary.Axis1.position(); // L/R
        int axis2_pos = m_controller_primary.Axis2.position(); // U/D
        current_joystick_primary->setAxis12(axis1_pos, axis2_pos);
        StickEvent new_state = StickCenter;
        if (std::abs(axis1_pos) > std::abs(axis2_pos)) {
            if (axis1_pos > joystick_threshold) {
                new_state = StickRight;
            }else if (axis1_pos < -joystick_threshold) {
                new_state = StickLeft;
            }
        }else {
            if (axis2_pos > joystick_threshold) {
                new_state = StickUp;
            }else if (axis2_pos < -joystick_threshold) {
                new_state = StickDown;
            }
        }
        if (new_state != leftStickState_Primary) {
            leftStickState_Primary = new_state;
            current_joystick_primary->triggerEvent(LeftStick, new_state);
        }
    };
    StickEvent rightStickState_Primary{ StickCenter };
    void rightStickMoved_Primary() {
        int axis4_pos = m_controller_primary.Axis4.position(); // L/R
        int axis3_pos = m_controller_primary.Axis3.position(); // U/D
        current_joystick_primary->setAxis34(axis3_pos, axis4_pos);
        StickEvent new_state = StickCenter;
        if (std::abs(axis4_pos) > std::abs(axis3_pos)) {
            if (axis4_pos > joystick_threshold) {
                new_state = StickRight;
            }else if (axis4_pos < -joystick_threshold) {
                new_state = StickLeft;
            }
        }else {
            if (axis3_pos > joystick_threshold) {
                new_state = StickUp;
            }else if (axis3_pos < -joystick_threshold) {
                new_state = StickDown;
            }
        }
        if (new_state != rightStickState_Primary) {
            rightStickState_Primary = new_state;
            current_joystick_primary->triggerEvent(RightStick, new_state);
        }
    };

    void buttonAPressed_Partner() {
        current_joystick_partner->triggerEvent(AButton, ButtonPressed);
    };
    void buttonAReleased_Partner() {
        current_joystick_partner->triggerEvent(AButton, ButtonReleased);
    };
    void buttonBPressed_Partner() {
        current_joystick_partner->triggerEvent(BButton, ButtonPressed);
    };
    void buttonBReleased_Partner() {
        current_joystick_partner->triggerEvent(BButton, ButtonReleased);
    };
    void buttonXPressed_Partner() {
        current_joystick_partner->triggerEvent(XButton, ButtonPressed);
    };
    void buttonXReleased_Partner() {
        current_joystick_partner->triggerEvent(XButton, ButtonReleased);
    };
    void buttonYPressed_Partner() {
        current_joystick_partner->triggerEvent(YButton, ButtonPressed);
    };
    void buttonYReleased_Partner() {
        current_joystick_partner->triggerEvent(YButton, ButtonReleased);
    };
    void buttonUpPressed_Partner() {
        current_joystick_partner->triggerEvent(UpButton, ButtonPressed);
    };
    void buttonUpReleased_Partner() {
        current_joystick_partner->triggerEvent(UpButton, ButtonReleased);
    };
    void buttonDownPressed_Partner() {
        current_joystick_partner->triggerEvent(DownButton, ButtonPressed);
    };
    void buttonDownReleased_Partner() {
        current_joystick_partner->triggerEvent(DownButton, ButtonReleased);
    };
    void buttonLeftPressed_Partner() {
        current_joystick_partner->triggerEvent(LeftButton, ButtonPressed);
    };
    void buttonLeftReleased_Partner() {
        current_joystick_partner->triggerEvent(LeftButton, ButtonReleased);
    };
    void buttonRightPressed_Partner() {
        current_joystick_partner->triggerEvent(RightButton, ButtonPressed);
    };
    void buttonRightReleased_Partner() {
        current_joystick_partner->triggerEvent(RightButton, ButtonReleased);
    };
    void buttonL1Pressed_Partner() {
        current_joystick_partner->triggerEvent(L1Button, ButtonPressed);
    };
    void buttonL1Released_Partner() {
        current_joystick_partner->triggerEvent(L1Button, ButtonReleased);
    };
    void buttonL2Pressed_Partner() {
        current_joystick_partner->triggerEvent(L2Button, ButtonPressed);
    };
    void buttonL2Released_Partner() {
        current_joystick_partner->triggerEvent(L2Button, ButtonReleased);
    };
    void buttonR1Pressed_Partner() {
        current_joystick_partner->triggerEvent(R1Button, ButtonPressed);
    };
    void buttonR1Released_Partner() {
        current_joystick_partner->triggerEvent(R1Button, ButtonReleased);
    };
    void buttonR2Pressed_Partner() {
        current_joystick_partner->triggerEvent(R2Button, ButtonPressed);
    };
    void buttonR2Released_Partner() {
        current_joystick_partner->triggerEvent(R2Button, ButtonReleased);
    };

    StickEvent leftStickState_Partner{ StickCenter };
    void leftStickMoved_Partner() {
        int axis1_pos = m_controller_partner.Axis1.position(); // L/R
        int axis2_pos = m_controller_partner.Axis2.position(); // U/D
        StickEvent new_state = StickCenter;
        if (std::abs(axis1_pos) > std::abs(axis2_pos)) {
            if (axis1_pos > joystick_threshold) {
                new_state = StickRight;
            }else if (axis1_pos < -joystick_threshold) {
                new_state = StickLeft;
            }
        }else {
            if (axis2_pos > joystick_threshold) {
                new_state = StickUp;
            }else if (axis2_pos < -joystick_threshold) {
                new_state = StickDown;
            }
        }
        if (new_state != leftStickState_Partner) {
            leftStickState_Partner = new_state;
            current_joystick_partner->triggerEvent(LeftStick, new_state);
        }
    };
    StickEvent rightStickState_Partner{ StickCenter };
    void rightStickMoved_Partner() {
        int axis4_pos = m_controller_partner.Axis4.position(); // L/R
        int axis3_pos = m_controller_partner.Axis3.position(); // U/D
        StickEvent new_state = StickCenter;
        if (std::abs(axis4_pos) > std::abs(axis3_pos)) {
            if (axis4_pos > joystick_threshold) {
                new_state = StickRight;
            }else if (axis4_pos < -joystick_threshold) {
                new_state = StickLeft;
            }
        }else {
            if (axis3_pos > joystick_threshold) {
                new_state = StickUp;
            }else if (axis3_pos < -joystick_threshold) {
                new_state = StickDown;
            }
        }
        if (new_state != rightStickState_Partner) {
            rightStickState_Partner = new_state;
            current_joystick_partner->triggerEvent(RightStick, new_state);
        }
    };

    void SetAsPrimaryJoystick(Joystick* joystick) {
        current_joystick_primary = joystick;

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
    };
    void SetAsPartnerJoystick(Joystick* joystick) {
        current_joystick_partner = joystick;

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
    };
#endif

#ifdef AUTOMAT_VEX_
    Joystick::Joystick(JoystickType type):
#endif
#ifdef AUTOMAT_ESP32_
    Joystick::Joystick():
#endif
        m_triggers{ },
        m_temp_triggers{ },
        m_triggered_commands{ },
        m_command_terminations{ },
        m_stick_state{ },
        m_axis_position{ },
        m_button_state{ }
    {
        for (int i = 0; i < 2; i++) {
            m_stick_state[i] = StickCenter;
        }
        for (int i = 0; i < 12; i++) {
            m_button_state[i] = ButtonReleased;
        }
#ifdef AUTOMAT_VEX_
        if (type == PrimaryJoystick) {
            SetAsPrimaryJoystick(this);
        }else if (type == PartnerJoystick) {
            SetAsPartnerJoystick(this);
        }
#endif
    };
    Joystick::~Joystick() {
        for (Trigger* trigger : m_triggers) {
            delete trigger;
        }
        m_triggers.clear();
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
                        Command* command = new Command(*baseCommand);
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

    void Joystick::setAxis12(int axis1, int axis2) {
        m_axis_position[Axis1] = axis1;
        m_axis_position[Axis2] = axis2;
    };
    void Joystick::setAxis34(int axis3, int axis4) {
        m_axis_position[Axis3] = axis3;
        m_axis_position[Axis4] = axis4;
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
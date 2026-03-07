#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_COMMAND_BASED_

#include "Trigger.h"

#include "Joystick.h"

#include <cstring>

namespace atmt {

    Trigger::Trigger():
        m_stick{ NULLStick },
        m_stick_event{ StickNULL },
        m_button{ NULLButton },
        m_button_event{ ButtonNULL },
        m_serial_event{ NULLSerial },
        m_serial_code{ 0 },
        m_serial_code_length{ 0 },
        m_type{ OnTrigger },
        m_modes{ ModeTeleopOnly },
        m_inverted{ false },
        m_criteria{ }
    {

    };
    Trigger::Trigger(StickIndicator stick, StickEvent event):
        Trigger()
    {
        m_stick = stick;
        m_stick_event = event;
    };
    Trigger::Trigger(ButtonIndicator button, ButtonEvent event):
        Trigger()
    {
        m_button = button;
        m_button_event = event;
    };
    Trigger::Trigger(SerialEvent event, uint8_t code[], uint8_t length):
        Trigger()
    {
        m_serial_event = event;
        m_serial_code_length = std::min(length, kMaxPacketSize);
        memcpy(m_serial_code, code, length);
    };

    Trigger* Trigger::setType(TriggerType type) {
        m_type = type;
        return this;
    };
    Trigger* Trigger::setCriteria(std::shared_ptr<Trigger> criteria) {
        // if (m_criteria == nullptr) {
        //     m_criteria = criteria;
        // }else {
        //     m_criteria->setCriteria(criteria);
        // }
        m_criteria.push_back(criteria);
        return this;
    };
    Trigger* Trigger::inMode(TriggerModes modes) {
        m_modes = modes;
        return this;
    };
    Trigger* Trigger::invert() {
        m_inverted = !m_inverted;
        return this;
    };

    bool Trigger::matchesEvent(StickIndicator stick, StickEvent event, RobotState state, Joystick* joystick) {
        if (!m_inverted) {
            return (m_stick == stick && m_stick_event == event && checkMode(state) && checkCriteria(joystick));
        }else {
            return ((m_stick == stick && m_stick_event != event) || !checkMode(state) || !checkCriteria(joystick));
        }
    };
    bool Trigger::matchesEvent(ButtonIndicator button, ButtonEvent event, RobotState state, Joystick* joystick) {
        if (!m_inverted) {
            return (m_button == button && m_button_event == event && checkMode(state) && checkCriteria(joystick));
        }else {
            return ((m_button == button && m_button_event != event) || !checkMode(state) || !checkCriteria(joystick));
        }
    };
    bool Trigger::matchesEvent(SerialEvent event, uint8_t code[], uint8_t length, RobotState state) {
        if (!m_inverted) {
            return (m_serial_event == event && serialCodeMatches(code, length) && checkMode(state));
        }else {
            return ((m_serial_event == event && !serialCodeMatches(code, length)) || !checkMode(state));
        }
    };
    bool Trigger::serialCodeMatches(uint8_t code[], uint8_t length) {
        if (m_serial_code_length != length) {
            return false;
        }
        for (int i = 0; i < length; i++) {
            if (m_serial_code[i] != code[i]) {
                return false;
            }
        }
        return true;
    };

    bool Trigger::checkMode(RobotState state) {
        switch (m_modes) {
            case ModeTeleopOnly:
                return (state == Teleop);
            case ModeAutonomousOnly:
                return (state == Autonomous);
            case ModeTeleopAndAuto:
                return (state == Teleop || state == Autonomous);
            case ModeNULL:
                return false;
            default:
                return false;
        }
    };
    bool Trigger::checkCriteria(Joystick* joystick) {
        // return (m_criteria ? m_criteria->criteriaSatisfied(joystick) : true);
        for (const std::shared_ptr<Trigger> &trigger : m_criteria) {
            if (!trigger->criteriaSatisfied(joystick)) {
                return false;
            }
        }
        return true;
    };
    bool Trigger::criteriaSatisfied(Joystick* joystick) {
        if (!joystick) {
            return false;
        }
        if (m_stick != NULLStick && m_stick_event != StickNULL) {
            if (joystick->getStickState(m_stick) != m_stick_event) {
                return false;
            }
        }
        if (m_button != NULLButton && m_button_event != ButtonNULL) {
            if (joystick->getButtonState(m_button) != m_button_event) {
                return false;
            }
        }
        // return (m_criteria ? m_criteria->criteriaSatisfied(joystick) : true);
        // return true;
        return checkCriteria(joystick);
    };

    TriggerType Trigger::getTriggerType() {
        return m_type;
    };


    Trigger_Event::Trigger_Event(TriggerEffect effect, Trigger* trigger):
        m_effect{ effect },
        m_trigger{ trigger },
        m_command{ nullptr },
        m_command_id{ -1 }
    {

    };
    Trigger_Event::Trigger_Event(TriggerEffect effect, Trigger* trigger, Command* command):
        Trigger_Event(effect, trigger)
    {
        m_command = command;
    };
    Trigger_Event::Trigger_Event(TriggerEffect effect, Trigger* trigger, int command_id):
        Trigger_Event(effect, trigger)
    {
        m_command_id = command_id;
    };
    Trigger_Event::~Trigger_Event() {
        delete m_trigger;
        m_trigger = nullptr;
    };

    bool Trigger_Event::matchesEvent(StickIndicator stick, StickEvent event, RobotState state, Joystick* joystick) {
        return m_trigger->matchesEvent(stick, event, state, joystick);
    };
    bool Trigger_Event::matchesEvent(ButtonIndicator button, ButtonEvent event, RobotState state, Joystick* joystick) {
        return m_trigger->matchesEvent(button, event, state, joystick);
    };
    bool Trigger_Event::matchesEvent(SerialEvent event, uint8_t code[], uint8_t length, RobotState state) {
        return m_trigger->matchesEvent(event, code, length, state);
    };

    TriggerEffect Trigger_Event::getTriggerEffect() {
        return m_effect;
    };
    Command* Trigger_Event::getCommand() {
        return m_command;
    };
    TriggerType Trigger_Event::getTriggerType() {
        return m_trigger->getTriggerType();
    };
    int Trigger_Event::getCommandId() {
        return m_command_id;
    };
    Trigger* Trigger_Event::getTrigger() {
        return m_trigger;
    };

};

#else
#error "Enable ATMT_SUBMODULE_COMMAND_BASED_ in automat_submodules.h to use Trigger, Trigger_Event"
#endif
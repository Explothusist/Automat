
#include "Trigger.h"

namespace atmt {

    Trigger::Trigger(TriggerEffect effect):
        m_effect{ effect },
        m_type{ OnTrigger },
        m_command{ nullptr },
        m_command_id{ -1 }
    {

    };
    Trigger::Trigger(TriggerEffect effect, TriggerType type, Command* command):
        m_effect{ effect },
        m_type{ type },
        m_command{ command },
        m_command_id{ -1 }
    {

    };
    Trigger::Trigger(TriggerEffect effect, int command_id):
        m_effect{ effect },
        m_type{ OnTrigger },
        m_command{ nullptr },
        m_command_id{ command_id }
    {

    };
    Trigger::~Trigger() {
        delete m_command;
        m_command = nullptr;
    };
    bool Trigger::matchesEvent(StickIndicator stick, StickEvent event) { return false; };
    bool Trigger::matchesEvent(ButtonIndicator button, ButtonEvent event) { return false; };
    Command* Trigger::getCommand() { return m_command; };
    TriggerType Trigger::getTriggerType() { return m_type; };
    TriggerEffect Trigger::getTriggerEffect() { return m_effect; };
    int Trigger::getCommandId() { return m_command_id; };

    StickTrigger::StickTrigger(TriggerEffect effect, StickIndicator stick, StickEvent event, Command* command):
        Trigger(effect, OnTrigger, command),
        m_stick{ stick },
        m_event{ event }
    {

    };
    StickTrigger::StickTrigger(TriggerEffect effect, StickIndicator stick, StickEvent event, TriggerType type, Command* command):
        Trigger(effect, type, command),
        m_stick{ stick },
        m_event{ event }
    {

    };
    StickTrigger::StickTrigger(TriggerEffect effect, StickIndicator stick, StickEvent event, int command_id):
        Trigger(effect, command_id),
        m_stick{ stick },
        m_event{ event }
    {

    };
    StickTrigger::StickTrigger(TriggerEffect effect, StickIndicator stick, StickEvent event):
        Trigger(effect),
        m_stick{ stick },
        m_event{ event }
    {

    };
    StickTrigger::~StickTrigger() {
        // delete m_command;
        // m_command = nullptr;
    };

    bool StickTrigger::matchesEvent(StickIndicator stick, StickEvent event) {
        if (m_stick == stick && m_event == event) {
            return true;
        }
        return false;
    };
    bool StickTrigger::matchesEvent(ButtonIndicator button, ButtonEvent event) {
        return false;
    };

    ButtonTrigger::ButtonTrigger(TriggerEffect effect, ButtonIndicator button, ButtonEvent event, Command* command):
        Trigger(effect, OnTrigger, command),
        m_button{ button },
        m_event{ event }
    {

    };
    ButtonTrigger::ButtonTrigger(TriggerEffect effect, ButtonIndicator button, ButtonEvent event, TriggerType type, Command* command):
        Trigger(effect, type, command),
        m_button{ button },
        m_event{ event }
    {

    };
    ButtonTrigger::ButtonTrigger(TriggerEffect effect, ButtonIndicator button, ButtonEvent event, int command_id):
        Trigger(effect, command_id),
        m_button{ button },
        m_event{ event }
    {

    };
    ButtonTrigger::ButtonTrigger(TriggerEffect effect, ButtonIndicator button, ButtonEvent event):
        Trigger(effect),
        m_button{ button },
        m_event{ event }
    {

    };
    ButtonTrigger::~ButtonTrigger() {};

    bool ButtonTrigger::matchesEvent(StickIndicator stick, StickEvent event) {
        return false;
    };
    bool ButtonTrigger::matchesEvent(ButtonIndicator button, ButtonEvent event) {
        if (m_button == button && m_event == event) {
            return true;
        }
        return false;
    };



    // EndingTrigger::EndingTrigger(int command_id):
    //     m_command_id{ command_id }
    // {

    // };
    // EndingTrigger::~EndingTrigger() {};
    // bool EndingTrigger::matchesEvent(StickIndicator stick, StickEvent event) { return false; };
    // bool EndingTrigger::matchesEvent(ButtonIndicator button, ButtonEvent event) { return false; };

    // int EndingTrigger::getCommandId() {
    //     return m_command_id;
    // };



    StickEndingTrigger::StickEndingTrigger(TriggerEffect effect, StickIndicator stick, StickEvent event, Command* command):
        StickTrigger(effect, stick, event, command)
    {

    };
    StickEndingTrigger::StickEndingTrigger(TriggerEffect effect, StickIndicator stick, StickEvent event, TriggerType type, Command* command):
        StickTrigger(effect, stick, event, type, command)
    {

    };
    StickEndingTrigger::StickEndingTrigger(TriggerEffect effect, StickIndicator stick, StickEvent event, int command_id):
        StickTrigger(effect, stick, event, command_id)
    {

    };
    StickEndingTrigger::StickEndingTrigger(TriggerEffect effect, StickTrigger* trigger, int command_id):
        StickTrigger(effect, trigger->m_stick, trigger->m_event, command_id)
    {

    };
    StickEndingTrigger::StickEndingTrigger(TriggerEffect effect, StickIndicator stick, StickEvent event):
        StickTrigger(effect, stick, event)
    {

    };
    StickEndingTrigger::~StickEndingTrigger() {};

    bool StickEndingTrigger::matchesEvent(StickIndicator stick, StickEvent event) {
        if (m_stick == stick && m_event != event) { // != is the key to ending trigger
            return true;
        }
        return false;
    };
    bool StickEndingTrigger::matchesEvent(ButtonIndicator button, ButtonEvent event) {
        return false;
    };

    ButtonEndingTrigger::ButtonEndingTrigger(TriggerEffect effect, ButtonIndicator button, ButtonEvent event, Command* command):
        ButtonTrigger(effect, button, event, command)
    {

    };
    ButtonEndingTrigger::ButtonEndingTrigger(TriggerEffect effect, ButtonIndicator button, ButtonEvent event, TriggerType type, Command* command):
        ButtonTrigger(effect, button, event, type, command)
    {

    };
    ButtonEndingTrigger::ButtonEndingTrigger(TriggerEffect effect, ButtonIndicator button, ButtonEvent event, int command_id):
        ButtonTrigger(effect, button, event, command_id)
    {

    };
    ButtonEndingTrigger::ButtonEndingTrigger(TriggerEffect effect, ButtonTrigger* trigger, int command_id):
        ButtonTrigger(effect, trigger->m_button, trigger->m_event, command_id)
    {

    };
    ButtonEndingTrigger::ButtonEndingTrigger(TriggerEffect effect, ButtonIndicator button, ButtonEvent event):
        ButtonTrigger(effect, button, event)
    {

    };
    ButtonEndingTrigger::~ButtonEndingTrigger() {};

    bool ButtonEndingTrigger::matchesEvent(StickIndicator stick, StickEvent event) {
        return false;
    };
    bool ButtonEndingTrigger::matchesEvent(ButtonIndicator button, ButtonEvent event) {
        if (m_button == button && m_event != event) { // != is the key to ending trigger
            return true;
        }
        return false;
    };


};
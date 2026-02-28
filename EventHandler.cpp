
#include "EventHandler.h"
#include "Joystick.h"
#include "SerialReader.h"
#include "TimedRobot.h"

#ifdef AUTOMAT_VEX_
#include "vex.h"
#endif

#include <cmath>

namespace atmt {
    
    void SetReadEvents(bool to_read) {
        SetReadJoystickEvents(to_read);
        SetReadSerialEvents(to_read);
    };

    EventHandler::EventHandler():
        m_triggered_commands{ std::vector<Command*>() },
        m_command_terminations{ std::vector<int>() },
        m_autonomous_triggered{ false }//,
        // m_robot{ nullptr },
        // m_robot_state{ nullptr }
    {
        
    };
    EventHandler::~EventHandler() {

    };

    // void EventHandler::init(RobotState* robot_state, TimedRobot* robot) {
    void EventHandler::init() {
        // m_robot = robot;
        // m_robot_state = robot_state;
    };

    std::vector<Command*> EventHandler::pollEvents() {
        std::vector<Command*> commands = m_triggered_commands;
        m_triggered_commands.clear();
        return commands;
    };
    std::vector<int> EventHandler::pollEventTerminations() {
        std::vector<int> commands = m_command_terminations;
        m_command_terminations.clear();
        return commands;
    };
    bool EventHandler::pollAutonomousTriggers() {
        bool triggered = m_autonomous_triggered;
        m_autonomous_triggered = false;
        return triggered;
    };

    Trigger_Event* EventHandler::interpretTrigger(Trigger_Event* trigger, bool is_stick) {
        switch (trigger->getTriggerEffect()) {
            case StartCommand:
                {
                    Command* baseCommand = trigger->getCommand();
                    if (baseCommand != nullptr) {
                        Command* command = baseCommand->clone();
                        // command->setId(global_command_id_counter);
                        // global_command_id_counter += 1;
                        assignCommandId(command);
                        m_triggered_commands.push_back(command);

                        if (trigger->getTriggerType() == WhileTrigger) {
                            // if (is_stick) {
                                // m_temp_triggers.push_back(new StickEndingTrigger(EndCommand, static_cast<StickTrigger*>(trigger), command->getId()));
                                // m_temp_triggers.push_back(new Trigger_Event(EndCommand, static_cast<StickTrigger*>(trigger), command->getId()));
                                Trigger* copy = new Trigger(*trigger->getTrigger());
                                // m_temp_triggers.push_back(new Trigger_Event(EndCommand, copy->invert(), command->getId()));
                                return new Trigger_Event(EndCommand, copy->invert(), command->getId());
                            // }else {
                            //     Trigger* copy = new Trigger(*trigger->getTrigger());
                            //     // m_temp_triggers.push_back(new Trigger_Event(EndCommand, copy->invert(), command->getId()));
                            //     return new Trigger_Event(EndCommand, copy->invert(), command->getId());
                            // }
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
        return nullptr;
    };
    int EventHandler::assignCommandId(Command* command) {
        // Set Id for later reference
        int command_id = m_command_id_counter;
        command->setId(command_id);
        m_command_id_counter += 1;

        return command_id;
    };

};
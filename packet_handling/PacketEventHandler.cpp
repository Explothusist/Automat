#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_PACKET_HANDLING_

#include "PacketEventHandler.h"

#ifdef ATMT_SUBMODULE_COMMAND_BASED_
#include "../command_based/EventHandler.h"
#endif

#include <cstring>
#include <string>

namespace atmt {

    PacketEventHandler::PacketEventHandler():
#ifdef ATMT_SUBMODULE_COMMAND_BASED_
        m_triggers{ std::vector<Trigger_Event*>() },
        m_temp_triggers{ std::vector<Trigger_Event*>() },
        m_robot_state{ nullptr },
        m_event_handler{ nullptr }
#endif
    {
        
    };
    PacketEventHandler::~PacketEventHandler() {
#ifdef ATMT_SUBMODULE_COMMAND_BASED_
        for (Trigger_Event* trigger : m_triggers) {
            delete trigger;
        }
        m_triggers.clear();
        for (Trigger_Event* trigger : m_temp_triggers) {
            delete trigger;
        }
        m_temp_triggers.clear();
#endif
    };

    void PacketEventHandler::init() {
        
    };
    void PacketEventHandler::periodic() {
        
    };
    
#ifdef ATMT_SUBMODULE_COMMAND_BASED_
    void PacketEventHandler::internal_init(RobotState* robot_state, EventHandler* event_handler) {
        m_event_handler = event_handler;
        m_robot_state = robot_state;
    };
    
    void PacketEventHandler::triggerEvent(SerialEvent event, uint8_t sender, uint8_t code[], uint8_t length, int id) {
        if (!m_robot_state) { // Uninitialized
            return;
        }
        for (size_t i = 0; i < m_temp_triggers.size(); ) {
            if (m_temp_triggers[i]->matchesEvent(event, sender, code, length, *m_robot_state)) {
                m_temp_triggers[i]->setSerialMessageId(id);
                Trigger_Event* temp_trigger = m_event_handler->interpretTrigger(m_temp_triggers[i], true);
                if (temp_trigger) {
                    m_temp_triggers.push_back(temp_trigger);
                }
                
                delete m_temp_triggers[i];
                vectorDeleteUnordered(m_temp_triggers, i);
            }else {
                i += 1;
            }
        }

        for (Trigger_Event* trigger : m_triggers) {
            if (trigger->matchesEvent(event, sender, code, length, *m_robot_state)) {
                trigger->setSerialMessageId(id);
                Trigger_Event* temp_trigger = m_event_handler->interpretTrigger(trigger, true);
                if (temp_trigger) {
                    m_temp_triggers.push_back(temp_trigger);
                }
            }
        }
    };

    void PacketEventHandler::bindToMessage(Trigger* trigger, Command* command) {
        m_triggers.push_back(new Trigger_Event(StartCommand, trigger, command));
    };
    void PacketEventHandler::bindAutoTrigger(Trigger* trigger) {
        m_triggers.push_back(new Trigger_Event(StartAutonomous, trigger->inMode(ModeDisabled)));
    };
    void PacketEventHandler::bindTeleopTrigger(Trigger* trigger) {
        m_triggers.push_back(new Trigger_Event(StartTeleop, trigger->inMode(ModeDisabledAndAuto)));
    };
    void PacketEventHandler::bindDisabledTrigger(Trigger* trigger) {
        m_triggers.push_back(new Trigger_Event(ForceDisabled, trigger->inMode(ModeTeleopAndAuto)));
    };
#endif
    
};

#else
// #error "Enable ATMT_SUBMODULE_PACKET_HANDLING_ in automat_submodules.h to use PacketEventHandler"
#endif
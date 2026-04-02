#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_COMMAND_BASED_

#include "Heartbeat.h"
#include "command_helpers/InstantCommand.h"
#include "utils.h"

namespace atmt {
    
    Heartbeat::Heartbeat(int timeout):
        m_heartbeat_timeout{ timeout },
        m_last_heartbeat{ getSystemTime() },
        m_type{ HeartbeatManual },
        m_is_state_controlling{ false }
    {

    }; // Manual
#ifdef ATMT_SUBMODULE_SERIAL_
    Heartbeat::Heartbeat(int timeout, SerialReader* serial, uint8_t message):
        m_heartbeat_timeout{ timeout },
        m_last_heartbeat{ getSystemTime() },
        m_type{ HeartbeatSerial },
        m_is_state_controlling{ false }
    {
        serial->bindToMessage(
            new Trigger(SerialReceive, message),
            new InstantCommand({}, &Heartbeat::beatHeart, this)
        );
    }; // Serial
    Heartbeat::Heartbeat(int timeout, SerialReader* serial, uint8_t message, uint8_t sender):
        m_heartbeat_timeout{ timeout },
        m_last_heartbeat{ getSystemTime() },
        m_type{ HeartbeatSerial },
        m_serial{ serial },
        m_serial_message{ message },
        m_is_state_controlling{ false }
    {
        serial->bindToMessage(
            (new Trigger(SerialReceive, message))->fromSender(sender),
            new InstantCommand({}, &Heartbeat::beatHeart, this)
        );
    }; // Serial
#endif
#ifdef ATMT_SUBMODULE_HTTP_SERVER_
    Heartbeat::Heartbeat(int timeout, RobotDashboardServer* server):
        m_heartbeat_timeout{ timeout },
        m_last_heartbeat{ getSystemTime() },
        m_type{ HeartbeatServer },
        m_server{ server },
        m_is_state_controlling{ false }
    {

    }; // Serial
#endif

    void Heartbeat::beatHeart() {
        m_last_heartbeat = getSystemTime();
        if (m_type == HeartbeatSerial) {
            m_serial->popNextMessage(); // Keep from polluting the message stack
        }
    };
    bool Heartbeat::isHeartbeatLost() {
        return m_last_heartbeat.getTimeDifferenceMS(getSystemTime()) > m_heartbeat_timeout;
    };

    bool Heartbeat::isStateControlling() {
        return m_is_state_controlling;
    };
    RobotState Heartbeat::getState() {
        return Disabled; // For state-controlling heartbeats
    };

};

#else
// #error "Enable ATMT_SUBMODULE_COMMAND_BASED_ in automat_submodules.h to use EventHandler"
#endif
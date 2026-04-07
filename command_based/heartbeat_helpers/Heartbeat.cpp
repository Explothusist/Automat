#include "../../automat_submodules.h"
#ifdef ATMT_SUBMODULE_COMMAND_BASED_

#include "Heartbeat.h"
#include "HeartbeatCommand.h"
#include "../command_helpers/InstantCommand.h"
#include "../../utils.h"

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
        m_serial{ serial },
        m_is_state_controlling{ false }
    {
        serial->bindToMessage(
            (new Trigger(SerialReceive, message))->allowPartial()->inMode(ModeAnyAndAll),
            // new InstantCommand({}, &Heartbeat::beatHeart, this)
            new HeartbeatCommand(this)
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
            (new Trigger(SerialReceive, message))->fromSender(sender)->allowPartial()->inMode(ModeAnyAndAll),
            // new InstantCommand({}, &Heartbeat::beatHeart, this)
            new HeartbeatCommand(this)
        );
    }; // Serial
#endif
#ifdef ATMT_SUBMODULE_HTTP_SERVER_
    Heartbeat::Heartbeat(int timeout, RobotDashboardServer* server):
        m_heartbeat_timeout{ timeout },
        m_last_heartbeat{ getSystemTime() },
        m_type{ HeartbeatHTTP },
        m_server{ server },
        m_is_state_controlling{ false }
    {

    }; // Serial
#endif

    void Heartbeat::beatHeart() {
        m_last_heartbeat = getSystemTime();
#ifdef ATMT_SUBMODULE_SERIAL_
        if (m_type == HeartbeatSerial) {
            m_serial->popMessage(m_last_serial_id); // Keep from polluting the message stack
        }
#endif
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

    
    bool Heartbeat::isSerial() {
#ifdef ATMT_SUBMODULE_SERIAL_
        return m_type == HeartbeatSerial;
#else   
        return false;
#endif
    };
    void Heartbeat::setLastSerialId(int id) {
#ifdef ATMT_SUBMODULE_SERIAL_
        if (m_type == HeartbeatSerial) {
            m_last_serial_id = id;
        }
#endif
    };

};

#else
// #error "Enable ATMT_SUBMODULE_COMMAND_BASED_ in automat_submodules.h to use EventHandler"
#endif
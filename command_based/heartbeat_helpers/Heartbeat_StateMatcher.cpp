#include "../../automat_submodules.h"
#ifdef ATMT_SUBMODULE_COMMAND_BASED_

#include "Heartbeat_StateMatcher.h"
#include "../command_helpers/InstantCommand.h"
#include "../../utils.h"

namespace atmt {
    
    Heartbeat_StateMatcher::Heartbeat_StateMatcher(int timeout):
        Heartbeat(timeout),
        m_last_state{ Disabled }
    {
        m_is_state_controlling = true;
    }; // Manual
#ifdef ATMT_SUBMODULE_SERIAL_
    Heartbeat_StateMatcher::Heartbeat_StateMatcher(int timeout, SerialReader* serial, uint8_t message):
        Heartbeat(timeout),
        m_last_state{ Disabled }
    {
        m_is_state_controlling = true;
        serial->bindToMessage(
            new Trigger(SerialReceive, message),
            new InstantCommand({}, &Heartbeat_StateMatcher::beatHeart, this)
        );
    }; // Serial
    Heartbeat_StateMatcher::Heartbeat_StateMatcher(int timeout, SerialReader* serial, uint8_t message, uint8_t sender):
        Heartbeat(timeout),
        m_last_state{ Disabled }
    {
        m_is_state_controlling = true;
        m_type = HeartbeatSerial;
        serial->bindToMessage(
            (new Trigger(SerialReceive, message))->fromSender(sender),
            new InstantCommand({}, &Heartbeat_StateMatcher::beatHeart, this)
        );
    }; // Serial
#endif
#ifdef ATMT_SUBMODULE_HTTP_SERVER_
    Heartbeat_StateMatcher::Heartbeat_StateMatcher(int timeout, RobotDashboardServer* server):
        Heartbeat(timeout),
        m_last_state{ Disabled }
    {
        m_is_state_controlling = true;
        m_type = HeartbeatHTTP;
    }; // Serial
#endif

    void Heartbeat_StateMatcher::beatHeart() {
        m_last_heartbeat = getSystemTime();
        if (m_type == HeartbeatSerial) {
            uint8_t output[kMaxPacketSize];
            uint8_t length = 0;
            m_serial->popNextMessage(output, length); // Keep from polluting the message stack
            if (length == 2) {
                m_last_state = static_cast<RobotState>(output[1]);
            }
        }
    };
    bool Heartbeat_StateMatcher::isHeartbeatLost() {
        return m_last_heartbeat.getTimeDifferenceMS(getSystemTime()) > m_heartbeat_timeout;
    };

    RobotState Heartbeat_StateMatcher::getState() {
        return isHeartbeatLost() ? Disabled : m_last_state;
    };

};

#else
// #error "Enable ATMT_SUBMODULE_COMMAND_BASED_ in automat_submodules.h to use EventHandler"
#endif
#include "../../automat_submodules.h"
#ifdef ATMT_SUBMODULE_COMMAND_BASED_

#include "Heartbeat_StateMatcher.h"
#include "HeartbeatCommand.h"
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
        Heartbeat(timeout, serial, message),
        // Heartbeat(timeout),
        m_last_state{ Disabled }//,
        // m_serial{ serial }
    {
        m_is_state_controlling = true;
        // m_type = HeartbeatSerial;
        // serial->bindToMessage(
        //     (new Trigger(SerialReceive, message))->allowPartial()->inMode(ModeAnyAndAll),
        //     // new InstantCommand({}, &Heartbeat::beatHeart, this)
        //     new HeartbeatCommand(this)
        // );
    }; // Serial
    Heartbeat_StateMatcher::Heartbeat_StateMatcher(int timeout, SerialReader* serial, uint8_t message, uint8_t sender):
        Heartbeat(timeout, serial, message),
        // Heartbeat(timeout),
        m_last_state{ Disabled }
    {
        m_is_state_controlling = true;
        // m_type = HeartbeatSerial;
        // serial->bindToMessage(
        //     (new Trigger(SerialReceive, message))->allowPartial()->inMode(ModeAnyAndAll)->fromSender(sender),
        //     // new InstantCommand({}, &Heartbeat::beatHeart, this)
        //     new HeartbeatCommand(this)
        // );
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
        atmt::platform_println("Heartbeat Received");
        m_last_heartbeat = getSystemTime();
#ifdef ATMT_SUBMODULE_SERIAL_
        if (m_type == HeartbeatSerial) {
            uint8_t output[kMaxPacketSize];
            uint8_t length = 0;
            m_serial->popMessage(m_last_serial_id, output, length); // Keep from polluting the message stack
            if (length == 2) {
                m_last_state = static_cast<RobotState>(output[1]);
            }
        }
#endif
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
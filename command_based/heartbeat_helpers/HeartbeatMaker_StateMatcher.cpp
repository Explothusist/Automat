#include "../../automat_submodules.h"
#ifdef ATMT_SUBMODULE_COMMAND_BASED_

#include "HeartbeatMaker_StateMatcher.h"
#include "../command_helpers/InstantCommand.h"
#include "../../utils.h"

namespace atmt {
    
    HeartbeatMaker_StateMatcher::HeartbeatMaker_StateMatcher(int timeout):
        HeartbeatMaker(timeout)
    {
        m_is_state_controlling = true;

    }; // Manual
#ifdef ATMT_SUBMODULE_SERIAL_
    HeartbeatMaker_StateMatcher::HeartbeatMaker_StateMatcher(int timeout, SerialReader* serial, uint8_t message):
        HeartbeatMaker_StateMatcher(timeout, serial, message, KSerialAddressSendAll)
    {
        
    }; // Serial
    HeartbeatMaker_StateMatcher::HeartbeatMaker_StateMatcher(int timeout, SerialReader* serial, uint8_t message, uint8_t recipient):
        HeartbeatMaker(timeout, serial, message, recipient)
    {
        m_is_state_controlling = true;
        
    }; // Serial
#endif
#ifdef ATMT_SUBMODULE_HTTP_SERVER_
    HeartbeatMaker_StateMatcher::HeartbeatMaker_StateMatcher(int timeout, RobotDashboardServer* server):
        HeartbeatMaker(timeout, server)
    {
        m_is_state_controlling = true;

    }; // Serial
#endif

    void HeartbeatMaker_StateMatcher::beatHeart(RobotState state) {
        switch (m_type) {
            case HeartbeatManual:
                break;
#ifdef ATMT_SUBMODULE_SERIAL_
            case HeartbeatSerial:
                m_serial->sendMessagePrefixed(m_serial_recipient, m_serial_message, static_cast<uint8_t>(state), 1);
                break;
#endif
#ifdef ATMT_SUBMODULE_HTTP_SERVER_
            case HeartbeatHTTP:
                break;
#endif
        }
    };

    void HeartbeatMaker_StateMatcher::runLoop(RobotState state) {
        if (m_last_heartbeat.getTimeDifferenceMS(getSystemTime()) > m_heartbeat_timeout) {
            beatHeart(state);
            m_last_heartbeat = getSystemTime();
        }
    };

};

#else
// #error "Enable ATMT_SUBMODULE_COMMAND_BASED_ in automat_submodules.h to use EventHandler"
#endif
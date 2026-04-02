#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_COMMAND_BASED_

#include "HeartbeatMaker.h"
#include "command_helpers/InstantCommand.h"
#include "utils.h"

namespace atmt {
    
    HeartbeatMaker::HeartbeatMaker(int timeout):
        m_heartbeat_timeout{ timeout },
        m_last_heartbeat{ getSystemTime() },
        m_type{ HeartbeatManual }
    {

    }; // Manual
#ifdef ATMT_SUBMODULE_SERIAL_
    HeartbeatMaker::HeartbeatMaker(int timeout, SerialReader* serial, uint8_t message):
        HeartbeatMaker(timeout, serial, message, KSerialAddressSendAll)
    {
        
    }; // Serial
    HeartbeatMaker::HeartbeatMaker(int timeout, SerialReader* serial, uint8_t message, uint8_t recipient):
        m_heartbeat_timeout{ timeout },
        m_last_heartbeat{ getSystemTime() },
        m_type{ HeartbeatSerial },
        m_serial{ serial },
        m_serial_message{ message },
        m_serial_recipient{ recipient }
    {
        
    }; // Serial
#endif
#ifdef ATMT_SUBMODULE_HTTP_SERVER_
    HeartbeatMaker::HeartbeatMaker(int timeout, RobotDashboardServer* server):
        m_heartbeat_timeout{ timeout },
        m_last_heartbeat{ getSystemTime() },
        m_type{ HeartbeatServer },
        m_server{ server }
    {

    }; // Serial
#endif

    void HeartbeatMaker::beatHeart() {
        switch (m_type) {
            case HeartbeatManual:
                break;
#ifdef ATMT_SUBMODULE_SERIAL_
            case HeartbeatSerial:
                m_serial->sendMessage(m_serial_recipient, m_serial_message);
                break;
#endif
#ifdef ATMT_SUBMODULE_HTTP_SERVER_
            case HeartbeatServer:
                break;
#endif
        }
    };

    void HeartbeatMaker::runLoop() {
        if (m_last_heartbeat.getTimeDifferenceMS(getSystemTime()) > m_heartbeat_timeout) {
            beatHeart();
            m_last_heartbeat = getSystemTime();
        }
    };

};

#else
// #error "Enable ATMT_SUBMODULE_COMMAND_BASED_ in automat_submodules.h to use EventHandler"
#endif
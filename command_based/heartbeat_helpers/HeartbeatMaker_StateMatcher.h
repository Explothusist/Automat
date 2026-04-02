#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_COMMAND_BASED_

#ifndef AUTOMAT_ROBOT_HEARTBEAT_MAKER_STATE_MATCHER_
#define AUTOMAT_ROBOT_HEARTBEAT_MAKER_STATE_MATCHER_

#include "HeartbeatMaker.h"
#include "../utils.h"

#ifdef ATMT_SUBMODULE_SERIAL_
#include "../serial/SerialReader.h"
#endif
#ifdef ATMT_SUBMODULE_HTTP_SERVER_
#include "../http_server/RobotDashboardServer.h"
#endif


namespace atmt {

    class HeartbeatMaker_StateMatcher : public HeartbeatMaker {
        public:
            HeartbeatMaker_StateMatcher(int timeout); // Manual (which has no structure right now)
#ifdef ATMT_SUBMODULE_SERIAL_
            HeartbeatMaker_StateMatcher(int timeout, SerialReader* serial, uint8_t message); // Serial
            HeartbeatMaker_StateMatcher(int timeout, SerialReader* serial, uint8_t message, uint8_t recipient); // Serial
#endif
#ifdef ATMT_SUBMODULE_HTTP_SERVER_
            HeartbeatMaker_StateMatcher(int timeout, RobotDashboardServer* server); // Serial
#endif

            void beatHeart() override;
            // void runLoop() override;

        private:
            int m_heartbeat_timeout; // ms
            Timestamp m_last_heartbeat;
            HeartbeatType m_type;

#ifdef ATMT_SUBMODULE_SERIAL_
            SerialReader* m_serial;
            uint8_t m_serial_message;
            uint8_t m_serial_recipient;
#endif
#ifdef ATMT_SUBMODULE_HTTP_SERVER_
            RobotDashboardServer* m_server;
#endif

    };

};

#endif

#else
#error "Enable ATMT_SUBMODULE_COMMAND_BASED_ in automat_submodules.h to use Hearbeat"
#endif
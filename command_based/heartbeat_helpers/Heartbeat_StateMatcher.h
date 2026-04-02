#include "../../automat_submodules.h"
#ifdef ATMT_SUBMODULE_COMMAND_BASED_

#ifndef AUTOMAT_ROBOT_HEARTBEAT_STATE_MATCHER_
#define AUTOMAT_ROBOT_HEARTBEAT_STATE_MATCHER_

#include "../../utils.h"
#include "Heartbeat.h"

#ifdef ATMT_SUBMODULE_SERIAL_
#include "../../serial/SerialReader.h"
#endif
#ifdef ATMT_SUBMODULE_HTTP_SERVER_
#include "../../http_server/RobotDashboardServer.h"
#endif


namespace atmt {

    class Heartbeat_StateMatcher : public Heartbeat {
        public:
            Heartbeat_StateMatcher(int timeout); // Manual
#ifdef ATMT_SUBMODULE_SERIAL_
            Heartbeat_StateMatcher(int timeout, SerialReader* serial, uint8_t message); // Serial
            Heartbeat_StateMatcher(int timeout, SerialReader* serial, uint8_t message, uint8_t sender); // Serial
#endif
#ifdef ATMT_SUBMODULE_HTTP_SERVER_
            Heartbeat_StateMatcher(int timeout, RobotDashboardServer* server); // Serial
#endif

            void beatHeart() override;
            bool isHeartbeatLost() override;

            RobotState getState() override;

        private:
            RobotState m_last_state;
    };

};

#endif

#else
#error "Enable ATMT_SUBMODULE_COMMAND_BASED_ in automat_submodules.h to use Hearbeat"
#endif
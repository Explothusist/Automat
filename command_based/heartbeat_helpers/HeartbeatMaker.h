#include "../../automat_submodules.h"
#ifdef ATMT_SUBMODULE_COMMAND_BASED_

#ifndef AUTOMAT_ROBOT_HEARTBEAT_MAKER_
#define AUTOMAT_ROBOT_HEARTBEAT_MAKER_

#include "Heartbeat.h"
#include "../../utils.h"

#ifdef ATMT_SUBMODULE_SERIAL_
#include "../../serial/SerialReader.h"
#endif
#ifdef ATMT_SUBMODULE_HTTP_SERVER_
#include "../../http_server/RobotDashboardServer.h"
#endif


namespace atmt {

    class HeartbeatMaker {
        public:
            HeartbeatMaker(int timeout); // Manual (which has no structure right now)
#ifdef ATMT_SUBMODULE_SERIAL_
            HeartbeatMaker(int timeout, SerialReader* serial, uint8_t message); // Serial
            HeartbeatMaker(int timeout, SerialReader* serial, uint8_t message, uint8_t recipient); // Serial
#endif
#ifdef ATMT_SUBMODULE_HTTP_SERVER_
            HeartbeatMaker(int timeout, RobotDashboardServer* server); // Serial
#endif

            void beatHeart();
            virtual void runLoop(RobotState state);

            bool isStateControlling();
            // void stateControllingInit(RobotState* state);

        protected:
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
            bool m_is_state_controlling;
            // RobotState* m_state;

        private:

    };

};

#endif

#else
#error "Enable ATMT_SUBMODULE_COMMAND_BASED_ in automat_submodules.h to use Hearbeat"
#endif
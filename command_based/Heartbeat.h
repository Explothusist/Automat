#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_COMMAND_BASED_

#ifndef AUTOMAT_ROBOT_HEARTBEAT_
#define AUTOMAT_ROBOT_HEARTBEAT_

#include "../utils.h"

#ifdef ATMT_SUBMODULE_SERIAL_
#include "../serial/SerialReader.h"
#endif
#ifdef ATMT_SUBMODULE_HTTP_SERVER_
#include "../http_server/RobotDashboardServer.h"
#endif


namespace atmt {

    typedef enum HeartbeatType {
        HeartbeatManual = 0,
#ifdef ATMT_SUBMODULE_SERIAL_
        HeartbeatSerial = 1,
#endif
#ifdef ATMT_SUBMODULE_HTTP_SERVER_
        HeartbeatHTTP = 2,
#endif
    };

    class Heartbeat {
        public:
            Heartbeat(int timeout); // Manual
#ifdef ATMT_SUBMODULE_SERIAL_
            Heartbeat(int timeout, SerialReader* serial, uint8_t message); // Serial
#endif
#ifdef ATMT_SUBMODULE_HTTP_SERVER_
            Heartbeat(int timeout, RobotDashboardServer* serial); // Serial
#endif

            void beatHeart();
            bool isHeartbeatLost();

        private:
            int m_heartbeat_timeout; // ms
            Timestamp m_last_heartbeat;
            HeartbeatType m_type;

#ifdef ATMT_SUBMODULE_SERIAL_
            SerialReader* m_serial;
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
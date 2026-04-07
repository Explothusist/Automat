#include "../../automat_submodules.h"
#ifdef ATMT_SUBMODULE_COMMAND_BASED_

#ifndef AUTOMAT_ROBOT_HEARTBEAT_
#define AUTOMAT_ROBOT_HEARTBEAT_

#include "../../utils.h"

#ifdef ATMT_SUBMODULE_SERIAL_
#include "../../serial/SerialReader.h"
#endif
#ifdef ATMT_SUBMODULE_HTTP_SERVER_
#include "../../http_server/RobotDashboardServer.h"
#endif


namespace atmt {

    typedef enum {
        HeartbeatManual = 0,
#ifdef ATMT_SUBMODULE_SERIAL_
        HeartbeatSerial = 1,
#endif
#ifdef ATMT_SUBMODULE_HTTP_SERVER_
        HeartbeatHTTP = 2,
#endif
    } HeartbeatType;

    class Heartbeat {
        public:
            Heartbeat(int timeout); // Manual
#ifdef ATMT_SUBMODULE_SERIAL_
            Heartbeat(int timeout, SerialReader* serial, uint8_t message); // Serial
            Heartbeat(int timeout, SerialReader* serial, uint8_t message, uint8_t sender); // Serial
#endif
#ifdef ATMT_SUBMODULE_HTTP_SERVER_
            Heartbeat(int timeout, RobotDashboardServer* server); // Serial
#endif

            virtual void beatHeart();
            virtual bool isHeartbeatLost();

            bool isStateControlling();
            virtual RobotState getState();

            bool isSerial();
            void setLastSerialId(int id);

        protected:
            int m_heartbeat_timeout; // ms
            Timestamp m_last_heartbeat;
            HeartbeatType m_type;

#ifdef ATMT_SUBMODULE_SERIAL_
            SerialReader* m_serial;
            uint8_t m_serial_message;
            int m_last_serial_id;
#endif
#ifdef ATMT_SUBMODULE_HTTP_SERVER_
            RobotDashboardServer* m_server;
#endif

            bool m_is_state_controlling;
        
        private:

    };

};

#endif

#else
#error "Enable ATMT_SUBMODULE_COMMAND_BASED_ in automat_submodules.h to use Hearbeat"
#endif
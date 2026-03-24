#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_HTTP_SERVER_CAMERA_STREAMING_SERVER_

#ifndef AUTOMAT_CAMERA_STREAMING_SERVER_
#define AUTOMAT_CAMERA_STREAMING_SERVER_

#include <string>

#include "HTMLPage.h"
#include "HTTPServer.h"

#ifdef ATMT_SUBMODULE_COMMAND_BASED_
#include "../command_based/Subsystem.h"
#endif

namespace atmt {

#ifdef ATMT_SUBMODULE_COMMAND_BASED_
    class CameraStreamingServer : public Subsystem {
#else
    class CameraStreamingServer {
#endif
        public:
            CameraStreamingServer(std::string wifi_ssid, std::string wifi_pass, std::function<char*(size_t&, void*)> jpeg_getter, int frame_rate, void* arg);
#ifdef ATMT_SUBMODULE_COMMAND_BASED_
            ~CameraStreamingServer() override;

            void init() override;
            void periodic() override;

            // void internal_init(RobotState* robot_state, EventHandler* event_handler);
#else
            ~CameraStreamingServer();

            void init();
            void periodic();
#endif

            std::string getIPAddress();

        private:
            int m_frame_rate;
            std::function<char*(size_t&, void*)> m_jpeg_getter;
            void* m_arg;
            HTTPServer* m_server;
    };

}

#endif

#else
#error "Enable ATMT_SUBMODULE_HTTP_SERVER_CAMERA_STREAMING_SERVER_ in automat_submodules.h to use CameraStreamingServer"
#endif
#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_HTTP_SERVER_CAMERA_STREAMING_SERVER_

#include "CameraStreamingServer.h"

namespace atmt {

    CameraStreamingServer::CameraStreamingServer(const std::string& device_name, const std::string& wifi_ssid, const std::string& wifi_pass, std::function<char*(size_t&, void*)> jpeg_getter, double frame_rate, void* arg):
        m_device_name{ device_name },
        m_frame_rate{ frame_rate },
        m_jpeg_getter{ jpeg_getter },
        m_arg{ arg },
        m_server{ new HTTPServer(wifi_ssid, wifi_pass) }
    {
        
    };
    CameraStreamingServer::~CameraStreamingServer() {

    };

    void CameraStreamingServer::init() {
        m_server->registerPage_IdentifyAsAutomat(m_device_name);
        m_server->registerPage_AutomatFavicon();
        m_server->registerPage_Dynamic_JPEGStreamer(
            "/stream",
            m_jpeg_getter,
            m_frame_rate,
            m_arg
        );
        m_server->registerPage_Static_RawHTML(
            "/",
            "<!DOCTYPE html><html><title>JPEG Streaming - Automat</title><head></head><body><img src=\"/stream\" /></body></html>"
        );

        m_server->init();
    };
    void CameraStreamingServer::periodic() {
        m_server->periodic();
    };
    
#ifdef ATMT_SUBMODULE_COMMAND_BASED_
    void CameraStreamingServer::systemPeriodic() {
        periodic();
    };
    void CameraStreamingServer::disabledPeriodic() {};
    void CameraStreamingServer::autonomousPeriodic() {};
    void CameraStreamingServer::teleopPeriodic() {};
#endif

    std::string CameraStreamingServer::getIPAddress() {
        return m_server->getIPAddress();
    };

}

#else
// #error "Enable ATMT_SUBMODULE_HTTP_SERVER_CAMERA_STREAMING_SERVER_ in automat_submodules.h to use CameraStreamingServer"
#endif
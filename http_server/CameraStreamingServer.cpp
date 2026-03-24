#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_HTTP_SERVER_CAMERA_STREAMING_SERVER_

#include "CameraStreamingServer.h"

namespace atmt {

    CameraStreamingServer::CameraStreamingServer(std::string wifi_ssid, std::string wifi_pass, std::function<char*(size_t&, void*)> jpeg_getter, int frame_rate, void* arg):
        m_frame_rate{ frame_rate },
        m_jpeg_getter{ jpeg_getter },
        m_arg{ arg },
        m_server{ new HTTPServer(wifi_ssid, wifi_pass) }
    {
        
    };
    CameraStreamingServer::~CameraStreamingServer() {

    };

    void CameraStreamingServer::init() {
        m_server->registerPage(new HTMLPage_Static_Favicon());
        // m_server->registerPage_Static_RawHTML(
        //     "/stream",
        //     "<!DOCTYPE html><html><head></head><body><h1>Stream Page Working<h1/></body></html>"
        // );
        m_server->registerPage_Dynamic_JPEGStreamer(
            "/stream",
            m_jpeg_getter,
            m_frame_rate,
            m_arg
        );
        // const char* html = 
        //     "<!DOCTYPE html><html><head></head><body><img src=\"/stream\" /></body></html>";
        m_server->registerPage_Static_RawHTML(
            "/",
            "<!DOCTYPE html><html><title>Automat JPEG Streaming</title><head></head><body><img src=\"/stream\" /></body></html>"
        );

        m_server->init();
    };
    void CameraStreamingServer::periodic() {
        m_server->periodic();
    };

    std::string CameraStreamingServer::getIPAddress() {
        return m_server->getIPAddress();
    };

}

#else
// #error "Enable ATMT_SUBMODULE_HTTP_SERVER_CAMERA_STREAMING_SERVER_ in automat_submodules.h to use CameraStreamingServer"
#endif
#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_HTTP_SERVER_

#ifndef AUTOMAT_HTML_PAGE_
#define AUTOMAT_HTML_PAGE_

#include <vector>
#include <functional>
#include <string>

#include "../utils.h"

#ifdef AUTOMAT_ESP32_ESPIDF_
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_http_server.h"
#endif
#ifdef AUTOMAT_ESP32_ARDUINO_
#include <WiFi.h>
#include <WebServer.h>
#include <esp_wifi.h>
#endif

#ifdef ATMT_SUBMODULE_HTTP_SERVER_JPEG_STREAMING_
// #include "esp_camera.h"
#endif

#include "HTMLPageInternals.h"

namespace atmt {

    class HTMLPage_Static_RawHTML : public HTMLPage {
        public:
            HTMLPage_Static_RawHTML(const std::string& path, const std::string& html);
            ~HTMLPage_Static_RawHTML() override = default;

            esp_err_t handle_request(HTTPRequest* request) override;
        private:
            std::string m_html;
    };
    
    class HTMLPage_Static_DynamicHTML : public HTMLPage {
        public:
            HTMLPage_Static_DynamicHTML(const std::string& path, std::function<std::string(void*)> html_getter, void* arg);
            ~HTMLPage_Static_DynamicHTML() override = default;

            esp_err_t handle_request(HTTPRequest* request) override;
        private:
            std::function<std::string(void*)> m_html_getter;
            void* m_arg; // for html_getter
    };
    
    class HTMLPage_Static_DynamicPostHTML : public HTMLPage {
        public:
            HTMLPage_Static_DynamicPostHTML(const std::string& path, std::function<std::string(const std::vector<POSTInfo>&, void*)> post_sender, void* arg);
            ~HTMLPage_Static_DynamicPostHTML() override = default;

            esp_err_t handle_request(HTTPRequest* request) override;
        private:
            std::function<std::string(const std::vector<POSTInfo>&, void*)> m_post_sender;
            void* m_arg;
    };
    class HTMLPage_Static_DynamicPostRedirect : public HTMLPage {
        public:
            HTMLPage_Static_DynamicPostRedirect(const std::string& path, const std::string& redirect_path, std::function<void(const std::vector<POSTInfo>&, void*)> post_sender, void* arg);
            ~HTMLPage_Static_DynamicPostRedirect() override = default;

            esp_err_t handle_request(HTTPRequest* request) override;
        private:
            std::string m_redirect_path;
            std::function<void(const std::vector<POSTInfo>&, void*)> m_post_sender;
            void* m_arg;
    };

#ifdef ATMT_SUBMODULE_HTTP_SERVER_JPEG_STREAMING_
    class HTMLPage_Dynamic_JPEGStreamer : public HTMLPage {
        public:
            HTMLPage_Dynamic_JPEGStreamer(const std::string& path, std::function<char*(size_t&, void*)> jpeg_getter, int frame_rate, void* arg);
            ~HTMLPage_Dynamic_JPEGStreamer() override = default;

            esp_err_t handle_request(HTTPRequest* request) override;
        private:
            std::function<char*(size_t&, void*)> m_jpeg_getter;
            int m_frame_delay_mS;
            void* m_arg; // for html_getter
    };
#endif

}

#endif

#else
#error "Enable ATMT_SUBMODULE_HTTP_SERVER_ in automat_submodules.h to use HTMLPage"
#endif
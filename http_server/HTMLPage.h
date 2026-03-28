#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_HTTP_SERVER_

#ifndef AUTOMAT_HTML_PAGE_
#define AUTOMAT_HTML_PAGE_

#include <vector>
#include <functional>
#include <string>

#include "../utils.h"

#ifdef ATMT_SUBMODULE_SERVER_ESP32_HTTPD_
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_http_server.h"
#endif
#ifdef ATMT_SUBMODULE_SERVER_ARDUINO_WIFI_
#include <WiFi.h>
#include <WebServer.h>
#include <esp_wifi.h>
#endif
#ifdef ATMT_SUBMODULE_SERVER_ARDUINO_ASYNC_WIFI_
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <esp_wifi.h>
#endif
#ifdef AUTOMAT_ESP32_ARDUINO_
#include <Arduino.h>
#endif

#ifdef ATMT_SUBMODULE_HTTP_SERVER_JPEG_STREAMING_
// #include "esp_camera.h"
#endif

#include "HTMLPageInternals.h"

namespace atmt {
    
    class HTMLPage_Static_ReturnCode : public HTMLPage {
        public:
            HTMLPage_Static_ReturnCode(const std::string& path, const std::string& response, int code);
            ~HTMLPage_Static_ReturnCode() override = default;

            esp_err_t handle_request(HTTPRequest* request) override;
        private:
            std::string m_response;
            int m_code;
    };

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
#ifdef ATMT_SUBMODULE_SERVER_ARDUINO_ASYNC_WIFI_
    class HTMLPage_Static_DynamicChunkedHTML : public HTMLPage {
        public:
            HTMLPage_Static_DynamicChunkedHTML(const std::string& path, std::function<const char*(int, size_t&, void*)> html_get_next_chunk, void* arg);
            ~HTMLPage_Static_DynamicChunkedHTML() override = default;

            esp_err_t handle_request(HTTPRequest* request) override;
#ifdef ATMT_SUBMODULE_SERVER_ARDUINO_ASYNC_WIFI_
            static size_t streamCallback(uint8_t* buffer, size_t maxLen, size_t index, void* arg);
#endif
        private:
            std::function<const char*(int, size_t&, void*)> m_html_get_next_chunk;
            void* m_arg; // for html_getter

            // std::vector<char*> m_chunks;
            const char* m_chunk;
            size_t m_chunk_length;
            size_t m_chunk_index;
            int m_chunk_count;
    };
#endif
    
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
            HTMLPage_Dynamic_JPEGStreamer(const std::string& path, std::function<char*(size_t&, void*)> jpeg_getter, double frame_rate, void* arg);
            ~HTMLPage_Dynamic_JPEGStreamer() override = default;

            esp_err_t handle_request(HTTPRequest* request) override;
// #ifdef ATMT_SUBMODULE_SERVER_ARDUINO_WIFI_
#ifndef ATMT_SUBMODULE_SERVER_ARDUINO_ASYNC_WIFI_
            esp_err_t continue_connection(HTTPRequest* request) override;
#endif
// #endif
// #ifdef ATMT_SUBMODULE_SERVER_ESP32_HTTPD_
//             esp_err_t continue_connection(HTTPSocket* request) override;
// #endif
#ifdef ATMT_SUBMODULE_SERVER_ARDUINO_ASYNC_WIFI_
            static size_t streamCallback(uint8_t* buffer, size_t maxLen, size_t index, void* arg);
#endif
        private:
            std::function<char*(size_t&, void*)> m_jpeg_getter;
            int m_frame_delay_mS;
            void* m_arg; // for html_getter

            char m_part_buffer[128];
            size_t m_img_length;
            size_t m_old_img_length;
            char* m_raw_buffer;
            size_t m_header_length;
            
            size_t m_header_index;
            size_t m_buffer_index;
            size_t m_footer_index;
    };
#endif

    class HTMLPage_Static_Favicon : public HTMLPage {
        public:
            HTMLPage_Static_Favicon();
            HTMLPage_Static_Favicon(char* favicon, size_t favicon_length);
            HTMLPage_Static_Favicon(const std::string& path);
            HTMLPage_Static_Favicon(const std::string& path, char* favicon, size_t favicon_length);
            ~HTMLPage_Static_Favicon() override = default;

            esp_err_t handle_request(HTTPRequest* request) override;
#ifdef ATMT_SUBMODULE_SERVER_ARDUINO_ASYNC_WIFI_
            static size_t streamCallback(uint8_t* buffer, size_t maxLen, size_t index, void* arg);
#endif
        private:
            char* m_favicon;
            size_t m_favicon_length;
    };

}

#endif

#else
#error "Enable ATMT_SUBMODULE_HTTP_SERVER_ in automat_submodules.h to use HTMLPage"
#endif
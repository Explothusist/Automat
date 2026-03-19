#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_HTTP_SERVER_

#ifndef AUTOMAT_HTTP_SERVER_
#define AUTOMAT_HTTP_SERVER_

#include <vector>
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

#include "HTMLPageInternals.h"

namespace atmt {

    constexpr uint32_t kReconnectDelayMS = 5000;

#ifdef ATMT_SUBMODULE_COMMAND_BASED_
    class HTTPServer : public Subsystem {
#else
    class HTTPServer {
#endif
        public:
            HTTPServer(std::string wifi_ssid, std::string wifi_password);
#ifdef ATMT_SUBMODULE_COMMAND_BASED_
            ~HTTPServer() override;

            void init() override;
            void periodic() override;

            void internal_init(RobotState* robot_state, EventHandler* event_handler);
#else
            ~HTTPServer();

            void init();
            void periodic();
#endif

            void registerPage_Static_RawHTML(std::string url, std::string html);
            void registerPage_Static_DynamicHTML(std::string url, std::function<std::string()> html_getter);
            void registerPage_Static_DynamicPost(std::string url, std::function<void(std::vector<POSTInfo>)> post_sender);
            void registerPage(HTMLPage* page);

            void wifiInit();
            void startServer();
            void killServer();
#ifdef AUTOMAT_ESP32_ESPIDF_
            static void wifiEventHandler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
            static esp_err_t HTTPRequestHandler(httpd_req_t* request);
#endif
#ifdef AUTOMAT_ESP32_ARDUINO_
            static void wifiEventHandler(WiFiEvent_t event, WiFiEventInfo_t info, HTTPServer* server);
            void HTTPRequestHandler(HTMLPage* page);
#endif

            void addAllPages();
            void addPageToServer(HTMLPage* page);

        private:
#ifdef AUTOMAT_ESP32_ESPIDF_
            httpd_handle_t m_server;
#endif
#ifdef AUTOMAT_ESP32_ARDUINO_
            WebServer m_server;
            uint32_t m_lastReconnectAttempt;
#endif
            std::vector<HTMLPage*> m_html_pages;

            std::string m_wifi_ssid;
            std::string m_wifi_password;

            bool m_wifi_init;
            bool m_server_init;
    };

}

#endif

#else
#error "Enable ATMT_SUBMODULE_HTTP_SERVER_ in automat_submodules.h to use HTTPServer"
#endif
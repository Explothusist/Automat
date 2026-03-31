#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_HTTP_SERVER_

#ifndef AUTOMAT_HTTP_SERVER_
#define AUTOMAT_HTTP_SERVER_

#include <vector>
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

#ifdef ATMT_SUBMODULE_COMMAND_BASED_
#include "../command_based/Subsystem.h"
#endif

#include "HTMLPageInternals.h"
#include "HTMLPage.h"

namespace atmt {

    constexpr uint32_t kReconnectDelayMS = 5000;

#ifndef ATMT_SUBMODULE_SERVER_ARDUINO_ASYNC_WIFI_
    typedef struct {
        HTMLPage* page;
        // HTTPSocket* socket;
        HTTPRequest* request;
        Timestamp scheduled_at;
    } OngoingConnection;
#endif

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
            void systemPeriodic() override;
            void disabledPeriodic() override;
            void autonomousPeriodic() override;
            void teleopPeriodic() override;

            // void internal_init(RobotState* robot_state, EventHandler* event_handler);
#else
            ~HTTPServer();

            void init();
#endif
            void periodic();

            void registerPage_IdentifyAsAutomat();
            void registerPage_IdentifyAsAutomat(const std::string& device_name);
            void registerPage_AutomatFavicon();
            void registerPage_Static_RawHTML(const std::string& path, const std::string& html);
            void registerPage_Static_DynamicHTML(const std::string& path, std::function<std::string(void*)> html_getter, void* arg);
            void registerPage_Static_DynamicPostHTML(const std::string& path, std::function<std::string(const std::vector<POSTInfo>&, void*)> post_sender, void* arg);
            void registerPage_Static_DynamicPostRedirect(const std::string& path, const std::string& redirect_path, std::function<void(const std::vector<POSTInfo>&, void*)> post_sender, void* arg);
#ifdef ATMT_SUBMODULE_HTTP_SERVER_JPEG_STREAMING_
            void registerPage_Dynamic_JPEGStreamer(const std::string& path, std::function<char*(size_t&, void*)> jpeg_getter, double frame_rate, void* arg);
#endif
            void registerPage_Static_Favicon(char* favicon, size_t favicon_length);
            void registerPage(HTMLPage* page);

            void wifiInit();
            void startServer();
            void killServer();
#ifdef ATMT_SUBMODULE_SERVER_ESP32_HTTPD_
            static void wifiEventHandler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
            static esp_err_t HTTPRequestHandler(httpd_req_t* request);
#endif
#if defined(ATMT_SUBMODULE_SERVER_ARDUINO_WIFI_) || defined(ATMT_SUBMODULE_SERVER_ARDUINO_ASYNC_WIFI_)
            static void wifiEventHandler(WiFiEvent_t event, WiFiEventInfo_t info, HTTPServer* server);
#endif
#ifdef ATMT_SUBMODULE_SERVER_ARDUINO_WIFI_
            void HTTPRequestHandler(HTMLPage* page);
#endif
#ifdef ATMT_SUBMODULE_SERVER_ARDUINO_ASYNC_WIFI_
            void HTTPRequestHandler(HTMLPage* page, AsyncWebServerRequest* request);
#endif

            std::string getIPAddress();

            void addAllPages();
            void addPageToServer(HTMLPage* page);

#ifndef ATMT_SUBMODULE_SERVER_ARDUINO_ASYNC_WIFI_
            // void scheduleOngoingConnection(HTMLPage* page, HTTPSocket* socket, Timestamp scheduled_at);
            void scheduleOngoingConnection(HTMLPage* page, HTTPRequest* request, Timestamp scheduled_at);
#endif
#ifdef ATMT_SUBMODULE_SERVER_ESP32_HTTPD_
            httpd_handle_t* getHTTPDHandle();
#endif

        private:
#ifdef ATMT_SUBMODULE_SERVER_ESP32_HTTPD_
            httpd_handle_t m_server;
#endif
#ifdef ATMT_SUBMODULE_SERVER_ARDUINO_WIFI_
            WebServer m_server;
            uint32_t m_lastReconnectAttempt;
#endif
#ifdef ATMT_SUBMODULE_SERVER_ARDUINO_ASYNC_WIFI_
            AsyncWebServer m_server;
            uint32_t m_lastReconnectAttempt;
#endif
            std::vector<HTMLPage*> m_html_pages;
#ifndef ATMT_SUBMODULE_SERVER_ARDUINO_ASYNC_WIFI_
            std::vector<OngoingConnection*> m_ongoing_connections;
#endif

            std::string m_wifi_ssid;
            std::string m_wifi_password;
            std::string m_ip_address;

            bool m_wifi_init;
            bool m_server_init;
    };

}

#endif

#else
#error "Enable ATMT_SUBMODULE_HTTP_SERVER_ in automat_submodules.h to use HTTPServer"
#endif
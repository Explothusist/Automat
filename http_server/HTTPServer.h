#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_HTTP_SERVER_

#ifndef AUTOMAT_HTTP_SERVER_
#define AUTOMAT_HTTP_SERVER_

#include <vector>

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

#include "HTMLPage.h"

namespace atmt {

    class HTTPServer {
        public:
            HTTPServer();
            ~HTTPServer();

            void registerStaticPage_RawHTML(const char* url, const char* html);
            // void registerAdaptivePage_RawHTML();

            void wifiInit(const char* wifi_ssid, const char* wifi_password);
            void startServer();
            void killServer();
            static void wifiEventHandler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
            static esp_err_t HTTPRequestHandler(httpd_req_t* request);

            void registerPage(HTMLPage* page);

        private:
            httpd_handle_t m_server;
            std::vector<HTMLPage*> m_html_pages;

            bool m_wifi_init;
            bool m_server_init;
    };

}

#endif

#else
#error "Enable ATMT_SUBMODULE_HTTP_SERVER_ in automat_submodules.h to use HTTPServer"
#endif
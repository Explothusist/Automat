#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_HTTP_SERVER_

#ifndef AUTOMAT_HTML_PAGE_INTERNALS_
#define AUTOMAT_HTML_PAGE_INTERNALS_

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

namespace atmt {

    constexpr int kFailedPostRequestBeforeFail = 5;
    constexpr int kDelayAfterFailedPostRequestTicks = pdMS_TO_TICKS(1);

    class HTTPRequest {
        public:
#ifdef AUTOMAT_ESP32_ESPIDF_
            HTTPRequest(httpd_req_t* request);
#endif
#ifdef AUTOMAT_ESP32_ARDUINO_
            HTTPRequest(WebServer* page);
#endif

            atmtHTTPError sendResponse(const std::string& type, const std::string& content);

            atmtHTTPError getPostData(std::string& data);
            atmtHTTPError getPostType(std::string& type, std::string& raw_header);

            atmtHTTPError getParsedPostData(std::vector<POSTInfo>& parsed);

        private:
#ifdef AUTOMAT_ESP32_ESPIDF_
            httpd_req_t* m_request;
#endif
#ifdef AUTOMAT_ESP32_ARDUINO_
            WebServer* m_page;
#endif
    };

    class HTMLPage {
        public:
            HTMLPage(const std::string& path, atmtHTTPMethod method);
            virtual ~HTMLPage() = default;
            
            virtual esp_err_t handle_request(HTTPRequest* request);
            const std::string& getPath() const;
#ifdef AUTOMAT_ESP32_ESPIDF_
            http_method getMethod() const;
#endif
#ifdef AUTOMAT_ESP32_ARDUINO_
            HTTPMethod getMethod() const;
#endif
        private:
            std::string m_path;
            atmtHTTPMethod m_method;
    };

}

#endif

#else
#error "Enable ATMT_SUBMODULE_HTTP_SERVER_ in automat_submodules.h to use HTMLPage"
#endif
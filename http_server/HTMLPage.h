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
#include <Wifi.h>
#include <WebServer.h>
#include <esp_wifi.h>
#endif

namespace atmt {

    class HTMLPage {
        public:
            HTMLPage(const std::string& path, atmtHTTPMethod method);
            virtual ~HTMLPage() = default;
            
#ifdef AUTOMAT_ESP32_ESPIDF_
            virtual esp_err_t handle_request(httpd_req_t* request);
#endif
#ifdef AUTOMAT_ESP32_ARDUINO_
            virtual void handle_request(WebServer* server);
#endif
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

    class HTMLPage_Static_RawHTML : public HTMLPage {
        public:
            HTMLPage_Static_RawHTML(const std::string& path, const std::string& html);
            ~HTMLPage_Static_RawHTML();

#ifdef AUTOMAT_ESP32_ESPIDF_
            esp_err_t handle_request(httpd_req_t* request) override;
#endif
#ifdef AUTOMAT_ESP32_ARDUINO_
            void handle_request(WebServer* page) override;
#endif
        private:
            std::string m_html;
    };
    
    class HTMLPage_Static_DynamicHTML : public HTMLPage {
        public:
            HTMLPage_Static_DynamicHTML(const std::string& path, std::function<std::string()> html_getter);
            ~HTMLPage_Static_DynamicHTML();

#ifdef AUTOMAT_ESP32_ESPIDF_
            esp_err_t handle_request(httpd_req_t* request) override;
#endif
#ifdef AUTOMAT_ESP32_ARDUINO_
            void handle_request(WebServer* page) override;
#endif
        private:
            std::function<std::string()> m_html_getter;
    };
    
    class HTMLPage_Static_DynamicPost : public HTMLPage {
        public:
            HTMLPage_Static_DynamicPost(const std::string& path, std::function<void(std::vector<POSTInfo>)> post_sender);
            ~HTMLPage_Static_DynamicPost();

#ifdef AUTOMAT_ESP32_ESPIDF_
            esp_err_t handle_request(httpd_req_t* request) override;
#endif
#ifdef AUTOMAT_ESP32_ARDUINO_
            void handle_request(WebServer* page) override;
#endif
        private:
            std::function<void(std::vector<POSTInfo>)> m_post_sender;
    };

}

#endif

#else
#error "Enable ATMT_SUBMODULE_HTTP_SERVER_ in automat_submodules.h to use HTMLPage"
#endif
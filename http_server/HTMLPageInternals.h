#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_HTTP_SERVER_

#ifndef AUTOMAT_HTML_PAGE_INTERNALS_
#define AUTOMAT_HTML_PAGE_INTERNALS_

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
#include "cJSON.h"
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
#ifdef ATMT_SUBMODULE_HTTP_SERVER_JSON_PARSING_
#include <ArduinoJson.h>
#endif
#endif

namespace atmt {

    class HTTPServer;
    class HTMLPage;
    class HTTPRequest;
    // class HTTPSocket;

    constexpr int kFailedPostRequestBeforeFail = 5;
    constexpr int kDelayAfterFailedPostRequestTicks = pdMS_TO_TICKS(1);
    constexpr int kHttpPostBufferSize = 512;

    atmtHTTPError translateStatusCode(int code, std::string& status_message);

    class HTTPRequest {
        public:
#ifdef ATMT_SUBMODULE_SERVER_ESP32_HTTPD_
            // HTTPRequest(httpd_req_t* request, HTTPServer* server);
            HTTPRequest(httpd_req_t* request);
#endif
#ifdef ATMT_SUBMODULE_SERVER_ARDUINO_WIFI_
            HTTPRequest(WebServer* page, HTTPServer* server);
#endif
#ifdef ATMT_SUBMODULE_SERVER_ARDUINO_ASYNC_WIFI_
            HTTPRequest(AsyncWebServerRequest* request, HTTPServer* server);
#endif

            void setFailedPostRequestsBeforeFail(int failed_requests);
            void setDelayAfterFailedPostRequests(int milliseconds);
            void setHttpPostBufferSize(int buffer_size);

            atmtHTTPError sendResponse(const std::string& type, const std::string& content, int code = 200);
            atmtHTTPError sendResponseRaw(const std::string& type, const char* content, size_t cont_length, int code = 200);
            atmtHTTPError sendResponseRaw(const char* type, size_t type_length, const char* content, size_t cont_length, int code = 200);
            atmtHTTPError throwRedirect(const std::string& url, int code = 303);
            atmtHTTPError throwRedirect(const char* url, size_t url_length, int code = 303);
#ifndef ATMT_SUBMODULE_SERVER_ARDUINO_ASYNC_WIFI_
            atmtHTTPError setResponseType(const std::string& type, int code = 200);
            atmtHTTPError setResponseType(const char* type, size_t type_length, int code = 200);
            atmtHTTPError sendResponseChunk(const std::string& content);
            atmtHTTPError sendResponseChunk(const char* content, size_t cont_length);
            atmtHTTPError writeRaw(const char* content, size_t cont_length);
            atmtHTTPError sendResponseEndChunks();
#else
            atmtHTTPError streamChunks(const std::string& type, std::function<size_t(uint8_t*, size_t, size_t, void*)> callback, void* arg);
            atmtHTTPError streamChunks(const char* type, size_t type_length, std::function<size_t(uint8_t*, size_t, size_t, void*)> callback, void* arg);
#endif

            atmtHTTPError getPostData(std::string& data);
            atmtHTTPError getPostType(std::string& type, std::string& raw_header);
#ifdef ATMT_SUBMODULE_HTTP_SERVER_JSON_PARSING_
            atmtHTTPError parseJSON(const std::string& post_data, const std::string& full_header, std::vector<POSTInfo>& parsed);
#endif
            atmtHTTPError parseMultipart(const std::string& post_data, const std::string& full_header, std::vector<POSTInfo>& parsed);
            atmtHTTPError parseUrlEncoded(const std::string& post_data, const std::string& full_header, std::vector<POSTInfo>& parsed);

            /*
                Supports application/json, multipart/form-data, and application/x-www-form-urlencoded
                application/json notes: Only supports flat, top-level JSON
                multipart/form-data notes: Does not support files
            */
            atmtHTTPError getParsedPostData(std::vector<POSTInfo>& parsed);

            // void scheduleOngoingConnection(HTMLPage* page, int ms_delay);
            
#ifdef ATMT_SUBMODULE_SERVER_ESP32_HTTPD_
            // HTTPSocket* toHTTPSocket();
            atmtHTTPError toAsyncRequest();
            httpd_req_t* getRequest();
#endif

        private:
#ifdef ATMT_SUBMODULE_SERVER_ESP32_HTTPD_
            httpd_req_t* m_request;
#endif
#ifdef ATMT_SUBMODULE_SERVER_ARDUINO_WIFI_
            WebServer* m_request;
#endif
#ifdef ATMT_SUBMODULE_SERVER_ARDUINO_ASYNC_WIFI_
            AsyncWebServerRequest* m_request;
            AsyncWebServerResponse* m_response;
#endif
            HTTPServer* m_server;
            int m_post_requests_before_fail;
            int m_post_delay_after_failed;
            int m_post_buffer_size;
#ifdef ATMT_SUBMODULE_SERVER_ESP32_HTTPD_
            httpd_req_t* m_async_request;
            bool m_async_setup;
#endif
    };

// #ifdef ATMT_SUBMODULE_SERVER_ESP32_HTTPD_
//     class HTTPSocket {
//         public:
//             HTTPSocket(httpd_handle_t* httpd_handle, int socket);
//             ~HTTPSocket();
            
//             atmtHTTPError sendResponseChunk(const std::string& content);
//             atmtHTTPError sendResponseChunk(const char* content, size_t cont_length);
//             atmtHTTPError sendResponseEndChunks();

//         private:
//             httpd_handle_t* m_httpd_handle;
//             int m_socketfd;
//     };
// #endif

    class HTMLPage {
        public:
            HTMLPage(const std::string& path, atmtHTTPMethod method);
            virtual ~HTMLPage() = default;

            void setServer(HTTPServer* server);
            
            virtual esp_err_t handle_request(HTTPRequest* request);
// #ifdef ATMT_SUBMODULE_SERVER_ARDUINO_WIFI_
#ifndef ATMT_SUBMODULE_SERVER_ARDUINO_ASYNC_WIFI_
            virtual esp_err_t continue_connection(HTTPRequest* request);
#endif
// #endif
// #ifdef ATMT_SUBMODULE_SERVER_ESP32_HTTPD_
//             virtual esp_err_t continue_connection(HTTPSocket* socket);
// #endif
            const std::string& getPath() const;
#ifdef ATMT_SUBMODULE_SERVER_ARDUINO_WIFI_
            HTTPMethod getMethod() const;
#endif
#ifdef ATMT_SUBMODULE_SERVER_ARDUINO_ASYNC_WIFI_
            WebRequestMethodComposite getMethod() const;
#endif
#ifdef ATMT_SUBMODULE_SERVER_ESP32_HTTPD_
            http_method getMethod() const;
#endif

#ifndef ATMT_SUBMODULE_SERVER_ARDUINO_ASYNC_WIFI_
            // void scheduleOngoingConnection(HTTPSocket* socket, int ms_delay);
            void scheduleOngoingConnection(HTTPRequest* request, int ms_delay);
#endif
        private:
            std::string m_path;
            atmtHTTPMethod m_method;
            HTTPServer* m_server;
    };

}

#endif

#else
#error "Enable ATMT_SUBMODULE_HTTP_SERVER_ in automat_submodules.h to use HTMLPage"
#endif

#include "HTMLPage.h"

namespace atmt {

    HTMLPage::HTMLPage(const std::string& path, atmtHTTPMethod method):
        m_path{ path },
        m_method{ method }
    {
        // strncpy(m_path, path, sizeof(m_path) - 1);
        // m_path[sizeof(m_path) - 1] = '\0';
    };
#ifdef AUTOMAT_ESP32_ESPIDF_
    esp_err_t HTMLPage::handle_request(httpd_req_t* request) {
        return ESP_OK;
    };
#endif
#ifdef AUTOMAT_ESP32_ARDUINO_
    void HTMLPage::handle_request(WebServer* server) {
        
    };
#endif
    const std::string& HTMLPage::getPath() const {
        return m_path;
    }
#ifdef AUTOMAT_ESP32_ESPIDF_
    http_method HTMLPage::getMethod() const {
        switch (m_method) {
            case Method_Get:
                return HTTP_GET;
            case Method_Post:
                return HTTP_POST;
        }
    };
#endif
#ifdef AUTOMAT_ESP32_ARDUINO_
    HTTPMethod HTMLPage::getMethod() const {
        switch (m_method) {
            case Method_Get:
                return HTTP_GET;
            case Method_Post:
                return HTTP_POST;
        }
    };
#endif

    HTMLPage_Static_RawHTML::HTMLPage_Static_RawHTML(const std::string& path, const std::string& html):
        HTMLPage(path, Method_Get),
        m_html{ html }
    {
        // if (html) {
        //     m_html = strdup(html);
        // }
    };
    HTMLPage_Static_RawHTML::~HTMLPage_Static_RawHTML() {
        // if (m_html) {
        //     free((void*) m_html);
        // }
    };
#ifdef AUTOMAT_ESP32_ESPIDF_
    esp_err_t HTMLPage_Static_RawHTML::handle_request(httpd_req_t* request) {
        // Tell the client to expect raw HTML text
        httpd_resp_set_type(request, "text/html");
        // Send the basic webpage text once
        httpd_resp_send(request, m_html.c_str(), HTTPD_RESP_USE_STRLEN);

        return ESP_OK;
    };
#endif
#ifdef AUTOMAT_ESP32_ARDUINO_
    void HTMLPage_Static_RawHTML::handle_request(WebServer* server) {
        server->send(200, "text/html", "");
        server->sendContent(m_html.c_str());
    };
#endif
    
    HTMLPage_Static_DynamicHTML::HTMLPage_Static_DynamicHTML(const std::string& path, std::function<std::string()> html_getter):
        HTMLPage(path, Method_Get),
        m_html_getter{ html_getter }
    {
        
    };
    HTMLPage_Static_DynamicHTML::~HTMLPage_Static_DynamicHTML() {
        
    };
#ifdef AUTOMAT_ESP32_ESPIDF_
    esp_err_t HTMLPage_Static_DynamicHTML::handle_request(httpd_req_t* request) {
        // Tell the client to expect raw HTML text
        httpd_resp_set_type(request, "text/html");
        // Send the basic webpage text once
        std::string html = m_html_getter();
        httpd_resp_send(request, html.c_str(), HTTPD_RESP_USE_STRLEN);

        // free(html);
        return ESP_OK;
    };
#endif
#ifdef AUTOMAT_ESP32_ARDUINO_
    void HTMLPage_Static_DynamicHTML::handle_request(WebServer* server) {
        std::string html = m_html_getter();
        server->send(200, "text/html", "");
        server->sendContent(html.c_str());
    };
#endif

    HTMLPage_Static_DynamicPost::HTMLPage_Static_DynamicPost(const std::string& path, std::function<void(std::vector<POSTInfo>)> post_sender):
        HTMLPage(path, Method_Post),
        m_post_sender{ post_sender }
    {
        
    };
    HTMLPage_Static_DynamicPost::~HTMLPage_Static_DynamicPost() {
        
    };
#ifdef AUTOMAT_ESP32_ESPIDF_
    esp_err_t HTMLPage_Static_DynamicPost::handle_request(httpd_req_t* request) {
        // First, read the incoming information
        int bytes_read = 0;
        char buffer[256];
        std::string full_data = "";
        while (bytes_read < request->content_len) { // WRONG!!!
            int new_bytes = httpd_req_recv(request, buffer, sizeof(buffer)); // WRONG!!!
            if (new_bytes > 0) {
                full_data += std::string(buffer);
                bytes_read += new_bytes;
            }
        }

        char content_type[32];
        httpd_req_get_hdr_value_str(request, "Content-Type", content_type, sizeof(content_type));

        if (content_type == "application/json") { // WRONG!!!

        }//else (content_type == "application/")

        return ESP_OK;
    };
#endif
#ifdef AUTOMAT_ESP32_ARDUINO_
    void HTMLPage_Static_DynamicPost::handle_request(WebServer* server) {
        
    };
#endif

};
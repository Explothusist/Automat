
#include "HTMLPage.h"

namespace atmt {

    HTMLPage::HTMLPage(const std::string& path, http_method method):
        m_path{ path },
        m_method{ method }
    {
        // strncpy(m_path, path, sizeof(m_path) - 1);
        // m_path[sizeof(m_path) - 1] = '\0';
    };
    esp_err_t HTMLPage::handle_request(httpd_req_t* request) {
        return ESP_OK;
    };
    const std::string& HTMLPage::getPath() const {
        return m_path;
    }
    http_method HTMLPage::getMethod() const {
        return m_method;
    }

    HTMLPage_Static_RawHTML::HTMLPage_Static_RawHTML(const std::string& path, const std::string& html):
        HTMLPage(path, HTTP_GET),
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
    esp_err_t HTMLPage_Static_RawHTML::handle_request(httpd_req_t* request) {
        // Tell the client to expect raw HTML text
        httpd_resp_set_type(request, "text/html");
        // Send the basic webpage text once
        httpd_resp_send(request, m_html.c_str(), HTTPD_RESP_USE_STRLEN);

        return ESP_OK;
    };
    
    HTMLPage_Static_DynamicHTML::HTMLPage_Static_DynamicHTML(const std::string& path, std::function<std::string()> html_getter):
        HTMLPage(path, HTTP_GET),
        m_html_getter{ html_getter }
    {
        
    };
    HTMLPage_Static_DynamicHTML::~HTMLPage_Static_DynamicHTML() {
        
    };
    esp_err_t HTMLPage_Static_DynamicHTML::handle_request(httpd_req_t* request) {
        // Tell the client to expect raw HTML text
        httpd_resp_set_type(request, "text/html");
        // Send the basic webpage text once
        std::string html = m_html_getter();
        httpd_resp_send(request, html.c_str(), HTTPD_RESP_USE_STRLEN);

        // free(html);
        return ESP_OK;
    };

    HTMLPage_Static_DynamicPost::HTMLPage_Static_DynamicPost(const std::string& path, std::function<void(std::vector<POSTInfo>)> post_sender):
        HTMLPage(path, HTTP_GET),
        m_post_sender{ post_sender }
    {
        
    };
    HTMLPage_Static_DynamicPost::~HTMLPage_Static_DynamicPost() {
        
    };
    esp_err_t HTMLPage_Static_DynamicPost::handle_request(httpd_req_t* request) {
        // First, read the incoming information
        int bytes_read = 0;
        char buffer[256];
        while (bytes_read < request->content_len) { // WRONG!!!
            bytes_read += httpd_req_recv(request, buffer, sizeof(buffer)); // WRONG!!!
        }

        char content_type[32];
        httpd_req_get_hdr_value_str(request, "Content-Type", content_type, sizeof(content_type));

        if (content_type == "application/json") { // WRONG!!!

        }//else (content_type == "application/")

        return ESP_OK;
    };

};
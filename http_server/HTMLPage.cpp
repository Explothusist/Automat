#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_HTTP_SERVER_

#include "HTMLPage.h"

namespace atmt {

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
    esp_err_t HTMLPage_Static_RawHTML::handle_request(HTTPRequest* request) {
        // Tell the client to expect raw HTML text
        // Send the basic webpage text once
        atmtHTTPError error = request->sendResponse("text/html", m_html);
        if (error != HTTP_OK) {
            return ESP_FAIL;
        }

        return ESP_OK;
    };
    
    HTMLPage_Static_DynamicHTML::HTMLPage_Static_DynamicHTML(const std::string& path, std::function<std::string(void*)> html_getter, void* arg):
        HTMLPage(path, Method_Get),
        m_html_getter{ html_getter },
        m_arg{ arg }
    {
        
    };
    HTMLPage_Static_DynamicHTML::~HTMLPage_Static_DynamicHTML() {
        
    };
    esp_err_t HTMLPage_Static_DynamicHTML::handle_request(HTTPRequest* request) {
        // Tell the client to expect raw HTML text
        // Send the basic webpage text once
        std::string html = m_html_getter(m_arg);
        atmtHTTPError error = request->sendResponse("text/html", html);
        if (error != HTTP_OK) {
            return ESP_FAIL;
        }

        return ESP_OK;
    };

    HTMLPage_Static_DynamicPost::HTMLPage_Static_DynamicPost(const std::string& path, std::function<void(std::vector<POSTInfo>, void*)> post_sender, void* arg):
        HTMLPage(path, Method_Post),
        m_post_sender{ post_sender },
        m_arg{ arg }
    {
        
    };
    HTMLPage_Static_DynamicPost::~HTMLPage_Static_DynamicPost() {
        
    };
    esp_err_t HTMLPage_Static_DynamicPost::handle_request(HTTPRequest* request) {
        // First, read the incoming information
        // int bytes_read = 0;
        // char buffer[256];
        // std::string full_data = "";
        // int failed_reads = 0;
        // while (bytes_read < request->content_len) {
        //     int new_bytes = httpd_req_recv(request, buffer, sizeof(buffer));
        //     if (new_bytes > 0) {
        //         // full_data += std::string(buffer); // Not null-terminated, so could read garbage
        //         full_data.append(buffer, new_bytes);
        //         bytes_read += new_bytes;
        //         failed_reads = 0;
        //     }else {
        //         failed_reads += 1;
        //         if (failed_reads > 5) {
        //             return ESP_FAIL;
        //         }
        //     }
        // }

        // char content_type_raw[64];
        // httpd_req_get_hdr_value_str(request, "Content-Type", content_type_raw, sizeof(content_type_raw));
        // std::string content_type = std::string(content_type_raw); // Looks something like: 'multipart/form-data; boundary=----WebKitFormBoundaryXYZ'

        // if (content_type.find("application/json") != std::string::npos) {

        // }else if (content_type.find("multipart/form-data") != std::string::npos) {

        // }else { // Assume content_type = "application/x-www-form-urlencoded"

        // }

        return ESP_OK;
    };

};

#else
// #error "Enable ATMT_SUBMODULE_HTTP_SERVER_ in automat_submodules.h to use HTMLPage"
#endif
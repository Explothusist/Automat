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
        atmtHTTPError error = request->sendResponse("text/html", m_html_getter(m_arg));
        if (error != HTTP_OK) {
            return ESP_FAIL;
        }

        return ESP_OK;
    };

    HTMLPage_Static_DynamicPostHTML::HTMLPage_Static_DynamicPostHTML(const std::string& path, std::function<std::string(const std::vector<POSTInfo>&, void*)> post_sender, void* arg):
        HTMLPage(path, Method_Post),
        m_post_sender{ post_sender },
        m_arg{ arg }
    {
        
    };
    HTMLPage_Static_DynamicPostHTML::~HTMLPage_Static_DynamicPostHTML() {
        
    };
    esp_err_t HTMLPage_Static_DynamicPostHTML::handle_request(HTTPRequest* request) {
        std::vector<POSTInfo> post_data;
        atmtHTTPError error = request->getParsedPostData(post_data);
        if (error != HTTP_OK) {
            return ESP_FAIL;
        }
        error = request->sendResponse("text/html", m_post_sender(post_data, m_arg));
        if (error != HTTP_OK) {
            return ESP_FAIL;
        }

        return ESP_OK;
    };
    HTMLPage_Static_DynamicPostRedirect::HTMLPage_Static_DynamicPostRedirect(const std::string& path, const std::string& redirect_path, std::function<void(const std::vector<POSTInfo>&, void*)> post_sender, void* arg):
        HTMLPage(path, Method_Post),
        m_redirect_path{ redirect_path },
        m_post_sender{ post_sender },
        m_arg{ arg }
    {
        
    };
    HTMLPage_Static_DynamicPostRedirect::~HTMLPage_Static_DynamicPostRedirect() {
        
    };
    esp_err_t HTMLPage_Static_DynamicPostRedirect::handle_request(HTTPRequest* request) {
        std::vector<POSTInfo> post_data;
        atmtHTTPError error = request->getParsedPostData(post_data);
        if (error != HTTP_OK) {
            return ESP_FAIL;
        }
        m_post_sender(post_data, m_arg);
        error = request->throwRedirect(m_redirect_path, 303);
        if (error != HTTP_OK) {
            return ESP_FAIL;
        }

        return ESP_OK;
    };

};

#else
// #error "Enable ATMT_SUBMODULE_HTTP_SERVER_ in automat_submodules.h to use HTMLPage"
#endif
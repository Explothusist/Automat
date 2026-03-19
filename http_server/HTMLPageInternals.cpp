#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_HTTP_SERVER_

#include <algorithm>

#include "HTMLPageInternals.h"
#include "../utils.h"

namespace atmt {

#ifdef AUTOMAT_ESP32_ESPIDF_
    HTTPRequest::HTTPRequest(httpd_req_t* request): 
        m_request{ request }
    {

    };
#endif
#ifdef AUTOMAT_ESP32_ARDUINO_
    HTTPRequest::HTTPRequest(WebServer* page):
        m_page{ page }
    {

    };
#endif
    
    atmtHTTPError HTTPRequest::sendResponse(const std::string& type, const std::string& content) {
#ifdef AUTOMAT_ESP32_ESPIDF_
        esp_err_t error =  httpd_resp_set_type(m_request, type.c_str());
        if (error != ESP_OK) {
            return HTTP_FAIL;
        }
        error = httpd_resp_send(m_request, content.c_str(), HTTPD_RESP_USE_STRLEN);
        if (error != ESP_OK) {
            return HTTP_FAIL;
        }
        return HTTP_OK;
#endif
#ifdef AUTOMAT_ESP32_ARDUINO_
        m_page->send(200, type.c_str(), content.c_str());
        return HTTP_OK;
#endif

        return HTTP_FAIL;
    };

    atmtHTTPError HTTPRequest::getPostData(std::string& data) {
#ifdef AUTOMAT_ESP32_ESPIDF_
        int bytes_read = 0;
        char buffer[256];
        std::string full_data = "";
        int failed_reads = 0;
        while (bytes_read < m_request->content_len) {
            int left_to_read = std::min(static_cast<unsigned int>(sizeof(buffer)), m_request->content_len - bytes_read);
            int new_bytes = httpd_req_recv(m_request, buffer, left_to_read);
            if (new_bytes > 0) {
                // full_data += std::string(buffer); // Not null-terminated, so could read garbage
                full_data.append(buffer, new_bytes);
                bytes_read += new_bytes;
                failed_reads = 0;
            }else {
                failed_reads += 1;
                if (failed_reads > kFailedPostRequestBeforeFail) {
                    return HTTP_FAIL;
                }
                vTaskDelay(kDelayAfterFailedPostRequestTicks);
            }
        }
        data = std::move(full_data);
        return HTTP_OK;
#endif
#ifdef AUTOMAT_ESP32_ARDUINO_
        std::string full_data = m_page->arg("plain").c_str(); // Gets the request in plain text, but must check becomes sometimes it autoparses

        if (full_data.length() == 0 && m_page->args() > 0) {
            // This means that the data is in a form that the server auto parses (x-www-form-urlencoded)
            // For consistency, we will reconstruct the data here, but in reality we theoretically won't call this method if its already parsed
            full_data = "";
            for (int i = 0; i < m_page->args(); i++) {
                if (i > 0) {
                    full_data += "&";
                }
                full_data += m_page->argName(i).c_str();
                full_data += "=";
                full_data += m_page->arg(i).c_str();
            }
        }
        data = std::move(full_data);

        return HTTP_OK;
#endif

        return HTTP_FAIL;
    };
    atmtHTTPError HTTPRequest::getPostType(std::string& type, std::string& raw_header) {
#ifdef AUTOMAT_ESP32_ESPIDF_
        char content_type_raw[256];
        esp_err_t error = httpd_req_get_hdr_value_str(m_request, "Content-Type", content_type_raw, sizeof(content_type_raw));
        if (error != ESP_OK) {
            return HTTP_FAIL;
        }
        raw_header = std::string(content_type_raw); // Looks something like: 'multipart/form-data; boundary=----WebKitFormBoundaryXYZ'
#endif
#ifdef AUTOMAT_ESP32_ARDUINO_
        raw_header = m_page->header("Content-Type").c_str();
        if (raw_header.empty()) {
            return HTTP_FAIL;
        }
#endif
        std::transform(raw_header.begin(), raw_header.end(), raw_header.begin(), ::tolower);
        if (raw_header.find("application/json") != std::string::npos) {
            type = "application/json";
        }else if (raw_header.find("multipart/form-data") != std::string::npos) {
            type = "multipart/form-data";
        }else if (raw_header.find("application/x-www-form-urlencoded") != std::string::npos) {
            type = "application/x-www-form-urlencoded";
        }else { // Assume content_type = "application/x-www-form-urlencoded"
            type = "unknown";
        }
        return HTTP_OK;
    };
    atmtHTTPError HTTPRequest::getParsedPostData(std::vector<POSTInfo>& parsed) {
        std::string post_data;
        std::string post_type;
        std::string full_header;
        atmtHTTPError error;

        parsed.clear();

        error = getPostType(post_type, full_header);
        if (error != HTTP_OK) {
            return HTTP_FAIL;
        }
#ifdef AUTOMAT_ESP32_ARDUINO_
        if (post_type != "x-www-form-urlencoded") {
#endif
            error = getPostData(post_data);
            if (error != HTTP_OK) {
                return HTTP_FAIL;
            }
#ifdef AUTOMAT_ESP32_ARDUINO_
        }
#endif

        if (post_type == "application/json") {

        }else if (post_type == "multipart/form-data") {
            /*
                ------WebKitFormBoundaryA1B2C3D4\r\n
                Content-Disposition: form-data; name="username"\r\n
                \r\n
                alice\r\n
                ------WebKitFormBoundaryA1B2C3D4\r\n
                Content-Disposition: form-data; name="avatar"; filename="photo.jpg"\r\n
                Content-Type: image/jpeg\r\n
                \r\n
                (binary JPEG data)\r\n
                ------WebKitFormBoundaryA1B2C3D4--\r\n
            */
            std::string current_buffer = "--" + trimWhitespace(substrBetween(full_header, "=", ";")); // set to Boundary
            std::string current_chunk;
            std::vector parts = splitString(post_data, current_buffer);
            for (std::string part : parts) {
                if (part.empty() || trimWhitespace(part) == "--") {
                    continue;
                }
                POSTInfo element;

                current_buffer = substrUntil(part, "\r\n\r\n"); // set to Header
                // current_chunk = substrAfter(current_buffer, "Content-Disposition");
                current_chunk = trimWhitespace(substrBetween(current_buffer, "Content-Disposition:", ";")); // set to Content-Disposition
                if (current_chunk == "form-data") { // If Content-Disposition does not exist, chunk will be ""
                    // current_buffer = substrAfter(current_buffer, ";"); // set to " name=""; ..." part of line 1
                    element.name = substrBetween(current_buffer, "name=\"", "\"");
                    // current_buffer = substrAfter(current_buffer, ";"); // remove 'name=' from the buffer

                    if (current_buffer.find("filename*=") != std::string::npos) {
                        // File, but extra parameters
                        element.data = "";
                    }else if (current_buffer.find("filename=") != std::string::npos) {
                        // File upload
                        element.data = "";
                    }else {
                        // Standard input field
                        current_buffer = substrAfter(part, "\r\n\r\n");
                        element.data = trimTrailingCRLF(current_buffer);
                    }
                    parsed.push_back(element);
                }
            }
            // std::string current_buffer = "--" + trimWhitespace(substrAfter(full_header, "=")); // set to Boundary
            // // std::string current_chunk;
            // std::vector parts = splitString(post_data, current_buffer);
            // for (std::string part : parts) {
            //     if (part.empty() || trimWhitespace(part) == "--") {
            //         continue;
            //     }
            //     element.name = "";
            //     element.data = "";
            //     // if (part != "" && part != "--\r\n") { // Not first line and not last line (usually)
            //     std::vector lines = splitString(part, "\r\n");
            //     current_buffer = trimWhitespace(substrBetween(lines[0], ":", ";")); // set to Content-Disposition
            //     if (current_buffer == "form-data") {
            //         current_buffer = substrAfter(lines[0], ";"); // set to " name=""; ..." part of line 1
            //         element.name = urlDecode(substrBetween(current_buffer, "\"", "\""));
            //         current_buffer = substrAfter(current_buffer, ";"); // remove 'name=' from the buffer
            //         // current_chunk = trimWhitespace(substrUntil(current_buffer, "=")); // name of attribute after 'name='
            //         if (current_buffer.find("filename*") != std::string::npos) {
            //             // File, but extra parameters
            //             element.data = "";
            //         }else if (current_buffer.find("filename") != std::string::npos) {
            //             // File upload
            //             element.data = "";
            //         }else {
            //             // Standard input field
            //             size_t i;
            //             for (i = 1; i < lines.size(); i++) {
            //                 current_buffer = trimWhitespace(substrUntil(lines[i], ":"));
            //                 if (current_buffer == "") {
            //                     // Empty line, so next line is body
            //                     break;
            //                 }else if (current_buffer == "Content-Type") {
            //                     continue;
            //                 }else if (current_buffer == "Content-Encoding") {
            //                     continue;
            //                 }
            //             }
            //             if (i + 1 < lines.size()) {
            //                 element.data = urlDecode(trimWhitespace(lines[i+1]));
            //                 for (i = i+2; i < lines.size(); i++) {
            //                     element.data += "\n"+urlDecode(trimWhitespace(lines[i]));
            //                 }
            //             }else {
            //                 element.data = "";
            //             }
            //         }
            //         parsed.push_back(element);
            //         // }else {
            //         //     continue;
            //         // }
            //     }
            // }
        }else if (post_type == "application/x-www-form-urlencoded") {
#ifdef AUTOMAT_ESP32_ARDUINO_
            for (int i = 0; i < m_page->args(); i++) {
                element.name = m_page->argName(i).c_str();
                element.data = m_page->arg(i).c_str();
                parsed.push_back(element);
            }
#else
            std::vector<std::string> parts = splitString(post_data, "&");
            for (std::string part : parts) {
                POSTInfo element;
                element.name = urlDecode(substrUntil(part, "=")); // Url decode fixes space and special char handling
                element.data = urlDecode(substrAfter(part, "=")); // Urls encode ' ' as '+' and some char as '%XX'
                parsed.push_back(element);
            }
#endif
        }else {
            return HTTP_FAIL;
        }

        return HTTP_OK;
    };


    HTMLPage::HTMLPage(const std::string& path, atmtHTTPMethod method):
        m_path{ path },
        m_method{ method }
    {
        // strncpy(m_path, path, sizeof(m_path) - 1);
        // m_path[sizeof(m_path) - 1] = '\0';
    };
    esp_err_t HTMLPage::handle_request(HTTPRequest* request) {
        return ESP_OK;
    };
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
            default:
                return HTTP_GET;
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
            default:
                return HTTP_GET;
        }
    };
#endif

};

#else
// #error "Enable ATMT_SUBMODULE_HTTP_SERVER_ in automat_submodules.h to use HTMLPage"
#endif
#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_HTTP_SERVER_

#include <algorithm>

#include "HTMLPageInternals.h"
#include "../utils.h"

namespace atmt {

#ifdef AUTOMAT_ESP32_ESPIDF_
    HTTPRequest::HTTPRequest(httpd_req_t* request): 
        m_request{ request },
        m_post_requests_before_fail{ kFailedPostRequestBeforeFail },
        m_post_delay_after_failed{ kDelayAfterFailedPostRequestTicks },
        m_post_buffer_size{ kHttpPostBufferSize }
    {

    };
#endif
#ifdef AUTOMAT_ESP32_ARDUINO_
    HTTPRequest::HTTPRequest(WebServer* page):
        m_page{ page },
        m_post_requests_before_fail{ kFailedPostRequestBeforeFail },
        m_post_delay_after_failed{ kDelayAfterFailedPostRequestTicks },
        m_post_buffer_size{ kHttpPostBufferSize }
    {

    };
#endif

    void HTTPRequest::setFailedPostRequestsBeforeFail(int failed_requests) {
        m_post_requests_before_fail = failed_requests;
    };
    void HTTPRequest::setDelayAfterFailedPostRequests(int milliseconds) {
        m_post_delay_after_failed = pdMS_TO_TICKS(milliseconds);
    };
    void HTTPRequest::setHttpPostBufferSize(int buffer_size) {
        m_post_buffer_size = buffer_size;
    };
    
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
        // char buffer[m_post_buffer_size];
        std::vector<char> buffer(m_post_buffer_size);
        std::string full_data = "";
        int failed_reads = 0;
        while (bytes_read < m_request->content_len) {
            int left_to_read = std::min(static_cast<unsigned int>(buffer.size()), m_request->content_len - bytes_read);
            int new_bytes = httpd_req_recv(m_request, buffer.data(), left_to_read);
            if (new_bytes > 0) {
                // full_data += std::string(buffer); // Not null-terminated, so could read garbage
                full_data.append(buffer.data(), new_bytes);
                bytes_read += new_bytes;
                failed_reads = 0;
            }else {
                failed_reads += 1;
                if (failed_reads > m_post_requests_before_fail) {
                    return HTTP_FAIL;
                }
                vTaskDelay(m_post_delay_after_failed);
            }
        }
        data = std::move(full_data);
        return HTTP_OK;
#endif
#ifdef AUTOMAT_ESP32_ARDUINO_
        String temp_str = m_page->arg("plain"); // Gets the request in plain text, but must check becomes sometimes it autoparses
        std::string full_data = temp_str.c_str();

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
        // std::transform(raw_header.begin(), raw_header.end(), raw_header.begin(), ::tolower);
        std::transform(raw_header.begin(), raw_header.end(), raw_header.begin(), [](unsigned char c){ return std::tolower(c); });
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
    
    atmtHTTPError HTTPRequest::parseJSON(const std::string& post_data, const std::string& full_header, std::vector<POSTInfo>& parsed) {
#ifdef AUTOMAT_ESP32_ESPIDF_
        cJSON* json = cJSON_Parse(post_data.c_str());
        if (json) {
            for (cJSON* item = json->child; item != nullptr; item = item->next) { // Linked list style for loop
                POSTInfo element;
                element.name = item->string ? item->string : "";
                if (cJSON_IsString(item)) {
                    element.data = item->valuestring;
                }else if (cJSON_IsNumber(item)) {
                    element.data = std::to_string(item->valuedouble);
                }else if (cJSON_IsBool(item)) {
                    element.data = std::to_string(cJSON_IsTrue(item));
                }else if (cJSON_IsNull(item)) {
                    element.data = "NULL";
                }else {
                    element.data = "Parsing Error";
                }
                parsed.push_back(element);
            }
            cJSON_Delete(json);
        }else {
            return HTTP_FAIL;
        }
#endif
#ifdef AUTOMAT_ESP32_ARDUINO_
        DynamicJsonDocument json_raw{ m_post_buffer_size };
        DeserializationError error = deserializeJson(json_raw, post_data);
        if (!error) {
            JsonObject json_obj = json_raw.as<JsonObject>();
            for (JsonPair key_value : json_obj) {
                POSTInfo element;
                element.name = key_value.key().c_str();
                // element.data = key_value.value().as<std::string>();
                if (key_value.value().is<const char*>()) {
                    element.data = key_value.value().as<const char*>();
                }else if (key_value.value().is<std::string>()) {
                    element.data = key_value.value().as<std::string>();
                }else if (key_value.value().is<int>()) {
                    element.data = std::to_string(key_value.value().as<int>());
                }else if (key_value.value().is<float>()) {
                    element.data = std::to_string(key_value.value().as<float>());
                }else if (key_value.value().is<bool>()) {
                    element.data = key_value.value().as<bool>() ? "true" : "false";
                }else {
                    element.data = "Parsing Error";
                }
                parsed.push_back(element);
            }
        }else {
            return HTTP_FAIL;
        }
#endif
        return HTTP_OK;
    };
    atmtHTTPError HTTPRequest::parseMultipart(const std::string& post_data, const std::string& full_header, std::vector<POSTInfo>& parsed) {
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
        std::string header_lower = full_header;
        std::transform(header_lower.begin(), header_lower.end(), header_lower.begin(), [](unsigned char c){ return std::tolower(c); });
        // std::string data_lower = post_data;
        // std::transform(data_lower.begin(), data_lower.end(), data_lower.begin(), [](unsigned char c){ return std::tolower(c); });
        
        // std::string current_buffer = "--" + trimWhitespace(substrBetween(header_lower, "boundary=\"", "\"")); // set to Boundary
        // if (current_buffer == "") { // Fall back to quoteless
        //     current_buffer = "--" + trimWhitespace(substrBetween(header_lower, "boundary=", ";")); // set to Boundary
        // }
        // std::string current_chunk;

        // std::string current_buffer = substrAfter(full_header, "boundary=\"");
        // if (current_buffer == "") { // Fall back to quoteless
        //     current_buffer = substrAfter(full_header, "boundary="); // set to Boundary
        // }
        // current_buffer = substrUntilFirstOf(current_buffer, "\"", ";");
        // current_buffer = "--" + trimWhitespace(substrUntil(current_buffer, "\r\n")); // set to Boundary

        std::string current_buffer = substrBetween(header_lower, "boundary", "\r");
        current_buffer = substrAfter(current_buffer, "=");
        std::string current_chunk = substrBetween(current_buffer, "\"", "\"");
        if (current_chunk == "") { // Falls back to quoteless
            current_chunk = substrUntil(current_buffer, ";");
        }
        current_buffer = "--" + trimWhitespace(current_chunk); // set to Boundary

        std::vector<std::string> parts = splitString(post_data, current_buffer);
        for (std::string part : parts) {
            if (part.empty() || trimWhitespace(part) == "--") {
                continue;
            }
            POSTInfo element;

            current_buffer = substrUntil(part, "\r\n\r\n"); // set to Header
            if (current_buffer == part) {
                current_buffer = substrUntil(part, "\n\n");
            }
            std::transform(current_buffer.begin(), current_buffer.end(), current_buffer.begin(), [](unsigned char c){ return std::tolower(c); });
            // current_chunk = substrAfter(current_buffer, "Content-Disposition");
            current_chunk = substrAfter(current_buffer, "content-disposition:"); // set to Content-Disposition
            current_chunk = trimWhitespace(substrUntilFirstOf(current_chunk, ";", "\r\n")); // set to Content-Disposition
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
                    if (current_buffer == "") {
                        current_buffer = substrAfter(part, "\n\n");
                    }
                    element.data = trimTrailingCRLF(current_buffer);
                }
                parsed.push_back(element);
            }
        }
        return HTTP_OK;
    };
    atmtHTTPError HTTPRequest::parseUrlEncoded(const std::string& post_data, const std::string& full_header, std::vector<POSTInfo>& parsed) {
#ifdef AUTOMAT_ESP32_ARDUINO_
        for (int i = 0; i < m_page->args(); i++) {
            POSTInfo element;
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
        if (post_type != "application/x-www-form-urlencoded") {
#endif
            error = getPostData(post_data);
            if (error != HTTP_OK) {
                return HTTP_FAIL;
            }
#ifdef AUTOMAT_ESP32_ARDUINO_
        }
#endif

        if (post_type == "application/json") {
            error = parseJSON(post_data, full_header, parsed);
        }else if (post_type == "multipart/form-data") {
            error = parseMultipart(post_data, full_header, parsed);
        }else if (post_type == "application/x-www-form-urlencoded") {
            error = parseUrlEncoded(post_data, full_header, parsed);
        }else {
            return HTTP_FAIL;
        }
        if (error != HTTP_OK) {
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
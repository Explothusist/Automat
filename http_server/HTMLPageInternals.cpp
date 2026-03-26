#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_HTTP_SERVER_

#include <algorithm>

#include "HTMLPageInternals.h"
#include "HTTPServer.h"
#include "../utils.h"

namespace atmt {

    atmtHTTPError translateStatusCode(int code, std::string& status_message) {
        status_message = "404 Not Found";
        if (code < 100 || code > 599) {
            return HTTP_FAIL;
        }else if (code <= 199) {
            // Informational
            switch (code) {
                case 100:
                    status_message = "100 Continue";
                    break;
                case 101:
                    status_message = "101 Switching Protocols";
                    break;
                case 102:
                    status_message = "102 Processing";
                    break;
                case 103:
                    status_message = "103 Early Hints";
                    break;
                default:
                    status_message = std::to_string(code)+" Informational";
            }
        }else if (code <= 299) {
            // Successful
            switch (code) {
                case 200:
                    status_message = "200 OK";
                    break;
                case 201:
                    status_message = "201 Created";
                    break;
                case 202:
                    status_message = "202 Accepted";
                    break;
                case 203:
                    status_message = "203 Non-Authoritative Information";
                    break;
                case 204:
                    status_message = "204 No Content";
                    break;
                case 205:
                    status_message = "205 Reset Content";
                    break;
                case 206:
                    status_message = "206 Partial Content";
                    break;
                case 207:
                    status_message = "207 Multi-Status";
                    break;
                case 208:
                    status_message = "208 Already Reported";
                    break;
                case 226:
                    status_message = "226 IM Used";
                    break;
                default:
                    status_message = std::to_string(code)+" Successful";
            }
        }else if (code <= 399) {
            // Redirection
            switch (code) {
                case 300:
                    status_message = "300 Multiple Choices";
                    break;
                case 301:
                    status_message = "301 Moved Permanently";
                    break;
                case 302:
                    status_message = "302 Found";
                    break;
                case 303:
                    status_message = "303 See Other";
                    break;
                case 304:
                    status_message = "304 Not Modified";
                    break;
                case 305:
                    status_message = "305 Use Proxy (Deprecated)";
                    break;
                case 306:
                    status_message = "306 unused (Deprecated)";
                    break;
                case 307:
                    status_message = "307 Temporary Redirect";
                    break;
                case 308:
                    status_message = "308 Permanent Redirect";
                    break;
                default:
                    status_message = std::to_string(code)+" Redirection";
            }
        }else if (code <= 499) {
            // Client Error Responses
            switch (code) {
                case 400:
                    status_message = "400 Bad Request";
                    break;
                case 401:
                    status_message = "401 Unauthorized";
                    break;
                case 402:
                    status_message = "402 Payment Required";
                    break;
                case 403:
                    status_message = "403 Forbidden";
                    break;
                case 404:
                    status_message = "404 Not Found";
                    break;
                case 405:
                    status_message = "405 Method Not Allowed";
                    break;
                case 406:
                    status_message = "406 Not Acceptable";
                    break;
                case 407:
                    status_message = "407 Proxy Authentication Required";
                    break;
                case 408:
                    status_message = "408 Request Timeout";
                    break;
                case 409:
                    status_message = "409 Conflict";
                    break;
                case 410:
                    status_message = "410 Gone";
                    break;
                case 411:
                    status_message = "411 Length Required";
                    break;
                case 412:
                    status_message = "412 Precondition Failed";
                    break;
                case 413:
                    status_message = "413 Content Too Large";
                    break;
                case 414:
                    status_message = "414 URI Too Long";
                    break;
                case 415:
                    status_message = "415 Unsupported Media Type";
                    break;
                case 416:
                    status_message = "416 Range Not Satisfiable";
                    break;
                case 417:
                    status_message = "417 Expectation Failed";
                    break;
                case 418:
                    status_message = "418 I'm a teapot";
                    break;
                case 421:
                    status_message = "421 Misdirected Request";
                    break;
                case 422:
                    status_message = "422 Unprocessable Content";
                    break;
                case 423:
                    status_message = "423 Locked";
                    break;
                case 424:
                    status_message = "424 Failed Dependency";
                    break;
                case 425:
                    status_message = "425 Too Early";
                    break;
                case 426:
                    status_message = "426 Upgrade Required";
                    break;
                case 428:
                    status_message = "428 Precondition Required";
                    break;
                case 429:
                    status_message = "429 Too Many Requests";
                    break;
                case 431:
                    status_message = "431 Request Header Fields Too Large";
                    break;
                case 451:
                    status_message = "451 Unavailable For Legal Reasons";
                    break;
                default:
                    status_message = std::to_string(code)+" Client Error";
            }
        }else if (code <= 599) {
            // Server Error Responses
            switch (code) {
                case 500:
                    status_message = "500 Internal Server Error";
                    break;
                case 501:
                    status_message = "501 Not Implemented";
                    break;
                case 502:
                    status_message = "502 Bad Gateway";
                    break;
                case 503:
                    status_message = "503 Service Unavailable";
                    break;
                case 504:
                    status_message = "504 Gateway Timeout";
                    break;
                case 505:
                    status_message = "505 HTTP Version Not Supported";
                    break;
                case 506:
                    status_message = "506 Variant Also Negotiates";
                    break;
                case 507:
                    status_message = "507 Insufficient Storage";
                    break;
                case 508:
                    status_message = "508 Loop Detected";
                    break;
                case 510:
                    status_message = "510 Not Extended";
                    break;
                case 511:
                    status_message = "511 Network Authentication Required";
                    break;
                default:
                    status_message = std::to_string(code)+" Server Error";
            }
        }
        return HTTP_OK;
    };

#ifdef ATMT_SUBMODULE_SERVER_ESP32_HTTPD_
    HTTPRequest::HTTPRequest(httpd_req_t* request): 
        m_request{ request },
        // m_server{ server },
        m_post_requests_before_fail{ kFailedPostRequestBeforeFail },
        m_post_delay_after_failed{ kDelayAfterFailedPostRequestTicks },
        m_post_buffer_size{ kHttpPostBufferSize },
        m_async_request{ nullptr },
        m_async_setup{ false }
    {

    };
#endif
#ifdef ATMT_SUBMODULE_SERVER_ARDUINO_WIFI_
    HTTPRequest::HTTPRequest(WebServer* page, HTTPServer* server):
        m_request{ page },
        m_server{ server },
        m_post_requests_before_fail{ kFailedPostRequestBeforeFail },
        m_post_delay_after_failed{ kDelayAfterFailedPostRequestTicks },
        m_post_buffer_size{ kHttpPostBufferSize }
    {

    };
#endif
#ifdef ATMT_SUBMODULE_SERVER_ARDUINO_ASYNC_WIFI_
    HTTPRequest::HTTPRequest(AsyncWebServerRequest* request, HTTPServer* server):
        m_request{ request },
        m_server{ server },
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
    
    atmtHTTPError HTTPRequest::sendResponse(const std::string& type, const std::string& content, int code) {
        return sendResponseRaw(type.c_str(), type.length(), content.c_str(), content.length(), code);
    };
    atmtHTTPError HTTPRequest::sendResponseRaw(const std::string& type, const char* content, size_t cont_length, int code) {
        return sendResponseRaw(type.c_str(), type.length(), content, cont_length, code);
    };
    atmtHTTPError HTTPRequest::sendResponseRaw(const char* type, size_t type_length, const char* content, size_t cont_length, int code) {
        // if (code < 200 || code > 299) {
        //     return HTTP_FAIL;
        // }
#ifdef ATMT_SUBMODULE_SERVER_ESP32_HTTPD_
        atmtHTTPError atmt_error;
        esp_err_t error;
        atmt_error = setResponseType(type, type_length);
        if (atmt_error != HTTP_OK) {
            return HTTP_FAIL;
        }
        error = httpd_resp_send(getRequest(), content, cont_length);
        if (error != ESP_OK) {
            return HTTP_FAIL;
        }
        return HTTP_OK;
#endif
#ifdef ATMT_SUBMODULE_SERVER_ARDUINO_WIFI_
        m_request->send(code, type, content);
        return HTTP_OK;
#endif
#ifdef ATMT_SUBMODULE_SERVER_ARDUINO_ASYNC_WIFI_
        m_request->send(code, type, content);
        return HTTP_OK;
#endif

        return HTTP_FAIL;
    };
    atmtHTTPError HTTPRequest::throwRedirect(const std::string& url, int code) {
        return throwRedirect(url.c_str(), url.length(), code);
    };
    atmtHTTPError HTTPRequest::throwRedirect(const char* url, size_t url_length, int code) {
        if (code < 300 || code > 399) {
            return HTTP_FAIL;
        }
#ifdef ATMT_SUBMODULE_SERVER_ESP32_HTTPD_
        esp_err_t error;
        atmtHTTPError atmt_error;
        std::string status_code;
        atmt_error = translateStatusCode(code, status_code);
        if (atmt_error != HTTP_OK) {
            return HTTP_FAIL;
        }
        error = httpd_resp_set_status(getRequest(), status_code.c_str());
        if (error != ESP_OK) {
            return HTTP_FAIL;
        }
        error = httpd_resp_set_hdr(getRequest(), "Location", url);
        if (error != ESP_OK) {
            return HTTP_FAIL;
        }
        const char* body = "<html><body>Redirecting...</body></html>";
        error = httpd_resp_send(getRequest(), body, strlen(body));
        if (error != ESP_OK) {
            return HTTP_FAIL;
        }
        return HTTP_OK;
#endif
#ifdef ATMT_SUBMODULE_SERVER_ARDUINO_WIFI_
        m_request->sendHeader("Location", url);
        m_request->send(code, "text/html", "<html><body>Redirecting...</body></html>");
        return HTTP_OK;
#endif
#ifdef ATMT_SUBMODULE_SERVER_ARDUINO_ASYNC_WIFI_
        // Create a response object
        AsyncWebServerResponse *response = m_request->beginResponse( 
            code,               // HTTP status code, e.g., 303
            "text/html",        // Content-Type
            "<html><body>Redirecting...</body></html>" // Body
        );
        // Add the Location header for the redirect
        response->addHeader("Location", url);
        // Send the response
        m_request->send(response);
        return HTTP_OK;
#endif

        return HTTP_FAIL;
    };
#ifndef ATMT_SUBMODULE_SERVER_ARDUINO_ASYNC_WIFI_
    atmtHTTPError HTTPRequest::setResponseType(const std::string& type, int code) {
        return setResponseType(type.c_str(), type.length(), code);
    };
    atmtHTTPError HTTPRequest::setResponseType(const char* type, size_t type_length, int code) {
#ifdef ATMT_SUBMODULE_SERVER_ESP32_HTTPD_
        esp_err_t error;
        if (code != 200) {
            std::string status_code;
            atmtHTTPError atmt_error = translateStatusCode(code, status_code);
            if (atmt_error != HTTP_OK) {
                return HTTP_FAIL;
            }
            error = httpd_resp_set_status(getRequest(), status_code.c_str());
            if (error != ESP_OK) {
                return HTTP_FAIL;
            }
        }
        error = httpd_resp_set_type(getRequest(), type);
        if (error != ESP_OK) {
            return HTTP_FAIL;
        }
        return HTTP_OK;
#endif
#ifdef ATMT_SUBMODULE_SERVER_ARDUINO_WIFI_
        // Set status code and content-type header
        m_request->sendHeader("Content-Type", type, true); // first = true replaces existing
        // m_request->setContentLength(0); // we will stream content later
        m_request->sendHeader("Connection", "keep-alive", false); // keep connection open for chunked
        return HTTP_OK;
#endif

        return HTTP_FAIL;
    };
    atmtHTTPError HTTPRequest::sendResponseChunk(const std::string& content) {
        return sendResponseChunk(content.c_str(), content.length());
    };
    atmtHTTPError HTTPRequest::sendResponseChunk(const char* content, size_t cont_length) {
#ifdef ATMT_SUBMODULE_SERVER_ESP32_HTTPD_
        esp_err_t error = httpd_resp_send_chunk(getRequest(), content, cont_length);
        if (error != ESP_OK) {
            return HTTP_FAIL;
        }
        return HTTP_OK;
#endif
#ifdef ATMT_SUBMODULE_SERVER_ARDUINO_WIFI_
        // WiFiClient client = m_request->client();
        // if (!client.connected()) return HTTP_FAIL;

        // client.write(content, cont_length); // raw send

        m_request->sendContent(content);

        return HTTP_OK;
#endif
#ifdef ATMT_SUBMODULE_SERVER_ARDUINO_ASYNC_WIFI_
        // WiFiClient client = m_request->client();
        // if (!client.connected()) return HTTP_FAIL;

        // client.write(content, cont_length); // raw send

        m_request->sendContent(content);

        return HTTP_OK;
#endif

        return HTTP_FAIL;
    };
    atmtHTTPError HTTPRequest::writeRaw(const char* content, size_t cont_length) {
#ifdef ATMT_SUBMODULE_SERVER_ESP32_HTTPD_
        return sendResponseChunk(content, cont_length);
#endif
#ifdef ATMT_SUBMODULE_SERVER_ARDUINO_WIFI_
        WiFiClient client = m_request->client();
        if (!client.connected()) return HTTP_FAIL;

        client.write(content, cont_length); // raw send

        // m_request->sendContent(content);

        return HTTP_OK;
#endif

        return HTTP_FAIL;
    };
    atmtHTTPError HTTPRequest::sendResponseEndChunks() {
#ifdef ATMT_SUBMODULE_SERVER_ESP32_HTTPD_
        esp_err_t error = httpd_resp_send_chunk(getRequest(), NULL, 0);
        if (error != ESP_OK) {
            return HTTP_FAIL;
        }
        return HTTP_OK;
#endif
#ifdef ATMT_SUBMODULE_SERVER_ARDUINO_WIFI_
        WiFiClient client = m_request->client();
        if (!client.connected()) return HTTP_FAIL;

        client.flush(); // ensure all are sent
        return HTTP_OK;
#endif

        return HTTP_FAIL;
    };
#else
    atmtHTTPError HTTPRequest::streamChunks(const std::string& type, std::function<size_t(uint8_t*, size_t, size_t, void*)> callback, void* arg) {
        return streamChunks(type.c_str(), type.length(), callback, arg);
    };
    atmtHTTPError HTTPRequest::streamChunks(const char* type, size_t type_length, std::function<size_t(uint8_t*, size_t, size_t, void*)> callback, void* arg) {
        AsyncWebServerResponse* response = m_request->beginChunkedResponse(
            type,
            [callback, arg](uint8_t* buffer, size_t maxLen, size_t index) -> size_t {
                return callback(buffer, maxLen, index, arg);
            }
        );
        response->addHeader("Cache-Control", "no-cache");
        response->addHeader("Connection", "keep-alive");
        m_request->send(response);
        return HTTP_OK;
    };
#endif

    atmtHTTPError HTTPRequest::getPostData(std::string& data) {
#ifdef ATMT_SUBMODULE_SERVER_ESP32_HTTPD_
        int bytes_read = 0;
        // char buffer[m_post_buffer_size];
        std::vector<char> buffer(m_post_buffer_size);
        std::string full_data = "";
        int failed_reads = 0;
        while (bytes_read < getRequest()->content_len) {
            int left_to_read = std::min(static_cast<unsigned int>(buffer.size()), getRequest()->content_len - bytes_read);
            int new_bytes = httpd_req_recv(getRequest(), buffer.data(), left_to_read);
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
#ifdef ATMT_SUBMODULE_SERVER_ARDUINO_WIFI_
        String temp_str = m_request->arg("plain"); // Gets the request in plain text, but must check becomes sometimes it autoparses
        std::string full_data = temp_str.c_str();

        if (full_data.length() == 0 && m_request->args() > 0) {
            // This means that the data is in a form that the server auto parses (x-www-form-urlencoded)
            // For consistency, we will reconstruct the data here, but in reality we theoretically won't call this method if its already parsed
            full_data = "";
            for (int i = 0; i < m_request->args(); i++) {
                if (i > 0) {
                    full_data += "&";
                }
                String temp = m_request->argName(i);
                full_data += temp.c_str();
                full_data += "=";
                temp = m_request->arg(i);
                full_data += temp.c_str();
            }
        }
        data = std::move(full_data);

        return HTTP_OK;
#endif

        return HTTP_FAIL;
    };
    atmtHTTPError HTTPRequest::getPostType(std::string& type, std::string& raw_header) {
#ifdef ATMT_SUBMODULE_SERVER_ESP32_HTTPD_
        char content_type_raw[256];
        esp_err_t error = httpd_req_get_hdr_value_str(getRequest(), "Content-Type", content_type_raw, sizeof(content_type_raw));
        if (error != ESP_OK) {
            return HTTP_FAIL;
        }
        raw_header = std::string(content_type_raw); // Looks something like: 'multipart/form-data; boundary=----WebKitFormBoundaryXYZ'
#endif
#ifdef ATMT_SUBMODULE_SERVER_ARDUINO_WIFI_
        String cont_type = m_request->header("Content-Type");
        raw_header = cont_type.c_str();
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
    
#ifdef ATMT_SUBMODULE_HTTP_SERVER_JSON_PARSING_
    atmtHTTPError HTTPRequest::parseJSON(const std::string& post_data, const std::string& full_header, std::vector<POSTInfo>& parsed) {
#ifdef ATMT_SUBMODULE_SERVER_ESP32_HTTPD_
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
#ifdef ATMT_SUBMODULE_SERVER_ARDUINO_WIFI_
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
#endif
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
        for (const std::string& part : parts) {
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
#ifdef ATMT_SUBMODULE_SERVER_ARDUINO_WIFI_
        for (int i = 0; i < m_request->args(); i++) {
            POSTInfo element;
            String temp = m_request->argName(i);
            element.name = temp.c_str();
            temp = m_request->arg(i);
            element.data = temp.c_str();
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
#ifdef ATMT_SUBMODULE_SERVER_ARDUINO_WIFI_
        if (post_type != "application/x-www-form-urlencoded") {
#endif
            error = getPostData(post_data);
            if (error != HTTP_OK) {
                return HTTP_FAIL;
            }
#ifdef ATMT_SUBMODULE_SERVER_ARDUINO_WIFI_
        }
#endif

        if (post_type == "application/json") {
#ifdef ATMT_SUBMODULE_HTTP_SERVER_JSON_PARSING_
            error = parseJSON(post_data, full_header, parsed);
#else
            return HTTP_FAIL;
#endif
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
    
    // void HTTPRequest::scheduleOngoingConnection(HTMLPage* page, int ms_delay) {
    //     Timestamp time = getSystemTime();
    //     platform_print("At: ");
    //     platform_println(time.getTimeMS());
    //     time.addMS(ms_delay);
    //     platform_print("Scheduled for: ");
    //     platform_println(time.getTimeMS());
    //     m_server->scheduleOngoingConnection(page, this, time);
    // };
#ifdef ATMT_SUBMODULE_SERVER_ESP32_HTTPD_
    // HTTPSocket* HTTPRequest::toHTTPSocket() {
    //     return new HTTPSocket(m_server->getHTTPDHandle(), httpd_req_to_sockfd(getRequest()));
    // };
    atmtHTTPError HTTPRequest::toAsyncRequest() {
        esp_err_t error = httpd_req_async_handler_begin(m_request, &m_async_request);
        if (error != ESP_OK) {
            return HTTP_FAIL;
        }
        m_async_setup = true;
        return HTTP_OK;
    };
    httpd_req_t* HTTPRequest::getRequest() {
        if (!m_async_setup) {
            return m_request;
        }else {
            return m_async_request;
        }
    };
#endif

// #ifdef ATMT_SUBMODULE_SERVER_ESP32_HTTPD_
//     HTTPSocket::HTTPSocket(httpd_handle_t* httpd_handle, int socket):
//         m_httpd_handle{ httpd_handle },
//         m_socketfd{ socket }
//     {
        
//     };
//     HTTPSocket::~HTTPSocket() {

//     };
    
//     atmtHTTPError HTTPSocket::sendResponseChunk(const std::string& content) {
//         return sendResponseChunk(content.c_str(), content.length());
//     };
//     atmtHTTPError HTTPSocket::sendResponseChunk(const char* content, size_t cont_length) {
//         esp_err_t error = httpd_socket_send(*m_httpd_handle, m_socketfd, content, cont_length, 0);
//         if (error != ESP_OK) {
//             return HTTP_FAIL;
//         }
//         return HTTP_OK;
//     };
//     atmtHTTPError HTTPSocket::sendResponseEndChunks() {
//         esp_err_t error = httpd_socket_send(*m_httpd_handle, m_socketfd, nullptr, 0, 0);
//         if (error != ESP_OK) {
//             return HTTP_FAIL;
//         }
//         return HTTP_OK;
//     };
// #endif


    HTMLPage::HTMLPage(const std::string& path, atmtHTTPMethod method):
        m_path{ path },
        m_method{ method },
        m_server{ nullptr }
    {
        // strncpy(m_path, path, sizeof(m_path) - 1);
        // m_path[sizeof(m_path) - 1] = '\0';
    };
    void HTMLPage::setServer(HTTPServer* server) {
        m_server = server;
    };
    esp_err_t HTMLPage::handle_request(HTTPRequest* request) {
        return ESP_OK;
    };
// #ifdef ATMT_SUBMODULE_SERVER_ARDUINO_WIFI_
#ifndef ATMT_SUBMODULE_SERVER_ARDUINO_ASYNC_WIFI_
    esp_err_t HTMLPage::continue_connection(HTTPRequest* request) {
        return ESP_OK;
    };
#endif
// #endif
// #ifdef ATMT_SUBMODULE_SERVER_ESP32_HTTPD_
//     esp_err_t HTMLPage::continue_connection(HTTPSocket* socket) {
//         return ESP_OK;
//     };
// #endif
    const std::string& HTMLPage::getPath() const {
        return m_path;
    }
#ifdef ATMT_SUBMODULE_SERVER_ESP32_HTTPD_
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
#ifdef ATMT_SUBMODULE_SERVER_ARDUINO_WIFI_
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
#ifdef ATMT_SUBMODULE_SERVER_ARDUINO_ASYNC_WIFI_
    WebRequestMethodComposite HTMLPage::getMethod() const {
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

#ifndef ATMT_SUBMODULE_SERVER_ARDUINO_ASYNC_WIFI_
    // void HTMLPage::scheduleOngoingConnection(HTTPSocket* socket, int ms_delay) {
    void HTMLPage::scheduleOngoingConnection(HTTPRequest* request, int ms_delay) {
        Timestamp time = getSystemTime();
        // platform_print("At: ");
        // platform_println(time.getTimeMS());
        time.addMS(ms_delay);
        // platform_print("Scheduled for: ");
        // platform_println(time.getTimeMS());
        m_server->scheduleOngoingConnection(this, request, time);
    };
#endif

};

#else
// #error "Enable ATMT_SUBMODULE_HTTP_SERVER_ in automat_submodules.h to use HTMLPage"
#endif
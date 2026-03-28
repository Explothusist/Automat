#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_HTTP_SERVER_

#include "HTMLPage.h"

namespace atmt {

    HTMLPage_Static_ReturnCode::HTMLPage_Static_ReturnCode(const std::string& path, const std::string& response, int code):
        HTMLPage(path, Method_Get),
        m_response{ response },
        m_code{ code }
    {

    };
    esp_err_t HTMLPage_Static_ReturnCode::handle_request(HTTPRequest* request) {
        // Tell the client to expect raw HTML text
        // Send the basic webpage text once
        atmtHTTPError error = request->sendResponse("text/html", m_response, m_code);
        if (error != HTTP_OK) {
            return ESP_FAIL;
        }

        return ESP_OK;
    };
    
    HTMLPage_Static_RawHTML::HTMLPage_Static_RawHTML(const std::string& path, const std::string& html):
        HTMLPage(path, Method_Get),
        m_html{ html }
    {
        // if (html) {
        //     m_html = strdup(html);
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
    esp_err_t HTMLPage_Static_DynamicHTML::handle_request(HTTPRequest* request) {
        // Tell the client to expect raw HTML text
        // Send the basic webpage text once
        atmtHTTPError error = request->sendResponse("text/html", m_html_getter(m_arg));
        if (error != HTTP_OK) {
            return ESP_FAIL;
        }

        return ESP_OK;
    };
#ifdef ATMT_SUBMODULE_SERVER_ARDUINO_ASYNC_WIFI_
    HTMLPage_Static_DynamicChunkedHTML::HTMLPage_Static_DynamicChunkedHTML(const std::string& path, std::function<const char*(int, size_t&, void*)> html_get_next_chunk, void* arg):
        HTMLPage(path, Method_Get),
        m_html_get_next_chunk{ html_get_next_chunk },
        m_arg{ arg }
    {
        
    };
    esp_err_t HTMLPage_Static_DynamicChunkedHTML::handle_request(HTTPRequest* request) {
        // Tell the client to expect raw HTML text
        // Send the basic webpage text once
        // atmtHTTPError error = request->sendResponse("text/html", m_html_getter(m_arg));
        // if (error != HTTP_OK) {
        //     return ESP_FAIL;
        // }
#ifdef ATMT_SUBMODULE_SERVER_ARDUINO_ASYNC_WIFI_
        // m_chunks = m_html_getter(m_arg);
        m_chunk_count = 0;
        m_chunk = m_html_get_next_chunk(m_chunk_count, m_chunk_length, m_arg);
        m_chunk_index = 0;
        // m_chunk_char_index = 0;
        request->streamChunks("text/html", streamCallback, this);
#endif

        return ESP_OK;
    };
#ifdef ATMT_SUBMODULE_SERVER_ARDUINO_ASYNC_WIFI_
    size_t HTMLPage_Static_DynamicChunkedHTML::streamCallback(uint8_t* buffer, size_t maxLen, size_t index, void* arg) {
        HTMLPage_Static_DynamicChunkedHTML* page = static_cast<HTMLPage_Static_DynamicChunkedHTML*>(arg);
        // Copy up to maxLen bytes from content to buffer
        size_t total_sent = 0;
        while (total_sent < maxLen && page->m_chunk_length != 0 && page->m_chunk) {
            size_t bytes_to_send = std::min(page->m_chunk_length - page->m_chunk_index, maxLen - total_sent);
            if (bytes_to_send > 0) {
                memcpy(buffer, page->m_chunk + page->m_chunk_index, bytes_to_send);
                page->m_chunk_index += bytes_to_send;
                total_sent += bytes_to_send;
            }
            if (page->m_chunk_index >= page->m_chunk_length) {
                page->m_chunk_index = 0;
                page->m_chunk_count += 1;
                page->m_chunk = page->m_html_get_next_chunk(page->m_chunk_count, page->m_chunk_length, page->m_arg);
            }
        }
        // size_t total_sent = 0;
        // while (total_sent < maxLen && page->m_chunk_index < page->m_chunks.size()) {
        //     size_t bytes_to_send = std::min(page->m_chunks[page->m_chunk_index].size() - page->m_chunk_char_index, maxLen - total_sent);
        //     if (bytes_to_send > 0) {
        //         memcpy(buffer, page->m_chunks[page->m_chunk_index].c_str() + page->m_chunk_char_index, bytes_to_send);
        //         page->m_chunk_char_index += bytes_to_send;
        //         total_sent += bytes_to_send;
        //     }
        //     if (page->m_chunk_char_index >= page->m_chunks[page->m_chunk_index].size()) {
        //         page->m_chunk_char_index = 0;
        //         page->m_chunk_index += 1;
        //     }
        // }
        return total_sent;
    };
#endif
#endif

    HTMLPage_Static_DynamicPostHTML::HTMLPage_Static_DynamicPostHTML(const std::string& path, std::function<std::string(const std::vector<POSTInfo>&, void*)> post_sender, void* arg):
        HTMLPage(path, Method_Post),
        m_post_sender{ post_sender },
        m_arg{ arg }
    {
        
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


#ifdef ATMT_SUBMODULE_HTTP_SERVER_JPEG_STREAMING_
    HTMLPage_Dynamic_JPEGStreamer::HTMLPage_Dynamic_JPEGStreamer(const std::string& path, std::function<char*(size_t&, void*)> jpeg_getter, double frame_rate, void* arg):
        HTMLPage(path, Method_Get),
        m_jpeg_getter{ jpeg_getter },
        m_frame_delay_mS{ static_cast<int>(1000.0 / frame_rate) },
        m_arg{ arg }
    {
        
    };
    esp_err_t HTMLPage_Dynamic_JPEGStreamer::handle_request(HTTPRequest* request) {
        // httpd_req_t contains http metadata about the incoming request
        // So at this point, someone has visited http://<ip-address>/stream
        // camera_fb_t* frame_buffer = NULL;

        // Tell the client we are not sending one image, but many images each seperated by boundaries
        // static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=frame";

        // static const char* _STREAM_BOUNDARY = "\r\n--frame\r\n"; // Indicates boundary (and draws prior image)

        // static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n"; // Indicates image attributes

        // httpd_resp_set_type(request, _STREAM_CONTENT_TYPE); // Sets the response type according to above
        // request->setResponseType("multipart/x-mixed-replace;boundary=frame");
        // String response = 
        //     "HTTP/1.1 200 OK\r\n"
        //     "Content-Type: multipart/x-mixed-replace; boundary=frame\r\n\r\n";

#ifdef ATMT_SUBMODULE_SERVER_ARDUINO_WIFI_
        atmtHTTPError error = request->sendResponseChunk("HTTP/1.1 200 OK\r\nContent-Type: multipart/x-mixed-replace; boundary=frame\r\n\r\n");
        if (error != HTTP_OK) {
            return ESP_FAIL;
        }
#endif
#ifdef ATMT_SUBMODULE_SERVER_ESP32_HTTPD_
        atmtHTTPError error = request->setResponseType("multipart/x-mixed-replace; boundary=frame");
        if (error != HTTP_OK) {
            return ESP_FAIL;
        }
#endif
        platform_println("Header Sent");

        // char part_buffer[128]; // Temporary storage for c-strings
        // size_t img_length;
        // size_t old_img_length;
        // char* raw_buffer = m_jpeg_getter(img_length, m_arg);
        m_header_index = 0;
        m_buffer_index = 0;
        m_footer_index = 0;
        m_raw_buffer = m_jpeg_getter(m_img_length, m_arg);
        // std::string buffer = std::string(raw_buffer, img_length);
        m_old_img_length = m_img_length;

        m_header_length = snprintf(m_part_buffer, sizeof(m_part_buffer), "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n", m_img_length);

#ifndef ATMT_SUBMODULE_SERVER_ARDUINO_ASYNC_WIFI_
        // esp_err_t still_connected = httpd_resp_send_chunk(request, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY)); // Starts new frame
        error = request->sendResponseChunk("\r\n--frame\r\n"); // Starts new frame
        if (error != HTTP_OK) {
            return ESP_FAIL; // Ends request, probably because client disconnected
        }
#endif
        platform_println("First Boundary Sent");

#ifdef ATMT_SUBMODULE_SERVER_ARDUINO_WIFI_
        while (true) { // Connection continues as long as client stays active and connected
            esp_err_t esp_error = continue_connection(request);
            if (esp_error != ESP_OK) {
                return ESP_FAIL;
            }
        }
#endif
#ifdef ATMT_SUBMODULE_SERVER_ESP32_HTTPD_
        platform_println("Preparing Socket");
        esp_err_t esp_error = request->toAsyncRequest();
        if (esp_error != ESP_OK) {
            return ESP_FAIL;
        }
        // esp_error = continue_connection(request->toHTTPSocket());
        esp_error = continue_connection(request);
        if (esp_error != ESP_OK) {
            return ESP_FAIL;
        }
#endif
        platform_println("Continue Connection Complete");

#ifdef ATMT_SUBMODULE_SERVER_ARDUINO_ASYNC_WIFI_
        request->streamChunks("multipart/x-mixed-replace; boundary=frame", streamCallback, this);
#endif

        // platform_println("Request Closed");
        return ESP_OK;
    };
// #ifdef ATMT_SUBMODULE_SERVER_ARDUINO_WIFI_
#ifndef ATMT_SUBMODULE_SERVER_ARDUINO_ASYNC_WIFI_
    esp_err_t HTMLPage_Dynamic_JPEGStreamer::continue_connection(HTTPRequest* request) {
        Timestamp start_of_request = getSystemTime();

        // frame_buffer = esp_camera_fb_get();
        // if (!frame_buffer) {
        //     ESP_LOGE("STREAM", "Camera Capture Failed");
        //     return ESP_FAIL; // Ends request (requires reload?)
        // }
        m_raw_buffer = m_jpeg_getter(m_img_length, m_arg);
        if (m_raw_buffer) {
            // buffer = std::string(raw_buffer, img_length);
            if (m_img_length != m_old_img_length) {
                m_old_img_length = m_img_length;
                m_header_length = snprintf(m_part_buffer, sizeof(m_part_buffer), "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n", m_img_length);
            }

            // Sets up the header to tell the client what it is receiving
            // size_t header_length = snprintf(part_buffer, sizeof(part_buffer), _STREAM_PART, frame_buffer->len);

            // httpd_resp_send_chunk(request, part_buffer, header_length);
            atmtHTTPError error = request->sendResponseChunk(m_part_buffer);
            if (error != HTTP_OK) {
                return ESP_FAIL; // Ends request, probably because client disconnected
            }

            // Send the image itself as a series of chars (a c-string)
            // httpd_resp_send_chunk(request, (const char*)frame_buffer->buf, frame_buffer->len);
            error = request->writeRaw(m_raw_buffer, m_img_length);
            if (error != HTTP_OK) {
                return ESP_FAIL; // Ends request, probably because client disconnected
            }
            error = request->sendResponseChunk("\r\n");
            if (error != HTTP_OK) {
                return ESP_FAIL; // Ends request, probably because client disconnected
            }
            
            // esp_err_t still_connected = httpd_resp_send_chunk(request, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY)); // Starts new frame
            error = request->sendResponseChunk("\r\n--frame\r\n"); // Starts new frame
            if (error != HTTP_OK) {
                return ESP_FAIL; // Ends request, probably because client disconnected
            }

            // request->sendResponseEndChunks();
            // httpd_resp_send_chunk(request, NULL, 0); // Specifies the end of the chunks
            // Only needed because we are sending individual chunks

            // esp_camera_fb_return(frame_buffer); // We are done with the capture now
        }else {
            // request->sendResponseEndChunks();
            // return ESP_FAIL;
            // continue;
        }

        Timestamp end_of_request = getSystemTime();
        int ms_delay = std::max(m_frame_delay_mS - start_of_request.getTimeDifferenceMS(end_of_request), 0);

        // vTaskDelay(pdMS_TO_TICKS(m_frame_delay_mS)); // Wait for a moment to reset watchdogs and keep CPU below 100%
#ifndef ATMT_SUBMODULE_SERVER_ARDUINO_WIFI_
        scheduleOngoingConnection(request, ms_delay);
#else
        delay(ms_delay);
#endif
        
        return ESP_OK;
    };
#endif
#ifdef ATMT_SUBMODULE_SERVER_ARDUINO_ASYNC_WIFI_
    size_t HTMLPage_Dynamic_JPEGStreamer::streamCallback(uint8_t* buffer, size_t maxLen, size_t index, void* arg) {
        HTMLPage_Dynamic_JPEGStreamer* page = static_cast<HTMLPage_Dynamic_JPEGStreamer*>(arg);
        // Timestamp start_of_request = getSystemTime();
        // Build MJPEG multipart chunk
        // static const char* header = "--frame\r\nContent-Type: image/jpeg\r\nContent-Length: ";
        // static const int header_length = 52;
        static const char* footer = "\r\n";
        static const int footer_length = 2;

        if (!page->m_raw_buffer) {
            page->m_raw_buffer = page->m_jpeg_getter(page->m_img_length, page->m_arg);
            if (page->m_raw_buffer) {
                page->m_header_index = 0;
                page->m_buffer_index = 0;
                page->m_footer_index = 0;
                page->m_header_length = snprintf(page->m_part_buffer, sizeof(page->m_part_buffer), "--frame\r\nContent-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n", page->m_img_length);
            }
        }

        if (page->m_raw_buffer) { // Half streamed image
            size_t total_bytes_sent = 0;
            if (page->m_header_index < page->m_header_length) {
                size_t bytes_to_send = std::min(maxLen, page->m_header_length - page->m_header_index);
                if (bytes_to_send > 0) {
                    memcpy(buffer + total_bytes_sent, page->m_part_buffer + page->m_header_index, bytes_to_send);
                    page->m_header_index += bytes_to_send;
                }
                maxLen -= bytes_to_send;
                total_bytes_sent += bytes_to_send;
                if (maxLen == 0) {
                    return total_bytes_sent;
                }
            }
            if (page->m_buffer_index < page->m_img_length) {
                size_t bytes_to_send = std::min(maxLen, page->m_img_length - page->m_buffer_index);
                if (bytes_to_send > 0) {
                    memcpy(buffer + total_bytes_sent, page->m_raw_buffer + page->m_buffer_index, bytes_to_send);
                    page->m_buffer_index += bytes_to_send;
                }
                maxLen -= bytes_to_send;
                total_bytes_sent += bytes_to_send;
                // if (maxLen == 0) {
                    // return total_bytes_sent;
                // }
            }
            if (page->m_footer_index < footer_length) {
                size_t bytes_to_send = std::min(maxLen, footer_length - (page->m_footer_index));
                if (bytes_to_send > 0) {
                    memcpy(buffer + total_bytes_sent, footer + (page->m_footer_index), bytes_to_send);
                    page->m_footer_index += bytes_to_send;
                }
                maxLen -= bytes_to_send;
                total_bytes_sent += bytes_to_send;
                if (maxLen == 0) {
                    return total_bytes_sent;
                }
            }

            if (page->m_footer_index >= footer_length) {
                page->m_raw_buffer = nullptr;
            }
            return total_bytes_sent;
        }

        // page->m_raw_buffer = page->m_jpeg_getter(page->m_img_length, page->m_arg);
        // if (page->m_raw_buffer) {
        //     if (page->m_img_length != page->m_old_img_length) {
        //         page->m_old_img_length = page->m_img_length;
        //         page->m_header_length = snprintf(page->m_part_buffer, sizeof(page->m_part_buffer), "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n", page->m_img_length);
        //     }

        //     int headerLen = snprintf(nullptr, 0, "%s%zu\r\n\r\n", header, page->m_img_length);

        //     // Ensure we don't exceed maxLen
        //     if (headerLen + page->m_img_length + 2 > maxLen) {
        //         return 0; // Can't fit
        //     }

        //     int pos = 0;
        //     pos += sprintf((char*)buffer + pos, "%s%zu\r\n\r\n", header, page->m_img_length);
        //     memcpy(buffer + pos, page->m_raw_buffer, page->m_img_length);
        //     pos += page->m_img_length;
        //     memcpy(buffer + pos, "\r\n", 2);
        //     pos += 2;

        //     return pos; // bytes written to buffer
        // }

        // Timestamp end_of_request = getSystemTime();
        // int ms_delay = std::max(m_frame_delay_mS - start_of_request.getTimeDifferenceMS(end_of_request), 0);
        return 0;
    };
#endif
// #endif
// #ifdef ATMT_SUBMODULE_SERVER_ESP32_HTTPD_
//     esp_err_t HTMLPage_Dynamic_JPEGStreamer::continue_connection(HTTPSocket* socket) {
//         platform_println("Done Preparing Socket");
//         Timestamp start_of_request = getSystemTime();
//
//         platform_println("Fetching Image Buffer");
//         m_raw_buffer = m_jpeg_getter(m_img_length, m_arg);
//         if (m_raw_buffer) {
//             platform_println("Buffer Obtained");
//             if (m_img_length != m_old_img_length) {
//                 m_old_img_length = m_img_length;
//                 m_header_length = snprintf(m_part_buffer, sizeof(m_part_buffer), "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n", m_img_length);
//             }
//
//             platform_println("Prep Send First Chunk");
//             // Sets up the header to tell the client what it is receiving
//             atmtHTTPError error = socket->sendResponseChunk(m_part_buffer);
//             platform_println("Send First Chunk");
//             if (error != HTTP_OK) {
//                 return ESP_FAIL; // Ends request, probably because client disconnected
//             }
//
//             // Send the image itself as a series of chars (a c-string)
//             error = socket->sendResponseChunk(m_raw_buffer, m_img_length);
//             platform_println("Send Second Chunk");
//             if (error != HTTP_OK) {
//                 return ESP_FAIL; // Ends request, probably because client disconnected
//             }
//             error = socket->sendResponseChunk("\r\n");
//             platform_println("Send Third Chunk");
//             if (error != HTTP_OK) {
//                 return ESP_FAIL; // Ends request, probably because client disconnected
//             }
//            
//             error = socket->sendResponseChunk("\r\n--frame\r\n"); // Starts new frame
//             platform_println("Send Fourth Chunk");
//             if (error != HTTP_OK) {
//                 return ESP_FAIL; // Ends request, probably because client disconnected
//             }
//         }
//
//         Timestamp end_of_request = getSystemTime();
//         int ms_delay = std::max(m_frame_delay_mS - start_of_request.getTimeDifferenceMS(end_of_request), 0);
//
// #ifndef ATMT_SUBMODULE_SERVER_ARDUINO_WIFI_
//         scheduleOngoingConnection(socket, ms_delay);
// #else
//         delay(ms_delay);
// #endif
//         platform_println("Next Frame Scheduled");
//        
//         return ESP_OK;
//     };
// #endif
#endif

    static const char default_favicon[318] = {
        // Offset 0x00000000 to 0x0000013D
        0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x10, 0x10, 0x10, 0x00, 0x01, 0x00,
        0x04, 0x00, 0x28, 0x01, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x28, 0x00,
        0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x01, 0x00,
        0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0x48,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x11, 0x11, 0x11, 0x11,
        0x10, 0x00, 0x01, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x10, 0x02, 0x21,
        0x11, 0x11, 0x12, 0x21, 0x11, 0x10, 0x12, 0x21, 0x11, 0x11, 0x12, 0x21,
        0x12, 0x21, 0x12, 0x21, 0x11, 0x11, 0x12, 0x21, 0x22, 0x22, 0x12, 0x22,
        0x21, 0x11, 0x12, 0x21, 0x22, 0x12, 0x12, 0x22, 0x22, 0x22, 0x12, 0x21,
        0x22, 0x11, 0x11, 0x22, 0x22, 0x22, 0x22, 0x11, 0x22, 0x11, 0x11, 0x22,
        0x11, 0x22, 0x22, 0x11, 0x22, 0x11, 0x11, 0x22, 0x11, 0x11, 0x22, 0x11,
        0x22, 0x11, 0x11, 0x12, 0x21, 0x12, 0x21, 0x22, 0x22, 0x21, 0x11, 0x12,
        0x21, 0x12, 0x21, 0x22, 0x22, 0x22, 0x11, 0x12, 0x21, 0x12, 0x21, 0x11,
        0x22, 0x11, 0x01, 0x11, 0x22, 0x22, 0x11, 0x11, 0x22, 0x10, 0x01, 0x11,
        0x22, 0x22, 0x11, 0x11, 0x12, 0x10, 0x00, 0x01, 0x11, 0x11, 0x11, 0x11,
        0x10, 0x00, 0xE0, 0x07, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0x80, 0x01,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0x80, 0x01,
        0x00, 0x00, 0xE0, 0x07, 0x00, 0x00
    };
    HTMLPage_Static_Favicon::HTMLPage_Static_Favicon():
        HTMLPage_Static_Favicon("/favicon.ico", (char*)default_favicon, 318)
    {
        
    };
    HTMLPage_Static_Favicon::HTMLPage_Static_Favicon(char* favicon, size_t favicon_length):
        HTMLPage_Static_Favicon("/favicon.ico", favicon, favicon_length)
    {

    };
    HTMLPage_Static_Favicon::HTMLPage_Static_Favicon(const std::string& path):
        HTMLPage_Static_Favicon(path, (char*)default_favicon, 318)
    {

    };
    HTMLPage_Static_Favicon::HTMLPage_Static_Favicon(const std::string& path, char* favicon, size_t favicon_length):
        HTMLPage(path, Method_Get),
        m_favicon{ favicon },
        m_favicon_length{ favicon_length }
    {
        
    };
    esp_err_t HTMLPage_Static_Favicon::handle_request(HTTPRequest* request) {
        platform_println(std::to_string(*m_favicon));
        platform_println(std::to_string(m_favicon_length));
#ifdef ATMT_SUBMODULE_SERVER_ARDUINO_WIFI_
        atmtHTTPError error = request->sendResponseChunk("HTTP/1.1 200 OK\r\nContent-Type: image/x-icon\r\n\r\n");
        if (error != HTTP_OK) {
            return ESP_FAIL;
        }
#endif
#ifdef ATMT_SUBMODULE_SERVER_ESP32_HTTPD_
        atmtHTTPError error = request->setResponseType("image/x-icon");
        if (error != HTTP_OK) {
            return ESP_FAIL;
        }
#endif
#ifndef ATMT_SUBMODULE_SERVER_ARDUINO_ASYNC_WIFI_
        error = request->writeRaw((const char*)m_favicon, m_favicon_length);
        request->sendResponseChunk("");
        request->sendResponseEndChunks();
        if (error != HTTP_OK) {
            return ESP_FAIL;
        }
#else
        // request->sendResponseRaw("image/x-icon", (const char*)m_favicon, m_favicon_length);
        request->streamChunks("application/octet-stream", streamCallback, this);
#endif

        return ESP_OK;
    };
#ifdef ATMT_SUBMODULE_SERVER_ARDUINO_ASYNC_WIFI_
    size_t HTMLPage_Static_Favicon::streamCallback(uint8_t* buffer, size_t maxLen, size_t index, void* arg) {
        HTMLPage_Static_Favicon* page = static_cast<HTMLPage_Static_Favicon*>(arg);
        // Copy up to maxLen bytes from content to buffer
        size_t bytesToSend = std::min(page->m_favicon_length - index, maxLen);
        if (bytesToSend > 0) {
            memcpy(buffer, page->m_favicon + index, bytesToSend);
            return bytesToSend;
        }
        return 0; // done sending
    };
#endif

};

#else
// #error "Enable ATMT_SUBMODULE_HTTP_SERVER_ in automat_submodules.h to use HTMLPage"
#endif
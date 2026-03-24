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
    HTMLPage_Dynamic_JPEGStreamer::HTMLPage_Dynamic_JPEGStreamer(const std::string& path, std::function<char*(size_t&, void*)> jpeg_getter, int frame_rate, void* arg):
        HTMLPage(path, Method_Get),
        m_jpeg_getter{ jpeg_getter },
        m_frame_delay_mS{ 1000 / frame_rate },
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

        atmtHTTPError error = request->sendResponseChunk("HTTP/1.1 200 OK\r\nContent-Type: multipart/x-mixed-replace; boundary=frame\r\n\r\n");
        if (error != HTTP_OK) {
            return ESP_FAIL;
        }

        // char part_buffer[128]; // Temporary storage for c-strings
        // size_t img_length;
        // size_t old_img_length;
        // char* raw_buffer = m_jpeg_getter(img_length, m_arg);
        m_raw_buffer = m_jpeg_getter(m_img_length, m_arg);
        // std::string buffer = std::string(raw_buffer, img_length);
        m_old_img_length = m_img_length;

        m_header_length = snprintf(m_part_buffer, sizeof(m_part_buffer), "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n", m_img_length);

        // while (true) { // Connection continues as long as client stays active and connected
        // }
        continue_connection(request);

        // Serial.println("Request Closed");
        return ESP_OK;
    };
    esp_err_t HTMLPage_Dynamic_JPEGStreamer::continue_connection(HTTPRequest* request) {
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

            // esp_err_t still_connected = httpd_resp_send_chunk(request, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY)); // Starts new frame
            atmtHTTPError error = request->sendResponseChunk("\r\n--frame\r\n"); // Starts new frame
            if (error != HTTP_OK) {
                return ESP_FAIL; // Ends request, probably because client disconnected
            }

            // Sets up the header to tell the client what it is receiving
            // size_t header_length = snprintf(part_buffer, sizeof(part_buffer), _STREAM_PART, frame_buffer->len);

            // httpd_resp_send_chunk(request, part_buffer, header_length);
            error = request->sendResponseChunk(m_part_buffer);
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

            // request->sendResponseEndChunks();
            // httpd_resp_send_chunk(request, NULL, 0); // Specifies the end of the chunks
            // Only needed because we are sending individual chunks

            // esp_camera_fb_return(frame_buffer); // We are done with the capture now
        }else {
            // request->sendResponseEndChunks();
            // return ESP_FAIL;
            // continue;
        }

        // vTaskDelay(pdMS_TO_TICKS(m_frame_delay_mS)); // Wait for a moment to reset watchdogs and keep CPU below 100%
        request->scheduleOngoingConnection(this, m_frame_delay_mS);
        
        return ESP_OK;
    };
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
        HTMLPage_Static_Favicon((char*)default_favicon, 318, "/favicon.ico")
    {
        
    };
    HTMLPage_Static_Favicon::HTMLPage_Static_Favicon(char* favicon, size_t favicon_length):
        HTMLPage_Static_Favicon(favicon, favicon_length, "/favicon.ico")
    {

    };
    HTMLPage_Static_Favicon::HTMLPage_Static_Favicon(const std::string& path):
        HTMLPage_Static_Favicon((char*)default_favicon, 318, path)
    {

    };
    HTMLPage_Static_Favicon::HTMLPage_Static_Favicon(char* favicon, size_t favicon_length, const std::string& path):
        HTMLPage(path, Method_Get),
        m_favicon{ favicon },
        m_favicon_length{ favicon_length }
    {
        
    };
    esp_err_t HTMLPage_Static_Favicon::handle_request(HTTPRequest* request) {
        Serial.println(*m_favicon);
        Serial.println(m_favicon_length);
        atmtHTTPError error = request->sendResponseChunk("HTTP/1.1 200 OK\r\nContent-Type: image/x-icon\r\n\r\n");
        if (error != HTTP_OK) {
            return ESP_FAIL;
        }
        error = request->writeRaw((const char*)m_favicon, m_favicon_length);
        request->sendResponseChunk("");
        request->sendResponseEndChunks();
        if (error != HTTP_OK) {
            return ESP_FAIL;
        }

        return ESP_OK;
    };

};

#else
// #error "Enable ATMT_SUBMODULE_HTTP_SERVER_ in automat_submodules.h to use HTMLPage"
#endif
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


#ifdef ATMT_SUBMODULE_HTTP_SERVER_JPEG_STREAMING_
    HTMLPage_Dynamic_JPEGStreamer::HTMLPage_Dynamic_JPEGStreamer(const std::string& path, std::function<char*(size_t&, void*)> jpeg_getter, int frame_rate, void* arg):
        HTMLPage(path, Method_Post),
        m_jpeg_getter{ jpeg_getter },
        m_frame_delay_mS{ 1000 / frame_rate },
        m_arg{ arg }
    {
        
    };
    HTMLPage_Dynamic_JPEGStreamer::~HTMLPage_Dynamic_JPEGStreamer() {
        
    };
    esp_err_t HTMLPage_Dynamic_JPEGStreamer::handle_request(HTTPRequest* request) {
        // httpd_req_t contains http metadata about the incoming request
        // So at this point, someone has visited http://<ip-address>/stream
        // camera_fb_t* frame_buffer = NULL;
        char part_buffer[128]; // Temporary storage for c-strings

        // Tell the client we are not sending one image, but many images each seperated by boundaries
        // static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=frame";

        // static const char* _STREAM_BOUNDARY = "\r\n--frame\r\n"; // Indicates boundary (and draws prior image)

        // static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n"; // Indicates image attributes

        // httpd_resp_set_type(request, _STREAM_CONTENT_TYPE); // Sets the response type according to above
        request->setResponseType("multipart/x-mixed-replace;boundary=frame");

        size_t img_length;
        size_t old_img_length;
        char* raw_buffer = m_jpeg_getter(img_length, m_arg);
        // std::string buffer = std::string(raw_buffer, img_length);
        old_img_length = img_length;

        size_t header_length = snprintf(part_buffer, sizeof(part_buffer), "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n", img_length);

        while (true) { // Connection continues as long as client stays active and connected
            // frame_buffer = esp_camera_fb_get();
            // if (!frame_buffer) {
            //     ESP_LOGE("STREAM", "Camera Capture Failed");
            //     return ESP_FAIL; // Ends request (requires reload?)
            // }
            raw_buffer = m_jpeg_getter(img_length, m_arg);
            if (!raw_buffer) {
                request->sendResponseEndChunks();
                return ESP_FAIL;
            }
            // buffer = std::string(raw_buffer, img_length);
            if (img_length != old_img_length) {
                old_img_length = img_length;
                header_length = snprintf(part_buffer, sizeof(part_buffer), "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n", img_length);
            }

            // esp_err_t still_connected = httpd_resp_send_chunk(request, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY)); // Starts new frame
            atmtHTTPError error = request->sendResponseChunk("\r\n--frame\r\n"); // Starts new frame
            if (error != HTTP_OK) {
                return ESP_FAIL; // Ends request, probably because client disconnected
            }

            // Sets up the header to tell the client what it is receiving
            // size_t header_length = snprintf(part_buffer, sizeof(part_buffer), _STREAM_PART, frame_buffer->len);

            // httpd_resp_send_chunk(request, part_buffer, header_length);
            error = request->sendResponseChunk(part_buffer);
            if (error != HTTP_OK) {
                return ESP_FAIL; // Ends request, probably because client disconnected
            }

            // Send the image itself as a series of chars (a c-string)
            // httpd_resp_send_chunk(request, (const char*)frame_buffer->buf, frame_buffer->len);
            error = request->sendResponseChunk(raw_buffer, img_length);
            if (error != HTTP_OK) {
                return ESP_FAIL; // Ends request, probably because client disconnected
            }

            // request->sendResponseEndChunks();
            // httpd_resp_send_chunk(request, NULL, 0); // Specifies the end of the chunks
            // Only needed because we are sending individual chunks

            // esp_camera_fb_return(frame_buffer); // We are done with the capture now

            vTaskDelay(pdMS_TO_TICKS(m_frame_delay_mS)); // Wait for a moment to reset watchdogs and keep CPU below 100%
        }

        return ESP_OK;
    };
#endif

};

#else
// #error "Enable ATMT_SUBMODULE_HTTP_SERVER_ in automat_submodules.h to use HTMLPage"
#endif
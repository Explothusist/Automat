
#include "HTTPServer.h"


namespace atmt {

    HTTPServer::HTTPServer():
        m_server{ nullptr },
        m_html_pages{ },
        m_wifi_init{ false },
        m_server_init{ false }
    {

    };
    HTTPServer::~HTTPServer() {
        killServer();

        // for (size_t i = 0; i < m_html_pages.size(); i++) {
        for (HTMLPage* page : m_html_pages) {
            delete page;
            // page = nullptr; // This line actually does nothing, because it is copy, not real
        }
        m_html_pages.clear();
    };

    void HTTPServer::registerStaticPage_RawHTML(const char* url, const char* html) {
        HTMLPage* page = new HTMLPage_Static_RawHTML(url, html);
        m_html_pages.push_back(page);
        if (m_server_init) {
            registerPage(page);
        }
    };

    void HTTPServer::wifiInit(const char* wifi_ssid, const char* wifi_password) {
        if (!m_wifi_init) {
            ESP_ERROR_CHECK(nvs_flash_init()); // NVS (Non-Volatile Storage)
            ESP_ERROR_CHECK(esp_netif_init()); // Initializes TCP/IP stack
            ESP_ERROR_CHECK(esp_event_loop_create_default()); // Creates event dispatcher

            esp_netif_create_default_wifi_sta(); // STA - client, AP - router
            wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT(); // Just use default settings
            ESP_ERROR_CHECK(esp_wifi_init(&config));

            // Register callbacks for the events we want to the wifi_event_handler() function above
            ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &HTTPServer::wifiEventHandler, this, NULL));
            ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &HTTPServer::wifiEventHandler, this, NULL));

            wifi_config_t wifi_config = { 0 };
            strncpy((char*)wifi_config.sta.ssid, wifi_ssid, sizeof(wifi_config.sta.ssid) - 1); // Grab the wifi network info so it knows what and how to connect
            strncpy((char*)wifi_config.sta.password, wifi_password, sizeof(wifi_config.sta.password) - 1); // -1 is to ensure there is a character to null terminate the string
            wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK; // Prevents connecting to open networks (may remove temporarily)
            // wifi_config.sta.threshold.authmode = WIFI_AUTH_OPEN;
            wifi_config.sta.pmf_cfg.capable = true; // Tells the router "I am modern and flexible"
            wifi_config.sta.pmf_cfg.required = false;

            esp_wifi_set_mode(WIFI_MODE_STA); // STA - client, AP - router
            esp_wifi_set_config(WIFI_IF_STA, &wifi_config); // Actually loads config
            esp_wifi_start(); // Finally actually starts the wifi

            esp_wifi_set_ps(WIFI_PS_NONE); // Turns off power saving measures because we are streaming
            m_wifi_init = true;
        }
    };
    void HTTPServer::startServer() {
        if (!m_server_init) {
            httpd_config_t config = HTTPD_DEFAULT_CONFIG(); // Start with default, then adjust specific

            config.server_port = 80; // Means that client does not need to specify a port 
            config.ctrl_port = 32768; // Private internal port
            config.max_uri_handlers = 8; // Number of unique requests to endpoints (/, /stream, different users)
            config.stack_size = 16384; // Memory allocated to each incoming request
            config.lru_purge_enable = true; // Drops older connections and caches

            m_server = nullptr; // Reference to the server
            if (httpd_start(&m_server, &config) == ESP_OK) {

                // for (size_t i = 0; i < m_html_pages.size(); i++) {
                for (HTMLPage* page : m_html_pages) {
                    // registerPage(m_html_pages[i]);
                    registerPage(page);
                }

                ESP_LOGI("HTTP", "HTTP Server Started");
                m_server_init = true;
            }else {
                // Server failed to start
                ESP_LOGE("HTTP", "Failed to Start HTTP Server");
            }
        }
    };
    void HTTPServer::killServer() {
        if (m_server_init && m_server) {
            httpd_stop(m_server);
            m_server = nullptr;
            m_server_init = false;
        }
    };
    void HTTPServer::wifiEventHandler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
        HTTPServer* server = static_cast<HTTPServer*>(arg);
        if (event_base == WIFI_EVENT) {
            switch (event_id) {
                case WIFI_EVENT_STA_START: // Fires when esp_wifi_start() completes
                    ESP_LOGI("WIFI", "Connecting...");
                    esp_wifi_connect();
                    break;
                case WIFI_EVENT_STA_DISCONNECTED: // In case disconnection is temporary or just a glitch
                    ESP_LOGI("WIFI", "Disconnected! Reconnecting...");
                    esp_wifi_connect();
                    break;
            }
        }else if (event_base == IP_EVENT) {
            switch (event_id) {
                case IP_EVENT_STA_GOT_IP:
                    ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data; // Interpret the void* event_data
                    ESP_LOGI("WIFI", "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
                    server->startServer();
                    break;
            }
        }
    };
    esp_err_t HTTPServer::HTTPRequestHandler(httpd_req_t* request) {
        if (!request->user_ctx) {
            return ESP_FAIL;
        }
        // HTTPServer* server = static_cast<HTTPServer*>(request->user_ctx);
        HTMLPage* html_page = static_cast<HTMLPage*>(request->user_ctx);
        return html_page->handle_request(request);
    };

    void HTTPServer::registerPage(HTMLPage* page) {
        httpd_uri_t new_uri = {
            .uri = page->getPath().c_str(),
            .method = page->getMethod(),
            .handler = HTTPServer::HTTPRequestHandler, // Master handler
            // .user_ctx = this // void* arg, but referencing it is complicated
            .user_ctx = page // void* arg, but referencing it is complicated
        };
        esp_err_t error = httpd_register_uri_handler(m_server, &new_uri);
        if (error != ESP_OK) {
            ESP_LOGE("HTTP", "Failed to register URI: %s", page->getPath());
        }
    };

};

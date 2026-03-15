
#include "HTTPServer.h"


namespace atmt {

    HTTPServer::HTTPServer(std::string wifi_ssid, std::string wifi_password):
#ifdef AUTOMAT_ESP32_ESPIDF_
        m_server{ nullptr },
#endif
#ifdef AUTOMAT_ESP32_ARDUINO_
        m_server{ 80 },
#endif
        m_html_pages{ },
        m_wifi_ssid{ wifi_ssid },
        m_wifi_password{ wifi_password },
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

    void HTTPServer::init() {
        wifiInit();
    };
    void HTTPServer::periodic() {
#ifdef AUTOMAT_ESP32_ARDUINO_
        if (m_server_init) {
            m_server.handleClient();
            if (WiFi.status() != WL_CONNECTED && m_lastReconnectAttempt + kReconnectDelayMS < millis()) {
                WiFi.reconnect();
                m_lastReconnectAttempt = millis();
            }
        }
#endif
    };

    void HTTPServer::registerPage_Static_RawHTML(std::string url, std::string html) {
        HTMLPage* page = new HTMLPage_Static_RawHTML(url, html);
        registerPage(page);
    };
    void HTTPServer::registerPage_Static_DynamicHTML(std::string url, std::function<std::string()> html_getter) {
        HTMLPage* page = new HTMLPage_Static_DynamicHTML(url, html_getter);
        registerPage(page);
    };
    void HTTPServer::registerPage_Static_DynamicPost(std::string url, std::function<void(std::vector<POSTInfo>)> post_sender) {
        HTMLPage* page = new HTMLPage_Static_DynamicPost(url, post_sender);
        registerPage(page);
    };
    void HTTPServer::registerPage(HTMLPage* page) {
        m_html_pages.push_back(page);
        if (m_server_init) {
            addPageToServer(page);
        }
    };

    void HTTPServer::wifiInit() {
        if (!m_wifi_init) {
#ifdef AUTOMAT_ESP32_ESPIDF_
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
            strncpy((char*)wifi_config.sta.ssid, m_wifi_ssid.c_str(), sizeof(wifi_config.sta.ssid) - 1); // Grab the wifi network info so it knows what and how to connect
            strncpy((char*)wifi_config.sta.password, m_wifi_password.c_str(), sizeof(wifi_config.sta.password) - 1); // -1 is to ensure there is a character to null terminate the string
            wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK; // Prevents connecting to open networks (may remove temporarily)
            // wifi_config.sta.threshold.authmode = WIFI_AUTH_OPEN;
            wifi_config.sta.pmf_cfg.capable = true; // Tells the router "I am modern and flexible"
            wifi_config.sta.pmf_cfg.required = false;

            esp_wifi_set_mode(WIFI_MODE_STA); // STA - client, AP - router
            esp_wifi_set_config(WIFI_IF_STA, &wifi_config); // Actually loads config
            esp_wifi_start(); // Finally actually starts the wifi

            esp_wifi_set_ps(WIFI_PS_NONE); // Turns off power saving measures because we are streaming
#endif
#ifdef AUTOMAT_ESP32_ARDUINO_
            WiFi.mode(WIFI_STA);
            WiFi.onEvent(
                [this](WiFiEvent_t event, WiFiEventInfo_t info) {
                    wifiEventHandler(event, info, this);
                }
            );
            WiFi.begin(m_wifi_ssid.c_str(), m_wifi_password.c_str());
            // while (WiFi.status() != WL_CONNECTED) {
            //     delay(500);
            // }
            // // Serial.println(WiFi.localIP());
            // startServer();
#endif
            m_wifi_init = true;
        }
    };
    void HTTPServer::startServer() {
        if (!m_server_init) {
#ifdef AUTOMAT_ESP32_ESPIDF_
            httpd_config_t config = HTTPD_DEFAULT_CONFIG(); // Start with default, then adjust specific

            config.server_port = 80; // Means that client does not need to specify a port 
            config.ctrl_port = 32768; // Private internal port
            config.max_uri_handlers = 8; // Number of unique requests to endpoints (/, /stream, different users)
            config.stack_size = 16384; // Memory allocated to each incoming request
            config.lru_purge_enable = true; // Drops older connections and caches

            m_server = nullptr; // Reference to the server
            if (httpd_start(&m_server, &config) == ESP_OK) {

                addAllPages();

                ESP_LOGI("HTTP", "HTTP Server Started");
                m_server_init = true;
            }else {
                // Server failed to start
                ESP_LOGE("HTTP", "Failed to Start HTTP Server");
            }
#endif
#ifdef AUTOMAT_ESP32_ARDUINO_
            addAllPages();

            m_server.begin();
            m_server_init = true;
#endif
        }
    };
    void HTTPServer::killServer() {
#ifdef AUTOMAT_ESP32_ESPIDF_
        if (m_server_init && m_server) {
            httpd_stop(m_server);
            m_server = nullptr;
            m_server_init = false;
        }
#endif
#ifdef AUTOMAT_ESP32_ARDUINO_
        if (m_server_init) {
            m_server.stop();
            m_server_init = false;
        }
#endif
    };
#ifdef AUTOMAT_ESP32_ESPIDF_
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
#endif
#ifdef AUTOMAT_ESP32_ARDUINO_
    void HTTPServer::wifiEventHandler(WiFiEvent_t event, WiFiEventInfo_t info, HTTPServer* server) {
        if (event == ARDUINO_EVENT_WIFI_STA_GOT_IP) {
            server->startServer();
        }
    };
    void HTTPServer::HTTPRequestHandler(HTMLPage* page) {
        page->handle_request(&m_server);
    };
#endif

    void HTTPServer::addAllPages() {
        // for (size_t i = 0; i < m_html_pages.size(); i++) {
        for (HTMLPage* page : m_html_pages) {
            // registerPage(m_html_pages[i]);
            addPageToServer(page);
        }
    };
    void HTTPServer::addPageToServer(HTMLPage* page) {
#ifdef AUTOMAT_ESP32_ESPIDF_
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
#endif
#ifdef AUTOMAT_ESP32_ARDUINO_
        m_server.on(
            page->getPath().c_str(),
            page->getMethod(),
            [this, page]() {
                HTTPRequestHandler(page);
            }
        );
#endif
    };

};

#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_HTTP_SERVER_

#include "HTTPServer.h"


namespace atmt {

    std::string getDisconnectReason(uint8_t reason) {
        switch (reason) {
            case 1:   return "UNSPECIFIED";               // Unspecified reason
            case 2:   return "AUTH_EXPIRE";               // Authentication expired
            case 3:   return "AUTH_LEAVE";                // Leaving (deauth)
            case 4:   return "ASSOC_EXPIRE";              // Association expired
            case 5:   return "ASSOC_TOOMANY";             // Too many associations
            case 6:   return "NOT_AUTHED";                // Not authenticated
            case 7:   return "NOT_ASSOCED";               // Not associated
            case 8:   return "ASSOC_LEAVE";               // Left association
            case 9:   return "ASSOC_NOT_AUTHED";          // Assoc without auth
            case 10:  return "DISASSOC_PWRCAP_BAD";       // Power capability mismatch
            case 11:  return "DISASSOC_SUPCHAN_BAD";      // Unsupported channel
            case 13:  return "IE_INVALID";                // Invalid information element
            case 14:  return "MIC_FAILURE";               // MIC failure
            case 15:  return "4WAY_HANDSHAKE_TIMEOUT";    // WPA handshake timeout
            case 16:  return "GROUP_KEY_UPDATE_TIMEOUT";  // Group key update timeout
            case 17:  return "IE_IN_4WAY_DIFFERS";        // IE differs in handshake
            case 18:  return "GROUP_CIPHER_INVALID";      // Invalid group cipher
            case 19:  return "PAIRWISE_CIPHER_INVALID";   // Invalid pairwise cipher
            case 20:  return "AKMP_INVALID";              // Invalid AKMP
            case 21:  return "UNSUPP_RSN_IE_VERSION";     // Unsupported RSN IE version
            case 22:  return "INVALID_RSN_IE_CAP";        // Invalid RSN IE capabilities
            case 23:  return "802_1X_AUTH_FAILED";        // 802.1X authentication failed
            case 24:  return "CIPHER_SUITE_REJECTED";     // Cipher suite rejected
            case 200: return "BEACON_TIMEOUT";            // Beacon not received
            case 201: return "NO_AP_FOUND";               // Cannot find AP
            case 202: return "AUTH_FAIL";                 // Auth failed (wrong password?)
            case 203: return "ASSOC_FAIL";                // Association failed
            case 204: return "HANDSHAKE_TIMEOUT";         // WPA handshake timeout (legacy)
            case 205: return "HANDSHAKE_TIMEOUT_4WAY";    // 4-way handshake timeout
            default:  return "UNKNOWN_REASON: " + std::to_string(reason);
        }
    }

    HTTPServer::HTTPServer(std::string wifi_ssid, std::string wifi_password):
#ifdef ATMT_SUBMODULE_SERVER_ESP32_HTTPD_
        m_server{ nullptr },
#endif
#ifdef ATMT_SUBMODULE_SERVER_ARDUINO_WIFI_
        m_server{ 80 },
#endif
#ifdef ATMT_SUBMODULE_SERVER_ARDUINO_ASYNC_WIFI_
        m_server{ 80 },
#endif
        m_html_pages{ },
#ifndef ATMT_SUBMODULE_SERVER_ARDUINO_ASYNC_WIFI_
        m_ongoing_connections{ },
#endif
        m_wifi_ssid{ wifi_ssid },
        m_wifi_password{ wifi_password },
        m_ip_address{ "" },
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
#ifndef ATMT_SUBMODULE_SERVER_ARDUINO_ASYNC_WIFI_
        for (OngoingConnection* connection : m_ongoing_connections) {
            delete connection;
        }
        m_ongoing_connections.clear();
#endif
    };

    void HTTPServer::init() {
        wifiInit();
    };
    void HTTPServer::periodic() {
        // platform_println("Periodic Runs!");
#ifdef ATMT_SUBMODULE_SERVER_ARDUINO_WIFI_
        if (m_server_init) {
            m_server.handleClient();
        }
#endif
#if defined(ATMT_SUBMODULE_SERVER_ARDUINO_WIFI_) || defined(ATMT_SUBMODULE_SERVER_ARDUINO_ASYNC_WIFI_)
        // if (m_wifi_init) {
            // platform_println("Well, WiFi at least");
        // }
        // if (m_server_init) {
            // platform_println("Init Happened!>>>>>>>>>>>>>>>>>");
        if (WiFi.status() != WL_CONNECTED && m_lastReconnectAttempt + kReconnectDelayMS < millis()) {
            WiFi.reconnect();
            m_lastReconnectAttempt = millis();
        }
        // }
#endif

#ifdef ATMT_SUBMODULE_SERVER_ESP32_HTTPD_
        size_t i = 0;
        size_t connections = m_ongoing_connections.size();
        Timestamp time = getSystemTime();
        while (i < connections) {
            if (time > m_ongoing_connections[i]->scheduled_at) {
                // platform_println(std::to_string(time.getTimeMS()));
                // platform_println(std::to_string(m_ongoing_connections[i]->scheduled_at.getTimeMS()));
                // m_ongoing_connections[i]->page->continue_connection(m_ongoing_connections[i]->socket);
                m_ongoing_connections[i]->page->continue_connection(m_ongoing_connections[i]->request);
                delete m_ongoing_connections[i];
                m_ongoing_connections.erase(m_ongoing_connections.begin() + i);
                connections -= 1;
                // platform_println("Serviced");
            }else {
                i += 1;
                // platform_println("Skipped for a frame");
            }
        }
#endif
    };

#ifdef ATMT_SUBMODULE_COMMAND_BASED_
    void HTTPServer::systemPeriodic() {
        periodic();
    };
    void HTTPServer::disabledPeriodic() {};
    void HTTPServer::autonomousPeriodic() {};
    void HTTPServer::teleopPeriodic() {};
#endif

    void HTTPServer::registerPage_IdentifyAsAutomat() {
        registerPage_IdentifyAsAutomat("");
    };
    void HTTPServer::registerPage_IdentifyAsAutomat(const std::string& device_name) {
        HTMLPage* page = new HTMLPage_Static_ReturnCode("/is_atmt", device_name, 418); // 418 I'm a teapot
        registerPage(page);
    };
    void HTTPServer::registerPage_AutomatFavicon() {
        HTMLPage* page = new HTMLPage_Static_Favicon();
        registerPage(page);
    };
    void HTTPServer::registerPage_Static_RawHTML(const std::string& path, const std::string& html) {
        HTMLPage* page = new HTMLPage_Static_RawHTML(path, html);
        registerPage(page);
    };
    void HTTPServer::registerPage_Static_DynamicHTML(const std::string& path, std::function<std::string(void*)> html_getter, void* arg) {
        HTMLPage* page = new HTMLPage_Static_DynamicHTML(path, html_getter, arg);
        registerPage(page);
    };
    void HTTPServer::registerPage_Static_DynamicPostHTML(const std::string& path, std::function<std::string(const std::vector<POSTInfo>&, void*)> post_sender, void* arg) {
        HTMLPage* page = new HTMLPage_Static_DynamicPostHTML(path, post_sender, arg);
        registerPage(page);
    };
    void HTTPServer::registerPage_Static_DynamicPostRedirect(const std::string& path, const std::string& redirect_path, std::function<void(const std::vector<POSTInfo>&, void*)> post_sender, void* arg) {
        HTMLPage* page = new HTMLPage_Static_DynamicPostRedirect(path, redirect_path, post_sender, arg);
        registerPage(page);
    };
#ifdef ATMT_SUBMODULE_HTTP_SERVER_JPEG_STREAMING_
    void HTTPServer::registerPage_Dynamic_JPEGStreamer(const std::string& path, std::function<char*(size_t&, void*)> jpeg_getter, double frame_rate, void* arg) {
        HTMLPage* page = new HTMLPage_Dynamic_JPEGStreamer(path, jpeg_getter, frame_rate, arg);
        registerPage(page);
    };
#endif
    void HTTPServer::registerPage_Static_Favicon(char* favicon, size_t favicon_length) {
        HTMLPage* page = new HTMLPage_Static_Favicon(favicon, favicon_length);
        registerPage(page);
    };
    void HTTPServer::registerPage(HTMLPage* page) {
        m_html_pages.push_back(page);
        if (m_server_init) {
            addPageToServer(page);
        }
    };

    void HTTPServer::wifiInit() {
        platform_println("Init begun");
        if (!m_wifi_init) {
#ifdef ATMT_SUBMODULE_SERVER_ESP32_HTTPD_
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
#if defined(ATMT_SUBMODULE_SERVER_ARDUINO_WIFI_) || defined(ATMT_SUBMODULE_SERVER_ARDUINO_ASYNC_WIFI_)
            platform_println("Starting WiFi stuff");
            WiFi.mode(WIFI_STA);
            platform_println("That mode thing is done");
            WiFi.onEvent(
                [this](WiFiEvent_t event, WiFiEventInfo_t info) {
                    platform_println("Eventer Schedulered Runned");
                    wifiEventHandler(event, info, this);
                }
            );
            platform_println("Eventer Schedulered");
            WiFi.begin(m_wifi_ssid.c_str(), m_wifi_password.c_str());
            platform_println("iFi egun");
            // while (WiFi.status() != WL_CONNECTED) {
            //     delay(500);
            // }
            // // platform_println(WiFi.localIP());
            // startServer();
#endif
            m_wifi_init = true;
        }
    };
    void HTTPServer::startServer() {
        if (!m_server_init) {
#ifdef ATMT_SUBMODULE_SERVER_ESP32_HTTPD_
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
#if defined(ATMT_SUBMODULE_SERVER_ARDUINO_WIFI_) || defined(ATMT_SUBMODULE_SERVER_ARDUINO_ASYNC_WIFI_)
            addAllPages();

            m_server.begin();
            m_server_init = true;
#endif
        }
    };
    void HTTPServer::killServer() {
#ifdef ATMT_SUBMODULE_SERVER_ESP32_HTTPD_
        if (m_server_init && m_server) {
            httpd_stop(m_server);
            m_server = nullptr;
            m_server_init = false;
        }
#endif
#if defined(ATMT_SUBMODULE_SERVER_ARDUINO_WIFI_) || defined(ATMT_SUBMODULE_SERVER_ARDUINO_ASYNC_WIFI_)
        if (m_server_init) {
#ifdef ATMT_SUBMODULE_SERVER_ARDUINO_WIFI_
            m_server.stop();
#endif
            m_server_init = false;
        }
#endif
    };
#ifdef ATMT_SUBMODULE_SERVER_ESP32_HTTPD_
    void HTTPServer::wifiEventHandler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
        HTTPServer* server = static_cast<HTTPServer*>(arg);
        if (event_base == WIFI_EVENT) {
            switch (event_id) {
                case WIFI_EVENT_STA_START: // Fires when esp_wifi_start() completes
                    // ESP_LOGI("WIFI", "Connecting...");
                    esp_wifi_connect();
                    break;
                case WIFI_EVENT_STA_DISCONNECTED: // In case disconnection is temporary or just a glitch
                    // ESP_LOGI("WIFI", "Disconnected! Reconnecting...");
                    esp_wifi_connect();
                    break;
            }
        }else if (event_base == IP_EVENT) {
            switch (event_id) {
                case IP_EVENT_STA_GOT_IP:
                    ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data; // Interpret the void* event_data
                    // ESP_LOGI("WIFI", "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
                    const esp_ip4_addr_t& ip = event->ip_info.ip;
                    server->m_ip_address = 
                        std::to_string(ip.addr & 0xFF) + "." +
                        std::to_string((ip.addr >> 8) & 0xFF) + "." +
                        std::to_string((ip.addr >> 16) & 0xFF) + "." +
                        std::to_string((ip.addr >> 24) & 0xFF);
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
        // return html_page->handle_request(new HTTPRequest(request, this));
        return html_page->handle_request(new HTTPRequest(request));
    };
#endif
#if defined(ATMT_SUBMODULE_SERVER_ARDUINO_WIFI_) || defined(ATMT_SUBMODULE_SERVER_ARDUINO_ASYNC_WIFI_)
    void HTTPServer::wifiEventHandler(WiFiEvent_t event, WiFiEventInfo_t info, HTTPServer* server) {
        platform_println("Event: "+std::to_string(event));
        platform_println("Free heap: " + std::to_string(ESP.getFreeHeap()));
        if (event == ARDUINO_EVENT_WIFI_STA_GOT_IP) {
            server->m_ip_address = std::string(WiFi.localIP().toString().c_str());
            platform_println("Connected! IP: "+server->m_ip_address);
            server->startServer();
        }else if (event == ARDUINO_EVENT_WIFI_STA_DISCONNECTED) {
            platform_println("Disconnected: "+getDisconnectReason(info.wifi_sta_disconnected.reason)+". Retrying...");
            // WiFi.reconnect();
            server->m_lastReconnectAttempt = millis();
        }
    };
#endif
#ifdef ATMT_SUBMODULE_SERVER_ARDUINO_WIFI_
    void HTTPServer::HTTPRequestHandler(HTMLPage* page) {
        page->handle_request(new HTTPRequest(&m_server, this));
    };
#endif
#ifdef ATMT_SUBMODULE_SERVER_ARDUINO_ASYNC_WIFI_
    void HTTPServer::HTTPRequestHandler(HTMLPage* page, AsyncWebServerRequest* request) {
        page->handle_request(new HTTPRequest(request, this));
    };
#endif

    std::string HTTPServer::getIPAddress() {
        return m_ip_address != "" ? m_ip_address : "Server Not Connected";
    };

    void HTTPServer::addAllPages() {
        // for (size_t i = 0; i < m_html_pages.size(); i++) {
        for (HTMLPage* page : m_html_pages) {
            // registerPage(m_html_pages[i]);
            addPageToServer(page);
        }
    };
    void HTTPServer::addPageToServer(HTMLPage* page) {
#ifdef ATMT_SUBMODULE_SERVER_ESP32_HTTPD_
        page->setServer(this);
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
#ifdef ATMT_SUBMODULE_SERVER_ARDUINO_WIFI_
        m_server.on(
            page->getPath().c_str(),
            page->getMethod(),
            [this, page]() {
                HTTPRequestHandler(page);
            }
        );
#endif
#ifdef ATMT_SUBMODULE_SERVER_ARDUINO_ASYNC_WIFI_
        // Register a route for asynchronous server
        m_server.on(
            page->getPath().c_str(),
            page->getMethod(),
            [this, page](AsyncWebServerRequest* request) {
                HTTPRequestHandler(page, request);
            }
        );
#endif
    };
    
#ifndef ATMT_SUBMODULE_SERVER_ARDUINO_ASYNC_WIFI_
    // void HTTPServer::scheduleOngoingConnection(HTMLPage* page, HTTPSocket* socket, Timestamp scheduled_at) {
    void HTTPServer::scheduleOngoingConnection(HTMLPage* page, HTTPRequest* request, Timestamp scheduled_at) {
        OngoingConnection* connection = new OngoingConnection();
        connection->page = page;
        connection->request = request;
        connection->scheduled_at = scheduled_at;
        m_ongoing_connections.push_back(connection);
    };
#endif
#ifdef ATMT_SUBMODULE_SERVER_ESP32_HTTPD_
    httpd_handle_t* HTTPServer::getHTTPDHandle() {
        return &m_server;
    };
#endif

};

#else
// #error "Enable ATMT_SUBMODULE_HTTP_SERVER_ in automat_submodules.h to use HTTPServer"
#endif
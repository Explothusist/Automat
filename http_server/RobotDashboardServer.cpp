#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_HTTP_SERVER_ROBOT_DASHBOARD_SERVER_

#include "RobotDashboardServer.h"
#include "dashboard_html.h"

namespace atmt {

    RobotDashboardServer::RobotDashboardServer(const std::string& device_name, const std::string& wifi_ssid, const std::string& wifi_pass):
        m_device_name{ device_name },
        m_server{ new HTTPServer(wifi_ssid, wifi_pass) }
    {
        
    };
    RobotDashboardServer::~RobotDashboardServer() {

    };

    void RobotDashboardServer::init() {
        m_server->registerPage_IdentifyAsAutomat(m_device_name);
        m_server->registerPage_AutomatFavicon();
        m_server->registerPage(new HTMLPage_Static_DynamicChunkedHTML(
            "/",
            getNextDashboardHTMLChunk,
            this
        ));

        m_server->init();
    };
    void RobotDashboardServer::periodic() {
        m_server->periodic();
    };

    std::string RobotDashboardServer::getIPAddress() {
        return m_server->getIPAddress();
    };
    const char* RobotDashboardServer::getNextDashboardHTMLChunk(int chunk_index, size_t& chunk_length, void* arg) {
        RobotDashboardServer* dbd = static_cast<RobotDashboardServer*>(arg);

        if (chunk_index == 0) {
            dbd->m_header_sent = false;
            dbd->m_footer_sent = false;
            dbd->m_row = 0;
            dbd->m_row_header_sent = false;
            dbd->m_row_footer_sent = false;
            dbd->m_box = 0;
            dbd->m_box_header_sent = false;
            dbd->m_box_title_sent = false;
            dbd->m_box_separator_sent = false;
            dbd->m_box_content_sent = false;
            dbd->m_box_footer_sent = false;
        }

        // std::string website = "";
        if (!dbd->m_header_sent) {
            dbd->m_header_sent = true;
            chunk_length = strlen(kDBD_HTML_HEADER);
            return kDBD_HTML_HEADER;
        }
        if (dbd->m_row < 4) {
            if (!dbd->m_row_header_sent) {
                dbd->m_row_header_sent = true;
                chunk_length = strlen(kDBD_HTML_STARTROW);
                return kDBD_HTML_STARTROW;
            }
            if (dbd->m_box < 4) {
                if (!dbd->m_box_header_sent) {
                    dbd->m_box_header_sent = true;
                    chunk_length = strlen(kDBD_HTML_STARTBOX);
                    return kDBD_HTML_STARTBOX;
                }
                if (!dbd->m_box_title_sent) {
                    dbd->m_box_title_sent = true;
                    // Read from Tile
                }
                if (!dbd->m_box_separator_sent) {
                    dbd->m_box_separator_sent = true;
                    chunk_length = strlen(kDBD_HTML_CONTENTSEPARATOR);
                    return kDBD_HTML_CONTENTSEPARATOR;
                }
                if (!dbd->m_box_content_sent) {
                    dbd->m_box_content_sent = true;
                    // Read from Tile
                }
                if (!dbd->m_box_footer_sent) {
                    dbd->m_box += 1;
                    dbd->m_box_header_sent = false;
                    dbd->m_box_title_sent = false;
                    dbd->m_box_separator_sent = false;
                    dbd->m_box_content_sent = false;
                    dbd->m_box_footer_sent = false;
                    chunk_length = strlen(kDBD_HTML_ENDBOX);
                    return kDBD_HTML_ENDBOX;
                }
            }
            if (!dbd->m_row_footer_sent) {
                dbd->m_box = 0;
                dbd->m_row += 1;
                dbd->m_row_header_sent = false;
                dbd->m_row_footer_sent = false;
                chunk_length = strlen(kDBD_HTML_ENDROW);
                return kDBD_HTML_ENDROW;
            }
        }
        if (!dbd->m_footer_sent) {
            dbd->m_footer_sent = true;
            chunk_length = strlen(kDBD_HTML_FOOTER);
            return kDBD_HTML_FOOTER;
        }
        chunk_length = 0;
        return nullptr;
    };

};

#else
// #error "Enable ATMT_SUBMODULE_HTTP_SERVER_CAMERA_STREAMING_SERVER_ in automat_submodules.h to use RobotDashboardServer"
#endif
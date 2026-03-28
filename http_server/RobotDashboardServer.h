#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_HTTP_SERVER_ROBOT_DASHBOARD_SERVER_

#ifndef AUTOMAT_ROBOT_DASHBOARD_SERVER_
#define AUTOMAT_ROBOT_DASHBOARD_SERVER_

#include <string>

#include "HTMLPage.h"
#include "HTTPServer.h"
#include "dashboard_tiles/DashboardTile.h"

#ifdef ATMT_SUBMODULE_COMMAND_BASED_
#include "../command_based/Subsystem.h"
#endif

namespace atmt {

#ifdef ATMT_SUBMODULE_COMMAND_BASED_
    class RobotDashboardServer : public Subsystem {
#else
    class RobotDashboardServer {
#endif
        public:
            RobotDashboardServer(const std::string& device_name, const std::string& wifi_ssid, const std::string& wifi_pass);
#ifdef ATMT_SUBMODULE_COMMAND_BASED_
            ~RobotDashboardServer() override;

            void init() override;
            void periodic() override;

            // void internal_init(RobotState* robot_state, EventHandler* event_handler);
#else
            ~RobotDashboardServer();

            void init();
            void periodic();
#endif

            std::string getIPAddress();
            static const char* getNextDashboardHTMLChunk(int chunk_index, size_t& chunk_length, void* arg);

        private:
            std::string m_device_name;
            HTTPServer* m_server;

            bool m_header_sent;
            bool m_footer_sent;
            size_t m_row;
            bool m_row_header_sent;
            bool m_row_footer_sent;
            size_t m_box;
            bool m_box_header_sent;
            bool m_box_title_sent;
            bool m_box_separator_sent;
            bool m_box_content_sent;
            bool m_box_footer_sent;
    };

}

#endif

#else
#error "Enable ATMT_SUBMODULE_HTTP_SERVER_ROBOT_DASHBOARD_SERVER_ in automat_submodules.h to use RobotDashboardServer"
#endif
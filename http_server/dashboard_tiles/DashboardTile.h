#include "../../automat_submodules.h"
#ifdef ATMT_SUBMODULE_HTTP_SERVER_ROBOT_DASHBOARD_SERVER_

#ifndef AUTOMAT_ROBOT_DASHBOARD_SERVER_TILE_
#define AUTOMAT_ROBOT_DASHBOARD_SERVER_TILE_

#include <string>

namespace atmt {

    class DashboardTile {
        public:
            DashboardTile();
            ~DashboardTile();

            std::string getTileHTML();

        private:
    };

}

#endif

#else
#error "Enable ATMT_SUBMODULE_HTTP_SERVER_ROBOT_DASHBOARD_SERVER_ in automat_submodules.h to use RobotDashboardServer"
#endif
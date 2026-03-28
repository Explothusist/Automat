#include "../../automat_submodules.h"
#ifdef ATMT_SUBMODULE_HTTP_SERVER_ROBOT_DASHBOARD_SERVER_

#include "DashboardTile.h"

namespace atmt {

    DashboardTile::DashboardTile()//:
    {
        
    };
    DashboardTile::~DashboardTile() {

    };

    std::string getTileHTML() {
        return "";
    };

}

#else
// #error "Enable ATMT_SUBMODULE_HTTP_SERVER_CAMERA_STREAMING_SERVER_ in automat_submodules.h to use RobotDashboardServer"
#endif
#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_HTTP_SERVER_ROBOT_DASHBOARD_SERVER_

#ifndef AUTOMAT_ROBOT_DASHBOARD_SERVER_HTML_
#define AUTOMAT_ROBOT_DASHBOARD_SERVER_HTML_

// TODO: Minify this
const char kDBD_HTML_HEADER[] = R"rawliteral(
<!DOCTYPE html>
<html>
    <head>
        <title>Automat Dashboard</title>

        <style lang="CSS">
            :root {
                --gap: 10px;
            }
            * {
                box-sizing: border-box;
            }

            html, body {
                height: 100%;
                margin: 0px;
            }
            flex-cont {
                display: flex;
                flex-direction: column;

                height: 100%;
                
                justify-content: space-around;
                gap: var(--gap);
                padding: var(--gap);
            }
            flex-row {
                display: flex;
                flex-direction: row;

                justify-content: space-around;
                flex: 1;
                gap: var(--gap);
            }
            flex-box {
                display: flex;
                flex: 1;
            }
            display-box {
                display: flex;
                flex-direction: column;
                border-radius: 5px;
                flex: 1;
            }
            /* display-box:hover {
                background-color: #8b2d2d;
            } */
            title-row {
                display: flex;
                flex: 1;
                background-color: brown;
                color: white;
                align-items: center;
                justify-content: center;
                font-size: large;
            }
            content-box {
                display: flex;
                flex: 4;
                background-color: grey;
                color: black;
                align-items: center;
                justify-content: center;
                font-size: large;
            }
        </style>
    </head>
    <body>
        <flex-cont>
)rawliteral";
const char kDBD_HTML_FOOTER[] = R"rawliteral(
        </flex-cont>
    </body>
</html>
)rawliteral";

const char kDBD_HTML_STARTROW[] = R"rawliteral(
            <flex-row>
)rawliteral";
const char kDBD_HTML_ENDROW[] = R"rawliteral(
            </flex-row>
)rawliteral";
const char kDBD_HTML_STARTBOX[] = R"rawliteral(
                <flex-box>
                    <display-box>
                        <title-row>
)rawliteral";
const char kDBD_HTML_CONTENTSEPARATOR[] = R"rawliteral(
                        </title-row>
                        <content-box>
)rawliteral";
const char kDBD_HTML_ENDBOX[] = R"rawliteral(
                        </content-box>
                    </display-box>
                </flex-box>
)rawliteral";

#endif

#else
#error "Enable ATMT_SUBMODULE_HTTP_SERVER_ROBOT_DASHBOARD_SERVER_ in automat_submodules.h to use RobotDashboardServer"
#endif
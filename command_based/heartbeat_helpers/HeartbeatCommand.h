
#ifndef STORMBOT_HEARTBEATCOMMAND_
#define STORMBOT_HEARTBEATCOMMAND_

#include "../Command.h"
// #include "Heartbeat.h"

namespace atmt {

    class Heartbeat;

    class HeartbeatCommand : public atmt::Command {
        public:
            HeartbeatCommand(Heartbeat* heartbeat); // Put subsystems as parameters
            HeartbeatCommand(HeartbeatCommand& command); // Copy constructor
            ~HeartbeatCommand();
            atmt::Command* cloneSelf() const override;

            void initialize() override; // User-made
            void execute() override; // User-made
            void end(bool interrupted) override; // User-made
            bool is_finished() override; // User-made
            
        private:
            Heartbeat* m_heartbeat;
    };

};

#endif
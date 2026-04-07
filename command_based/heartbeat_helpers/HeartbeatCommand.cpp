
#include "HeartbeatCommand.h"
#include "Heartbeat.h"

#include <cmath>

namespace atmt {

    HeartbeatCommand::HeartbeatCommand(Heartbeat* heartbeat):
        atmt::Command(),
        m_heartbeat{ heartbeat }
    {
        // Does not use CameraReader* at the moment (somewhat risky)
        // usesSubsystem(m_heartbeat);
    };
    HeartbeatCommand::HeartbeatCommand(HeartbeatCommand& command):
        atmt::Command(command)
    {
        m_heartbeat = command.m_heartbeat;
    };
    HeartbeatCommand::~HeartbeatCommand() {
        // Will run ~Command() after this is complete
    };
    atmt::Command* HeartbeatCommand::cloneSelf() const {
        return new HeartbeatCommand(m_heartbeat);
    };

    void HeartbeatCommand::initialize() {
        m_heartbeat->setLastSerialId(getSerialMessageId());
        m_heartbeat->beatHeart();
    };
    void HeartbeatCommand::execute() {
        
    };
    void HeartbeatCommand::end(bool interrupted) {
        
    };
    bool HeartbeatCommand::is_finished() {
        return true;
    };

};
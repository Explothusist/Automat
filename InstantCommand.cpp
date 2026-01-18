
#include "InstantCommand.h"

namespace atmt {

    InstantCommand::InstantCommand(std::function<void()> lambda):
        Command(),
        m_lambda{ lambda },
        m_continuous{ false }
    {

    };
    InstantCommand::InstantCommand(std::function<void()> lambda, bool continuous):
        Command(),
        m_lambda{ lambda },
        m_continuous{ continuous }
    {

    };
    InstantCommand::InstantCommand(InstantCommand& command):
        Command(command)
    {
        m_lambda = command.m_lambda;
    };
    InstantCommand::~InstantCommand() {
        // Will run ~Command() after this is complete
    };
    Command* InstantCommand::clone() const {
        return new InstantCommand(m_lambda);
    };

    void InstantCommand::initialize() {

    };
    void InstantCommand::execute() {
        m_lambda();
    };
    void InstantCommand::end(bool interrupted) {

    };
    bool InstantCommand::is_finished() {
        return !m_continuous;
    };

};
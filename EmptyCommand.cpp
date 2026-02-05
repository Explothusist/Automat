
#include "EmptyCommand.h"

namespace atmt {

    EmptyCommand::EmptyCommand():
        Command()
    {

    };
    EmptyCommand::EmptyCommand(EmptyCommand& command):
        Command(command)
    {
        
    };
    EmptyCommand::~EmptyCommand() {
        // Will run ~Command() after this is complete
    };
    Command* EmptyCommand::clone() const {
        return new EmptyCommand();
    };

    void EmptyCommand::initialize() {

    };
    void EmptyCommand::execute() {
        
    };
    void EmptyCommand::end(bool interrupted) {

    };
    bool EmptyCommand::is_finished() {
        return true;
    };

};
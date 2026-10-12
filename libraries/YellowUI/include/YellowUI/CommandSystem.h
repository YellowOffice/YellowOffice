#ifndef YELLOW_UI_COMMAND_SYSTEM_H
#define YELLOW_UI_COMMAND_SYSTEM_H

#include <SupportDefs.h>
#include <Message.h>

namespace YellowUI {

class CommandSystem {
public:
    CommandSystem();
    ~CommandSystem();

    // Command execution
    void ExecuteCommand(uint32 command);
    void ExecuteCommand(BMessage* message);
    
    // Command state
    bool CanExecuteCommand(uint32 command) const;
    void UpdateCommandState(uint32 command, bool enabled);

private:
    // Command handlers would be registered here
};

} // namespace YellowUI

#endif // YELLOW_UI_COMMAND_SYSTEM_H

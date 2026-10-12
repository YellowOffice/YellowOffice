#ifndef YELLOW_UI_ACTION_MANAGER_H
#define YELLOW_UI_ACTION_MANAGER_H

#include <SupportDefs.h>

namespace YellowUI {

class ActionManager {
public:
    ActionManager();
    ~ActionManager();

    // Action management
    void RegisterAction(uint32 command, const char* name, const char* description = nullptr);
    void UnregisterAction(uint32 command);
    
    // Action execution
    void ExecuteAction(uint32 command);
    bool CanExecuteAction(uint32 command) const;
    
    // Enable/disable actions
    void SetActionEnabled(uint32 command, bool enabled);
    bool IsActionEnabled(uint32 command) const;

private:
    struct ActionInfo {
        uint32 command;
        BString name;
        BString description;
        bool enabled;
    };
    
    std::vector<ActionInfo*> fActions;
};

} // namespace YellowUI

#endif // YELLOW_UI_ACTION_MANAGER_H

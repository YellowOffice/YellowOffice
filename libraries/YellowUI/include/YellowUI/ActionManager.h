#ifndef YELLOW_UI_ACTION_MANAGER_H
#define YELLOW_UI_ACTION_MANAGER_H

#include <SupportDefs.h>
#include <String.h>
#include <vector>

namespace YellowUI {

/**
 * ActionListener - implemented by windows/views that react to actions.
 */
class ActionListener {
public:
    virtual ~ActionListener();
    virtual void ActionPerformed(uint32 command) = 0;
};

/**
 * ActionManager - central registry of named actions with enable state and
 * a single global instance (ActionManager::Global()) shared by windows.
 */
class ActionManager {
public:
    ActionManager();
    ~ActionManager();

    static ActionManager* Global();

    // Action management
    void RegisterAction(uint32 command, const char* name, const char* description = nullptr);
    void UnregisterAction(uint32 command);
    bool IsActionRegistered(uint32 command) const;

    // Action execution: notifies all listeners if the action is enabled.
    void ExecuteAction(uint32 command);
    bool CanExecuteAction(uint32 command) const;

    // Enable/disable actions
    void SetActionEnabled(uint32 command, bool enabled);
    bool IsActionEnabled(uint32 command) const;

    // Listeners
    void AddListener(ActionListener* listener);
    void RemoveListener(ActionListener* listener);

private:
    struct ActionInfo {
        uint32 command;
        BString name;
        BString description;
        bool enabled;
    };

    ActionInfo* _Find(uint32 command) const;

    std::vector<ActionInfo*> fActions;
    std::vector<ActionListener*> fListeners;
};

} // namespace YellowUI

#endif // YELLOW_UI_ACTION_MANAGER_H

#include <YellowUI/ActionManager.h>

namespace YellowUI {

ActionListener::~ActionListener()
{
}

ActionManager::ActionManager()
{
}

ActionManager::~ActionManager()
{
    for (ActionInfo* info : fActions) {
        delete info;
    }
    fActions.clear();
    fListeners.clear();
}

ActionManager* ActionManager::Global()
{
    static ActionManager manager;
    return &manager;
}

ActionManager::ActionInfo* ActionManager::_Find(uint32 command) const
{
    for (ActionInfo* info : fActions) {
        if (info && info->command == command) return info;
    }
    return nullptr;
}

void ActionManager::RegisterAction(uint32 command, const char* name,
                                   const char* description)
{
    ActionInfo* info = _Find(command);
    if (info) {
        info->name = name ? name : "";
        info->description = description ? description : "";
        return;
    }
    info = new ActionInfo();
    info->command = command;
    info->name = name ? name : "";
    info->description = description ? description : "";
    info->enabled = true;
    fActions.push_back(info);
}

void ActionManager::UnregisterAction(uint32 command)
{
    for (size_t i = 0; i < fActions.size(); ++i) {
        if (fActions[i] && fActions[i]->command == command) {
            delete fActions[i];
            fActions.erase(fActions.begin() + i);
            return;
        }
    }
}

bool ActionManager::IsActionRegistered(uint32 command) const
{
    return _Find(command) != nullptr;
}

void ActionManager::ExecuteAction(uint32 command)
{
    if (!CanExecuteAction(command)) return;
    for (ActionListener* listener : fListeners) {
        if (listener) listener->ActionPerformed(command);
    }
}

bool ActionManager::CanExecuteAction(uint32 command) const
{
    ActionInfo* info = _Find(command);
    return info != nullptr && info->enabled;
}

void ActionManager::SetActionEnabled(uint32 command, bool enabled)
{
    ActionInfo* info = _Find(command);
    if (info) info->enabled = enabled;
}

bool ActionManager::IsActionEnabled(uint32 command) const
{
    ActionInfo* info = _Find(command);
    return info ? info->enabled : false;
}

void ActionManager::AddListener(ActionListener* listener)
{
    if (!listener) return;
    for (ActionListener* existing : fListeners) {
        if (existing == listener) return;
    }
    fListeners.push_back(listener);
}

void ActionManager::RemoveListener(ActionListener* listener)
{
    for (size_t i = 0; i < fListeners.size(); ++i) {
        if (fListeners[i] == listener) {
            fListeners.erase(fListeners.begin() + i);
            return;
        }
    }
}

} // namespace YellowUI

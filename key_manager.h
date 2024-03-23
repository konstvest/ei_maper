#ifndef KEY_MANAGER_H
#define KEY_MANAGER_H

#include <QKeySequence>
#include <QSet>
#include "types.h"

///
/// \brief The CKeyManager class provides keystroke handling in the scene (change of operations, camera movement, etc.)
///
class CKeyManager
{
public:
    CKeyManager() {}
    ~CKeyManager() {}
    void press(EKeyCode key);
    void release(EKeyCode key);
    bool isPressed(EKeyCode key) {return m_aKeyPressed.contains(key);}
    QSet<EKeyCode>& keys() {return m_aKeyPressed;}

private:
    QSet<EKeyCode> m_aKeyPressed; //needs to use something more suitable without allocating memory.
};

#endif // KEY_MANAGER_H

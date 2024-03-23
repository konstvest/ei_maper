#include "key_manager.h"

void CKeyManager::press(EKeyCode key)
{
    if(!m_aKeyPressed.contains(key))
        m_aKeyPressed.insert(key);
}

void CKeyManager::release(EKeyCode key)
{
    if(m_aKeyPressed.contains(key))
        m_aKeyPressed.remove(key);
}


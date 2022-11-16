#include "key_manager.h"

void CKeyManager::press(Qt::Key key)
{
    if(!m_aKeyPressed.contains(key))
        m_aKeyPressed.insert(key);
}

void CKeyManager::release(Qt::Key key)
{
    if(m_aKeyPressed.contains(key))
        m_aKeyPressed.remove(key);
}


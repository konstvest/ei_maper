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

void CKeyManager::releaseAllButtons()
{
    QSet<Qt::Key> arrKey = m_aKeyPressed;
    Qt::Key key;
    foreach(key, arrKey)
    {
        release(key);
    }
    return;
}

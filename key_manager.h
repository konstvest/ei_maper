#ifndef KEY_MANAGER_H
#define KEY_MANAGER_H

#include <QKeySequence>
#include <QSet>

class CKeyManager
{
public:
    CKeyManager() {}
    ~CKeyManager() {}
    void press(Qt::Key key);
    void release(Qt::Key key);
    bool isPressed(Qt::Key key) {return m_aKeyPressed.contains(key);}
    QSet<Qt::Key>& keys() {return m_aKeyPressed;}

private:
    QSet<Qt::Key> m_aKeyPressed; //needs to use something more suitable without allocating memory.
};

#endif // KEY_MANAGER_H

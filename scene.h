#ifndef CSCENE_H
#define CSCENE_H

#include "types.h"

class CScene : public QObject
{
Q_OBJECT

public:
    static CScene* getInstance();
    CScene(CScene const&) = delete;
    void operator=(CScene const&)  = delete;
    ~CScene() override {} //for QObject

    const EEditMode& getMode() {return m_editMode;}
    void changeMode(EEditMode mode);

signals:
    void modeChanged();

private:
    CScene();


private:
    static CScene* m_pScene;
    EEditMode m_editMode;

};

#endif // CSCENE_H

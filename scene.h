#ifndef CSCENE_H
#define CSCENE_H

#include "types.h"

class CScene
{
public:
    static CScene* getInstance();
    CScene(CScene const&) = delete;
    void operator=(CScene const&)  = delete;

    const EEditMode& getMode() {return m_editMode;}
    void changeMode(EEditMode mode);

private:
    CScene();
    ~CScene();

private:
    static CScene* m_pScene;
    EEditMode m_editMode;

};

#endif // CSCENE_H

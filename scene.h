#ifndef CSCENE_H
#define CSCENE_H

#include "types.h"

///
/// \brief The CScene class stores the current scene state mode and updates the control hint
/// \todo change the name to something more appropriate
///
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
    void switchTileEditMode() {m_bLandEditMode = !m_bLandEditMode;};
    bool isLandTileEditMode() {return m_bLandEditMode;}

signals:
    void modeChanged();

private:
    CScene();


private:
    static CScene* m_pScene;
    EEditMode m_editMode;
    bool m_bLandEditMode;

};

#endif // CSCENE_H

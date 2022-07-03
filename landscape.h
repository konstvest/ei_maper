#ifndef LANDSCAPE_H
#define LANDSCAPE_H

#include <QString>
#include <QFileInfo>
#include <QOpenGLShaderProgram>
#include <QList>

#include "map_mp.h"
#include "sector.h"

class CView;
class CNode;

class CLandscape
{
public:
    CLandscape();
    ~CLandscape();
    void setParentView(CView* view) {m_parentView = view;}
    void readMap(QFileInfo& path);
    void draw(QOpenGLShaderProgram* program);
    void drawWater(QOpenGLShaderProgram* program);
    bool projectPt(QVector3D& point);
    bool projectPt(QVector<QVector3D>& aPoint);
    void projectPositions(QList<CNode*>& aNode);
    void projectPosition(CNode* pNode);
private:
    bool readHeader(QDataStream& stream);

private:
    SMapHeader m_header;
    QVector<SMaterial> m_aMaterial; //use pointers instead refs?
    QVector<int> m_aTileTypes; //array using tyle type on current map
    QVector<SAnimTile> m_aAnimTile;
    QVector<QVector<CSector*>> m_aSector;
    QOpenGLTexture* m_texture;
    CView* m_parentView;
};

#endif // LANDSCAPE_H

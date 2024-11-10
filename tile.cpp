#include "log.h"
#include "tile.h"
#include "math_utils.h"

CTile::CTile():
    m_materialIndex(-1)
{
    reset();
}

CTile::CTile(ushort packedData, ushort x, ushort y, float maxZ, int atlasNumber):
    m_x(x)
  ,m_y(y)
  ,m_maxZ(maxZ)
  ,m_texAtlasNumber(atlasNumber)
{
    reset();
    m_x = x;
    m_y = y;
    m_index = packedData & 63; //first 6 bits
    m_atlasTexIndex = (packedData >> 6) & 255; // second 8 bits
    m_rotNum = (packedData >> 14) & 3; // 2 bits more
}

CTile::~CTile()
{

}

// nine vertices of tile
/*
^|   6 _7 _8
c|   |\ |\ |
o|   |_\|_\|
l|   3 _4 _5
u|   |\ |\ |
m|   |_\|_\|
n|   0  1  2 ROW---->
*/
void CTile::resetVertices(QVector<SSecVertex>& arrVertex)
{
    if(arrVertex.size() != 9)
    {
        ei::log(eLogWarning, "Cannot reset tile vertices with " + QString::number(arrVertex.size()) + " vertices");
        return;
    }

    for(int i(0); i<arrVertex.size(); ++i)
    {
        m_arrVertex[i/3][i%3] = arrVertex[i];
    }
}


// Функция для вычисления нового индекса после определённого количества поворотов
QPair<int, int> getNewIndex(int i, int j, int rotations) {
    // Привести количество поворотов к диапазону 0-3 (модуль 4)
    rotations = rotations % 4;

    // В зависимости от количества поворотов, возвращаем новый индекс
    switch (rotations) {
        case 0: // 0 поворотов (исходный индекс)
            return {i, j};
        case 1: // 1 поворот
            return {j, 2 - i};
        case 2: // 2 поворота
            return {2 - i, 2 - j};
        case 3: // 3 поворота
            return {2 - j, i};
        default:
            return {i, j}; // Это никогда не произойдет
    }
}

// nine vertices of tile
/*
6 _7 _8
|\ |\ |
|_\|_\|
3 _4 _5
|\ |\ |
|_\|_\|
0  1  2
9 land vertice -> 16 indexes:
ind num:   0 1 2 3    4 5 6 7     8 9 10 11  12 13 14 15
quat ind: {0,1,4,3}, {1,2,5,4}, {3,4,7, 6},  {4, 5, 8, 7}
1 tile -> 16 tex coords with the same indexes
*/
void CTile::generateDrawVertexData(QVector<SVertexData>& outData, int& curIndex)
{
    QVector<QVector2D> tCoord; // store x,y min and x,y max
    tCoord.resize(2);
    int texCount = m_texAtlasNumber;
    tCoord[0].setX((m_index%8/8.0f+m_atlasTexIndex)/texCount); // A landscape texture is expected to be a set of textures attached along the X axis, so for the X coordinate we need to take this offset into account.
    tCoord[0].setY((7-int(m_index/8))/8.0f);

    tCoord[1].setX(tCoord[0].x() + 1.0f/8.0f/texCount);
    tCoord[1].setY(tCoord[0].y() + 1.0f/8.0f);
    tCoord[0] += QVector2D(4.0f/512.0f/texCount, 4.0f/512.0f/texCount); // cut a little piece from tile texture (dxt provide some "trash' on border)
    tCoord[1] -= QVector2D(4.0f/512.0f/texCount, 4.0f/512.0f/texCount); // cut a little piece from tile texture (dxt provide some "trash' on border)

    auto calcTexCoord = [&tCoord](QVector2D& texCoord, int row, int col)
    {
        float xStep = (tCoord[0].x() + tCoord[1].x())/2.0f - tCoord[0].x();
        float yStep = (tCoord[0].y() + tCoord[1].y())/2.0f - tCoord[0].y();
        texCoord.setX(tCoord[0].x() + xStep * col);
        texCoord.setY(tCoord[1].y() - yStep * row);
    };

    for(int row(0); row<3; ++row)
        for(int col(0); col<3; ++col)
        {

            const SSecVertex& vrt = m_arrVertex[row][col];
            outData[curIndex].position.setX(m_x + col + vrt.xOffset/254.0f);
            outData[curIndex].position.setY(m_y + row + vrt.yOffset/254.0f);
            outData[curIndex].position.setZ(vrt.z * m_maxZ/65535.0f);
            outData[curIndex].normal = vrt.normal;
            QVector2D test1;
            calcTexCoord(test1, row, col);
            auto indN = getNewIndex(row, col, m_rotNum);
            QVector2D test2;
            calcTexCoord(test2, indN.first, indN.second);
            calcTexCoord(outData[curIndex].texCoord, indN.first, indN.second);
            ++curIndex;
        }

}

bool CTile::isProjectPoint(QVector3D& outPoint)
{
    float u,v,t;
    QVector3D dir(0.0f, 0.0f, 1.0f);
    QVector<int> arrTriangleId = {0, 1, 3, 3, 1, 4, 1, 2, 4, 4, 2, 5, 3, 4, 6, 6, 4, 7, 4, 5, 7, 7, 5, 8};
    QVector3D pt1;
    QVector3D pt2;
    QVector3D pt3;
    for(int i(0); i<arrTriangleId.size(); i+=3)
    {
        pt1 = pos(arrTriangleId[(i+0)]/3, arrTriangleId[(i+0)]%3);
        pt2 = pos(arrTriangleId[(i+1)]/3, arrTriangleId[(i+1)]%3);
        pt3 = pos(arrTriangleId[(i+2)]/3, arrTriangleId[(i+2)]%3);
        if (util::ptToTriangle(t, u, v, outPoint, dir, pt1, pt2, pt3))
        {
            outPoint.setZ(outPoint.z() + t);
            return true;
        }
    }
    return false;
}

bool CTile::isProjectTile(QVector3D& outPoint)
{
    float u,v,t;
    QVector3D dir(0.0f, 0.0f, 1.0f);
    //find suitable vertex data cut for projection, don't need to process all vertices

    QVector3D pt1(pos(0, 0));
    QVector3D pt2(pos(0, 2));
    QVector3D pt3(pos(2, 0));
    QVector3D pt4(pos(2, 2));

    if (util::ptToTriangle(t, u, v, outPoint, dir, pt1, pt2, pt3) ||
        util::ptToTriangle(t, u, v, outPoint, dir, pt3, pt2, pt4))
    {
        outPoint.setZ(outPoint.z() + t);
        return true;
    }


    return false;
}

int CTile::tileIndex() const
{
    int atlasIndex = m_atlasTexIndex >= m_texAtlasNumber ? (m_texAtlasNumber-1) : m_atlasTexIndex;
    return m_index + (atlasIndex*64);
}

void CTile::setTile(int index, int rotNum)
{
    m_index = index%64;
    m_atlasTexIndex = index/64;
    m_rotNum = rotNum;
}

ushort CTile::packData()
{
    return (m_index & 63) | ((m_atlasTexIndex & 255) << 6) | ((m_rotNum & 3) << 14);
}

void CTile::reset()
{
    m_index = 0;
    m_atlasTexIndex = 0;
    m_rotNum = 0;
    m_arrVertex.clear();
    m_arrVertex.resize(3);
    for(int i(0); i<3; ++i)
        m_arrVertex[i].resize(3);
}

QVector3D CTile::pos(int row, int col)
{
    const SSecVertex& vrt = m_arrVertex[row][col];
    QVector3D pos;
    pos.setX(m_x + col + vrt.xOffset/254.0f);
    pos.setY(m_y + row + vrt.yOffset/254.0f);
    pos.setZ(vrt.z * m_maxZ/65535.0f);
    return pos;
}

CPreviewTile::CPreviewTile():
    m_vertexBuf(QOpenGLBuffer::VertexBuffer)
   ,m_indexBuf(QOpenGLBuffer::IndexBuffer)
{
    m_vertexBuf.create();
    m_indexBuf.create();
    //m_modelMatrix.setToIdentity();
}

CPreviewTile::~CPreviewTile()
{
    m_vertexBuf.destroy();
    m_indexBuf.destroy();
}

void CPreviewTile::updateTile(const CTile& tile, int tIndex, int rotation, int mIndex, int xSector, int ySector)
{
    m_arrLandVrtData.clear();
    m_arrLandVrtData.resize(9);
    CTile previewTile = tile;
    previewTile.setTile(tIndex, rotation);
    previewTile.setMaterialIndex(mIndex);

    int curIndex(0);
    previewTile.generateDrawVertexData(m_arrLandVrtData, curIndex);
    m_modelMatrix.setToIdentity();
    m_modelMatrix.translate(QVector3D(xSector*32.0f, ySector*32.0f, .0f));

    m_vertexBuf.bind();
    m_vertexBuf.allocate(m_arrLandVrtData.data(), m_arrLandVrtData.count()*int(sizeof(SVertexData)));
    m_vertexBuf.release();

    QVector<ushort> arrTileInd{0,1,4,3, 1,2,5,4, 3,4,7,6, 4,5,8,7}; // indices of quad vertices
    m_indexBuf.bind();
    m_indexBuf.allocate(arrTileInd.data(), arrTileInd.size() * sizeof(ushort));
    m_indexBuf.release();
}

void CPreviewTile::draw(QOpenGLShaderProgram* program)
{
    if (m_arrLandVrtData.count() == 0)
        return;

    program->setUniformValue("u_modelMmatrix", m_modelMatrix);

    int offset = 0;
    m_vertexBuf.bind();
    int vertLoc = program->attributeLocation("a_position");
    program->enableAttributeArray(vertLoc);
    program->setAttributeBuffer(vertLoc, GL_FLOAT, offset, 3, int(sizeof(SVertexData)));

    offset += int(sizeof(QVector3D));
    int normLoc = program->attributeLocation("a_normal");
    program->enableAttributeArray(normLoc);
    program->setAttributeBuffer(normLoc, GL_FLOAT, offset, 3, int(sizeof(SVertexData)));

    offset+=int(sizeof(QVector3D)); // size of normal
    int textureLocation = program->attributeLocation("a_texture");
    program->enableAttributeArray(textureLocation);
    program->setAttributeBuffer(textureLocation, GL_FLOAT, offset, 2, int(sizeof(SVertexData)));
    m_indexBuf.bind();
    glDrawElements(GL_QUADS, 16, GL_UNSIGNED_SHORT, nullptr); // 16 indices for tile
}

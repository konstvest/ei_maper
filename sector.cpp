#include "landscape.h"
#include "sector.h"
#include "math_utils.h"
#include "log.h"

static const uint secSignature = 0xcf4bf774;
static const int nVertex = 33; // vertices count by 1 side of sector
//const int nSecVertex = nVertex * nVertex;
static const int nTile = 16;   //tiles count by 1 side of sector
static constexpr int nSecTile = nTile * nTile;

CSector::~CSector()
{
    m_vertexBuf.destroy();
    m_indexBuf.destroy();
}

CSector::CSector(QDataStream& stream, float maxZ, int texCount)
    :m_indexBuf(QOpenGLBuffer::IndexBuffer)
    ,m_waterIndexBuf(QOpenGLBuffer::IndexBuffer)
{
    SSecHeader header;
    stream >> header;
    if(header.signature != secSignature)
    {
        ei::log(eLogFatal, "Incorrect sector signature");
        return;
    }
    m_modelMatrix.setToIdentity();
    m_vertexBuf.create();
    m_indexBuf.create();
    m_waterVertexBuf.create();
    m_waterIndexBuf.create();
    m_arrTile.clear();
    SSecVertex sv;
    QVector<QVector<SSecVertex>> landVertex;
    QVector<SSecVertex> aLine;
    for (int column(0); column<nVertex; ++column)
    {
        aLine.clear();
        for(int row(0); row<nVertex; ++row)
        {
            stream >> sv;
            aLine.append(sv);
        }
        landVertex.append(aLine);
    }

    QVector<QVector<SSecVertex>> waterVertex;
    if(header.type != eTerrainBase)
    {
        for (int column(0); column<nVertex; ++column)
        {
            aLine.clear();
            for(int row(0); row<nVertex; ++row)
            {
                stream >> sv;
                aLine.append(sv);
            }
            waterVertex.append(aLine);
        }
    }

    // convert filedata to tile-specific quads. x,y(0,0) is left down corner. Move to right by column then up by row
    m_arrTile.resize(nTile);
    QVector<SSecVertex> arrVertex;
    for(int i(0); i<nTile; ++i)
    {
        m_arrTile[i].resize(nTile);
    }

    QVector<STile> aLandTiles; //todo: remove
    ushort landTilePacked;
    for (int row(0); row<nTile; ++row)
    {
        aLine.clear();
        for(int col(0); col<nTile; ++col)
        {
            stream >> landTilePacked;
            STile tile(landTilePacked); //todo: remove
            aLandTiles.append(tile); //todo: remove
            m_arrTile[row][col] = CLandTile(landTilePacked, col*2, row*2, maxZ, texCount);

            arrVertex.clear();
            arrVertex.resize(9);
            // step for vertices = 2*step for tiles
            arrVertex[0] = landVertex[row*2][col*2];
            arrVertex[1] = landVertex[row*2][col*2+1];
            arrVertex[2] = landVertex[row*2][col*2+2];
            arrVertex[3] = landVertex[row*2+1][col*2];
            arrVertex[4] = landVertex[row*2+1][col*2+1];
            arrVertex[5] = landVertex[row*2+1][col*2+2];
            arrVertex[6] = landVertex[row*2+2][col*2];
            arrVertex[7] = landVertex[row*2+2][col*2+1];
            arrVertex[8] = landVertex[row*2+2][col*2+2];
            m_arrTile[row][col].resetVertices(arrVertex);
        }
    }

    QVector<STile> aWaterTiles;
    if(header.type != eTerrainBase)
    {
        ushort waterTilePacked;
        for(int i(0); i<nSecTile; ++i)
        {
            stream >> waterTilePacked;
            STile tile(waterTilePacked);
            aWaterTiles.append(tile);
        }

        short wa;
        for(int i(0); i<nSecTile; ++i)
        {
            stream >> wa;
            m_aWaterAllow.append(wa);
        }
    }

    //makeVertexData(landVertex, aLandTiles, waterVertex, aWaterTiles, maxZ, texCount);
    generateVertexDataFromTile();
}

void SSpecificQuad::rotate(int step)
{
    Q_ASSERT(m_aVertexPointer.size() == 16);
    QVector<QVector2D> aTmpRow;
    aTmpRow.resize(4);
    for(int i(0); i<step; ++i)
    {
        aTmpRow[0] = m_aVertexPointer[0]->texCoord;
        aTmpRow[1] = m_aVertexPointer[1]->texCoord;
        aTmpRow[2] = m_aVertexPointer[2]->texCoord;
        aTmpRow[3] = m_aVertexPointer[3]->texCoord;

        m_aVertexPointer[0]->texCoord = m_aVertexPointer[5]->texCoord;
        m_aVertexPointer[1]->texCoord = m_aVertexPointer[6]->texCoord;
        m_aVertexPointer[2]->texCoord = m_aVertexPointer[7]->texCoord;
        m_aVertexPointer[3]->texCoord = m_aVertexPointer[4]->texCoord;

        m_aVertexPointer[4]->texCoord  = m_aVertexPointer[13]->texCoord;
        m_aVertexPointer[5]->texCoord  = m_aVertexPointer[14]->texCoord;
        m_aVertexPointer[6]->texCoord = m_aVertexPointer[15]->texCoord;
        m_aVertexPointer[7]->texCoord = m_aVertexPointer[12]->texCoord;

        m_aVertexPointer[12]->texCoord = m_aVertexPointer[9]->texCoord;
        m_aVertexPointer[13]->texCoord = m_aVertexPointer[10]->texCoord;
        m_aVertexPointer[14]->texCoord = m_aVertexPointer[11]->texCoord;
        m_aVertexPointer[15]->texCoord = m_aVertexPointer[8]->texCoord;

        m_aVertexPointer[8]->texCoord  = aTmpRow[1];
        m_aVertexPointer[9]->texCoord  = aTmpRow[2];
        m_aVertexPointer[10]->texCoord = aTmpRow[3];
        m_aVertexPointer[11]->texCoord = aTmpRow[0];
    }
    m_rotateState = step;
}

void CSector::makeVertexData(QVector<QVector<SSecVertex>>& aLandVertex, QVector<STile>& aLandTile,
                             QVector<QVector<SSecVertex>>& aWaterVertex, QVector<STile>& aWaterTile,
                             float maxZ, int texCount)
{
/*
^|   6 _7 _8
c|   |\ |\ |
o|   |_\|_\|
l|   3 _4 _5
u|   |\ |\ |
m|   |_\|_\|
n|   0  1  2 ROW---->
specific quad is
4 land quads -> 16 points -> 16 indexes,  {0,1,4,3}, {1,2,5,4}, {3,4,7,6}, {4,5,8,7}
1 tile -> 16 tex coords with the same indexes
TODO: use 9 vertex, make quad via index array
*/

    Q_ASSERT(aLandVertex.size() == 33);
    Q_ASSERT(aLandTile.size() == 256);
    QVector<QVector2D> tCoord; // min + max
    tCoord.resize(2);
    //SVertexData vData;
    m_aVertexData.resize(aLandTile.size()*16);
    m_aWaterData.resize(aWaterTile.size()*16);
    int idVert(0);

    auto calcTexCoordBox = [&tCoord, texCount](STile& tile)
    {
      tCoord[0].setX((tile.m_index%8/8.0f+tile.m_texture)/texCount);
      tCoord[0].setY((7-int(tile.m_index/8))/8.0f);

      tCoord[1].setX(tCoord[0].x() + 1.0f/8.0f/texCount);
      tCoord[1].setY(tCoord[0].y() + 1.0f/8.0f);

      tCoord[0] += QVector2D(4.0f/512.0f/texCount, 4.0f/512.0f/texCount);
      tCoord[1] -= QVector2D(4.0f/512.0f/texCount, 4.0f/512.0f/texCount);
    };

    int vertIndexQuad(0);

    auto calcTexCoord = [&vertIndexQuad, &tCoord](SVertexData& vrt)
    {
        QVector2D texC = QVector2D(tCoord[0].x(), tCoord[1].y());
        switch(vertIndexQuad)
        {
        case 1:
        case 2:
        case 9:
        case 10:
        case 4:
        case 7:
        case 12:
        case 15:
        {
            texC.setX((tCoord[0].x() + tCoord[1].x())/2.0f);
            break;
        }
        case 5:
        case 6:
        case 13:
        case 14:
        {
            texC.setX(tCoord[1].x());
            break;
        }
        default:
            break;
        }

        switch (vertIndexQuad)
        {
        case 3:
        case 2:
        case 7:
        case 6:
        case 8:
        case 9:
        case 12:
        case 13:
        {
            texC.setY((tCoord[0].y() + tCoord[1].y())/2.0f);
            break;
        }
        case 11:
        case 10:
        case 15:
        case 14:
        {
            texC.setY(tCoord[0].y());
            break;
        }
        }

//        if(vertIndexQuad == 1 || vertIndexQuad == 2 || vertIndexQuad == 9 || vertIndexQuad == 10
//            || vertIndexQuad == 4 || vertIndexQuad == 7 || vertIndexQuad == 12 || vertIndexQuad == 15)
//            texC.setX((tCoord[0].x() + tCoord[1].x())/2.0f);
//        else if(vertIndexQuad == 5 || vertIndexQuad == 6 || vertIndexQuad == 13 || vertIndexQuad == 14)
//            texC.setX(tCoord[1].x());

//        if(vertIndexQuad == 3 || vertIndexQuad == 2 || vertIndexQuad == 7 || vertIndexQuad == 6
//            || vertIndexQuad == 8 || vertIndexQuad == 9 || vertIndexQuad == 12 || vertIndexQuad == 13)
//            texC.setY((tCoord[0].y() + tCoord[1].y())/2.0f);
//        else if(vertIndexQuad == 11 || vertIndexQuad == 10 || vertIndexQuad == 15 || vertIndexQuad == 14)
//            texC.setY(tCoord[0].y());

        vrt.texCoord = texC;
    };

    auto buildGeom = [maxZ, &vertIndexQuad, &calcTexCoord](QVector<QVector<SSecVertex>>& aVertex, SVertexData& vrt, int x, int y)
    {
        if(vertIndexQuad == 1 || vertIndexQuad == 2 || vertIndexQuad == 9 || vertIndexQuad == 10
            || vertIndexQuad == 4 || vertIndexQuad == 7 || vertIndexQuad == 12 || vertIndexQuad == 15)
            x+=1;
        else if(vertIndexQuad == 5 || vertIndexQuad == 6 || vertIndexQuad == 13 || vertIndexQuad == 14)
            x+=2;
        if(vertIndexQuad == 3 || vertIndexQuad == 2 || vertIndexQuad == 7 || vertIndexQuad == 6
            || vertIndexQuad == 8 || vertIndexQuad == 9 || vertIndexQuad == 12 || vertIndexQuad == 13)
            y+=1;
        else if(vertIndexQuad == 11 || vertIndexQuad == 10 || vertIndexQuad == 15 || vertIndexQuad == 14)
            y+=2;
        vrt.position.setX(x+aVertex[y][x].xOffset/254.0f);
        vrt.position.setY(y+aVertex[y][x].yOffset/254.0f);
        vrt.position.setZ(aVertex[y][x].z*maxZ/65535.0f);
        vrt.normal = aVertex[y][x].normal;
        calcTexCoord(vrt);
    };

    for(int i(0); i<aLandTile.size(); ++i)
    {
        calcTexCoordBox(aLandTile[i]);
        SSpecificQuad quad;
        int vrtX = i%16*2;
        int vrtY = int(i/16*2);
        for(vertIndexQuad = 0; vertIndexQuad < 16 ; ++vertIndexQuad)
        {
            buildGeom(aLandVertex, m_aVertexData[idVert], vrtX, vrtY);
            quad.addVertex(&m_aVertexData[idVert]);
            ++idVert;
        }
        quad.rotate(aLandTile[i].m_rotation);
    }

    idVert=0;
    for(int i(0); i<aWaterTile.size(); ++i)
    {
        calcTexCoordBox(aWaterTile[i]);
        SSpecificQuad quad;
        int vrtX = i%16*2;
        int vrtY = int(i/16*2);
        for(vertIndexQuad = 0; vertIndexQuad < 16 ; ++vertIndexQuad)
        {
            buildGeom(aWaterVertex, m_aWaterData[idVert], vrtX, vrtY);
            quad.addVertex(&m_aWaterData[idVert]);
            ++idVert;
        }
        quad.rotate(aWaterTile[i].m_rotation);
    }
}

void CSector::generateVertexDataFromTile()
{
    uint vertSize = m_arrTile.size() * m_arrTile.front().size()*16; // 9 vertices, 16 indices per tile
    //m_aVertexData.clear(); // to recalc textures ?
    m_aVertexData.resize(vertSize);
    int curIndex(0);
    for(int row(0); row<m_arrTile.size(); ++row)
    {
        for(int col(0); col<m_arrTile[row].size(); ++col)
        {
            m_arrTile[row][col].generateDrawVertexData(m_aVertexData, curIndex);
        }
    }
}

void CSector::updatePosition()
{
    m_modelMatrix.translate(QVector3D(m_index.x*32.0f, m_index.y*32.0f, .0f));
    m_vertexBuf.bind();
    m_vertexBuf.allocate(m_aVertexData.data(), m_aVertexData.count()*int(sizeof(SVertexData)));
    m_vertexBuf.release();

    QVector<ushort> aInd;
    ushort indOffset = 0;
    QVector<ushort> arrTileInd{0,1,4,3, 1,2,5,4, 3,4,7,6, 4,5,8,7}; // indices of quad vertices
    for(int i(0); i<256; ++i) // 256 - tile number per sector
    {
        for(auto ind: arrTileInd)
        {
            aInd.append(ind += indOffset);
        }
        indOffset += 9; // vertices per tile.
    }

    m_indexBuf.bind();
    m_indexBuf.allocate(aInd.data(), aInd.count() * int(sizeof(ushort)));
    m_indexBuf.release();

    //water section
    //m_modelMatrix.translate(QVector3D(m_index.x*32.0f, m_index.y*32.0f, .0f));
    m_waterVertexBuf.bind();
    m_waterVertexBuf.allocate(m_aWaterData.data(), m_aWaterData.count()*int(sizeof(SVertexData)));
    m_waterVertexBuf.release();

    aInd.clear();
    for (ushort i(0); i<m_aWaterData.size(); ++i)
        aInd.append(i);

    m_waterIndexBuf.bind();
    m_waterIndexBuf.allocate(aInd.data(), aInd.count() * int(sizeof(ushort)));
    m_waterIndexBuf.release();
}

void CSector::draw(QOpenGLShaderProgram* program)
{

    if (m_aVertexData.count() == 0)
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
    glDrawElements(GL_QUADS, m_aVertexData.count(), GL_UNSIGNED_SHORT, nullptr);

}

void CSector::drawWater(QOpenGLShaderProgram *program)
{
    if (m_aWaterData.count() == 0)
        return;

    program->setUniformValue("u_modelMmatrix", m_modelMatrix);

    int offset = 0;
    m_waterVertexBuf.bind();
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

    m_waterIndexBuf.bind();
    glDrawElements(GL_QUADS, m_aWaterData.count(), GL_UNSIGNED_SHORT, nullptr);
}

bool CSector::projectPt(QVector3D& point)
{
    float u,v,t;
    QVector3D origin(point.x()-m_index.x*32.0f, point.y()-m_index.y*32.0f, 0.0f);
    QVector3D dir(0.0f, 0.0f, 1.0f);
    //find suitable vertex data cut for projection, don't need to process all vertices
    float dif=0.0f;
    for(int i(0); i < m_aVertexData.size(); i+=4)
    {
        dif = origin.x() - m_aVertexData[i].position.x();
        if (dif < -1.0f || dif > 2.0f)
            continue;
        dif = origin.y() - m_aVertexData[i].position.y();
        if (dif < -1.0f || dif > 2.0f)
            continue;

        if (util::ptToTriangle(t, u, v, origin, dir, m_aVertexData[i].position, m_aVertexData[i+1].position, m_aVertexData[i+3].position)
                || util::ptToTriangle(t, u, v, origin, dir, m_aVertexData[i+3].position, m_aVertexData[i+1].position, m_aVertexData[i+2].position))
        {
            point = QVector3D(point.x(), point.y(), point.z() + t);
            return true;
        }
    }

    return false;
}

STile::STile(ushort packedData)
{
    m_index = packedData & 63; //first 6 bits
    m_texture = (packedData >> 6) & 255; // second 8 bits
    m_rotation = (packedData >> 14) & 3; // 2 bits more
}

CLandTile::CLandTile()
{
    reset();
}

CLandTile::CLandTile(ushort packedData, ushort x, ushort y, float maxZ, int atlasNumber):
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

CLandTile::~CLandTile()
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
void CLandTile::resetVertices(QVector<SSecVertex>& arrVertex)
{
    if(arrVertex.size() != 9)
    {
        ei::log(eLogWarning, "Cannot reset tile vertices with " + QString::number(arrVertex.size()) + " vertices");
        return;
    }

    for(int i(0); i<arrVertex.size(); ++i)
    {
        int row = i/3;
        int col = i%3;
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
void CLandTile::generateDrawVertexData(QVector<SVertexData>& outData, int& curIndex)
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


    auto calcCoord = [this, &tCoord](QVector2D& texCoord, QVector3D& position, float xOffset, float yOffset, float altitude, int ind)
    {
        // x offset
        switch(ind)
        {
        case 0:
        case 3:
        case 8:
        case 11:
        {
            texCoord.setX(tCoord[0].x());
            position.setX(this->m_x + xOffset/254.0f); // start pos
            break;
        }
        case 1:
        case 2:
        case 9:
        case 10:
        case 4:
        case 7:
        case 12:
        case 15:
        {
            texCoord.setX((tCoord[0].x() + tCoord[1].x())/2.0f);
            position.setX(this->m_x+1+xOffset/254.0f); // mid point, tile offset = 1
            break;
        }
        case 5:
        case 6:
        case 13:
        case 14:
        {
            texCoord.setX(tCoord[1].x());
            position.setX(this->m_x+2+xOffset/254.0f); // right point, tile offset = 2
            break;
        }
        default:
        {
            break;
        }
        }

        // y offset
        switch (ind)
        {
        case 0:
        case 1:
        case 4:
        case 5:
        {
            texCoord.setY(tCoord[0].y());
            position.setY(this->m_y + yOffset/254.0f); // start pos
            break;
        }
        case 3:
        case 2:
        case 7:
        case 6:
        case 8:
        case 9:
        case 12:
        case 13:
        {
            texCoord.setY((tCoord[0].y() + tCoord[1].y())/2.0f);
            position.setY(this->m_y+1+yOffset/254.0f); // mid point, tile offset = 1
            break;
        }
        case 11:
        case 10:
        case 15:
        case 14:
        {
            texCoord.setY(tCoord[0].y());
            position.setY(this->m_y+2+yOffset/254.0f); // top point, tile offset = 2
            break;
        }
        default:
        {
            break;
        }
        }
        position.setZ(altitude*this->m_maxZ/65535.0f);
    };

    auto calcTexCoord = [&tCoord](QVector2D& texCoord, int row, int col)
    {
        float xStep = (tCoord[0].x() + tCoord[1].x())/2.0f - tCoord[0].x();
        float yStep = (tCoord[0].y() + tCoord[1].y())/2.0f - tCoord[0].y();
        texCoord.setX(tCoord[0].x() + xStep * col);
        texCoord.setY(tCoord[1].y() - yStep * row);
    };

    int i(0);
    //QVector<int> indArr{0,1,4,3, 1,2,5,4, 3,4,7,6, 4,5,8,7}; // indices of quad vertices
    //for(auto& ind: indArr)
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
            //calcTexCoord(outData[curIndex].texCoord, 2-row, col);
            //auto indN = getNewIndex(col, row, m_rotNum);
            //calcTexCoord(outData[curIndex].texCoord, indN.second, indN.first);
            ++curIndex;
            ++i;
        }

}

void CLandTile::reset()
{
    m_index = 0;
    m_atlasTexIndex = 0;
    m_rotNum = 0;
    m_arrVertex.clear();
    m_arrVertex.resize(3);
    for(int i(0); i<3; ++i)
        m_arrVertex[i].resize(3);
}

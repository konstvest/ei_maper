#include "ei_object.h"
#include "ei_utils.h"

ei::CFigure::CFigure()
{

}

ei::CFigure::~CFigure()
{

}

struct SHeader{
    int vertBlocks = 0;
    int normalBlocks = 0;
    int UVCount = 0;
    int indexCount = 0;
    int vertexComponentCount = 0;
    int morphingComponentCount = 0;
    int unknown = 0;
    int group = 0;
    int textureNumber = 0;
};

void readHeader(std::ifstream& file, SHeader& hd){
    file.read((char*)&hd, sizeof(hd));
}

// read xyz(3) for morph components(8)
void read24(std::ifstream& file, QVector<f3>& points){
    f3 temp;
    for (int i(0); i<8; i++){
        file.read((char*)&temp, sizeof(temp));
        points.push_back(temp);
    }
}

// read morph components(8)
void read8(std::ifstream& file, QVector<float>& points){
    float buf;
    for (int i(0); i<8;i++){
        file.read((char*)&buf, sizeof (buf));
        points.push_back(buf);
    }
}

//     vertex#1           vertex#2
// m |{x00,y00,z00}| |{x10,y10,z10}| .......
// o |.............| |.............| .......
// r |.............| |.............| .......
// p |.............| |.............| .......
// h |{x07,y07,z07}| |{x17,y17,z17}| .......
void readVertices(std::ifstream& file, QVector <QVector <f3>>& morphVerts, const int blockCount){
    f3 pt;
    QVector<f3> vertices;
    float buf;
    //fill zeroes
    for (int block(0); block<blockCount*4; ++block)
        vertices.append(pt);
    for (int morph(0); morph<8; ++morph)
        morphVerts.append(vertices);

    //mb change header[0] to header[morph component count]
    for (int block(0); block<blockCount; ++block){
        for (int xyz(0); xyz<3; ++xyz){
            for (int morph(0); morph<8; ++morph){
                for (int point(0); point<4; ++point){
                    file.read((char*)&buf, sizeof (buf));
                    morphVerts[morph][block*4+point][xyz]=buf;
                }
            }
        }
    }

}

//     nrml#1          nrml#2
// |{x0,y0,z0,w0}| |{x1,y1,z1,w1}| ....
void readNormals(std::ifstream& file, QVector<QVector<float>>& nrmls, const int blockCount){
    QVector<float> pt; //xyzw
    float buf;
    //fill 1.0 for all normals
    for (int curN(0); curN<4; ++curN)
        pt.push_back(1.0);
    for (int block(0); block<blockCount*4; ++block)
        nrmls.push_back(pt);

    for (int block(0); block<blockCount; ++block){   //mb change header[0] to header[morph component count]
        for (int xyzw(0); xyzw<4; ++xyzw){
            for (int point(0); point<4; ++point){
                file.read((char*)&buf, sizeof (buf));
                nrmls[block*4+point][xyzw]=buf;
            }
        }
    }
}

//     UV#1     UV#2
// |{x0,y0}| |{x1,y1}| ....
void readTextureCoords(std::ifstream& file, QVector<QVector<float>>& tCoords, const int uvCount){
    QVector<float> pt;    //xy
    float buf;
    //fill 0.0
    for (int t(0); t<2; ++t)
        pt.push_back(0.0);
    for (int pts(0); pts<uvCount; ++pts)
        tCoords.push_back(pt);

    for (int point(0); point<uvCount; point++)
        for (int xy(0); xy<2; ++xy){
            file.read((char*)&buf, sizeof (buf));
            tCoords[point][xy]=buf;
        }
}

void readIndices(std::ifstream& file, QVector<short>& indices, const int indCount){
    short buf;
    for (int i(0); i<indCount; i++){
        file.read((char*)&buf, sizeof(buf));
        indices.push_back(buf);
    }
}

void readVertexComponents(std::ifstream& file, QVector<indices_link>& vComp, const int vcCount){
    short buf;
    indices_link temp;
    for (int i(0); i<vcCount; i++){
        file.read((char*)&buf, sizeof(buf));
        temp.normal_ind = buf;
        file.read((char*)&buf, sizeof(buf));
        temp.vertex_ind = buf;
        file.read((char*)&buf, sizeof(buf));
        temp.texture_ind = buf;
        vComp.push_back(temp);
    }
}

//convert indices from ei_fig format to dif arrays for vertices, normals and texture coordinates
void convertIndices(QVector<short>& inds, QVector<indices_link>& comps, QVector<int>& vertInds, QVector<int>& normInds, QVector<int>& uvInds){
    for (int i(0); i<inds.size(); ++i){
        vertInds.push_back(comps[inds[i]].vertex_ind);
        normInds.push_back(comps[inds[i]].normal_ind);
        uvInds.push_back(comps[inds[i]].texture_ind);
    }
}

int recalcUV(const int typeUV){
    int t = 0;
    switch (typeUV){
    case 2:
        t=1;
        break;
    case 8:
        t=2;
        break;
    default:
        t=0;
        break;
    }
    return t;
}

void convertUVCoords(QVector<QVector<float>>& coordsUV, int convertCount){
    for (int i(0); i<convertCount; ++i){
        for (auto& pt:coordsUV){
            pt[0]/=2;
            pt[1]=0.5+pt[1]/2;
        }
    }
}

//load morphing_vertices, indices, normals, texture coordinates
bool ei::CFigure::loadFromFile(QString& pathFile){
    SHeader header;
    QVector<short> fIndices;
    QVector<indices_link> vComponents;
    std::ifstream figFile;
    figFile.open(pathFile.toLatin1(), std::ios::binary);
    if (!figFile){
        qDebug() << "Can't load file " << pathFile;
        return false;
    }
    //check signature
    if (!EI_Utils::checkSignature(figFile, EI_Utils::Signatures::fig8))
        return false;
    //read header
    readHeader(figFile, header);
    //read center
    read24(figFile, m_morphCenter);
    //read min
    read24(figFile, m_morphMin);
    //read max
    read24(figFile, m_morphMax);
    //read radius
    read8(figFile, m_morphRadius);
    //read vertices
    readVertices(figFile, m_morphVertices, header.vertBlocks);
    //read normals
    readNormals(figFile, m_normals, header.normalBlocks);
    //read texture coordinates
    readTextureCoords(figFile, m_uvCoords, header.UVCount);
    //read indices
    readIndices(figFile, fIndices, header.indexCount);
    //read vertex components
    readVertexComponents(figFile, vComponents, header.vertexComponentCount);
    //create indicies array of vertices, uv coords and normals
    convertIndices(fIndices, vComponents, m_vertIndices, m_normIndices, m_uvIndices);
    //convert x,y uvCoords from type of object
    convertUVCoords(m_uvCoords, recalcUV(header.textureNumber));

    calculateConstitution(1.0, 1.0, 1.0);

    figFile.close();
    return true;
}

void ei::CFigure::calculateConstitution(float str, float dex, float scale){
    //TODO: fill m_vertices
//    float temp1, temp2, res1, res2;
//    temp1 = m_morphVertices[0]+(m_morphVertices[1]-m_morphVertices[0])*str;
//    temp2 = m_morphVertices[2]+(m_morphVertices[3]-m_morphVertices[2])*str;
//    res1 = temp1+(temp2-temp1)*dex;
//    temp1 = m_morphVertices[4]+(m_morphVertices[5]-m_morphVertices[4])*str;
//    temp2 = m_morphVertices[6]+(m_morphVertices[7]-m_morphVertices[6])*str;
//    res2 = temp1+(temp2-temp1)*dex;
//    m_vertices[0]=res1+(res2-res1)*scale;
}

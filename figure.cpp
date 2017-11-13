#include "ei_object.h"

ei::CFigure::CFigure()
{

}

ei::CFigure::~CFigure()
{

}

void readHeader(std::ifstream& file, QVector<int>& header){
    unsigned int buf;
    for (int i(0); i<9; i++){
        file.read((char*)&buf, sizeof (unsigned int));
        header.push_back(buf);
    }
}

// read xyz(3) for morph components(8)
void read24(std::ifstream& file, QVector<vec3>& points){
    vec3 temp;
    float buf;
    for (int i(0); i<8; i++){
        file.read((char*)&buf, sizeof (float));
        temp.x = buf;
        file.read((char*)&buf, sizeof (float));
        temp.y = buf;
        file.read((char*)&buf, sizeof (float));
        temp.z = buf;
        points.push_back(temp);
    }
}

void read8(std::ifstream& file, QVector<float>& points){
    float buf;
    for (int i(0); i<8;i++){
        file.read((char*)&buf, sizeof (float));
        points.push_back(buf);
    }
}

void readVertices(std::ifstream& file, QVector <QVector <vec3>>& verts, QVector<int>& header){
    vec3 temp;
    QVector<vec3> tvertices;
    float buf;
    for (int i(0); i<header[0]; i++){
        //create empty q*q*vec3
        for (int i(0); i<4; i++){
            for (int j(0); j<8; j++){
                tvertices.push_back(temp);
            }
            verts.push_back(tvertices);
        }
        for (int morph(0); morph<8; morph++){
            for (int block(0); block<4; block++){
                file.read((char*)&buf, sizeof (float));
                verts[i*4+block][morph].x=buf;
            }
        }
        for (int morph(0); morph<8; morph++){
            for (int block(0); block<4; block++){
                file.read((char*)&buf, sizeof (float));
                verts[i*4+block][morph].y=buf;
            }
        }
        for (int morph(0); morph<8; morph++){
            for (int block(0); block<4; block++){
                file.read((char*)&buf, sizeof (float));
                verts[i*4+block][morph].z=buf;
            }
        }
    }
}

void readNormals(std::ifstream& file, QVector<vec4>& nrmls, QVector<int>& header){
    float buf;
    vec4 temp;
    temp.x = 1;
    temp.y = 1;
    temp.z = 1;
    temp.w = 1;
    for (int i(0); i<header[1]; i++){
        for (int j(0); j<4; j++){
            nrmls.push_back(temp);
        }
        for (int j(0); j<4; j++){
            file.read((char*)&buf, sizeof (float));
            nrmls[j+i*4].x = buf;
            //qDebug() << fbuf;
        }
        for (int j(0); j<4; j++){
            file.read((char*)&buf, sizeof (float));
            nrmls[j+i*4].y = buf;
        }
        for (int j(0); j<4; j++){
            file.read((char*)&buf, sizeof (float));
            nrmls[j+i*4].z = buf;
        }
        for (int j(0); j<4; j++){
            file.read((char*)&buf, sizeof (float));
            nrmls[j+i*4].w = buf;
        }
    }
}

void readTextureCoords(std::ifstream& file, QVector<vec2>& tCoords, QVector<int>& header){
    vec2 temp;
    float buf;
    for (int i(0); i<header[2]; i++){
        file.read((char*)&buf, sizeof (float));
        temp.x=buf;
        file.read((char*)&buf, sizeof (float));
        temp.y=buf;
        tCoords.push_back(temp);
    }
}

void readIndices(std::ifstream& file, QVector<short>& indices, QVector<int>& header){
    short buf;
    for (int i(0); i<header[3]; i++){
        file.read((char*)&buf, sizeof (short));
        indices.push_back(buf);
    }
}

void readVertexComponents(std::ifstream& file, QVector<indices_link>& vComp, QVector<int>& header){
    short buf;
    indices_link temp;
    for (int i(0); i<header[4]; i++){
        file.read((char*)&buf, sizeof (short));
        temp.normal_ind = buf;
        file.read((char*)&buf, sizeof (short));
        temp.vertex_ind = buf;
        file.read((char*)&buf, sizeof (short));
        temp.texture_ind = buf;
        vComp.push_back(temp);
    }
}

//load morphing_vertices, indices, normals, texture coordinates
bool ei::CFigure::loadFromFile(QString pathFile){

    QVector<vec3> fCenter;
    QVector<vec3> fMin;
    QVector<vec3> fMax;
    QVector<float> fRadius;
    QVector <QVector <vec3>> pre_vertices;
    QVector<vec2> pre_tCoords;
    QVector<short> fIndices;
    QVector<indices_link> vComponents;
    qDebug() << "loading file";
    std::ifstream figFile;
    figFile.open(pathFile.toLatin1(), std::ios::binary);
    if (!figFile){
        qDebug() << "Can't load file " << pathFile;
        return false;
    }
    QString signature;



    char buffer;

    qDebug() << "file " << pathFile << " opened";
    readHeader(figFile, m_header);

    for (int i(0); i<4; i++){
        figFile >> buffer;
        //qDebug() << buffer;
        signature +=buffer;
    }
    if (signature != "FIG8"){
        qDebug() << "incorrect signature";
        figFile.close();
        return false;
    }


    //read center
    read24(figFile, fCenter);
    //read min
    read24(figFile, fMin);
    //read max
    read24(figFile, fMax);
    //read radius
    read8(figFile, fRadius);
    //read vertices
    readVertices(figFile, pre_vertices, m_header);
    //read normals
    readNormals(figFile, m_normals, m_header);
    //read texture coordinates
    readTextureCoords(figFile, pre_tCoords, m_header);
    //read indices
    readIndices(figFile, fIndices, m_header);
    //read vertex components
    readVertexComponents(figFile, vComponents, m_header);
    //TODO
    //convert vertices, normals and texture coordinates !and indices! in pure

    //     vert#1       vert#2
    // m |{x0,y0,z0}| |{x0,y0,z0}| .......
    // o |..........| |..........| .......
    // r |..........| |..........| .......
    // p |..........| |..........| .......
    // h |{x7,y7,z7}| |{x7,y7,z7}| .......

    //uv: [x0,y0,x1,y1,x2,y2]

    //   nrml#1 nrml#2
    // n |{x0}| |{x1}| ....
    // o |{y0}| |{y1}| ....
    // r |{z0}| |{z1}| ....
    // m |{w0}| |{w1}| ....



    /*
    qDebug() << vertices[1][7].x;
    qDebug() << vertices[6][0].y;
    qDebug() << vertices[6][1].y;

    qDebug() << "center";
    for (int i(0); i<center.size(); i++){
        qDebug() << center[i].x << "\t" << center[i].y << "\t" << center[i].z;
    }
    qDebug() << "min";
    for (int i(0); i<min.size(); i++){
        qDebug() << min[i].x << "\t" << min[i].y << "\t" << min[i].z;
    }
    qDebug() << "max";
    for (int i(0); i<max.size(); i++){
        qDebug() << max[i].x << "\t" << max[i].y << "\t" << max[i].z;
    }

    qDebug() << "radius";
    for (int i(0); i<radius.size(); i++){
        qDebug() << radius[i];
    }
    */

    figFile.close();
    qDebug() << "closed";
    return true;
}

//void ei::CFigure::recalcConstitution(float str, float dex, float scale){
//    //recalc morphing vertices
//    //GLfloat figVertices[fig.vertices.size()*3];
//    int size = preVertices.size();
//    for (int i(0); i<size; i++){
//        //vertices[i*3] = fig.vertices[i][0].x;
//        //figVertices[i*3+1] = fig.vertices[i][0].y;
//        //figVertices[i*3+2] = fig.vertices[i][0].z;
//        vertices.push_back(preVertices[i][0].x);
//        vertices.push_back(preVertices[i][0].y);
//        vertices.push_back(preVertices[i][0].z);
//    }
//}

//void ei::CFigure::recalcTextureCoordinates(QString type){
//    //convert texture coords via type: world\weapon\etc
//}

//void ei::CFigure::convertToGLIndices(){
//    int size = preIndices.size();
//    for (int i(0); i<size; i++){
//        indices.push_back( (unsigned int)vert_comp[preIndices[i]].vertex_ind );
//    }
//}

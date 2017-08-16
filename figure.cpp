#include "figure.h"

figure::figure()
{

}

void figure::loadFromFile(QString pathFile){
    qDebug() << "load";
    std::ifstream figFile;
    figFile.open(pathFile.toLatin1(), std::ios::binary);
    if (figFile){
        QString signature;
        unsigned int buf;
        float fbuf;
        short sbuf;
        char buffer;
        //char buffer1[5];

        qDebug() << "opened";
        //First way
        for (int i(0); i<4; i++){
            figFile >> buffer;
            //qDebug() << buffer;
            signature +=buffer;
        }
        if (signature == "FIG8"){
            qDebug()<< "correct signature, continue...";
            for (int i(0); i<9; i++){
            figFile.read((char*)&buf, sizeof (unsigned int));
            name = "qq";
            header.push_back(buf);
            }
            //loading center
            vec3 temp;
            for (int i(0); i<8; i++){
                figFile.read((char*)&fbuf, sizeof (float));
                temp.x = fbuf;
                figFile.read((char*)&fbuf, sizeof (float));
                temp.y = fbuf;
                figFile.read((char*)&fbuf, sizeof (float));
                temp.z = fbuf;
                center.push_back(temp);
            }
            //loading min
            for (int i(0); i<8; i++){
                figFile.read((char*)&fbuf, sizeof (float));
                temp.x = fbuf;
                figFile.read((char*)&fbuf, sizeof (float));
                temp.y = fbuf;
                figFile.read((char*)&fbuf, sizeof (float));
                temp.z = fbuf;
                min.push_back(temp);
            }
            //loading max
            for (int i(0); i<8; i++){
                figFile.read((char*)&fbuf, sizeof (float));
                temp.x = fbuf;
                figFile.read((char*)&fbuf, sizeof (float));
                temp.y = fbuf;
                figFile.read((char*)&fbuf, sizeof (float));
                temp.z = fbuf;
                max.push_back(temp);
            }
            //loading radius
            for (int i(0); i<8;i++){
                figFile.read((char*)&fbuf, sizeof (float));
                radius.push_back(fbuf);
            }
            //loading vertices
            temp.x=0;
            temp.y=0;
            temp.z=0;
            tvertices.clear();
            for (int i(0); i<header[0]; i++){
                //create empty q*q*vec3
                for (int i(0); i<4; i++){
                    for (int j(0); j<8; j++){
                        tvertices.push_back(temp);
                    }
                    preVertices.push_back(tvertices);
                }
                for (int morph(0); morph<8; morph++){
                    for (int block(0); block<4; block++){
                        figFile.read((char*)&fbuf, sizeof (float));
                        preVertices[i*4+block][morph].x=fbuf;
                    }
                }
                for (int morph(0); morph<8; morph++){
                    for (int block(0); block<4; block++){
                        figFile.read((char*)&fbuf, sizeof (float));
                        preVertices[i*4+block][morph].y=fbuf;
                    }
                }
                for (int morph(0); morph<8; morph++){
                    for (int block(0); block<4; block++){
                        figFile.read((char*)&fbuf, sizeof (float));
                        preVertices[i*4+block][morph].z=fbuf;
                    }
                }
            }
            //loading normals
            vec4 temp4;
            temp4.x = 1;
            temp4.y = 1;
            temp4.z = 1;
            temp4.w = 1;
            for (int i(0); i<header[1]; i++){
                for (int j(0); j<4; j++){
                    normals.push_back(temp4);
                }
                for (int j(0); j<4; j++){
                    figFile.read((char*)&fbuf, sizeof (float));
                    normals[j+i*4].x = fbuf;
                    //qDebug() << fbuf;
                }
                for (int j(0); j<4; j++){
                    figFile.read((char*)&fbuf, sizeof (float));
                    normals[j+i*4].y = fbuf;
                }
                for (int j(0); j<4; j++){
                    figFile.read((char*)&fbuf, sizeof (float));
                    normals[j+i*4].z = fbuf;
                }
                for (int j(0); j<4; j++){
                    figFile.read((char*)&fbuf, sizeof (float));
                    normals[j+i*4].w = fbuf;
                }
            }
            //loading texture coordinates
            vec2 tex_coords;
            for (int i(0); i<header[2]; i++){
                figFile.read((char*)&fbuf, sizeof (float));
                tex_coords.x=fbuf;
                figFile.read((char*)&fbuf, sizeof (float));
                tex_coords.y=fbuf;
                preT_coords.push_back(tex_coords);
            }
            //loading indices
            for (int i(0); i<header[3]; i++){
                figFile.read((char*)&sbuf, sizeof (short));
                preIndices.push_back(sbuf);
            }
            //loading vertex components
            indices_link il;
            for (int i(0); i<header[4]; i++){
                figFile.read((char*)&sbuf, sizeof (short));
                il.normal_ind = sbuf;
                figFile.read((char*)&sbuf, sizeof (short));
                il.vertex_ind = sbuf;
                figFile.read((char*)&sbuf, sizeof (short));
                il.texture_ind = sbuf;
                vert_comp.push_back(il);
            }

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
        }else{
            qDebug() << "incorrect signature";
            figFile.close();
        }
    }else{
        qDebug() << "Can't load file " << pathFile;
    }
}

void figure::recalcConstitution(float str, float dex, float scale){
    //recalc morphing vertices
    //GLfloat figVertices[fig.vertices.size()*3];
    int size = preVertices.size();
    for (int i(0); i<size; i++){
        //vertices[i*3] = fig.vertices[i][0].x;
        //figVertices[i*3+1] = fig.vertices[i][0].y;
        //figVertices[i*3+2] = fig.vertices[i][0].z;
        vertices.push_back(preVertices[i][0].x);
        vertices.push_back(preVertices[i][0].y);
        vertices.push_back(preVertices[i][0].z);
    }
}

void figure::recalcTextureCoordinates(QString type){
    //convert texture coords via type: world\weapon\etc
}

void figure::convertToGLIndices(){
    int size = preIndices.size();
    for (int i(0); i<size; i++){
        indices.push_back( (unsigned int)vert_comp[preIndices[i]].vertex_ind );
    }
}

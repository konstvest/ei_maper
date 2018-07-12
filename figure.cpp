#include "ei_object.h"

void ei::SHeader::read(QDataStream& stream)
{
  stream >> vertBlocks >> normalBlocks >> uvCount >> indexCount >> vertexComponentCount >> morphingComponentCount >> unknown >> group >> textureNumber;
}

ei::CFigure::CFigure()
  : m_indicesBuf(QOpenGLBuffer::IndexBuffer)
{
  initializeOpenGLFunctions();

  // Generate 2 VBOs
  m_verticesBuf.create();
  m_indicesBuf.create();
}

ei::CFigure::~CFigure()
{
}

bool readSignature(QDataStream& st)
{
  unsigned int sign;

  //fread(&sign, sizeof(sign), 1, file);
  st >> sign;
  return sign == 0x38474946;
}

// read xyz(3) for morph components(8)
void read24(QDataStream& st, QVector<f3>& points)
{
  f3 temp;
  for (int i(0); i<8; i++)
  {
    //fread(&temp, sizeof(temp), 1, file);
    st >> temp.x >> temp.y >> temp.z;
    points.push_back(temp);
  }
}

// read morph components(8)
void read8(QDataStream& st, QVector<float>& points)
{
  float buf;

  for (int i(0); i<8;i++)
  {
    //fread(&buf, sizeof (buf), 1, file);
    st >> buf;
    points.push_back(buf);
  }
}

/*     vertex#1           vertex#2
m |{x00,y00,z00}| |{x10,y10,z10}| .......
o |.............| |.............| .......
r |.............| |.............| .......
p |.............| |.............| .......
h |{x07,y07,z07}| |{x17,y17,z17}| .......*/
void readVertices(QDataStream& st, QVector <QVector <f3>>& morphVerts, const int blockCount)
{
  f3 pt;
  QVector<f3> vertices;
  float buf;

  vertices.fill(pt, blockCount*4);
  morphVerts.fill(vertices, 8);
  //mb change header[0] to header[morph component count]
  for (int block(0); block<blockCount; ++block)
  {
    for (int xyz(0); xyz<3; ++xyz)
    {
      for (int morph(0); morph<8; ++morph)
      {
        for (int point(0); point<4; ++point)
        {
          //fread(&buf, sizeof (buf), 1, file);
          st >> buf;
          morphVerts[morph][block*4+point][xyz]=buf;
        }
      }
    }
  }
}

//read normals
void readNormals(QDataStream& st, QVector<f4>& nrmls, const int blockCount)
{
  //     nrml#1          nrml#2
  // |{x0,y0,z0,w0}| |{x1,y1,z1,w1}| ....
  f4 pt(1.0); //xyzw
  float buf;

  nrmls.fill(pt, blockCount*4);
  for (int block(0); block<blockCount; ++block)
  {   //mb change header[0] to header[morph component count]
    for (int xyzw(0); xyzw<4; ++xyzw)
    {
      for (int point(0); point<4; ++point)
      {
        //fread(&buf, sizeof (buf), 1, file);
        st >> buf;
        nrmls[block*4+point][xyzw]=buf;
      }
    }
  }
}

//     UV#1     UV#2
// |{x0,y0}| |{x1,y1}| ....
void readTextureCoords(QDataStream& st, QVector<f2>& tCoords, const int uvCount)
{
  f2 pt;    //xy
  float buf;

  tCoords.fill(pt, uvCount);
  for (int point(0); point<uvCount; point++)
    for (int xy(0); xy<2; ++xy)
    {
      //fread(&buf, sizeof (buf), 1, file);
      st >> buf;
      tCoords[point][xy]=buf;
    }
}

void readIndices(QDataStream& st, QVector<unsigned short>& indices, const int indCount)
{
  unsigned short buf;

  for (int i(0); i<indCount; i++)
  {
    //fread(&buf, sizeof(buf), 1, file);
    st >> buf;
    indices.push_back(buf);
  }
}

void readVertexComponents(QDataStream& st, QVector<f3>& vComp, const int vcCount)
{
  short buf;
  f3 vertexComponent; // x==normal y==vertex z==texture

  for (int i(0); i<vcCount; ++i)
  {
    for (int c(0); c<3; ++c)
    {
      //fread(&buf, sizeof(buf), 1, file);
      st >> buf;
      vertexComponent[c] = buf;
    }
    vComp.push_back(vertexComponent);
  }
}

//convert indices from ei_fig format to dif arrays of vertices, normals and texture coordinates
void convertIndices(QVector<unsigned short>& inds, QVector<f3>& comps, QVector<int>& normInds, QVector<int>& vertInds, QVector<int>& uvInds)
{
  for (int i(0); i<inds.size(); ++i)
  {
    normInds.push_back(comps[inds[i]].x);
    vertInds.push_back(comps[inds[i]].y);
    uvInds.push_back(comps[inds[i]].z);
  }
}
// 1 for shop weapons
// 2 for loot, quick, quest items
// 0 for world objects
int UVConvertId(const int typeUV)
{
  switch (typeUV)
  {
    case 2: return 1;
      //case 8: return 2;
    default: return 0;
  }
}

//convert x,y uvCoords from type of object
void convertUVCoords(QVector<f2>& coordsUV, int convertCount)
{
  for (int i(0); i<convertCount; ++i)
  {
    for (auto& pt:coordsUV)
    {
      pt[0]/=2;
      pt[1]=0.5+pt[1]/2;
    }
  }
}

//load morphing_vertices, indices, normals, texture coordinates
bool ei::CFigure::readData(QDataStream& stream)
{
  ei::SHeader header;
  //QVector<unsigned short> fIndices;
  //QVector<f3> vComponents;

  //check signature
  if (!readSignature(stream)){   // 0x38474946 == FIG8
    qDebug() << "incorrect signature";
    return false;
  }

  //read header
  //readHeader(figFile, header);
  header.read(stream);
  //read center
  read24(stream, m_morphCenter);
  //read min
  read24(stream, m_morphMin);
  //read max
  read24(stream, m_morphMax);
  //read radius
  read8(stream, m_morphRadius);

  readVertices(stream, m_morphVertices, header.vertBlocks);
  m_vertices.fill(f3(), m_morphVertices[0].size());
  readNormals(stream, m_normals, header.normalBlocks);
  readTextureCoords(stream, m_uvCoords, header.uvCount);
  readIndices(stream, m_indices, header.indexCount);
  readVertexComponents(stream, m_vComponents, header.vertexComponentCount);
  //create indicies array of vertices, uv coords and normals
  //convertIndices(fIndices, vComponents, m_normIndices, m_vertIndices, m_uvIndices);
  convertUVCoords(m_uvCoords, UVConvertId(header.textureNumber));

  calculateConstitution(f3(1.0, 1.0, 1.0));

  return true;
}

void ei::CFigure::verticesDataToOpenGlBufers()
{
  SVertexData vertData;
  QVector<SVertexData> vertices;
  //todo try to refactore this code
  for (int i(0); i < m_indices.size(); ++i){
    vertData.position = q_f3(m_vertices[m_vComponents[m_indices[i]].y].x,
        m_vertices[m_vComponents[m_indices[i]].y].y,
        m_vertices[m_vComponents[m_indices[i]].y].z);
    vertData.texCoord = q_f2(m_uvCoords[m_vComponents[m_indices[i]].z].x,
        m_uvCoords[m_vComponents[m_indices[i]].z].x);
    vertices.append(vertData);
  }
  // Transfer vertex data to VBO 0
  m_verticesBuf.bind();
  m_verticesBuf.allocate(vertices.data(), vertices.size() * sizeof(SVertexData));

  for (int i(0); i < m_indices.size(); ++i)
    m_indices[i] = (unsigned short)i;
  // Transfer vertex data to VBO 1
  m_indicesBuf.bind();
  m_indicesBuf.allocate(m_indices.data(), m_indices.size() * sizeof (unsigned short));
}

void ei::CFigure::calculateConstitution(f3 constitute)
{ //x == str, y == dex, z == scale
  f3 res0, res1, res2;
  for (int i(0); i<m_morphVertices[0].size(); ++i){
    res0 = m_morphVertices[0][i] + (m_morphVertices[1][i] - m_morphVertices[0][i]) * constitute.x;
    res1 = m_morphVertices[2][i] + (m_morphVertices[3][i] - m_morphVertices[2][i]) * constitute.x;
    res2 = res0 + (res1 - res0) * constitute.y;
    res0 = m_morphVertices[4][i] + (m_morphVertices[5][i] - m_morphVertices[4][i]) * constitute.x;
    res1 = m_morphVertices[6][i] + (m_morphVertices[7][i] - m_morphVertices[6][i]) * constitute.x;
    res0 = res0 + (res1 - res0) * constitute.y;
    //need check it
    m_vertices[i] = res2 + (res0 - res2) * constitute.z;
  }
  verticesDataToOpenGlBufers();
}

QVector<f3>& ei::CFigure::vertices()
{
  return m_vertices;
}

void ei::CFigure::draw(QOpenGLShaderProgram* shaders)
{
  // Tell OpenGL which VBOs to use
  m_verticesBuf.bind();
  m_indicesBuf.bind();

  // Tell OpenGL programmable pipeline how to locate vertex position data
  int vertexLocation = shaders->attributeLocation("a_position");
  shaders->enableAttributeArray(vertexLocation);
  shaders->setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 3, sizeof(SVertexData));

  // Offset for texture coordinate after position
  quintptr offset = sizeof(QVector3D);

  // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
  int texcoordLocation = shaders->attributeLocation("a_texcoord");
  shaders->enableAttributeArray(texcoordLocation);
  shaders->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(SVertexData));

  glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_SHORT, 0);
}

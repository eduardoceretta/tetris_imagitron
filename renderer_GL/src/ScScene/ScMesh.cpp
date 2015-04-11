/**
 *	Eduardo Ceretta Dalla Favera
 *  eduardo.ceretta@gmail.com
 *  Mar 2010
 *
 *  OpenGL Mesh Attributes Container
 *  A VBO with a material and a transformation
 */

#include <iostream>
#include <limits>
#include "defines.h"

#include "ScScene/ScMesh.h"
#include "GLUtils/GLVertexBufferObject.h"
#include "MeshLoaders/MeshLoader.h"

using namespace std;

int ScMesh :: s_meshNum = 0;


ScMesh :: ScMesh()
:m_vbo(NULL)
,m_numVertices(0u)
,m_numElements(0u)
,m_fileName("")
,m_fullFilePath("")
{
  m_myMeshNum = s_meshNum++;
}

ScMesh :: ~ScMesh()
{
}


int ScMesh :: getMyMeshNumber() const
{
   return m_myMeshNum;
}

void ScMesh :: readFromStr(char buffer[])
{
   int r = sscanf( buffer, "%d %f %f %f %f %f %f %s\n", &m_materialIndex, &m_pos.x, &m_pos.y, &m_pos.z,
      &m_scale.x, &m_scale.y, &m_scale.z, buffer);
   cout << "Reading Model: " << buffer << " ...\n"<<endl;
   assert(r == 8);

   string fileName(buffer);

   int index = fileName.find_last_of(".");
   int indexSlash = fileName.find_last_of("/");
   MyAssert("Invalid FileName: " + fileName, index!=string::npos);
   string sub = fileName.substr(index, string::npos);

   m_fileName = fileName.substr(indexSlash + 1, index - indexSlash - 1);
   m_fullFilePath = fileName;

   MeshLoader m;
   if(m.isValidFileType(sub))
   {
     m.readFile(fileName, m_pos, m_scale);
     m_vbo = m.getVbo();
     
     m_numVertices = m_vbo->getNumVertexes();
     m_numElements = m_vbo->getNumElements();
   }
}


int ScMesh :: getMaterialIndex()
{
   return m_materialIndex;
}


void ScMesh :: configure()
{
}

void ScMesh :: render()
{
  if(m_vbo)
  {
    glPushMatrix();
      glTranslatef(m_pos.x, m_pos.y, m_pos.z);
      m_vbo->render();
    glPopMatrix();
  }
}

GLVertexBufferObject* ScMesh::getVbo() const
{
  return m_vbo;
}

Vector3 ScMesh::getBoundingBoxSize() const
{
  Vector3 bb_max = getBoundingBoxMax();
  Vector3 bb_min = getBoundingBoxMin();
  return bb_max - bb_min;
}

Vector3 ScMesh::getBoundingBoxCenter() const
{
  Vector3 bb_max = getBoundingBoxMax();
  Vector3 bb_min = getBoundingBoxMin();
  return (bb_max + bb_min)*.5;
}

Vector3 ScMesh::getBoundingBoxMin() const
{
  if(m_vbo)
    return  m_pos + m_vbo->getBoundingBoxMin();

  return Vector3(-numeric_limits<float>::infinity( ), -numeric_limits<float>::infinity( ), -numeric_limits<float>::infinity( ));
}

Vector3 ScMesh::getBoundingBoxMax() const
{
  if(m_vbo)
    return  m_pos + m_vbo->getBoundingBoxMax();

  return Vector3(numeric_limits<float>::infinity( ), numeric_limits<float>::infinity( ), numeric_limits<float>::infinity( ));
}

string ScMesh::getFileName() const
{
  return m_fileName;
}

string ScMesh::getFullFilePath() const
{
  return m_fullFilePath;
}

unsigned int ScMesh::getNumVertices() const
{
  return m_numVertices;
}

unsigned int ScMesh::getNumElements() const
{
  return m_numElements;
}


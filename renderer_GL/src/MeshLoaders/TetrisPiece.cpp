/**
 *	Eduardo Ceretta Dalla Favera
 *  eduardo.ceretta@gmail.com
 *  Apr 2015  
 *
 *  TetrisPiece.
 */

#include "TetrisPiece.h"
#include <assert.h>
#include <algorithm>
#include "MeshLoaders/MeshLoader.h"
#include "MeshLoaders/MeshFileBase.h"
#include "GLUtils/GLVertexBufferObject.h"

std::map<unsigned int, std::pair<TetrisPiece::PieceType, float> > TetrisPiece::s_hashmap;
std::map<TetrisPiece::PieceType, GLVertexBufferObject* > TetrisPiece::s_pieceVbomap;

TetrisPiece::TetrisPiece()
:m_outdated(true)
,m_rotAngle(0.0f)
,m_type(TetrisPiece::PieceType::O)
,m_vbo(NULL)
{
  if(s_hashmap.empty()) {
    createHashMap();
  }
}

void TetrisPiece::setModelPath(string model_path)
{
  char *buffer = new char[model_path.length() + 1];
  // Trim leading and trailing spaces
  sscanf(model_path.c_str(), " %s ", buffer);
  if(s_pieceVbomap.empty()) {
    createPieceVboMap(string(buffer));
  }
  delete[] buffer;
}


void TetrisPiece::setBlock(const Vector3 v, const unsigned int i)
{
  assert(i <= 3);
  m_blocks[i] = v;
}


void TetrisPiece::configure()
{
  if(!m_outdated) return;
  const bool **m = getOriginMatrix();
  unsigned int hash = getHash(m);

  m_type = s_hashmap[hash].first;
  m_rotAngle = s_hashmap[hash].second;
  
  m_vbo = s_pieceVbomap[m_type];

  m_pos = getBoundingBoxCenter();
  m_localDiscreteCenter = calcLocalDiscreteCenter();
  m_color = Color(float(rand()%1000)/1000,float(rand()%1000)/1000,float(rand()%1000)/1000);

  std::cout << "Position: " << m_pos << " Type: " << m_type << " RotAngle: " << m_rotAngle << endl;
  for (int i = 0; i < 4; ++i) {
    std::cout << i << ") " << m_blocks[i];
  }

  m_outdated = false;
}


const bool** TetrisPiece::getOriginMatrix() const
{
  Vector3 min_p = m_blocks[0];
  for (int i = 0; i < 4; ++i) {
    min_p.x = std::min(m_blocks[i].x, min_p.x);
    min_p.y = std::min(m_blocks[i].y, min_p.y);
  }

  bool **m = new bool*[4];
  for (int i = 0; i< 4; ++i) {
    m[i] = new bool[4];
    memset(m[i], false, 4*sizeof(bool));
  }

  for (int i = 0; i < 4; ++i) {
    Vector3 origin = m_blocks[i] - min_p;
    m[(int)origin.y][(int)origin.x] = true;
  }

  return (const bool **)m;
}

unsigned int TetrisPiece::getHash( const bool** m ) const
{
  unsigned int hash = 0;
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      if (m[i][j]) {
        hash += (1 << (i*4+j));
      }
    }
  }
  return hash;
}

Vector3 TetrisPiece::getPosition() const
{
  return m_pos;
}


Color TetrisPiece::getColor() const
{
  return m_color;
}

float TetrisPiece::getRotation() const
{
  return m_rotAngle;
}


Vector3 TetrisPiece::getLocalDiscreteCenter() const
{
  return m_localDiscreteCenter;
}

GLVertexBufferObject* TetrisPiece::getVbo() const
{
  if(m_vbo)
    return m_vbo;
  return NULL;
}


Vector3 TetrisPiece::getBoundingBoxMin() const
{
  Vector3 min_p = m_blocks[0];
  for (int i = 0; i < 4; ++i) {
    min_p.x = std::min(m_blocks[i].x, min_p.x);
    min_p.y = std::min(m_blocks[i].y, min_p.y);
  }
  min_p.z = 0.0f;
  return  min_p;
}

Vector3 TetrisPiece::getBoundingBoxMax() const
{
  Vector3 max_p = m_blocks[0];
  for (int i = 0; i < 4; ++i) {
    max_p.x = std::max(m_blocks[i].x+1, max_p.x);
    max_p.y = std::max(m_blocks[i].y+1, max_p.y);
  }
  max_p.z = 0.0f;
  return  max_p;
}

Vector3 TetrisPiece::getBoundingBoxSize() const
{
  Vector3 bb_max = getBoundingBoxMax();
  Vector3 bb_min = getBoundingBoxMin();
  return bb_max - bb_min;
}

Vector3 TetrisPiece::getBoundingBoxCenter() const
{
  Vector3 bb_max = getBoundingBoxMax();
  Vector3 bb_min = getBoundingBoxMin();
  return (bb_max + bb_min)*.5;
}


Vector3 TetrisPiece::calcLocalDiscreteCenter()
{ 
  Vector3 size = getVbo()->getBoundingBoxSize();
  return Vector3((!((int)size.x%2)) * 0.5f,(!((int)size.y%2)) * 0.5f, 0.0f);
}


void TetrisPiece::createHashMap()
{
  //Generated by perl piece_hasher.pl
  s_hashmap[4369] = std::pair<PieceType, float> (PieceType::I, 0.0f);
  s_hashmap[15]   = std::pair<PieceType, float> (PieceType::I, 90.0f);
  s_hashmap[275]  = std::pair<PieceType, float> (PieceType::L, 0.0f);
  s_hashmap[71]   = std::pair<PieceType, float> (PieceType::L, -90.0f);
  s_hashmap[802]  = std::pair<PieceType, float> (PieceType::L, 180.0f);
  s_hashmap[113]  = std::pair<PieceType, float> (PieceType::L, 90.0f);
  s_hashmap[547]  = std::pair<PieceType, float> (PieceType::J, 0.0f);
  s_hashmap[23]   = std::pair<PieceType, float> (PieceType::J, 90.0f);
  s_hashmap[785]  = std::pair<PieceType, float> (PieceType::J, 180.0f);
  s_hashmap[116]  = std::pair<PieceType, float> (PieceType::J, -90.0f);
  s_hashmap[54]   = std::pair<PieceType, float> (PieceType::Z, 0.0f);
  s_hashmap[561]  = std::pair<PieceType, float> (PieceType::Z, 90.0f);
  s_hashmap[99]   = std::pair<PieceType, float> (PieceType::S, 0.0f);
  s_hashmap[306]  = std::pair<PieceType, float> (PieceType::S, 90.0f);
  s_hashmap[114]  = std::pair<PieceType, float> (PieceType::T, 0.0f);
  s_hashmap[562]  = std::pair<PieceType, float> (PieceType::T, 90.0f);
  s_hashmap[39]   = std::pair<PieceType, float> (PieceType::T, 180.0f);
  s_hashmap[305]  = std::pair<PieceType, float> (PieceType::T, -90.0f);
  s_hashmap[51]   = std::pair<PieceType, float> (PieceType::O, 0.0f);
}

void TetrisPiece::createPieceVboMap(string model_path)
{
  MeshLoader* mesh_i = new MeshLoader();
  mesh_i->readFile(model_path + "piece_I.obj");
  s_pieceVbomap[PieceType::I] = mesh_i->getVbo();

  MeshLoader* mesh_l = new MeshLoader();
  mesh_l->readFile(model_path + "piece_L.obj");
  s_pieceVbomap[PieceType::L] = mesh_l->getVbo();

  MeshLoader* mesh_j = new MeshLoader();
  mesh_j->readFile(model_path + "piece_J.obj");
  s_pieceVbomap[PieceType::J] = mesh_j->getVbo();

  MeshLoader* mesh_s = new MeshLoader();
  mesh_s->readFile(model_path + "piece_S.obj");
  s_pieceVbomap[PieceType::S] = mesh_s->getVbo();

  MeshLoader* mesh_z = new MeshLoader();
  mesh_z->readFile(model_path + "piece_Z.obj");
  s_pieceVbomap[PieceType::Z] = mesh_z->getVbo();

  MeshLoader* mesh_t = new MeshLoader();
  mesh_t->readFile(model_path + "piece_T.obj");
  s_pieceVbomap[PieceType::T] = mesh_t->getVbo();

  MeshLoader* mesh_o = new MeshLoader();
  mesh_o->readFile(model_path + "piece_O.obj");
  s_pieceVbomap[PieceType::O] = mesh_o->getVbo();
}

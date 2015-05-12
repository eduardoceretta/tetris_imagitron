/**
 *	Eduardo Ceretta Dalla Favera
 *  eduardo.ceretta@gmail.com
 *  Mar 2015
 *
 *  TetrisImagitron File Loader. 
 *  Imports a text file that contains a list of tetris pieces
 *  and a list of blocks.
 */
#include "ScScene/SCTetrisImagitronFile.h"
#include <string>
#include <limits>
#include "defines.h"
#include "GLUtils/GLVertexBufferObject.h"

ScTetrisImagitronFile::ScTetrisImagitronFile()
:m_outdated(true)
,m_currentPieceIndex(-1)
,m_pieceAngle(0.0f)
,m_speedV(0.0, -50.5, 0.0)
,m_speedVBoost(0.0, -100.5, 0.0)
,m_pieceDiscretePos(0, 0, 0)
,m_pieceDiscreteFinalPos(0,0,0)
,m_piecePos(0,0,0)
{
}

ScTetrisImagitronFile::~ScTetrisImagitronFile(void)
{
}


void ScTetrisImagitronFile :: readFromStr(char buffer[])
{
  //////////////////////////////////////////////////////////////////////////
  // Read data
  int r = sscanf( buffer, "%f %f %f %f %f %f %s", &m_pos.x, &m_pos.y, &m_pos.z,
    &m_scale.x, &m_scale.y, &m_scale.z, buffer);
  MyAssert("----\nNot Enough Parameters in ScTetrisImagitronFile!\nMust have pos(3) scale(3) filename.\n----", r == 7);
  
  string fileName(buffer);
  readFile(fileName);
 
  m_outdated = true;
}


// [BLOCK|SOFTBLOCK|FREE] X Y
class Cel {
public:
  Vector3 pos;
  string type;
};

void ScTetrisImagitronFile::readFile(string fileName)
{
  FILE *file;
  file = fopen(fileName.c_str(), "rt");
  MyAssert("File Not Found: " + fileName, file);

  float version = -1.0f;
  string model_path;
  
  vector<Cel> cels;
  char buffer[1024];
  while(!feof(file)) {
    fscanf(file, "%s", buffer);
    if(buffer[0] == '#' || (buffer[0] == '/' && buffer[1] == '/') ) {
      /* Ignore Comment */
      fscanf(file, "%*[^\n]s");
    } else if(!strcmp(buffer, "TetrisImagitron")) {
      // Get file Version
      fscanf(file, "%f", &version);
    } else if(!strcmp(buffer, "MODEL_PATH")) {
      // Get file Version
      fscanf(file, "%[^\n]s", buffer);
      model_path = string(buffer);
    } else if(!strcmp(buffer, "PIECE")) {
      //   PIECE PART0_X PART0_Y PART1_X PART1_Y PART2_X PART2_Y PART3_X PART3_Y
      TetrisPiece p;
      p.setModelPath(model_path);
      for (int i = 0; i < 4; ++i){
        Vector3 v = Vector3(0,0,0);
        fscanf(file, "%f %f ", &(v.x), &(v.y));
        p.setBlock(v, i);
      }
      p.configure();
      m_pieces.push_back(p);
    } else if(!strcmp(buffer, "BLOCK") || !strcmp(buffer, "SOFTBLOCK") || !strcmp(buffer, "FREE")) {
      Cel c;
      c.type = string(buffer);
      fscanf(file, "%f %f ", &(c.pos.x), &(c.pos.y));
      c.pos.z = 0.0f;
      cels.push_back(c);
    } else {
      fscanf(file, "%*[^\n]s");
    }
  }
  fclose( file );

}



void ScTetrisImagitronFile :: configure()
{
  if(m_outdated == true)
  {
    m_outdated = false;
  }  
  m_frames.update();
  
  m_pieceDiscretePos = Vector3(floor(m_piecePos.x), floor(m_piecePos.y), floor(m_piecePos.z)) + Vector3(0.5f, 0.5f, 0.0f);
  float y_dist = m_pieceDiscretePos.y - m_pieceDiscreteFinalPos.y;
  if(y_dist < 1) {
    //// LOOP REVIEW
    m_currentPieceIndex = (m_currentPieceIndex + 1)%m_pieces.size();
    //// ONCE REVIEW
    //m_currentPieceIndex = std::min(m_currentPieceIndex + 1, (int)m_pieces.size());
    if (m_currentPieceIndex < m_pieces.size()) {
      const TetrisPiece &p = m_pieces.at(m_currentPieceIndex);
      Vector3 p_pos = p.getPosition();
      Vector3 p_local_center = p.getLocalDiscreteCenter();
      Vector3 rotated = p_local_center;
      rotated.rotateAxis(p.getRotation(), Vector3(0,0,-1));
      m_pieceDiscreteFinalPos = p_pos + rotated;
      m_piecePos = Vector3(m_pieceDiscreteFinalPos.x, 40.5f, 0.0f);
      m_pieceDiscretePos = Vector3(floor(m_piecePos.x), floor(m_piecePos.y), floor(m_piecePos.z)) + Vector3(0.5f, 0.5f, 0.0f);
    }
  } else {
    if(y_dist < 5)
      m_piecePos += m_speedVBoost*m_frames.getTimeSinceLastFrame();
    else
      m_piecePos += m_speedV*m_frames.getTimeSinceLastFrame();
  }

  vector<TetrisPiece> :: iterator it;
  for (it = m_pieces.begin(); it != m_pieces.end(); ++it) {
    it->configure();
  }
}

void ScTetrisImagitronFile :: render()
{
    glPushMatrix();
      glTranslatef(m_pos.x, m_pos.y, m_pos.z);
      for(int i = 0; i < m_currentPieceIndex; ++i)
        renderPieceFinalPos(i);

      if(m_currentPieceIndex < m_pieces.size())
        renderMovingPiecePos(m_currentPieceIndex);
      
      renderGrid();
    glPopMatrix();
}

void ScTetrisImagitronFile::renderMovingPiecePos(int i) const 
{
  const TetrisPiece &p = m_pieces.at(i);

  Vector3 p_local_center = p.getLocalDiscreteCenter();

  Color p_color = p.getColor();
  glColor3f(p_color.r,p_color.g,p_color.b);
  glPushMatrix();
    glTranslatef(m_pieceDiscretePos.x, m_pieceDiscretePos.y, m_pieceDiscretePos.z);
    glRotatef(p.getRotation(), 0,0,-1);
    glTranslatef(-p_local_center.x, -p_local_center.y, -p_local_center.z);
    p.getVbo()->render();
  glPopMatrix();
}

void ScTetrisImagitronFile::renderPieceFinalPos(int i) const 
{
  const TetrisPiece &p = m_pieces.at(i);

  Vector3 p_pos = p.getPosition();
  Color p_color = p.getColor();

  glColor3f(p_color.r,p_color.g,p_color.b);
  glPushMatrix();
    glTranslatef(p_pos.x, p_pos.y, p_pos.z);
    glRotatef(p.getRotation(), 0,0,-1);
    p.getVbo()->render();
  glPopMatrix();
}

void ScTetrisImagitronFile::renderGrid() const
{
  Vector3 size = getBoundingBoxSize();
  Vector3 center = getBoundingBoxCenter();

  glColor3f(1.0,0.0,0.0);
  for (int i = 1; i < size.x; i+=2) {
    glPushMatrix();
      glTranslatef(center.x - size.x/2 + i + 0.5, center.y, center.z);
      glScalef(1.0, size.y, 0.0);
      glutWireCube(1.0);
    glPopMatrix();
  }

  for (int i = 1; i < size.y; i+=2) {
    glPushMatrix();
      glTranslatef(center.x, center.y - size.y/2 + i + 0.5, center.z);
      glScalef(size.x, 1.0, 0.0);
      glutWireCube(1.0);
    glPopMatrix();
  }

  glPushMatrix();
    glTranslatef(center.x, center.y, center.z);
    glScalef(size.x, size.y, 0.0);
    glutWireCube(1.0);
  glPopMatrix();
}

Vector3 ScTetrisImagitronFile::getBoundingBoxSize() const
{
  Vector3 bb_max = getBoundingBoxMax();
  Vector3 bb_min = getBoundingBoxMin();
  return bb_max - bb_min;
}

Vector3 ScTetrisImagitronFile::getBoundingBoxCenter() const
{
  Vector3 bb_max = getBoundingBoxMax();
  Vector3 bb_min = getBoundingBoxMin();
  return (bb_max + bb_min)*.5;
}

Vector3 ScTetrisImagitronFile::getBoundingBoxMin() const
{
  Vector3 bb_min = Vector3(numeric_limits<float>::infinity( ), numeric_limits<float>::infinity( ), numeric_limits<float>::infinity( ));

  vector<TetrisPiece> :: const_iterator it;
  for (it = m_pieces.begin(); it != m_pieces.end(); ++it) {
    Vector3 piece_bb_min = it->getBoundingBoxMin();
    bb_min.x = min(piece_bb_min.x, bb_min.x);
    bb_min.y = min(piece_bb_min.y, bb_min.y);
    bb_min.z = min(piece_bb_min.z, bb_min.z);
  }
  return  m_pos + bb_min;
}

Vector3 ScTetrisImagitronFile::getBoundingBoxMax() const
{
  Vector3 bb_max = Vector3(-numeric_limits<float>::infinity( ), -numeric_limits<float>::infinity( ), -numeric_limits<float>::infinity( ));

  vector<TetrisPiece> :: const_iterator it;
  for (it = m_pieces.begin(); it != m_pieces.end(); ++it) {
    Vector3 piece_bb_max = it->getBoundingBoxMax();
    bb_max.x = max(piece_bb_max.x, bb_max.x);
    bb_max.y = max(piece_bb_max.y, bb_max.y);
    bb_max.z = max(piece_bb_max.z, bb_max.z);
  }
  // We need extra space for the pieces to start //REVIEW
  bb_max.y *= 2.0;
  return  m_pos + bb_max;
}

Vector3 ScTetrisImagitronFile::getSpeedV() const
{
  return m_speedV;
}

void ScTetrisImagitronFile::setSpeedV( Vector3 val )
{
  if(m_speedVBoost < val ){
    val = m_speedVBoost;
  }
  m_speedV = val;
}
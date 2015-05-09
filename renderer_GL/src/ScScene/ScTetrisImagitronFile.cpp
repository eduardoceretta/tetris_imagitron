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

  vector<TetrisPiece> :: iterator it;
  for (it = m_pieces.begin(); it != m_pieces.end(); ++it) {
    it->configure();
  }
}

void ScTetrisImagitronFile :: render()
{
    glPushMatrix();
      glTranslatef(m_pos.x, m_pos.y, m_pos.z);
      vector<TetrisPiece> :: iterator it;
      for (it = m_pieces.begin(); it != m_pieces.end(); ++it) {
        Vector3 p_pos = it->getPosition();
        Color p_color = it->getColor();
        glPushMatrix();
        glTranslatef(p_pos.x, p_pos.y, p_pos.z);
        glRotatef(it->getRotation(), 0,0,-1);
        glColor3f(p_color.r,p_color.g,p_color.b);
        it->getVbo()->render();
        glPopMatrix();
      }
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
    Vector3 piece_bb_min = it->getPosition() + it->getVbo()->getBoundingBoxMin();
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
    Vector3 piece_bb_max = it->getPosition() + it->getVbo()->getBoundingBoxMax();
    bb_max.x = max(piece_bb_max.x, bb_max.x);
    bb_max.y = max(piece_bb_max.y, bb_max.y);
    bb_max.z = max(piece_bb_max.z, bb_max.z);
  }
  return  m_pos + bb_max;
}
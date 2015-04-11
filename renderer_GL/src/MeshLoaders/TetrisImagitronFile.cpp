/**
 *	Eduardo Ceretta Dalla Favera
 *  eduardo.ceretta@gmail.com
 *  Mar 2015
 *
 *  TetrisImagitron File Loader. 
 *  Imports a text file that contains a list of tetris pieces
 *  and a list of blocks.
 */
#include "MeshLoaders/TetrisImagitronFile.h"
#include <string>
#include "defines.h"

TetrisImagitronFile::TetrisImagitronFile(void):MeshFileBase()
{
  m_fileType = ".tia";
}

TetrisImagitronFile::~TetrisImagitronFile(void)
{
}

void TetrisImagitronFile::readFile(string fileName, Vector3 pos /*= Vector3(0,0,0)*/, Vector3 scale /*= Vector3(1,1,1)*/ )
{
  m_vbo = NULL;
  m_pos = pos;
  m_scale = scale;
  m_fileName = fileName;
  calcTriangles();
}

void TetrisImagitronFile::calcVBO()
{
  m_vbo = new GLVertexBufferObject();
  m_vbo->setVBOBuffer( GL_VERTEX_ARRAY, GL_FLOAT, m_numVertices, m_vertices);
  m_vbo->setVBOBuffer( GL_NORMAL_ARRAY, GL_FLOAT, m_numVertices, m_normals);
  m_vbo->setVBOIndexBuffer(GL_UNSIGNED_INT, m_numTriangles*3, m_indexes);
  m_vbo->calcVBO();
  if(m_writeBinaryFile)
    writeBinaryFile(m_fileName);
}


// [BLOCK|SOFTBLOCK|FREE] X Y
class Cel {
public:
  Vector3 pos;
  string type;
};

void TetrisImagitronFile::calcTriangles()
{
  FILE *file;
  file = fopen(m_fileName.c_str(), "rt");
  MyAssert("File Not Found: " + m_fileName, file);

  float version = -1.0f; 
  
  vector<TetrisPiece> pieces;
  vector<Cel> cels;
  char buffer[1024];
  while(!feof(file)) {
    fscanf(file, "%s", buffer);
    if(buffer[0] == '#' || (buffer[0] == '/' && buffer[1] == '/') ) {
      /* Ignore Coment */
      fscanf(file, "%*[^\n]s");
    } else if(!strcmp(buffer, "TetrisImagitron")) {
      // Get file Version
      fscanf(file, "%f", &version);
    } else if(!strcmp(buffer, "PIECE")) {
      //   PIECE PART0_X PART0_Y PART1_X PART1_Y PART2_X PART2_Y PART3_X PART3_Y
      TetrisPiece p;
      for (int i = 0; i < 4; ++i){
        Vector3 v = Vector3(0,0,0);
        fscanf(file, "%f %f ", &(v.x), &(v.y));
        p.setBlock(v, i);
      }
      p.process();
      pieces.push_back(p);
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

















  //int numVertex, numTriangles;

  //fscanf(file, "%d %d\n", &numVertex, &numTriangles);
  //printf("Reading %d Vertices...\n", numVertex);

  //GLfloat * vList = new GLfloat[numVertex*3];
  //GLfloat * nList = new GLfloat[numVertex*3];
  //unsigned int * iList = new unsigned int[numTriangles*3];
  //
  //for(int i = 0; i < numVertex; ++i)
  //{
  //  fscanf(file, "%*d %f %f %f\n", &vList[i*3], &vList[i*3+1], &vList[i*3+2]); 

  //  vList[i*3]*=m_scale.x;
  //  vList[i*3+1]*=m_scale.y;
  //  vList[i*3+2]*=m_scale.z;

  //  nList[i*3] = 0;
  //  nList[i*3+1] = 0;
  //  nList[i*3+2] = 0;

  //  m_bb_min.x = min(vList[i*3]  , m_bb_min.x);
  //  m_bb_min.y = min(vList[i*3+1], m_bb_min.y);
  //  m_bb_min.z = min(vList[i*3+2], m_bb_min.z);

  //  m_bb_max.x = max(vList[i*3]  , m_bb_max.x);
  //  m_bb_max.y = max(vList[i*3+1], m_bb_max.y);
  //  m_bb_max.z = max(vList[i*3+2], m_bb_max.z);
  //}

  //printf("Reading %d Triangles...\n", numTriangles);
  //for(int i = 0; i < numTriangles; ++i)
  //{
  //  unsigned int t1, t2, t3;
  //  fscanf(file, "%*d %u %u %u\n", &t1, &t2, &t3);
  //  Vector3 v1(vList[t1*3], vList[t1*3+1], vList[t1*3+2]);
  //  Vector3 v2(vList[t2*3], vList[t2*3+1], vList[t2*3+2]);
  //  Vector3 v3(vList[t3*3], vList[t3*3+1], vList[t3*3+2]);
  //  Vector3 normal = (v2 - v1) ^ (v3 - v1);
  //  normal = normal.unitary();

  //  Vector3 nListT1(nList[t1*3], nList[t1*3+1], nList[t1*3+1]);
  //  Vector3 normalT1 = (nListT1 + normal).unitary();
  //  nList[t1*3] = normalT1.x;
  //  nList[t1*3+1] = normalT1.y;
  //  nList[t1*3+2] = normalT1.z;

  //  Vector3 nListT2(nList[t2*3], nList[t2*3+1], nList[t2*3+1]);
  //  Vector3 normalT2 = (nListT2 + normal).unitary();
  //  nList[t2*3] = normalT2.x;
  //  nList[t2*3+1] = normalT2.y;
  //  nList[t2*3+2] = normalT2.z;

  //  Vector3 nListT3(nList[t3*3], nList[t3*3+1], nList[t3*3+1]);
  //  Vector3 normalT3 = (nListT3 + normal).unitary();
  //  nList[t3*3] = normalT3.x;
  //  nList[t3*3+1] = normalT3.y;
  //  nList[t3*3+2] = normalT3.z;


  //  iList[i*3] = t1;
  //  iList[i*3+1] = t2;
  //  iList[i*3+2] = t3;
  //}

  //for(int i = 0; i < numVertex; ++i)
  //{
  //  Vector3 normal(nList[i*3], nList[i*3+1], nList[i*3+2]);
  //  if(normal == Vector3(0,0,0))
  //    normal = Vector3(0,1,0);
  //  normal = normal.unitary();

  //  nList[i*3] = normal.x;
  //  nList[i*3+1] = normal.y;
  //  nList[i*3+2] = normal.z;
  //}

  //fclose(file);

  //cout << "BoundingBox Size:" << (m_bb_max - m_bb_min) ;
  //cout << "BoundingBox Center:"<<((m_bb_max + m_bb_min)*.5) <<endl;

  //m_numVertices = numVertex;
  //m_numTriangles = numTriangles;
  //m_vertices = vList;
  //m_normals = nList;
  //m_indexes = iList;
}
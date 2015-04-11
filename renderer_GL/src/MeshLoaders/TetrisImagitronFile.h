/**
 *	Eduardo Ceretta Dalla Favera
 *  eduardo.ceretta@gmail.com
 *  Mar 2015
 *
 *  TetrisImagitron File Loader. 
 *  Imports a text file that contains a list of tetris pieces
 *  and a list of blocks.
 */
#pragma once

#include <vector>
#include <string>

#include "MeshLoaders/MeshFileBase.h"
#include "TetrisImagitron/TetrisPiece.h"

using namespace std;

class TetrisImagitronFile : public MeshFileBase
{
  vector<TetrisPiece> m_pieces;
public:
  /**
   * Simple Constructor
   */
  TetrisImagitronFile(void);
  ~TetrisImagitronFile(void);

  /**
   * Read the file of the given name and scales it's vertexes by the scale factor
   */
  virtual void readFile(string fileName, Vector3 pos = Vector3(0,0,0), Vector3 scale = Vector3(1,1,1));
 
protected:
  /**
   * Calculate the VBO with the read vertexes, normals and indexes.
   *  Writes the binary file automatically if m_writeBinaryFile is true
   */
   virtual void calcVBO();
  
   /**
   * Import information from file and organize it in the array form needed by
   *  the VBO
   */
   void calcTriangles();
};


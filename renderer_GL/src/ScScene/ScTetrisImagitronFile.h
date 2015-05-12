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

#include "MeshLoaders/TetrisPiece.h"
#include "Objects/Frames.h"

using namespace std;

class ScTetrisImagitronFile
{
  vector<TetrisPiece> m_pieces;
  Vector3 m_pos;
  Vector3 m_scale;

  Vector3 m_speedH;
  Vector3 m_speedV;
  Vector3 m_speedVBoost;

  int m_currentPieceIndex;

  Frames m_frames;

  Vector3 m_piecePos;
  Vector3 m_pieceDiscretePos;
  Vector3 m_pieceDiscreteFinalPos;
  float m_pieceAngle;

  bool m_outdated;

public:
  /**
   * Simple Constructor
   */
  ScTetrisImagitronFile(void);
  ~ScTetrisImagitronFile(void);

  /**
   * Read the attributes from a space separated string.
   *  String format: 
   *   pos.x pos.y pos.z scale.x scale.y scale.z fileName
   */
  void readFromStr(char buffer[]);

  /**
   * Get Bounding Box info
   */
  Vector3 getBoundingBoxSize() const;
  Vector3 getBoundingBoxCenter() const;
  Vector3 getBoundingBoxMin() const;
  Vector3 getBoundingBoxMax() const;

  Vector3 getSpeedV() const;
  void setSpeedV(Vector3 val);
  
  /**
   * Update Animation Attributes
   */
  void configure();
  
  /**
   * Render the transformed Animation
   */
  void render();

 
protected:
  /**
   * Read the file of the given name
   */
  void readFile(string fileName);

  /**
   * Aux Renders
   */ 
  void renderGrid() const;
  void renderPieceFinalPos(int i) const;
  void renderMovingPiecePos(int i) const;
};


/**
 *	Eduardo Ceretta Dalla Favera
 *  eduardo.ceretta@gmail.com
 *  Apr 2015  
 *
 *  TetrisPiece.
 */
#ifndef _TETRISPIECE_H_
#define _TETRISPIECE_H_

#include "MathUtils/Vector3.h"
#include <map>
#include <utility>

class TetrisPiece
{
  enum PieceType {T, I, S, Z, L, J, O};
  static std::map<unsigned int, std::pair<PieceType, float> > s_hashmap;
  
  PieceType m_type;
  Vector3 m_blocks[4];
  float m_rot_angle;

  bool m_processed;
public:
   /**
   * Simple Constructor
   */
  TetrisPiece();
   
  /**
   * Set block
   */
  void setBlock(const Vector3 v, const unsigned int i);
  
//protected:
    
  /**
   * Process Piece Type and Rotation
   */
 void process();

private:
  /**
   * Get a matrix 4x4 with true on occupied spaces
   */
  const bool** getOriginMatrix() const;
    
  /**
   * Get a hash of the piece matrix
   */
  unsigned int getHash(const bool** m) const;

  /**
   * Create a static map that will map a hash to a type and rotation
   */ 
  static void createHashMap();
};

#endif


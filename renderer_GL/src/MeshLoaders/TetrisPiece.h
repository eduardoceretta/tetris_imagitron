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
#include "MathUtils/Color.h"
#include <map>
#include <utility>

class MeshFileBase;
class GLVertexBufferObject;

class TetrisPiece
{
  enum PieceType {T, I, S, Z, L, J, O};
  static std::map<unsigned int, std::pair<PieceType, float> > s_hashmap;
  static std::map<PieceType, GLVertexBufferObject* > s_pieceVbomap;
  
  PieceType m_type;
  Vector3 m_blocks[4];
  
  float m_rotAngle;
  Vector3 m_pos;
  Color m_color;
  Vector3 m_localDiscreteCenter;

  GLVertexBufferObject *m_vbo;
  
  bool m_outdated;
public:
   /**
   * Simple Constructor
   */
  TetrisPiece();

  /**
   * Set pieces models path
   */ 
  void setModelPath(std::string model_path);
   
  /**
   * Set block
   */
  void setBlock(const Vector3 v, const unsigned int i);

  /**
   * Get properties
   */
  Vector3 getPosition() const;
  Color getColor() const;
  float getRotation() const;

  /**
   * Return the center of a piece closest to the center of the BB
   */
  Vector3 getLocalDiscreteCenter() const;

   /**
   * Get the VBO of the model
   *  If the VBO is outdated calculates it.
   */
  GLVertexBufferObject* getVbo() const;

  /**
   * Get Bounding Box info
   */
  Vector3 getBoundingBoxSize() const;
  Vector3 getBoundingBoxCenter() const;
  Vector3 getBoundingBoxMin() const;
  Vector3 getBoundingBoxMax() const;
  
//protected:
    
  /**
   * Process Piece Type and Rotation
   */
 void configure();

private:
  /**
   * Calculate piece Discrete Center in local coordinates
   */ 
  Vector3 calcLocalDiscreteCenter();


  /**
   * Get a matrix 4x4 with true on occupied spaces
   */
  const bool** getOriginMatrix() const;
    
  /**
   * Get a hash of the piece matrix. Each space in the matrix maps to a specific power of 2
   */
  unsigned int getHash(const bool** m) const;

  /**
   * Create a static map that will map a hash to a type and rotation
   */ 
  static void createHashMap();
  
  /**
   * Create a static map that will map a piece type to a Mesh
   */ 
  static void createPieceVboMap(std::string model_path);
};

#endif


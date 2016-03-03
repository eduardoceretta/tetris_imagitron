/**
 *	Eduardo Ceretta Dalla Favera
 *  eduardo.ceretta@gmail.com
 *  Mar 2010
 *
 *  OpenGL Texture Attributes Container
 */
#pragma once
#include <string>

#include <GL/glut.h>

using namespace std;

class ScTexture
{
  /**
   * Static number of texture counter.
   *  Only counts the number of created textures, do not consider deleted textures
   */
  static int s_textureNum;

protected:
  /**
   * Indicate if the texture needs to be updated
   */
  bool m_calculed;

   /**
   * Texture's Attributes
   */
  string m_fileName;
  GLenum m_minFilter, m_magFilter;
  GLenum m_wrapS, m_wrapT;
  GLuint m_textureid;


  /**
   * This instance material number
   */
  int m_myTextureNum;
public:
  /**
   * Simple Constructor
   */
  ScTexture();

  /**
   * Return the number of the texture.
   *  Start at 0.
   */
  int getMyTextureNumber() const;

  /**
   * Read the attributes from a space separated string.
   *  String format: 
   *    min_filter max_filter wrap_s wrap_t file_name
   */
  void readFromStr(char buffer[]);

  /**
   * Update Attributes if needed
   */
  void configure();

  /**
   * Enable the OpenGL texture state
   */
  void render();

};


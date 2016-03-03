/**
 *	Eduardo Ceretta Dalla Favera
 *  eduardo.ceretta@gmail.com
 *  Mar 2010
 *
 *  OpenGL Texture Attributes Container
 */
#include <iostream>
#include "assert.h"

#include "GL/glew.h"
#include "GL/glext.h"
#include "ScScene/ScTexture.h"
#include "FreeImage/FreeImage.h"

using namespace std;

int ScTexture :: s_textureNum = 0;

ScTexture :: ScTexture()
:m_calculed(false)
,m_textureid(0)
,m_magFilter(GL_NEAREST)
,m_minFilter(GL_NEAREST)
,m_wrapS(GL_REPEAT)
,m_wrapT(GL_REPEAT)
{
   m_myTextureNum = s_textureNum++;
}


int ScTexture :: getMyTextureNumber() const
{
   return m_myTextureNum;
}

void ScTexture :: readFromStr(char buffer[])
{
   char minFilter[11], magFilter[11], wrapS[16], wrapT[16];
   int r = sscanf(buffer, "%10s %10s %15s %15s %[^\n]s\n",
   minFilter, magFilter, wrapS, wrapT, buffer);
  
   cout << "Reading Texture: " << buffer << " ...\n" << endl;
   assert(r == 5);

   if (strcmp(minFilter, "NEAREST") == 0) {
     m_minFilter = GL_NEAREST;
   } else if (strcmp(minFilter, "LINEAR") == 0) {
     m_minFilter = GL_LINEAR;
   } else {
     cout << "Invalid Min Filter: " << minFilter << endl;
   }

   if (strcmp(magFilter, "NEAREST") == 0) {
     m_magFilter = GL_NEAREST;
   }
   else if (strcmp(magFilter, "LINEAR") == 0) {
     m_magFilter = GL_LINEAR;
   }
   else {
     cout << "Invalid Mag Filter: " << magFilter << endl;
   }

   if (strcmp(wrapS, "REPEAT") == 0) {
     m_wrapS = GL_REPEAT;
   }
   else if (strcmp(wrapS, "MIRRORED_REPEAT") == 0) {
     m_wrapS = GL_MIRRORED_REPEAT;
   }
   else if (strcmp(wrapS, "CLAMP_TO_EDGE") == 0) {
     m_wrapS = GL_CLAMP_TO_EDGE;
   }
   else if (strcmp(wrapS, "CLAMP_TO_BORDER") == 0) {
     m_wrapS = GL_CLAMP_TO_BORDER;
   }
   else {
     cout << "Invalid Mag Filter: " << wrapS << endl;
   }

   if (strcmp(wrapT, "REPEAT") == 0) {
     m_wrapT = GL_REPEAT;
   }
   else if (strcmp(wrapT, "MIRRORED_REPEAT") == 0) {
     m_wrapT = GL_MIRRORED_REPEAT;
   }
   else if (strcmp(wrapT, "CLAMP_TO_EDGE") == 0) {
     m_wrapT = GL_CLAMP_TO_EDGE;
   }
   else if (strcmp(wrapT, "CLAMP_TO_BORDER") == 0) {
     m_wrapT = GL_CLAMP_TO_BORDER;
   }
   else {
     cout << "Invalid Mag Filter: " << wrapT << endl;
   }

   m_fileName = string(buffer);

}

void ScTexture :: configure()
{
   if(!m_calculed)
   {
     FIBITMAP* bitmap = FreeImage_Load(
       FreeImage_GetFileType(m_fileName.c_str(), 0),
       m_fileName.c_str());
     
     glPushAttrib(GL_TEXTURE_BIT);
     glGenTextures(1, &m_textureid);

     glBindTexture(GL_TEXTURE_2D, m_textureid);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_minFilter);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_magFilter);

     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_wrapS);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_wrapT);

     glGenerateMipmapEXT(GL_TEXTURE_2D);

     bitmap = FreeImage_ConvertTo32Bits(bitmap);
     unsigned int w = FreeImage_GetWidth(bitmap);
     unsigned int h = FreeImage_GetHeight(bitmap);
     unsigned char *bits = (unsigned char*) FreeImage_GetBits(bitmap);
     GLubyte *pixels = (GLubyte*)malloc(sizeof(char)*w*h*4);

     // Convert BGR to RGB
     for (int j = 0; j<w*h; j++) {
       pixels[j * 4 + 0] = (GLubyte) bits[j * 4 + 2];
       pixels[j * 4 + 1] = (GLubyte) bits[j * 4 + 1];
       pixels[j * 4 + 2] = (GLubyte) bits[j * 4 + 0];
       pixels[j * 4 + 3] = (GLubyte) bits[j * 4 + 3];
     }


     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h,
       0, GL_RGBA, GL_UNSIGNED_BYTE, (void*) pixels);

     FreeImage_Unload(bitmap);
     free(pixels);

     glPopAttrib();

     m_calculed = true;
   }
}

void ScTexture :: render()
{
  glEnable(GL_TEXTURE_2D);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_textureid);
}

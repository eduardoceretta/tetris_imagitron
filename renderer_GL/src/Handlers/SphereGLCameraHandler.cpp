/**
 *	Eduardo Ceretta Dalla Favera
 *  eduardo.ceretta@gmail.com
 *  May 2011
 *
 *  Camera Handler. Position the camera in the hull of
 *  a sphere looking at its center.
 */
#include <iostream>
#include <assert.h>
#include <cmath>

#include "Handlers/SphereGLCameraHandler.h"
#include "GLLights/GLPointLight.h"

using namespace std;

SphereGLCameraHandler::SphereGLCameraHandler(float r /*= 100.0f*/, float a /*= 0.0f*/, float b /*= 0.0f*/, float inc /*=5.0*/)
:GLCameraHandler()
,m_r(r)
,m_alpha(a)
,m_beta(b)
,m_inc(inc)
,m_rinc(m_r*.3)
{
}

SphereGLCameraHandler::~SphereGLCameraHandler()
{
}

void SphereGLCameraHandler::listenSpecialKeyboard(int key)
{
  m_modifier = glutGetModifiers();

  if(key == GLUT_KEY_LEFT) 
    m_beta = (int)(m_beta + m_inc)%360;
  else if(key == GLUT_KEY_RIGHT)  
    m_beta = (int)(m_beta - m_inc)%360;
  else if(key == GLUT_KEY_UP) 
    m_alpha = (int)(m_alpha - m_inc)%360;
  else if(key == GLUT_KEY_DOWN)
    m_alpha = (int)(m_alpha + m_inc)%360;
  
}

void SphereGLCameraHandler::listenMouseMove(float x, float y)
{
  if(m_mouseButton == GLUT_LEFT_BUTTON && m_mouseState == GLUT_DOWN){
    float angleX = (m_lastMousePosX - x)*180.f;
    float angleY = (y - m_lastMousePosY)*180.f;

    m_alpha = ((int)(m_alpha + angleY))%360;
    m_beta = ((int)(m_beta + angleX))%360;
  }
  else if(m_mouseButton == GLUT_RIGHT_BUTTON && m_mouseState == GLUT_DOWN){
    m_r += (y - m_lastMousePosY)*m_rinc;
  }else if(m_mouseButton == GLUT_MIDDLE_BUTTON && m_mouseState == GLUT_DOWN){
    m_at += (m_up^m_pos).unitary()*(m_lastMousePosX - x)*m_rinc;
    m_at += ((m_up^m_pos)^m_pos).unitary()*(y - m_lastMousePosY)*-m_rinc;
  }
  m_lastMousePosX = x;
  m_lastMousePosY = y;
}

void SphereGLCameraHandler::listenMouseClick(int button, int state, float x, float y)
{
  m_mouseState = state;
  m_mouseButton = button;

  m_lastMousePosX = x;
  m_lastMousePosY = y;
}

void SphereGLCameraHandler::render()
{
  m_pos.x = m_r*sin(DEG_TO_RAD(m_beta))*cos(DEG_TO_RAD(m_alpha));
  m_pos.y = m_r*sin(DEG_TO_RAD(m_alpha));
  m_pos.z = m_r*cos(DEG_TO_RAD(m_beta))*cos(DEG_TO_RAD(m_alpha));

  float nextAlpha =  min(m_alpha + m_inc,360.0f);

  m_up.x = sin(DEG_TO_RAD(m_beta))*cos(DEG_TO_RAD(nextAlpha)) - m_pos.x;
  m_up.y = sin(DEG_TO_RAD(nextAlpha)) - m_pos.y;
  m_up.z = cos(DEG_TO_RAD(m_beta))*cos(DEG_TO_RAD(nextAlpha)) - m_pos.z;

  glLoadIdentity();
  gluLookAt(m_pos.x + m_at.x, m_pos.y + m_at.y, m_pos.z + m_at.z, 
            m_at.x, m_at.y, m_at.z,
            m_up.x, m_up.y, m_up.z);

  m_minerLight->setPosition(m_pos + m_at);
  renderMinerLight();
}


void SphereGLCameraHandler::setViewBoundingBox(Vector3 bb_min, Vector3 bb_max , float fovy)
{
  Vector3 bb_center =  (bb_max + bb_min)*.5;
  Vector3 bb_size = bb_max - bb_min;
  m_at = bb_center;
  float size = max(max(bb_size.x, bb_size.y), bb_size.z)/2;
  m_r = (size/tan(DEG_TO_RAD(fovy/2)) + size)*1.2f;
  m_rinc = m_r*.3;
}

Vector3 SphereGLCameraHandler::getPos() const
{
  return m_pos + m_at;
}

float SphereGLCameraHandler::getSphereAlpha() const
{
  return m_alpha;
}

float SphereGLCameraHandler::getSphereBeta() const
{
  return m_beta;
}

float SphereGLCameraHandler::getSphereRadius() const
{
  return m_r;
}

void SphereGLCameraHandler::setSphereAlpha( float alpha )
{
  m_alpha = alpha;
}

void SphereGLCameraHandler::setSphereBeta( float beta )
{
  m_beta = beta;
}

void SphereGLCameraHandler::setSphereRadius( float radius )
{
  m_r = radius;
  m_rinc = m_r*.3;
}

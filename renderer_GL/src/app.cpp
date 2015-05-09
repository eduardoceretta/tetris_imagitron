/**
 *	Eduardo Ceretta Dalla Favera
 *  eduardo.ceretta@gmail.com
 *  May 2011
 *
 *  Receive a Scene description file renders the scene.
 *
 *  Initializes the scene, process the geometry via different kernel
 *  passes and combine results to produce the final scene.
 */
#include <iostream>
#include <GL\glew.h>
#include <GL\glut.h>
#include <cmath>

#include "MathUtils/Vector3.h"
#include "MathUtils/Matrix3.h"
#include "MathUtils/Matrix4.h"

#include "GLUtils/GLError.h"
#include "GLUtils/GLFont.h"
#include "GLLights/GLPointLight.h"

#include "Objects/Frames.h"

#include "Handlers/GLCameraHandler.h"
#include "Handlers/SphereGLCameraHandler.h"

#include "ScScene/ScScene.h"
#include "ScScene/ScAnimMesh.h"

#include "Kernels/KernelColor.h"

#include "GLUtils/GLTextureObject.h"
#include "GLUtils/GLOcclusionQuery.h"

#include "app.h"

#define STR(s) #s

string App::s_renderModeStr[] = {
  STR(NoShader)
  ,STR(Debug)
};


/*******************************************/
/* App                                     */
/*******************************************/
App::App()
:m_scenePath(APP_DEFAULT_SCENE_PATH)
,m_shaderPath(APP_DEFAULT_SHADER_PATH)
,m_appWidth(APP_DEFAULT_WIDTH)
,m_appHeight(APP_DEFAULT_HEIGHT)
,m_nearPlane(APP_DEFAULT_NEAR)
,m_farPlane(APP_DEFAULT_FAR)
,m_fov(APP_DEFAULT_FOV)
,m_camHandler(NULL)
,m_customCameraIndex(0)
,m_fontRender(new GLFont())
,m_frames(new Frames())
,m_fps(0.0f)
,m_rtScene(NULL)
,m_kernelColor(NULL)
,m_menu_on(false)
,m_lights_on(false)
,m_minerLight_on(false)
,m_updateCamHandler(true)
,m_wireframe_on(false)
,m_debugrender_on(false)
,m_diffuseEnabled(false)
,m_orthographicProjection_on(false)
,m_renderMode(NoShader)
,m_occlusionQuery(NULL)
,m_occlusionQueryEnabled(false)
{
  m_clearColor[0] =  0.8f;
  m_clearColor[1] =  0.8f;
  m_clearColor[2] =  1.0f;
  m_clearColor[3] = -1.0f;

  m_ambientColor[0] = 0.2f;
  m_ambientColor[1] = 0.2f;
  m_ambientColor[2] = 0.2f;
  m_ambientColor[3] = 1.0f;

  //Initializes Arguments
  m_acceptedArgsString["-scenepath"] = &m_scenePath;
  m_acceptedArgsString["-shaderpath"] = &m_shaderPath;
}

App::~App()
{
  delete m_fontRender;
  delete m_frames;

  delete m_rtScene;

  delete m_kernelColor;
}


void App::initGL(int *argc, char *argv[])
{
  glutInit(argc, argv);
  glewInit();	
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(m_appWidth, m_appHeight);
  glutInitWindowPosition(100, 100);
  glutCreateWindow(APP_NAME);

  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, m_ambientColor);
  glClearColor(m_clearColor[0], m_clearColor[1], m_clearColor[2], m_clearColor[3]);

  glShadeModel(GL_SMOOTH);

  // FILLED 
  glPolygonMode(GL_FRONT, GL_FILL);
  // WIREFRAME Clean
  // glPolygonMode(GL_BACK, GL_LINE); 

  // WIREFRAME Dense
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 

  glCullFace(GL_BACK); 
  //Enable Culling
  //glEnable(GL_CULL_FACE);

  glEnable(GL_DEPTH_TEST);

  //Disables cursor
  //glutSetCursor(GLUT_CURSOR_NONE);

  m_shader_version_str = glGetString(GL_SHADING_LANGUAGE_VERSION);
  sscanf((char*)m_shader_version_str, "%f", &m_shader_version);
  cout << "GLSL Version: " <<  string((char*)m_shader_version_str)<<endl;

  PrintGLErrorFunction();
}

void App::loadParameters(int argc, char *argv[])
{
  loadArgs(argc, argv);
  loadSceneParameters();
}

void App::loadResources()
{
  loadScene();
  loadKernels();
  loadCameras();
  listenReshape(m_appWidth, m_appHeight);
  PrintGLErrorFunction();
}


void App::render()
{
  m_frames->update();
  m_camHandler->setMinerLightOn(false);
  m_camHandler->render();
  
  if(m_debugrender_on)
  {
    glMatrixMode (GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity ();
    gluPerspective(m_fov, (GLfloat)m_appWidth/(GLfloat)m_appHeight, .0001, 1000.);
    glMatrixMode (GL_MODELVIEW);
    glPushMatrix();

    static int ccounter = 0;
    switch(m_renderMode)
    {
    default:
    case NoShader:
      break;
    }
    glPopMatrix();
    glMatrixMode (GL_PROJECTION);
    glPopMatrix();
    glMatrixMode (GL_MODELVIEW);
  }else
  {
    switch(m_renderMode)
    {
    default:
    case NoShader:
      renderNoShader();
      break;
    }
  }

  renderGUI();
  PrintGLErrorFunction();
}


void App::listenReshape( int w, int h )
{
  //Do not resize 
  //appWidth = w;
  //appHeight = h;
  glViewport (0, 0, (GLsizei)m_appWidth, (GLsizei)m_appHeight);
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  gluPerspective(m_fov, (GLfloat)m_appWidth / (GLfloat)m_appHeight, m_nearPlane, m_farPlane);
  glMatrixMode (GL_MODELVIEW);
}

/*************************************KeyBoard Help*************************************\
        ESC   F1 F2 F3 F4 F5 F6 F7 F8 F9 F10 F11 - - - - 
        - 1 2 3 - 5 6 7 8 9 0 _ +        - Home PgUp 
        - q w e r - y u i o p - - -      - End  PgDn
        - a s d - g h j k l - - - -
        - z x c - b n m - - - - - -

  ESC: Quit
  Home:
  End:
  PgDown:
  PgUp:
  *: Reset Camera Initial Position
  +:
  -:
  0:
  1:
  2:
  3:
  5:
  6:
  7:
  8:
  9:

  F3:
  F4:
  F5:
  F6:
  F7:
  F8:
  F9:
  F10: Toggle Wire frame
  F11: Set Regular (No Shader) Render
  F1: Toggle Menu Open

  a: Print Camera Info
  b:
  c: Change Between Camera Poses
  d: Toggle Debug Render Mode
  e:
  g:
  h:
  i:
  j:
  k:
  l: Toggle Point Light
  m: Toggle Miner Light
  n:
  o: Toggle Orthographic Projection
  p:
  q:
  r: Reload All Shaders
  s: Toggle Occlusion Query
  u:
  w:
  x: Toggle Diffuse Illumination
  y:
  z:
\***************************************************************************************/

void App::listenKeyboard( int key )
{
  int modifier = glutGetModifiers();

  switch(key)
  {
  case 27://ESC
    exit(42);
    break;

  case 'A':
  case 'a':
    {
      Vector3 pos = m_camHandler->getPos();
      Vector3 at = m_camHandler->getAt();
      Vector3 up = m_camHandler->getUp();
      cout << "CAMERA " << pos.x << " " << pos.y << " " << pos.z << "   " 
                       << at.x << " " << at.y << " " << at.z << "   " 
                       << up.x << " " << up.y << " " << up.z << "   " 
                       << m_fov << " " << m_nearPlane  << " " << m_farPlane << endl << endl;
    }
    break;


  case 'S':
  case 's':
    m_occlusionQueryEnabled = !m_occlusionQueryEnabled;
    break;

  case 'C':
    m_customCameraIndex = (m_customCameraIndex - 1 >= 0 ? m_customCameraIndex : m_rtScene->getNumCameras() - 1) - 1;
    m_camHandler = m_kernelsCamHandleres[CustomCameras + m_customCameraIndex];
    break;
  case 'c':
    m_customCameraIndex = (m_customCameraIndex + 1) % (m_rtScene->getNumCameras() - 1);
    m_camHandler = m_kernelsCamHandleres[CustomCameras + m_customCameraIndex];
    break;

  case 'D':
  case 'd':
    m_debugrender_on = !m_debugrender_on;
    m_camHandler = m_kernelsCamHandleres[(m_debugrender_on?Debug:0) + m_renderMode];
    break;

  case 'Q':
  case 'q':
    break;

  case 'W':
  case 'w':
    break;

  case 'E':
  case 'e':
    break;

  case 'M':
  case 'm':
    m_minerLight_on = !m_minerLight_on;
    break;

  case 'N':
  case 'n':
    break;

  case 'L':
  case 'l':
    m_lights_on = !m_lights_on;
    break;

  case 'B':
  case 'b':
    break;

  case 'Y':
  case 'y':
    break;

  case 'Z':
  case 'z':
    break;

  case 'X':
  case 'x':
    m_diffuseEnabled = !m_diffuseEnabled;
    break;

  case 'O':
  case 'o':
    m_orthographicProjection_on = !m_orthographicProjection_on;
    break;

  case '=':
  case '+':
    break;

  case '_':
  case '-':
    break;

  case '*':
    {
      SphereGLCameraHandler* p = dynamic_cast<SphereGLCameraHandler*>(m_camHandler);
      if(p)
      {
        p->setSphereAlpha(0.0);
        p->setSphereBeta(90.0);
      }
    }
    break;

  case 'K':
  case 'k':
    break;

  case 'J':
  case 'j':
    break;

  case 'R':
  case 'r':
    m_kernelColor->reloadShader();
    break;

  case 'I':
  case 'i':
    break;

  case 'U':
  case 'u':
    break;

  case 'H':
  case 'h':
    break;

  case 'G':
  case 'g':
    break;

  case '6':
  case 168://'¨'
    break;

  case '5':
  case '%':
    break;

  case '8':
    break;

  case '7':
    break;

  case '(':
  case '9':
    break;


  case ')':
  case '0':
    break;

  case 'P':
  case 'p':
    break;
  
  
  case '1':
  case '!':
    break;

  case '2':
  case '@':
    break;

  case '3':
  case '#':
    break;
  }
}

void App::listenKeyboardSpecial( int key )
{
  m_camHandler->listenSpecialKeyboard(key);
  int modifier = glutGetModifiers();

  switch(key)
  {
  case 1://F1
    m_menu_on = !m_menu_on;
    break;

  case 3: //F3
    break;

  case 4: //F4
    break;
  
  case 5: //F5
    break;

  case 6: //F6
    break;

  case 7: //F7
    break;

  case 8: //F8
    break;

  case 9: //F9
    break;

  case 10: //F10
    m_wireframe_on = !m_wireframe_on;

    if(m_wireframe_on)
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    break;

  case 11: //F11
    m_renderMode = NoShader;
    m_camHandler = m_kernelsCamHandleres[(m_debugrender_on?Debug:0) + m_renderMode];
    break;

  case GLUT_KEY_PAGE_UP: //PgUp
    break;

  case GLUT_KEY_PAGE_DOWN: //PgDown
    break;

  case GLUT_KEY_HOME: //Home
    //if(modifier == GLUT_ACTIVE_SHIFT)
    break;

  case GLUT_KEY_END: //END
    break;
  }
}

void App::listenMouseMove( int x, int y )
{
  m_camHandler->listenMouseMove((float)x/m_appWidth, (float)y/m_appHeight);
}

void App::listenMouseClick( int button, int state, int x, int y )
{
  m_camHandler->listenMouseClick(button, state, (float)x/m_appWidth, (float)y/m_appHeight);
}

void App::loadArgs(int argc, char *argv[])
{
  map<string, string*> :: iterator strIt;
  map<string, int*> :: iterator intIt;
  map<string, float*> :: iterator floatIt;
  map<string, bool*> :: iterator boolIt;

  for(int i = 1; i < argc; ++i)
  {
    if(argv[i][0] != '-')
      continue;

    string arg = string(argv[i]);

    strIt = m_acceptedArgsString.find(arg);
    if(strIt != m_acceptedArgsString.end())
    {
      if(argc > i + 1 && argv[i + 1][0] != '-') 
        *(strIt->second) = string(argv[i + 1]);
      else cout << "Argument passed to "<< arg << " is invalid!" <<endl;
    }

    floatIt = m_acceptedArgsFloat.find(arg);
    if(floatIt != m_acceptedArgsFloat.end())
    {
      if(argc > i + 1 && argv[i + 1][0] != '-') 
        *(floatIt->second) = atof(argv[i + 1]);
      else cout << "Argument passed to "<< arg << " is invalid!" <<endl;
    }

    intIt = m_acceptedArgsInt.find(arg);
    if(intIt != m_acceptedArgsInt.end())
    {
      if(argc > i + 1 && argv[i + 1][0] != '-') 
        *(intIt->second) = atoi(argv[i + 1]);
      else cout << "Argument passed to "<< arg << " is invalid!" <<endl;
    }

    boolIt = m_acceptedArgsBool.find(arg);
    if(boolIt != m_acceptedArgsBool.end())
    {
      if(argc > i + 1 && argv[i + 1][0] != '-') 
        *(boolIt->second) = !strcmp(argv[i + 1], "True") || !strcmp(argv[i + 1], "true");
      else cout << "Argument passed to "<< arg << " is invalid!" <<endl;
    }
  }
}
void App::loadSceneParameters()
{
  m_rtScene = new ScScene();
  m_rtScene->readSceneParameters(m_scenePath);

  m_appWidth = m_rtScene->getScreenWidth();
  m_appHeight = m_rtScene->getScreenHeight();

  Color ambientColor = m_rtScene->getAmbientColor();
  m_ambientColor[0] = ambientColor.r;
  m_ambientColor[1] = ambientColor.g;
  m_ambientColor[2] = ambientColor.b;

  Color sceneClearColor = m_rtScene->getClearColor();
  m_clearColor[0] = sceneClearColor.r;
  m_clearColor[1] = sceneClearColor.g;
  m_clearColor[2] = sceneClearColor.b;
}

void App::loadScene()
{
  if(!m_rtScene)
    m_rtScene = new ScScene();
  m_rtScene->readSceneObjects(m_scenePath);

  int numMeshes = m_rtScene->getNumMeshes() + m_rtScene->getNumAnimatedMeshes() + m_rtScene->getNumTetrisImagitronFiles();
  if(numMeshes == 0)
    cout << "No Mesh Loaded!!" <<endl;

  cout << "==================================="<<endl;
  cout << "Scene Number of Vertices: " << m_rtScene->getNumVertices() <<endl;
  cout << "Scene Number of Triangles: " << m_rtScene->getNumElements()<<endl;
  cout << "==================================="<<endl;
  PrintGLErrorFunction();
}

void App::loadKernels()
{
  m_kernelColor = new KernelColor(m_appWidth, m_appHeight);

  m_occlusionQuery = new GLOcclusionQuery();
  m_occlusionQuery->init();
  PrintGLErrorFunction();
}

void App::loadCameras()
{
  Vector3 bbSize = m_rtScene->getSceneBoundingBoxSize();
  float bbMaxSize = max(bbSize.x, max(bbSize.y, bbSize.z));

  m_fov = m_rtScene->getCamera()->getFovy();

  SphereGLCameraHandler *cam3D = new SphereGLCameraHandler(10.f, 0.f, 0.0f, 2.0f);
  cam3D->setViewBoundingBox(m_rtScene->getSceneBoundingBoxMin(), m_rtScene->getSceneBoundingBoxMax(),  m_fov);
  
  m_kernelsCamHandleres.push_back(cam3D); //NoShader
  //Debug
  m_kernelsCamHandleres.push_back(cam3D); //NoShader

  for(int i = 1; i < m_rtScene->getNumCameras(); ++i)
  {
    ScCamera* scCam = m_rtScene->getCameraAt(i);
    GLCameraHandler *custom = new GLCameraHandler(scCam->getPos(), scCam->getAt(), scCam->getUp());
    m_kernelsCamHandleres.push_back(custom);
  }

  m_camHandler = cam3D;

  SphereGLCameraHandler* p = dynamic_cast<SphereGLCameraHandler*>(m_camHandler);
  if(p)
  {
    m_nearPlane = p->getSphereRadius()*.05f; 
    m_farPlane = p->getSphereRadius() + bbMaxSize * 1.5f;

    //m_nearPlane = p->getSphereRadius()*.85; 
    //m_farPlane =  p->getSphereRadius()*1.15; 

    //m_nearPlane = p->getSphereRadius()*1.0; 
    //m_farPlane =  p->getSphereRadius()*1.1; 

    //m_nearPlane = p->getSphereRadius()*1.0f; //.1
    //m_farPlane = p->getSphereRadius()*.8 + bbMaxSize * .01f; //1.5

    //m_nearPlane = p->getSphereRadius();
    //m_farPlane = p->getSphereRadius() + bbMaxSize;
  }else
  {
    m_nearPlane = .5;
    m_farPlane  = 10000;
  }


  GLLight *minerLight = m_camHandler->getMinerLight();
  minerLight->setAmbientColor(Color(0.0, 0.0, 0.0));
  minerLight->setDiffuseColor(Color(0.8, 0.8, 0.8));
  minerLight->setSpecularColor(Color(1.0, 1.0, 1.0));
  minerLight->setPosition(Vector3(0, 100, 0));
  PrintGLErrorFunction();
}

void App::drawScene()
{
  glPushAttrib(GL_CURRENT_BIT|GL_LIGHTING_BIT);
  m_camHandler->setMinerLightOn(m_minerLight_on);
  m_camHandler->renderMinerLight();

  m_rtScene->setSceneLightEnabled(m_lights_on);
  m_rtScene->configure();
  m_rtScene->render();
  glPopAttrib();
}

void App::renderGUI()
{
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  m_fontRender->setSize(GLFont::Medium);
  m_fontRender->initText(m_appWidth, m_appHeight);

  char a[100];
  int yRight_i = 0;
  int yLeft_i = 0;
  float xTop = .65;
  float yRight = .03;
  
  ///Top Right//////////////////////////////
  //                                      //
  //////////////////////////////////////////
  sprintf(a,"(F1) %s Menu", m_menu_on?"Close":"Open");
  m_fontRender->print(m_appWidth*xTop*.95f, m_appHeight*yRight + 25*yRight_i++,a, Color(0., 0., 0.));

  if(m_menu_on)
  {
    sprintf(a,"(O) Proj: %s", m_orthographicProjection_on? "Orthographic":"Perspective");
    m_fontRender->print(m_appWidth*xTop, m_appHeight*yRight + 25*yRight_i++,a, Color(0., 0., 0.));
  }

  switch(m_renderMode)
  {
    default:
    case NoShader:
      if(m_menu_on)
      {
        sprintf(a,"(L) Lights %s", m_lights_on? "On":"Off");
        m_fontRender->print(m_appWidth*xTop, m_appHeight*yRight + 25*yRight_i++,a, Color(0., 0., 0.));

        sprintf(a,"(M) Mine Light: %s", m_minerLight_on? "On":"Off");
        m_fontRender->print(m_appWidth*xTop, m_appHeight*yRight + 25*yRight_i++,a, Color(0., 0., 0.));
      }
      sprintf(a,"%s", STR(NoShader));
      break;
  }

  ///Top Left///////////////////////////////
  //                                      //
  //////////////////////////////////////////
  m_fontRender->print(m_appWidth*.03,m_appHeight*.03 + 25*yLeft_i++, s_renderModeStr[m_renderMode].c_str(), Color(0., 0., 0.));

  if(m_wireframe_on)
  {
    sprintf(a,"(F10) WireFrame On");
    m_fontRender->print(m_appWidth*.03,m_appHeight*.03 + + 25*yLeft_i++, a, Color(0., 0., 0.));
  }

  if(m_debugrender_on)
  {
    sprintf(a,"(D) Render Debug");
    m_fontRender->print(m_appWidth*.03,m_appHeight*.03 + 25*yLeft_i++, a, Color(0., 0., 0.));
  }

  //////////////////////////////////////////
  //                                      //
  ////////////////////////////Bottom Right//
  sprintf(a,"%.2f FPS", m_frames->getFPS());
  m_fontRender->print(m_appWidth*.85,m_appHeight*.85 + 55,a, Color(0., 0., 0.));

  m_fontRender->endText();
  glPopAttrib();
  PrintGLErrorFunction();
}

void App::renderNoShader()
{
  if(m_orthographicProjection_on)
  {
    glMatrixMode (GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity ();
    Vector3 size = m_rtScene->getSceneBoundingBoxSize();
    float orthoSize = max(max(size.x, size.y), size.z);
    glOrtho(-orthoSize, orthoSize, -orthoSize*m_appHeight/m_appWidth, orthoSize*m_appHeight/m_appWidth, m_nearPlane, m_farPlane);

    glMatrixMode (GL_MODELVIEW);
    glPushMatrix();
  }
  drawScene();
  if(m_orthographicProjection_on)
  {
    glPopMatrix();
    glMatrixMode (GL_PROJECTION);
    glPopMatrix();
    glMatrixMode (GL_MODELVIEW);
  }
  PrintGLErrorFunction();
}

/**
 *	Eduardo Ceretta Dalla Favera
 *  eduardo.ceretta@gmail.com
 *  May 2011
 *
 *  Receive a Scene description file renders the scene with ambient occlusion.
 *
 *  Initializes the scene, process the geometry via different kernel
 *  passes and combine results to produce the final scene with the
 *  ambient occlusion.
 */
#ifndef _APP_H_
#define _APP_H_

#include <string>
#include <vector>
#include <map>

/**
 * Application Initial Definitions
 */
#define APP_NAME "TetrisImagitron::Animator_GL"

#define APP_DEFAULT_WIDTH 800
#define APP_DEFAULT_HEIGHT 600
#define APP_DEFAULT_FOV 60.0f
#define APP_DEFAULT_NEAR 5.0f
#define APP_DEFAULT_FAR 1000.0f
#define APP_DEFAULT_SCENE_PATH "./resources/Scenes/scene.rt4"
#define APP_DEFAULT_SHADER_PATH "./resources/Shaders/"

/**
 * Forward Class Declaration
 */
class ScScene;

class GLCameraHandler;
class GLOcclusionQuery;
class GLFont;
class Frames;

class GLProjectionMatrix;

class KernelColor;

using namespace std;

class App 
{
  map<string, string*> m_acceptedArgsString;
  map<string, int*> m_acceptedArgsInt;
  map<string, float*> m_acceptedArgsFloat;
  map<string, bool*> m_acceptedArgsBool;

  /**
   * App Resources Paths
   */
  string m_scenePath;
  string m_shaderPath;  

  /**
   * Application Parameters
   */
  int m_appWidth;
  int m_appHeight;
  float m_nearPlane;
  float m_farPlane;
  float m_fov;
  GLfloat m_clearColor[4];
  GLfloat m_ambientColor[4];
  const GLubyte* m_shader_version_str;
  float m_shader_version;

  /**
   * Render Objects
   */
  GLCameraHandler *m_camHandler;
  vector<GLCameraHandler*> m_kernelsCamHandleres;
  int m_customCameraIndex;

  GLFont *m_fontRender;
  Frames *m_frames;
  float m_fps;


  /**
   * Scene Objects
   */
  ScScene* m_rtScene;

  /**
   * Kernel Objects
   */
  KernelColor* m_kernelColor;

  /**
   * Interface control
   */
  bool m_menu_on;
  bool m_lights_on;
  bool m_minerLight_on;
  bool m_wireframe_on;
  bool m_updateCamHandler;
  bool m_orthographicProjection_on;
  bool m_debugrender_on;

  bool m_diffuseEnabled;
  
  /**
   * Render Mode
   */
  enum RenderMode{NoShader = 0
                 ,Debug
                 ,CustomCameras = 2*Debug};

  static string s_renderModeStr[];

  RenderMode m_renderMode;

  /**
   * Debug
   */
  GLenum e;
  GLOcclusionQuery * m_occlusionQuery;
  bool m_occlusionQueryEnabled;

public:
  App();
  ~App();
  
  void initGL(int *argc, char *argv[]);
    
  /**
   * Load scene and application parameters (win size, clear color...)
   */
  void loadParameters(int argc, char *argv[]);
  
  /**
   * Load and Initializes the Application Resources
   */
  void loadResources();

  /**
   * Draw the scene
   */
  void render();

  /**
   * Manages a window reshape event
   */
  void listenReshape(int w, int h);

  /**
   * Manages a simple keyboard event
   */
  void listenKeyboard(int key);

  /**
   * Manages a special keyboard event
   */
  void listenKeyboardSpecial( int key );

  /**
   * Manages a mouse movement event
   */
  void listenMouseMove(int x, int y);

  /**
   * Manages a mouse button press event
   */
  void listenMouseClick(int button, int state, int x, int y);

private:
  /**
   * Process the application arguments
   */
  void loadArgs(int argc, char *argv[]);
  
  /**
   * Load and Initializes the Scene objects to be rendered
   */
  void loadScene();
  
  /**
   * Load and Initializes the Scene parameters (win size, clear color...)
   */
  void loadSceneParameters();
  
  /**
   * Initializes the kernels that will process the scene
   */
  void loadKernels();

  /**
   * Initializes the cameras that will display the scene
   */
  void loadCameras();

  /**
   * Draw the Scene
   */
  void drawScene();
 
  /**
   * Draw the Graphical User Interface
   */
  void renderGUI();

  /**
   * Voxelize the Scene
   */
  void voxelize();
  
  /**
   * Draw Kernels
   */
  void renderNoShader();
};

#endif
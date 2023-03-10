 #include <algorithm>
#include <memory>
#include <vector>
#include <stdio.h>
#include <GLFW/glfw3.h>
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#undef GLAD_GL_IMPLEMENTATION
#include <glm/glm.hpp>
#include <math.h>
#include "camera.h"
#include "opengl_context.h"
#include "utils.h"

#define ANGEL_TO_RADIAN(x) (float)((x)*M_PI / 180.0f)
#define RADIAN_TO_ANGEL(x) (float)((x)*180.0f / M_PI)

#define CIRCLE_SEGMENT 64

/* Components size definition */
#define ARM_LEN 1.0f
#define ARM_RADIUS 0.05f
#define ARM_DIAMETER (ARM_RADIUS * 2)
#define JOINT_RADIUS 0.05f
#define JOINT_DIAMETER (JOINT_RADIUS * 2)
#define JOINT_WIDTH 0.1f
#define BASE_RADIUS 0.5f
#define BASEE_DIAMETER (BASE_RADIUS * 2)
#define BASE_HEIGHT 0.1f
#define TARGET_RADIUS 0.05f
#define TARGET_DIAMETER (TARGET_RADIUS * 2)
#define TARGET_HEIGHT 0.1f
#define ROTATE_SPEED 1.0f 
#define CATCH_POSITION_OFFSET 0.05f
#define TOLERANCE 0.1f

#define RED 0.905f, 0.298f, 0.235f
#define BLUE 0.203f, 0.596f, 0.858f
#define GREEN 0.18f, 0.8f, 0.443f

bool press = false;
bool catcht = false;
float joint0_speed = 0;
float joint1_speed = 0;
float joint2_speed = 0;
float joint0_degree = 0;
float joint1_degree = 0;
float joint2_degree = 0;
glm::vec3 target_pos(1.0f, 0.05f, 1.0f);
 
void resizeCallback(GLFWwindow* window, int width, int height) {
  OpenGLContext::framebufferResizeCallback(window, width, height);
  auto ptr = static_cast<Camera*>(glfwGetWindowUserPointer(window));
  if (ptr) {
    ptr->updateProjectionMatrix(OpenGLContext::getAspectRatio());
  }
}

void keyCallback(GLFWwindow* window, int key, int, int action, int) {
  // There are three actions: press, release, hold(repeat)
  if (action == GLFW_REPEAT) return;
  // Press ESC to close the window.
  if (key == GLFW_KEY_ESCAPE) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
    return;
  }
  /* TODO#4-1: Detect key-events, perform rotation or catch target object
   *       1. Use switch/case to find the key you want.
   *       2. Define and modify some global variable to trigger update in rendering loop
   * Hint:
   *       glfw3's key list (https://www.glfw.org/docs/3.3/group__keys.html)
   *       glfw3's action codes (https://www.glfw.org/docs/3.3/group__input.html#gada11d965c4da13090ad336e030e4d11f)
   * Note:
   *       You should finish your robotic arm first.
   *       Otherwise you will spend a lot of time debugging this with a black screen.
   */
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS ) {
        joint0_speed = -1;
    }
    else if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS )  {
        joint0_speed = 1;
    }
    else if( glfwGetKey(window, GLFW_KEY_J) == GLFW_RELEASE || glfwGetKey(window, GLFW_KEY_U) ==  GLFW_RELEASE ) {
        joint0_speed = 0;
    }
    
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS )  {
        joint1_speed = -1;
    }
    else if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS )  {
        joint1_speed = 1;
    }
    else if( glfwGetKey(window, GLFW_KEY_K) == GLFW_RELEASE || glfwGetKey(window, GLFW_KEY_I) ==  GLFW_RELEASE ) {
        joint1_speed = 0;
    }
    
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS )  {
        joint2_speed = 1;
    }
    else if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS )  {
        joint2_speed = -1;
    }
    else if( glfwGetKey(window, GLFW_KEY_O) == GLFW_RELEASE || glfwGetKey(window, GLFW_KEY_L) ==  GLFW_RELEASE ) {
        joint2_speed = 0;
    }
    
    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE ) {
        press = false;
        catcht = false;
    }
    else  if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS ) {
        press = true;
    }
}

void initOpenGL() {
  // Initialize OpenGL context, details are wrapped in class.
#ifdef __APPLE__
  // MacOS need explicit request legacy support
  OpenGLContext::createContext(21, GLFW_OPENGL_ANY_PROFILE);
#else
  OpenGLContext::createContext(21, GLFW_OPENGL_ANY_PROFILE);
//  OpenGLContext::createContext(43, GLFW_OPENGL_COMPAT_PROFILE);
#endif
  GLFWwindow* window = OpenGLContext::getWindow();
  /* TODO#0: Change window title to "HW1 - `your student id`"
   *         Ex. HW1 - 311550000 
   */
  glfwSetWindowTitle(window, "HW1 - 311553032");
  glfwSetKeyCallback(window, keyCallback);
  glfwSetFramebufferSizeCallback(window, resizeCallback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
#ifndef NDEBUG
  OpenGLContext::printSystemInfo();
  // This is useful if you want to debug your OpenGL API calls.
  OpenGLContext::enableDebugCallback();
#endif
}

void drawUnitCylinder() {
  /* TODO#2-1: Render a unit cylinder
   * Hint:
   *       glBegin/glEnd (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glBegin.xml)
   *       glColor3f (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glBegin.xml)
   *       glVertex3f (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glVertex.xml)
   *       glNormal (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glNormal.xml)
   *       glScalef (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glScale.xml)
   * Note:
   *       You can refer to ppt "Draw Cylinder" page and `CIRCLE_SEGMENT`
   *       You should set normal for lighting
   */
   GLfloat x   = 0.0;
   GLfloat y   = 0.0;
   GLfloat pi = 3.1415927;
   GLfloat angle  = 0.0;
   GLfloat sangle = 2*pi/CIRCLE_SEGMENT;
   glNormal3f(0.0f, -1.0f, 0.0f);
   glColor3f(RED);
   glBegin(GL_TRIANGLE_FAN); // draw bottle circle
   while( angle < 2*pi ) {
        x = TARGET_RADIUS * cos(angle);
        y = TARGET_RADIUS * sin(angle);
        glVertex3f( x, 0 ,  y );
        angle = angle + sangle;
   } // while
   glVertex3f(0 , 0 , 0 );
   glEnd();
    
   glBegin(GL_QUAD_STRIP); // draw Cylinder
   angle = 0.0;
   while( angle < 2*pi ) {
        x = TARGET_RADIUS * sin(angle);
        y = TARGET_RADIUS * cos(angle);
        glNormal3f( sin(angle), 0.0f, cos(angle));
        glVertex3f( x, TARGET_HEIGHT ,  y );
        glVertex3f( x, 0 ,  y );
        angle = angle + sangle;
   }
   glVertex3f( TARGET_RADIUS, TARGET_HEIGHT, 0);
   glVertex3f( TARGET_RADIUS, 0, 0);
   glEnd();
   
   angle = 0.0;
   glNormal3f(0.0f, 1.0f, 0.0f);
   glBegin(GL_TRIANGLE_FAN); // draw head circle
   while( angle < 2*pi ) {
         x = TARGET_RADIUS * sin(angle);
         y = TARGET_RADIUS * cos(angle);
         glVertex3f( x, TARGET_HEIGHT, y);
         angle = angle + sangle;
   } // while
   glVertex3f(0, TARGET_HEIGHT, 0);
   glEnd();
}

void drawBase() {
    GLfloat x   = 0.0;
    GLfloat y   = 0.0;
    GLfloat pi = 3.1415927;
    GLfloat angle  = 0.0;
    GLfloat sangle = 2*pi/CIRCLE_SEGMENT;
    glColor3f(GREEN);
    glBegin(GL_TRIANGLE_FAN); // draw bottle circle
    glNormal3f(0.0f, -1.0f, 0.0f);
    while( angle < 2*pi ) {
         x = BASE_RADIUS * cos(angle);
         y = BASE_RADIUS * sin(angle);
         glVertex3f( x, 0, y);
         angle = angle + sangle;
    } // while
    glVertex3f(0, 0, 0);
    glEnd();

    glBegin(GL_QUAD_STRIP); // draw Cylinder
    angle = 0.0;
    while( angle < 2*pi ) {
         x = BASE_RADIUS * sin(angle);
         y = BASE_RADIUS * cos(angle);
         glNormal3f(sin(angle), 0.0f, cos(angle));
         glVertex3f( x, BASE_HEIGHT ,  y );
         glVertex3f( x, 0 ,  y );
         angle = angle + sangle;
    }
    glVertex3f( BASE_RADIUS, BASE_HEIGHT, 0);
    glVertex3f(BASE_RADIUS, 0, 0);
    glEnd();
    
    angle = 0.0;
    glBegin(GL_TRIANGLE_FAN); // draw head circle
    glNormal3f(0.0f, 1.0f, 0.0f);
    while( angle < 2*pi ) {
          x = BASE_RADIUS * sin(angle);
          y = BASE_RADIUS * cos(angle);
          glVertex3f( x, BASE_HEIGHT, y);
          angle = angle + sangle;
    } // while
    glVertex3f(0, BASE_HEIGHT, 0);
    glEnd();
} // drawBAse

void drawArm() { // nead to rotation len
    GLfloat x   = 0.0;
    GLfloat y   = 0.0;
    GLfloat pi = 3.1415927;
    GLfloat angle  = 0.0;
    GLfloat sangle = 2*pi/CIRCLE_SEGMENT;
    glColor3f(BLUE);
    glBegin(GL_TRIANGLE_FAN); // draw bottle circle
    glNormal3f(0.0f, 1.0f, 0.0f);
    while( angle < 2*pi ) {
         x = ARM_RADIUS * cos(angle);
         y = ARM_RADIUS * sin(angle);
         glVertex3f( x, 0 ,  y );
         angle = angle + sangle;
    } // while
    glVertex3f(ARM_RADIUS ,0, 0 );
    glEnd();

    glBegin(GL_QUAD_STRIP); // draw Cylinder
    angle = 0.0;
    while( angle < 2*pi ) {
         x = ARM_RADIUS * sin(angle);
         y = ARM_RADIUS * cos(angle);
         glNormal3f(sin(angle), 0.0f, cos(angle));
         glVertex3f( x, ARM_LEN ,  y );
         glVertex3f(x, 0,  y );
         angle = angle + sangle;
    }
    glVertex3f( ARM_RADIUS, ARM_LEN, 0);
    glVertex3f( ARM_RADIUS, 0, 0);
    glEnd();
    
    angle = 0.0;
    glBegin(GL_TRIANGLE_FAN); // draw head circle
    glNormal3f(0.0f, -1.0f, 0.0f);
    while( angle < 2*pi ) {
          x = ARM_RADIUS * sin(angle);
          y = ARM_RADIUS * cos(angle);
          glVertex3f( x, ARM_LEN, y);
          angle = angle + sangle;
    } // while
    glVertex3f(ARM_RADIUS, ARM_LEN, 0);
    glEnd();
} // drawArm

void drawJoin(  ){
    /*glm::vec3 target_pos1;
    target_pos1[0] = target_pos2[0];*/
    
    GLfloat x   = 0.0;
    GLfloat y   = 0.0;
    GLfloat pi = 3.1415927;
    GLfloat angle  = 0.0;
    GLfloat sangle = 2*pi/CIRCLE_SEGMENT;
    glColor3f(GREEN);
    glBegin(GL_TRIANGLE_FAN); // draw bottle circle
    glNormal3f(0.0f, 0.0f, -1.0f);
    while( angle < 2*pi ) {
         x = JOINT_RADIUS * sin(angle);
         y = JOINT_RADIUS * cos(angle);
         glVertex3f( x, y, -JOINT_WIDTH/2 );
         angle = angle + sangle;
    } // while
    glVertex3f(0 ,  0 , -JOINT_WIDTH/2 );
    glEnd();

    glBegin(GL_QUAD_STRIP); // draw Cylinder
    angle = 0.0;
    while( angle < 2*pi ) {
         x = JOINT_RADIUS * cos(angle);
         y = JOINT_RADIUS * sin(angle);
         glNormal3f(cos(angle), sin(angle), 0);
         glVertex3f( x,  y , JOINT_WIDTH/2 );
         glVertex3f( x,  y , -JOINT_WIDTH/2 );
         angle = angle + sangle;
    }
    glVertex3f(0 ,  JOINT_RADIUS, JOINT_WIDTH/2);
    glVertex3f(0 ,  JOINT_RADIUS, -JOINT_WIDTH/2);
    glEnd();
    
    angle = 0.0;
    glBegin(GL_TRIANGLE_FAN); // draw head circle
    glNormal3f(0.0f, 0.0f, 1.0f);
    while( angle < 2*pi ) {
          x = JOINT_RADIUS * cos(angle);
          y = JOINT_RADIUS * sin(angle);
          glVertex3f( x,  y  , JOINT_WIDTH/2);
          angle = angle + sangle;
    } // while
    glVertex3f(0,  0 , JOINT_WIDTH/2);
    glEnd();
}

void light() {
  GLfloat light_specular[] = {0.6, 0.6, 0.6, 1};
  GLfloat light_diffuse[] = {0.6, 0.6, 0.6, 1};
  GLfloat light_ambient[] = {0.4, 0.4, 0.4, 1};
  GLfloat light_position[] = {50.0, 75.0, 80.0, 1.0};
  // z buffer enable
  glEnable(GL_DEPTH_TEST);
  // enable lighting
  glEnable(GL_LIGHTING);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_NORMALIZE);
  // set light property
  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
}

int main() {
  initOpenGL();
  GLFWwindow* window = OpenGLContext::getWindow();

  // Init Camera helper
  Camera camera(glm::vec3(0, 2, 5));
  camera.initialize(OpenGLContext::getAspectRatio());
  // Store camera as glfw global variable for callbasks use
  glfwSetWindowUserPointer(window, &camera);
  glm::vec3  prev = glm::vec3( 0, ARM_LEN*2+BASE_HEIGHT+2*JOINT_DIAMETER, 0 );
  // Main rendering loop
    
  while (!glfwWindowShouldClose(window)) {
    // Polling events.
    glfwPollEvents();
    // Update camera position and view
    camera.move(window);
    // GL_XXX_BIT can simply "OR" together to use.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    /// TO DO Enable DepthTest
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    // Projection Matrix
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(camera.getProjectionMatrix());
    // ModelView Matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(camera.getViewMatrix());

#ifndef DISABLE_LIGHT   
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearDepth(1.0f);
    light();
#endif

    /* TODO#4-2: Update joint degrees
     *       1. Finish keyCallback to detect key events
     *       2. Update jointx_degree if the correspond key is pressed
     * Note:
     *       You can use `ROTATE_SPEED` as the speed constant. 
     *       If the rotate speed is too slow or too fast, please change `ROTATE_SPEED` value
     */
      joint0_degree += joint0_speed;
      joint1_degree += joint1_speed;
      joint2_degree += joint2_speed;
    /* TODO#5: Catch the target object with robotic arm
     *       1. Calculate coordinate of the robotic arm endpoint
     *       2. Test if arm endpoint and the target object are close enough
     *       3. Update coordinate fo the target object to the arm endpoint
     *          if the space key is pressed
     * Hint: 
     *       GLM fransform API (https://glm.g-truc.net/0.9.4/api/a00206.html)
     * Note: 
     *       You might use `ANGEL_TO_RADIAN`
     *       and refer to `CATCH_POSITION_OFFSET` and `TOLERANCE`
     */
    glm::vec3 endpoint( 0.0, BASE_HEIGHT+ARM_LEN+JOINT_RADIUS, 0  );
    glm::mat4 trans1 = glm::identity<glm::mat4>();
    trans1 = glm::rotate(trans1, ANGEL_TO_RADIAN(joint0_degree), glm::vec3(0.0, 1.0, 0.0));
    trans1 = glm::rotate(trans1, ANGEL_TO_RADIAN(joint1_degree), glm::vec3(0.0, 0.0,1.0));
    glm::vec4 arm1( 0, ARM_LEN + JOINT_DIAMETER , 0 , 1);
    arm1 = trans1*arm1;
    glm::mat4 trans2  = glm::identity<glm::mat4>();
    trans2 = glm::rotate(trans2, ANGEL_TO_RADIAN(joint0_degree), glm::vec3(0.0, 1.0, 0.0));
    trans2 = glm::rotate(trans2, ANGEL_TO_RADIAN(joint1_degree), glm::vec3(0.0, 0.0, 1.0));
    trans2 = glm::rotate(trans2, ANGEL_TO_RADIAN(joint2_degree), glm::vec3(0.0, 0.0,1.0));
    glm::vec4 arm2( 0, ARM_LEN + JOINT_RADIUS+CATCH_POSITION_OFFSET, 0 , 1);
    arm2 = trans2*arm2;
    endpoint[0] += arm1[0] + arm2[0];
    endpoint[1] += arm1[1] + arm2[1];
    endpoint[2] += arm1[2] + arm2[2];
    if( press ) {
        double dist = sqrt( pow( target_pos[0]-endpoint[0],2 ) + pow( target_pos[1]-endpoint[1] ,2 ) +pow( target_pos[2]-endpoint[2],2)  );
        if( catcht ){
            target_pos[0]+=endpoint[0]-prev[0];
            target_pos[1]+=endpoint[1]-prev[1];
            target_pos[2]+=endpoint[2]-prev[2];
        }
        if( dist <= TOLERANCE ) {
                catcht = true;
        } // if
    }//if
    prev = endpoint;
    //printf( "\n<%f,%f,%f>\n", endpoint[0], endpoint[1], endpoint[2] );
    // Render a white board
    glPushMatrix();
    glScalef(3, 1, 3);
    glBegin(GL_TRIANGLE_STRIP);
    glColor3f(1.0f, 1.0f, 1.0f);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-1.0f, 0.0f, -1.0f);
    glVertex3f(-1.0f, 0.0f, 1.0f);
    glVertex3f(1.0f, 0.0f, -1.0f);
    glVertex3f(1.0f, 0.0f, 1.0f);
    glEnd();
    glPopMatrix();

    /* TODO#2: Render a cylinder at target_pos
     *       1. Translate to target_pos
     *       2. Setup vertex color
     *       3. Setup cylinder scale
     *       4. Call drawUnitCylinder
     * Hint: 
     *       glTranslatef (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glTranslate.xml)
     *       glColor3f (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glColor.xml)
     *       glScalef (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glScale.xml)
     * Note:
     *       The coordinates of the cylinder are `target_pos`
     *       The cylinder's size can refer to `TARGET_RADIUS`, `TARGET_DIAMETER` and `TARGET_DIAMETER`
     *       The cylinder's color can refer to `RED`
     */
    glPushMatrix();
    glTranslatef(target_pos[0], target_pos[1]-0.05, target_pos[2]);
    drawUnitCylinder();
    glPopMatrix();
    /* TODO#3: Render the robotic arm
     *       1. Render the base
     *       2. Translate to top of the base
     *       3. Render an arm
     *       4. Translate to top of the arm
     *       5. Render the joint
     *       6. Translate and rotate to top of the join
     *       7. Repeat step 3-6
     * Hint:
     *       glPushMatrix/glPopMatrix (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glPushMatrix.xml)
     *       glRotatef (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glRotate.xml)
     * Note:
     *       The size of every component can refer to `Components size definition` section
     *       Rotate degree for joints are `joint0_degree`, `joint1_degree` and `joint2_degree`
     *       You may implement drawBase, drawArm and drawJoin first
     */
      glPushMatrix();
      drawBase();
      glTranslatef(0, BASE_HEIGHT, 0);
      drawArm();
      glTranslatef(0, ARM_LEN, 0);
      glRotatef(joint0_degree, 0, 1, 0);
      glTranslatef(0, JOINT_RADIUS, 0);
      drawJoin();
      glRotatef(joint1_degree, 0.0f, 0.0f, 1);
      glTranslatef(0, JOINT_RADIUS, 0);
      drawArm();
      glTranslatef(0, ARM_LEN, 0);
      glTranslatef(0, JOINT_RADIUS, 0);
      drawJoin();
      glRotatef(joint2_degree, 0.0f, 0.0f, 1);
      glTranslatef(0, JOINT_RADIUS, 0);
      drawArm();
      glPopMatrix();
#ifdef __APPLE__
    // Some platform need explicit glFlush
    glFlush();
#endif
    glfwSwapBuffers(window);
  }
  return 0;
}

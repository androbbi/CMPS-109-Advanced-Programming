//Antony Robbins androbbi@ucsc.edu
//Brandon Gomez brlgomez@ucsc.edu
// $Id: graphics.cpp,v 1.11 2014-05-15 16:42:55-07 - - $

#include <iostream>
using namespace std;

#include <GL/freeglut.h>

#include "graphics.h"
#include "util.h"

int window::incr = 0;
int window::width = 640; // in pixels
int window::height = 480; // in pixels
vector<object> window::objects;
size_t window::selected_obj = 0;
mouse window::mus;
GLfloat window::move_p = 4;
int window::counter = 0;
rgbcolor window::border(0xFF, 0x00, 0x00);
GLfloat window::border_size = 4;

object::object(shape_ptr ptr, vertex vert, rgbcolor rgb){  
  color = rgb;
  center = vert;
  pshape = ptr;
}

// Executed when window system signals to shut down.
void window::close() {
   DEBUGF ('g', sys_info::execname() << ": exit ("
           << sys_info::exit_status() << ")");
   exit (sys_info::exit_status());
}

// Executed when mouse enters or leaves window.
void window::entry (int mouse_entered) {
   DEBUGF ('g', "mouse_entered=" << mouse_entered);
   window::mus.entered = mouse_entered;
   if (window::mus.entered == GLUT_ENTERED) {
      DEBUGF ('g', sys_info::execname() << ": width=" << window::width
           << ", height=" << window::height);
   }
   glutPostRedisplay();
}

// Called to display the objects in the window.
void window::display() {
   glClear (GL_COLOR_BUFFER_BIT);
   window::incr=0;
   //for (auto& object: window::objects) object.draw();
   for(auto i = window::objects.begin(); 
       i != window::objects.end(); ++i){
      window::incr++;
      if(i == window::objects.begin()+counter){
         i->draw_border();
      }
      i->draw();
   }
   mus.draw();
   glutSwapBuffers();
}

// Called when window is opened and when resized.
void window::reshape (int width, int height) {
   DEBUGF ('g', "width=" << width << ", height=" << height);
   window::width = width;
   window::height = height;
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity();
   gluOrtho2D (0, window::width, 0, window::height);
   glMatrixMode (GL_MODELVIEW);
   glViewport (0, 0, window::width, window::height);
   glClearColor (0.25, 0.25, 0.25, 1.0);
   glutPostRedisplay();
}

// Executed when a regular keyboard key is pressed.
enum {BS=8, TAB=9, ESC=27, SPACE=32, DEL=127};
void window::keyboard (GLubyte key, int x, int y) {
   DEBUGF ('g', "key=" << (unsigned)key << ", x=" << x << ", y=" << y);
   window::mus.set (x, y);
   switch (key) {
      case 'Q': case 'q': case ESC:
         window::close();
         break;
      case 'H': case 'h':
         for(auto i = window::objects.begin(); 
             i != window::objects.end(); ++i){
           if(i == window::objects.begin() + counter)
             i->move(-move_p,0);
         }
         break;
      case 'J': case 'j':
         for(auto i = window::objects.begin(); 
             i != window::objects.end(); ++i){
           if(i == window::objects.begin() + counter)
             i->move(0,-move_p);
         }
         break;
      case 'K': case 'k':
         for(auto i = window::objects.begin(); 
             i != window::objects.end(); ++i){
           if(i == window::objects.begin() + counter)
             i->move(move_p,0);
         }
         break;
      case 'L': case 'l':
         for(auto i = window::objects.begin(); 
             i != window::objects.end(); ++i){
           if(i == window::objects.begin() + counter)
             i->move(0,move_p);
         }
         break;
      case 'N': case 'n': case SPACE: case TAB:
         counter++;
         break;
      case 'P': case 'p': case BS:
         counter--;
         break;
      case '0'...'9':
         counter = (key - '0');
         break;
      default:
         cerr << (unsigned)key << ": invalid keystroke" << endl;
         break;
   }
   glutPostRedisplay();
}

// Executed when a special function key is pressed.
void window::special (int key, int x, int y) {
   DEBUGF ('g', "key=" << key << ", x=" << x << ", y=" << y);
   window::mus.set (x, y);
   switch (key) {
      case GLUT_KEY_LEFT: //move_selected_object (-1, 0); 
        for(auto i = window::objects.begin(); 
            i != window::objects.end(); ++i){
          if(i == window::objects.begin()+counter)
            i->move(-move_p,0);
        }
        break;
      case GLUT_KEY_DOWN: 
        for(auto i = window::objects.begin(); 
            i != window::objects.end(); ++i){
          if(i == window::objects.begin()+counter)
            i->move(0,-move_p);
        }
        break;
      case GLUT_KEY_UP: 
        for(auto i = window::objects.begin(); 
            i != window::objects.end(); ++i){
          if(i == window::objects.begin()+counter)
            i->move(0,move_p);
        }
        break;
      case GLUT_KEY_RIGHT:
        for(auto i = window::objects.begin(); 
            i != window::objects.end(); ++i){
          if(i == window::objects.begin()+counter)
            i->move(move_p,0);
        }
        break;
      case GLUT_KEY_F1: counter = 1; break;
      case GLUT_KEY_F2: counter = 2; break;
      case GLUT_KEY_F3: counter = 3; break;
      case GLUT_KEY_F4: counter = 4; break;
      case GLUT_KEY_F5: counter = 5; break;
      case GLUT_KEY_F6: counter = 6; break;
      case GLUT_KEY_F7: counter = 7; break;
      case GLUT_KEY_F8: counter = 8; break;
      case GLUT_KEY_F9: counter = 9; break;
      case GLUT_KEY_F10: counter = 10; break;
      case GLUT_KEY_F11: counter = 11; break;
      case GLUT_KEY_F12: counter = 12; break;
      default:
         cerr << (unsigned)key << ": invalid function key" << endl;
         break;
   }
   glutPostRedisplay();
}

void window::motion (int x, int y) {
   DEBUGF ('g', "x=" << x << ", y=" << y);
   window::mus.set (x, y);
   glutPostRedisplay();
}

void window::passivemotion (int x, int y) {
   DEBUGF ('g', "x=" << x << ", y=" << y);
   window::mus.set (x, y);
   glutPostRedisplay();
}

void window::mousefn (int button, int state, int x, int y) {
   DEBUGF ('g', "button=" << button << ", state=" << state
           << ", x=" << x << ", y=" << y);
   window::mus.state (button, state);
   window::mus.set (x, y);
   glutPostRedisplay();
}

void window::main () {
   static int argc = 0;
   glutInit (&argc, nullptr);
   glutInitDisplayMode (GLUT_RGBA | GLUT_DOUBLE);
   glutInitWindowSize (window::width, window::height);
   glutInitWindowPosition (128, 128);
   glutCreateWindow (sys_info::execname().c_str());
   glutCloseFunc (window::close);
   glutEntryFunc (window::entry);
   glutDisplayFunc (window::display);
   glutReshapeFunc (window::reshape);
   glutKeyboardFunc (window::keyboard);
   glutSpecialFunc (window::special);
   glutMotionFunc (window::motion);
   glutPassiveMotionFunc (window::passivemotion);
   glutMouseFunc (window::mousefn);
   DEBUGF ('g', "Calling glutMainLoop()");
   glutMainLoop();
}

void mouse::state (int button, int state) {
   switch (button) {
      case GLUT_LEFT_BUTTON: left_state = state; break;
      case GLUT_MIDDLE_BUTTON: middle_state = state; break;
      case GLUT_RIGHT_BUTTON: right_state = state; break;
   }
}

void mouse::draw() {
   static rgbcolor color ("green");
   ostringstream text;
   text << "(" << xpos << "," << window::height - ypos << ")";
   if (left_state == GLUT_DOWN) text << "L"; 
   if (middle_state == GLUT_DOWN) text << "M"; 
   if (right_state == GLUT_DOWN) text << "R"; 
   if (entered == GLUT_ENTERED) {
      void* font = GLUT_BITMAP_HELVETICA_18;
      glColor3ubv (color.ubvec);
      glRasterPos2i (10, 10);
      glutBitmapString (font, (GLubyte*) text.str().c_str());
   }
}


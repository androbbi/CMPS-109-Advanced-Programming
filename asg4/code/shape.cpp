//Antony Robbins androbbi@ucsc.edu
//Brandon Gomez brlgomez@ucsc.edu
// $Id: shape.cpp,v 1.7 2014-05-08 18:32:56-07 - - $

#include <typeinfo>
#include <unordered_map>
#include <math.h> 
using namespace std;

#include "shape.h"
#include "util.h"
#include "graphics.h"

const float DEG2RAD = 3.14159/180.0;

static unordered_map<void*,string> fontname {
   {GLUT_BITMAP_8_BY_13       , "Fixed-8x13"    },
   {GLUT_BITMAP_9_BY_15       , "Fixed-9x15"    },
   {GLUT_BITMAP_HELVETICA_10  , "Helvetica-10"  },
   {GLUT_BITMAP_HELVETICA_12  , "Helvetica-12"  },
   {GLUT_BITMAP_HELVETICA_18  , "Helvetica-18"  },
   {GLUT_BITMAP_TIMES_ROMAN_10, "Times-Roman-10"},
   {GLUT_BITMAP_TIMES_ROMAN_24, "Times-Roman-24"},
};

static unordered_map<string,void*> fontcode {
   {"Fixed-8x13"    , GLUT_BITMAP_8_BY_13       },
   {"Fixed-9x15"    , GLUT_BITMAP_9_BY_15       },
   {"Helvetica-10"  , GLUT_BITMAP_HELVETICA_10  },
   {"Helvetica-12"  , GLUT_BITMAP_HELVETICA_12  },
   {"Helvetica-18"  , GLUT_BITMAP_HELVETICA_18  },
   {"Times-Roman-10", GLUT_BITMAP_TIMES_ROMAN_10},
   {"Times-Roman-24", GLUT_BITMAP_TIMES_ROMAN_24},
};

ostream& operator<< (ostream& out, const vertex& where) {
   out << "(" << where.xpos << "," << where.ypos << ")";
   return out;
}

shape::shape() {   
   DEBUGF ('c', this);
}

text::text (void* glut_bitmap_font, const string& textdata):
      glut_bitmap_font(glut_bitmap_font), textdata(textdata) {
   DEBUGF ('c', this);
}

ellipse::ellipse (GLfloat width, GLfloat height):
dimension ({width, height}) {
   DEBUGF ('c', this);
}

circle::circle (GLfloat diameter): ellipse (diameter, diameter) {
   DEBUGF ('c', this);
}

polygon::polygon (const vertex_list& vertices): vertices(vertices) {
   DEBUGF ('c', this);
}

rectangle::rectangle (GLfloat width, GLfloat height): 
      polygon({{0, 0}, {width, 0}, {width, height}, {0, height}}) {
   DEBUGF ('c', this << "(" << width << "," << height << ")");
}

square::square (GLfloat width): rectangle (width, width) {
   DEBUGF ('c', this);
}

diamond::diamond (const GLfloat width, const GLfloat height):
  polygon({{-width/2, 0}, {0, height/2}, {width/2, 0}, {0, -height/2}}){
}

triangle::triangle(GLfloat x0, GLfloat y0, GLfloat x1,
                   GLfloat y1,GLfloat x2, GLfloat y2):
  polygon({{x0, y0}, {x1, y1}, {x2, y2}}){
}


right_triangle::right_triangle(GLfloat width, GLfloat height):
  triangle(0,0,0,height,width,0){
}


isosceles::isosceles(GLfloat width, GLfloat height):
  triangle(0,0,width/2,height,width,0){
}


equilateral::equilateral(GLfloat width): 
  triangle(0,0,width,0,width/2,(width/2)*sqrt(3)){
}


void text::draw (const vertex& center, const rgbcolor& color) const {
   rgbcolor col = color;
   glColor3ubv(col.ubvec3());
   auto text = reinterpret_cast<const GLubyte*> (textdata.c_str());
   glRasterPos2f (center.xpos, center.ypos);
   glutBitmapString (glut_bitmap_font, text);
   if(window::incr <= 12) {
      rgbcolor c;
      glColor3ubv(c.ubvec3());
      string str = to_string(window::incr);
      auto text2 = reinterpret_cast<const GLubyte*> (str.c_str());   
      glRasterPos2f (center.xpos, center.ypos);
      glutBitmapString (GLUT_BITMAP_8_BY_13, text2);
   }
   DEBUGF ('d', this << "(" << center << "," << color << ")");
}

void ellipse::draw (const vertex& center, const rgbcolor& color) const {
   glBegin(GL_POLYGON);
   rgbcolor col = color;
   glColor3ubv(col.ubvec3());
   for(int i = 0; i < 360; i++){
     float degInRad = i * DEG2RAD;
     glVertex2f((cos(degInRad)*dimension.xpos)+center.xpos 
                ,(sin(degInRad)*dimension.ypos)+center.ypos);
   }
   glEnd(); 
   rgbcolor c;
   glColor3ubv(c.ubvec3());
   string str = to_string(window::incr);
   auto text = reinterpret_cast<const GLubyte*> (str.c_str());   
   glRasterPos2f (center.xpos, center.ypos);
   glutBitmapString (GLUT_BITMAP_8_BY_13, text);
   DEBUGF ('d', this << "(" << center << "," << color << ")");
}

void polygon::draw (const vertex& center, const rgbcolor& color) const {
  if(vertices.begin() == vertices.end()){
     return;
   }
   glBegin(GL_POLYGON);
   rgbcolor col = color;
   glColor3ubv(col.ubvec3());
   float width_average = 0.0;
   float height_average = 0.0;
   for(auto i = vertices.begin(); i != vertices.end(); ++i){ 
     width_average += i->xpos;
     height_average += i->ypos;
   }
   width_average /= vertices.size();
   height_average /= vertices.size();
   vertex_list new_vertex;
   for(auto i = vertices.begin(); i != vertices.end(); ++i){
     new_vertex.push_back({(i->xpos - width_average + center.xpos), 
           (i->ypos - height_average + center.ypos)});
   }
   for(auto i = new_vertex.begin(); i != new_vertex.end(); ++i){
     glVertex2f(i->xpos, i->ypos); 
   }
   glEnd();
   rgbcolor c;
   glColor3ubv(c.ubvec3());
   string str = to_string(window::incr);
   auto text = reinterpret_cast<const GLubyte*> (str.c_str());   
   glRasterPos2f (center.xpos, center.ypos);
   glutBitmapString (GLUT_BITMAP_8_BY_13, text);
   DEBUGF ('d', this << "(" << center << "," << color << ")");
}

void text::draw_border (const vertex& center, 
                        const rgbcolor& color)const {
  rgbcolor col = window::border;
  glColor3ubv(col.ubvec3());
  auto text = reinterpret_cast<const GLubyte*> (textdata.c_str());
  glScalef(2,2,2);
  glRasterPos2f (center.xpos+window::border_size,
                 center.ypos+window::border_size);
  glutBitmapString (glut_bitmap_font, text);
  DEBUGF ('d', this << "(" << center << "," << color << ")");
}

void ellipse::draw_border (const vertex& center, 
                           const rgbcolor& color) const {
  glBegin(GL_POLYGON);
  rgbcolor col = window::border;
  glColor3ubv(col.ubvec3());
  for(int i = 0; i < 360; i++){
    float degInRad = i * DEG2RAD;
    glVertex2f((cos(degInRad)*(dimension.xpos + window::border_size))
               +center.xpos
               ,(sin(degInRad)*(dimension.ypos + window::border_size))
               +center.ypos);
  }
  glEnd();
  DEBUGF ('d', this << "(" << center << "," << color << ")");
}

void polygon::draw_border (const vertex& center,
                           const rgbcolor& color) const {
  if(vertices.begin() == vertices.end()){
    return;
  }
  glBegin(GL_POLYGON);
  rgbcolor col = window::border;
  glColor3ubv(col.ubvec3());
  float width_average = 0.0;
  float height_average = 0.0;
  float xtemp = 0;
  float ytemp = 0;
  // 
  for(auto i = vertices.begin(); i != vertices.end(); ++i){
    width_average += i->xpos;
    height_average += i->ypos;
  }
  width_average /= vertices.size();
  height_average /= vertices.size();
  //Normalize
  vertex_list new_vertex;
  for(auto i = vertices.begin(); i != vertices.end(); ++i){
    new_vertex.push_back({(i->xpos - width_average + center.xpos),
          (i->ypos - height_average + center.ypos)});
  }
  
  for(auto i = new_vertex.begin(); i != new_vertex.end(); ++i){
    xtemp = i->xpos;
   ytemp = i->ypos;
    if( xtemp < center.xpos) 
      xtemp-= window::border_size;
      
   if(xtemp > center.xpos)
      xtemp+= window::border_size;
   if(xtemp == center.xpos and ytemp > center.ypos)
      ytemp += window::border_size;
   if(xtemp == center.xpos and ytemp < center.ypos)
      ytemp -= window::border_size;

   
   if( ytemp < center.ypos)
      ytemp-= window::border_size;
   
   if(ytemp > center.ypos)
      ytemp+= window::border_size;
   if(ytemp == center.ypos and xtemp > center.xpos)
      xtemp += window::border_size;
   if(ytemp == center.ypos and xtemp < center.xpos)
      xtemp -= window::border_size;
      
      
   glVertex2f(xtemp, ytemp);
  }
  glEnd();
  DEBUGF ('d', this << "(" << center << "," << color << ")");
}


void shape::show (ostream& out) const {
   out << this << "->" << demangle (*this) << ": ";
}

void text::show (ostream& out) const {
   shape::show (out);
   out << glut_bitmap_font << "(" << fontname[glut_bitmap_font]
       << ") \"" << textdata << "\"";
}

void ellipse::show (ostream& out) const {
   shape::show (out);
   out << "{" << dimension << "}";
}

void polygon::show (ostream& out) const {
   shape::show (out);
   out << "{" << vertices << "}";
}

ostream& operator<< (ostream& out, const shape& obj) {
   obj.show (out);
   return out;
}


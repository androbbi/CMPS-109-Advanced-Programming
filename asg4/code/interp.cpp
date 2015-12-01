//Antony Robbins androbbi@ucsc.edu
//Brandon Gomez brlgomez@ucsc.edu
// $Id: interp.cpp,v 1.15 2014-07-22 20:03:19-07 - - $

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
using namespace std;

#include <GL/freeglut.h>

#include "debug.h"
#include "interp.h"
#include "shape.h"
#include "util.h"
#include "graphics.h"

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

map<string,interpreter::interpreterfn> interpreter::interp_map {
   {"define" , &interpreter::do_define },
   {"draw"   , &interpreter::do_draw   },
   {"moveby" , &interpreter::do_moveby },
   {"border" , &interpreter::do_border },
};

map<string,interpreter::factoryfn> interpreter::factory_map {
   {"text"     , &interpreter::make_text     },
   {"ellipse"  , &interpreter::make_ellipse  },
   {"circle"   , &interpreter::make_circle   },
   {"polygon"  , &interpreter::make_polygon  },
   {"rectangle", &interpreter::make_rectangle},
   {"square"   , &interpreter::make_square   },
   {"diamond"  , &interpreter::make_diamond  },
   {"triangle"  , &interpreter::make_triangle  },
   {"right_triangle", &interpreter::make_right_triangle},
   {"isosceles"   , &interpreter::make_isosceles   },
   {"equilateral"  , &interpreter::make_equilateral  }
};

interpreter::shape_map interpreter::objmap;

interpreter::~interpreter() {
   for (const auto& itor: objmap) {
      cout << "objmap[" << itor.first << "] = "
           << *itor.second << endl;
   }
}

void interpreter::interpret (const parameters& params) {
   DEBUGF ('i', params);
   param begin = params.cbegin();
   string command = *begin;
   auto itor = interp_map.find (command);
   if (itor == interp_map.end()) throw runtime_error ("syntax error");
   interpreterfn func = itor->second;
   func (++begin, params.cend());
}

void interpreter::do_define (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string name = *begin;
   objmap.insert ({name, make_shape (++begin, end)});
}

void interpreter::do_draw (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   if (end - begin != 4) throw runtime_error ("syntax error");
   rgbcolor color {begin[0]};
   string name = begin[1];
   shape_map::const_iterator itor = objmap.find (name);
   if (itor == objmap.end()) {
      throw runtime_error (name + ": no such shape");
   }
   vertex where {from_string<GLfloat> (begin[2]),
                 from_string<GLfloat> (begin[3])};
   //itor->second->draw (where, color);
   window::push_back(object(itor->second, where, color));
}

void interpreter::do_moveby (param begin, param end) {
   window::move_p = stoi(*begin);
   end = end;
}

void interpreter::do_border (param begin, param end) {
  rgbcolor color {begin[0]};
  window::border = color;
  begin++;
  window::border_size = stof(*begin);
  end = end;
}

shape_ptr interpreter::make_shape (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string type = *begin++;
   auto itor = factory_map.find(type);
   if (itor == factory_map.end()) {
      throw runtime_error (type + ": no such shape");
   }
   factoryfn func = itor->second;
   return func (begin, end);
}

shape_ptr interpreter::make_text (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string font = *begin;
   auto itor = fontcode.find(font);
   if(itor == fontcode.end()){
     throw runtime_error(font + " :no such font");
   }
   void* bitmap = itor->second;
   begin++;
   string words;
   bool space = false;
   for(auto i = begin; i != end; ++i){
     if(space) words += " ";
     words += *i;
     space = true;
   }
   return make_shared<text> (bitmap, words);
}

shape_ptr interpreter::make_ellipse (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   GLfloat width = stof(*begin);
   width/=2.0;
   begin++;
   GLfloat height = stof(*begin);
   height/=2.0;
   return make_shared<ellipse> (width, height);
}

shape_ptr interpreter::make_circle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   GLfloat diameter = stof(*begin);
   diameter/=2.0;
   return make_shared<circle> (diameter);
}

shape_ptr interpreter::make_polygon (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   vertex_list vert;
   for(auto i = begin; i != end; ++i){
     vertex ver({stof(*i++),stof(*i)});
     vert.push_back(ver);
   }
   return make_shared<polygon> (vert);
}

shape_ptr interpreter::make_rectangle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   GLfloat width = stof(*begin);
   begin++;
   GLfloat height = stof(*begin);
   return make_shared<rectangle> (width, height);
}

shape_ptr interpreter::make_square (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   GLfloat width = stof(*begin);
   return make_shared<square> (width);
}

shape_ptr interpreter::make_diamond (param begin, param end) {
  DEBUGF ('f', range (begin, end));
  GLfloat width = stof(*begin);
  begin++;
  GLfloat height = stof(*begin);
  return make_shared<diamond> (width, height);
}

shape_ptr interpreter::make_triangle (param begin, param end) {
  DEBUGF ('f', range (begin, end));
  GLfloat x0 = stof(*begin);
  begin++;
  GLfloat y0 = stof(*begin);
  begin++;
  GLfloat x1 = stof(*begin);
  begin++;
  GLfloat y1 = stof(*begin);
  begin++;
  GLfloat x2 = stof(*begin);
  begin++;
  GLfloat y2 = stof(*begin);
  return make_shared<triangle> (x0, y0, x1, y1, x2, y2);
}

shape_ptr interpreter::make_right_triangle (param begin, param end) {
  DEBUGF ('f', range (begin, end));
  GLfloat width = stof(*begin);
  begin++;
  GLfloat height = stof(*begin);
  return make_shared<right_triangle> (width, height);
}

shape_ptr interpreter::make_isosceles (param begin, param end) {
  DEBUGF ('f', range (begin, end));
  GLfloat width = stof(*begin);
  begin++;
  GLfloat height = stof(*begin);
  return make_shared<isosceles> (width, height);
}

shape_ptr interpreter::make_equilateral (param begin, param end) {
  DEBUGF ('f', range (begin, end));
  GLfloat width = stof(*begin);
  return make_shared<equilateral> (width);
}


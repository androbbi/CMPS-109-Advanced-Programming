// $Id: main.cpp,v 1.7 2015-02-03 15:39:46-08 - - $
//ASG 3 main
/*
  This Program was edited by
  Partner: Brandon Gomez (brlgomez@ucsc.edu)
  Partner: Antony Robbins (androbbi@ucsc.edu)
*/

#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <unistd.h>
#include <fstream>
#include <cstring>
#include <libgen.h>
using namespace std;

#include "listmap.h"
#include "xpair.h"
#include "util.h"

using str_str_pair = xpair<const string,string>;
using str_str_map = listmap<string,string>;

void scan_options (int argc, char** argv) {
   opterr = 0;
   for (;;) {
      int option = getopt (argc, argv, "@:");
      if (option == EOF) break;
      switch (option) {
         case '@':
            traceflags::setflags (optarg);
            break;
         default:
            complain() << "-" << (char) optopt << ": invalid option"
                       << endl;
            break;
      }
   }
}

/*
  Referenced by Mackey, splitline.cpp
*/
string trim(const std::string& str){
   size_t first = str.find_first_not_of (" \t");
   if (first == string::npos) return "";
   size_t last = str.find_last_not_of (" \t");
   return str.substr (first, last - first + 1);
}

string reduce(const std::string& str,
                   const std::string& fill = " ",
                   const std::string& whitespace = " \t"){
  // trim first
  auto result = trim(str);
  // replace sub ranges
  auto beginSpace = result.find_first_of(whitespace);
  while (beginSpace != std::string::npos)
    {
      const auto endSpace = result.find_first_not_of
        (whitespace, beginSpace);
      const auto range = endSpace - beginSpace;
      result.replace(beginSpace, range, fill);
      const auto newStart = beginSpace + fill.length();
      beginSpace = result.find_first_of(whitespace, newStart);
    }
  return result;
}

void key_value (istream &file, str_str_map &test, int &count,
                                   const string &file_name ){
  for(;;){
    string line;
    getline(file, line);
    if(file.eof()) break;

    // trim leading trailing and extra spaces
    cout << file_name << ": " << count << ": " << line << endl;
    count++;
   if(line.size() == 0){
      //cout << "this line is empty" << endl;
      continue;
    }
    line = trim(line);
    line = reduce(line);
     
    // do nothing on hash
    if(line.at(0) == '#') continue;

    // =, print all in lexico
    if(line.compare("=") == 0){
      for(auto it = test.begin(); it != test.end(); ++it){
        cout << it->first << " = " << it->second << endl;
      }
      continue;
    }
     
    std::size_t found = line.find_first_of("=");
    // KEY, print key and associated value, 
    // else print not found
    if(found == string::npos){
      auto found_it = test.find(line);
      if(found_it != test.end()){
        cout << found_it->first << " = " << found_it->second << endl;
      }
      else{
        cout << line << ": key not found" << endl;
      }
      //cout << line << endl;
      continue;
    }
    // split at the sight of a "="
    // Referenced from Mackey's splitline.cpp
    // http://www2.ucsc.edu/courses/cmps109-wm/:
    // /Assignments/asg3-listmap-templates/misc/splitline.cpp
    string key = trim(line.substr(0, found));
   string value = trim(line.substr(found + 1));
   // trim leading and trailing spaces
   
      if(key.compare(value) == 0){
      // = VALUE, print lexico that has value
      if(found == 0){
        for(auto it = test.begin(); it != test.end(); ++it){
          if(it->second == value){
            cout << it->first << " = " << it->second << endl;
          }
        }
        continue;
      }
       
      // KEY =, delete key and associated value
      else{
        auto found_it = test.find(key);
        if(found_it != test.end()){
          test.erase(found_it);
        }
        else{
          cout << key
               <<": key not found cannot delete" << endl;
        }
        //cout << splitted.front() << " =" << endl;
        continue;
      }
    }
       
    // KEY = VALUE 
    else{
      // insert into list
      str_str_pair pair({key, value});
      cout << key << " = " << value << endl;
      auto found_it = test.find(key);
      if(found_it != test.end()){ 
        found_it->second = value;
      }
      else{
        test.insert(pair);
      }
      continue;
    }
  }
}

int main (int argc, char** argv) {
   int status = 0;
   //sys_info::set_execname (argv[0]);
   delete sys_info::execname;
   string progname = argv[0];
   scan_options (argc, argv);
   str_str_map test{};
   int line_count = 1;
   if(argc > 1){
      for(int argi = 1; argi < argc; ++argi){
         string file_name = argv[argi];
        if(file_name == "-") {
           key_value(cin, test, line_count, file_name);
        }
        else{
            ifstream file(file_name); //.c_str
         if(file.fail()) {
            status = 1;
               cerr << progname << ": " << file_name << ": "
                    << strerror (errno) << endl;
         }
         else {
               key_value(file, test, line_count, file_name);
               file.close();
            }
         }
      }
   }
   else{
     string file_name = "-";
     key_value(cin, test, line_count, file_name);
   }
   cout << "EXIT_SUCCESS" << endl;
   return status;
}

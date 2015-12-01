// $Id: inode.cpp,v 1.12 2014-07-03 13:29:57-07 - - $

/* 
  This Program was edited by
  Partner: Brandon Gomez (brlgomez@ucsc.edu)
  Partner: Antony Robbins (androbbi@ucsc.edu)
*/

#include <iostream>
#include <stdexcept>

using namespace std;

#include "debug.h"
#include "inode.h"

int inode::next_inode_nr {1};

inode::inode(inode_t init_type):
   inode_nr (next_inode_nr++), type (init_type)
{
   switch (type) {
      case PLAIN_INODE:
           contents = make_shared<plain_file>();
           break;
      case DIR_INODE:
           contents = make_shared<directory>();
           break;
   }
   DEBUGF ('i', "inode " << inode_nr << ", type = " << type);
}

int inode::get_inode_nr() const {
   DEBUGF ('i', "inode = " << inode_nr);
   return inode_nr;
}

plain_file_ptr plain_file_ptr_of (file_base_ptr ptr) {
   plain_file_ptr pfptr = dynamic_pointer_cast<plain_file> (ptr);
   if (pfptr == nullptr) throw invalid_argument ("plain_file_ptr_of");
   return pfptr;
}

directory_ptr directory_ptr_of (file_base_ptr ptr) {
   directory_ptr dirptr = dynamic_pointer_cast<directory> (ptr);
   if (dirptr == nullptr) throw invalid_argument ("directory_ptr_of");
   return dirptr;
}

size_t plain_file::size() const {
   size_t size {0};
   for(auto iterate = data.begin(); iterate != data.end(); ++iterate){
     for(size_t j = 0; j < iterate->size(); ++j){
       size++;
     }
   }
   size = size + data.size();
   DEBUGF ('i', "size = " << size);
   return size;
}

int inode_state::get_size(const string& filename){
  directory_ptr the_dir = directory_ptr_of (cwd->contents);
  int size = 1;
  inode_ptr file_ptr = the_dir->return_data(filename);
  plain_file_ptr file = plain_file_ptr_of(file_ptr->contents);
  size = file->size();
  return size;
}

const wordvec& plain_file::readfile() const {
   DEBUGF ('i', data);
   return data;
}

wordvec inode_state::return_wordvec(const string& filename){
  directory_ptr the_dir = directory_ptr_of (root->contents);
  inode_ptr file_ptr = the_dir->return_data(filename);
  plain_file_ptr file = plain_file_ptr_of(file_ptr->contents);
  wordvec data = file->readfile();
  return data;
}

inode_ptr directory::return_data(const string& filename){
  inode_ptr dir;
  for(auto it = dirents.begin(); it != dirents.end(); ++it){
    if(filename == (it->first)){
      dir = (it->second);   
    }
  }
  return dir;
}

void plain_file::writefile (const wordvec& words) {
   data = words;
   DEBUGF ('i', words);
}

size_t directory::size() const {
   size_t size {0}; 
   size = dirents.size();
   DEBUGF ('i', "size = " << size);
   return size;
}

void directory::remove (const string& filename) {
   dirents.erase(filename);
   DEBUGF ('i', filename);
}

void inode_state::delete_file(const string& filename){
  directory_ptr the_dir = directory_ptr_of (cwd->contents);
  the_dir->remove(filename);
}

inode_state::inode_state() {
   DEBUGF ('i', "root = " << root << ", cwd = " << cwd
          << ", prompt = \"" << prompt << "\"");
   root = make_shared<inode>(DIR_INODE);
   cwd = root;
   directory_ptr the_dir = directory_ptr_of (root->contents);
   the_dir->insert(".", root);
   the_dir->insert("..", root);
}

int inode_state::return_nr(){
   return (cwd->inode_nr);  
}

inode_ptr inode_state::return_cwd() {
  return cwd;
}

inode_ptr inode_state::return_root(){
  return root;
}

directory_ptr directory::return_dir(){
  inode_ptr the_dir = make_shared<inode>(DIR_INODE);
  return directory_ptr_of (the_dir->get_contents());
}

ostream& operator<< (ostream& out, const inode_state& state) {
   out << "inode_state: root = " << state.root
       << ", cwd = " << state.cwd;
   return out;
}

inode& directory::mkdir (const string& dir, const inode_ptr& parent){
  inode_ptr the_dir = make_shared<inode>(DIR_INODE);
  directory_ptr x = directory_ptr_of (the_dir->get_contents());
  dirents.insert({dir, the_dir});
  x->dirents.insert({".", the_dir});
  x->dirents.insert({"..", parent});
  return *(the_dir.get());
}

inode& directory::mkfile(const string& filename, wordvec& data){
  inode_ptr the_file = make_shared<inode>(PLAIN_INODE);
  plain_file_ptr x = plain_file_ptr_of (the_file->get_contents());
  dirents.insert({filename, the_file});
  x->writefile(data);
  return *(the_file.get());
}

file_base_ptr inode::get_contents(){
  return contents;
}

void directory::insert(const string& dirname, inode_ptr node){
  dirents.insert({dirname, node});
}

void inode_state::reset_prompt(){
  prompt = "";
}

void inode_state::set_prompt(string pmt) {
  prompt += pmt;
  
}

string inode_state::return_prompt() {
  return prompt;
}

void inode_state::make_file(inode_ptr n, const string& f, wordvec d){
  directory_ptr the_dir = directory_ptr_of (n->contents);
  the_dir->mkfile(f, d);
}

void inode_state::make_dir(inode_ptr node, const string& file){
  directory_ptr the_dir = directory_ptr_of (node->contents);
  inode_ptr parent = the_dir->get_parent(file);
  the_dir->mkdir(file, parent);
}

wordvec directory::return_dirents(){
  wordvec data;
  for(auto it = dirents.begin(); it != dirents.end(); ++it){
    data.push_back(it->first);
  }
  return data;
}

wordvec inode_state::get_data(){
  directory_ptr the_dir = directory_ptr_of (root->contents);
  return the_dir->return_dirents();
}

void inode_state::next_direct(const string& path){
  directory_ptr the_dir = directory_ptr_of (root->contents);
  cwd = the_dir->traverse_map(path);  
}

inode_ptr directory::traverse_map(const string& path){
   inode_ptr dir;
   inode_ptr temp;
   for(auto it = dirents.begin(); it != dirents.end(); ++it){
     if(path == (it->first)){
       dir = (it->second);
      }
     temp = (it->second);
   }
   return dir;
}

inode_ptr directory::get_parent(const string& path){
  inode_ptr dir;
  inode_ptr temp;
  for(auto it = dirents.begin(); it != dirents.end(); ++it){
    if(path == (it->first)){
      dir = (it->second);
      break;
    }
    temp = (it->second);
  }
  return temp;
}

inode_t inode::return_my_type(){
  return type;
}

wordvec inode_state::return_path(){
  return input_path;
}

void inode_state::insert(const string& path){
  input_path.push_back(path);
}

void inode_state::pop_path(){
  input_path.pop_back();
}

inode_ptr inode_state::it_cwd(const string& file){
  inode_ptr my {nullptr};
  map<string,inode_ptr> temp_dirents;
  inode_ptr curr = return_cwd();
  directory_ptr ptr = directory_ptr_of(curr->contents);
  temp_dirents = ptr->return_map();
  for(auto it = temp_dirents.begin(); it != temp_dirents.end(); ++it){
    if(file == (it->first)){
      my = (it->second);
    }
    return my;
  }
  return my;
}

map<string,inode_ptr> directory::return_map(){
  return dirents;
}

inode_ptr inode::find_path(inode_ptr& curr, const string& path) {
  directory_ptr ptr = directory_ptr_of(curr->contents);
  if(path.empty()) {
    return curr;
  }
  else {
    wordvec conv_path = split(path, "/");
    for(auto it = conv_path.begin(); it != conv_path.end(); ++it){
      curr = ptr->traverse_map(*it);
    }
    return curr;
      
  }
}


// $Id: inode.h,v 1.13 2014-06-12 18:10:25-07 - - $

/*
  This Program was edited by
  Partner: Brandon Gomez (brlgomez@ucsc.edu)
  Partner: Antony Robbins (androbbi@ucsc.edu)
*/

#ifndef __INODE_H__
#define __INODE_H__ 

#include <exception>
#include <iostream>
#include <memory>
#include <map>
#include <vector>
using namespace std;

#include "util.h"

//
// inode_t -
//    An inode is either a directory or a plain file.
//

enum inode_t {PLAIN_INODE, DIR_INODE};
class inode;
class file_base;
class plain_file;
class directory;
using inode_ptr = shared_ptr<inode>;
using file_base_ptr = shared_ptr<file_base>;
using plain_file_ptr = shared_ptr<plain_file>;
using directory_ptr = shared_ptr<directory>;

//
// inode_state -
//    A small convenient class to maintain the state of the simulated
//    process:  the root (/), the current directory (.), and the
//    prompt.
// worvec input_path -
//    A data structure to hold the input path inorder to display the
//    cwd
// return_cwd -
//    Returns the states current working directory inode_ptr
// return_root -
//    Retunrs the states root inode_ptr
// set_prompt -
//    A function to change the starting prompt
// return_prompt -
//    Returns the prompt in order to display
// reset_prompt -
//    empties the prompt
// make_file - 
//    Makes a plain file ptr with the name and data the user inputed
//    and also gets the proper parent inode_ptr
// make_dir - 
//    Makes a directory ptr with the name inputed by the user
// get_data -
//    Retunrs the wordvec of the selected file
// next_direct -
//    get the next directory
// return_nr - 
//    Returns the inode's num
// delete_file - 
//    Deletes a file
// return_wordvec -
//   Returns the wordvec of the file
// get_size - 
//   Returns the size of the file
// insert - 
//    Inserts into the wordvec input_path in order to print cwd
// return_path - 
//    Returns the input_path
// pop_path - 
//    Deletes the top of the input_path
// it_cwd - 
//    Tries to get the inode_ptr of the file curr looking at

class inode_state {
   friend class inode;
   friend ostream& operator<< (ostream& out, const inode_state&);
   private:
      inode_state (const inode_state&) = delete; // copy ctor
      inode_state& operator= (const inode_state&) = delete; // op=
      inode_ptr root {nullptr};
      inode_ptr cwd {nullptr};
      string prompt {"% "};
   public:
      inode_state();
      wordvec input_path{""};
      inode_ptr return_cwd();
      inode_ptr return_root();
      void set_prompt(string pmt);
      void reset_prompt();
      string return_prompt();
      void make_file(inode_ptr node, const string& file, wordvec data);
      void make_dir(inode_ptr node, const string& file_name);
      wordvec get_data();
      void next_direct(const string& path);
      int return_nr();
      void delete_file(const string& filename);
      wordvec return_wordvec(const string& filename);
      int get_size(const string& filename);
      void insert(const string& path);
      wordvec return_path();
      void pop_path();
      inode_ptr it_cwd(const string& file);
};

//
// class inode -
//
// inode ctor -
//    Create a new inode of the given type.
// get_inode_nr -
//    Retrieves the serial number of the inode.  Inode numbers are
//    allocated in sequence by small integer.
// size -
//    Returns the size of an inode.  For a directory, this is the
//    number of dirents.  For a text file, the number of characters
//    when printed (the sum of the lengths of each word, plus the
//    number of words.
//    

class inode {
   friend class inode_state;
   private:
      static int next_inode_nr;
      int inode_nr;
      inode_t type;
      file_base_ptr contents;
   public:
      file_base_ptr get_contents();
      inode (inode_t init_type);
      int get_inode_nr() const;
      inode_t return_my_type();
      inode_ptr find_path(inode_ptr& curr, const string& path );
};

//
// class file_base -
//
// Just a base class at which an inode can point.  No data or
// functions.  Makes the synthesized members useable only from
// the derived classes.
//

class file_base {
   protected:
      file_base () = default;
      file_base (const file_base&) = default;
      file_base (file_base&&) = default;
      file_base& operator= (const file_base&) = default;
      file_base& operator= (file_base&&) = default;
      virtual ~file_base () = default;
      virtual size_t size() const = 0;
   public:
      friend plain_file_ptr plain_file_ptr_of (file_base_ptr);
      friend directory_ptr directory_ptr_of (file_base_ptr);
};

//
// class plain_file -
//
// Used to hold data.
// synthesized default ctor -
//    Default vector<string> is a an empty vector.
// readfile -
//    Returns a copy of the contents of the wordvec in the file.
//    Throws an yshell_exn for a directory.
// writefile -
//    Replaces the contents of a file with new contents.
//    Throws an yshell_exn for a directory.
//

class plain_file: public file_base {
   private:
      wordvec data;
   public:
      size_t size() const override;
      const wordvec& readfile() const;
      void writefile (const wordvec& newdata);
};

//
// class directory -
//
// Used to map filenames onto inode pointers.
// default ctor -
//    Creates a new map with keys "." and "..".
// remove -
//    Removes the file or subdirectory from the current inode.
//    Throws an yshell_exn if this is not a directory, the file
//    does not exist, or the subdirectory is not empty.
//    Here empty means the only entries are dot (.) and dotdot (..).
// mkdir -
//    Creates a new directory under the current directory and 
//    immediately adds the directories dot (.) and dotdot (..) to it.
//    Note that the parent (..) of / is / itself.  It is an error
//    if the entry already exists.
// mkfile -
//    Create a new empty text file with the given name.  Error if
//    a dirent with that name exists.
// insert - 
//    insert a new node into dirents
// returns_dir -
//    returns a directory ptr
// return_dirents - 
//    a helper to access the data inside a node file
// traverse_map - 
//    a function the gets the inode_ptr of what ever you are looking at
// return_data - 
//    returns the inode_ptr of the file or directory
// get_parent - 
//    retunrs the parents of the directory or file
// return_map -
//   returns dirents

class directory: public file_base {
   private:
      map<string,inode_ptr> dirents;
   public:
      size_t size() const override;
      void remove (const string& filename);
      inode& mkdir (const string& dirname, const inode_ptr& parent);
      inode& mkfile (const string& filename, wordvec& data);
      void insert(const string& dirname, inode_ptr node);
      directory_ptr return_dir();
      wordvec return_dirents();
      inode_ptr traverse_map(const string& path);
      inode_ptr return_data(const string& filename);
      inode_ptr get_parent(const string& path);
      map<string,inode_ptr> return_map();
};

#endif

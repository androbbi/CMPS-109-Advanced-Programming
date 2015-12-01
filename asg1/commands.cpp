
// $Id: commands.cpp,v 1.11 2014-06-11 13:49:31-07 - - $

/*
  This Program was edited by
  Partner: Brandon Gomez (brlgomez@ucsc.edu)
  Partner: Antony Robbins (androbbi@ucsc.edu) 
 */

#include "commands.h"
#include "debug.h"

commands::commands(): map ({
   {"#"     , fn_hash  },
   {"cat"   , fn_cat   },
   {"cd"    , fn_cd    },
   {"echo"  , fn_echo  },
   {"exit"  , fn_exit  },
   {"ls"    , fn_ls    },
   {"lsr"   , fn_lsr   },
   {"make"  , fn_make  },
   {"mkdir" , fn_mkdir },
   {"prompt", fn_prompt},
   {"pwd"   , fn_pwd   },
   {"rm"    , fn_rm    },
}){}

command_fn commands::at (const string& cmd) {
   // Note: value_type is pair<const key_type, mapped_type>
   // So: iterator->first is key_type (string)
   // So: iterator->second is mapped_type (command_fn)
   command_map::const_iterator result = map.find (cmd);
   if (result == map.end()) {
      throw yshell_exn (cmd + ": no such function");
   }
   return result->second;
}

void fn_hash (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
} 

void fn_cat (inode_state& state, const wordvec& words){
  for(unsigned int i = 1; i < words.size(); ++i){
    string file_name = words[i];
    wordvec data = state.return_wordvec(file_name);
    for(auto it = data.begin(); it != data.end(); it++){
      cout << *it << " ";
    }
    cout << endl;
  }
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_cd (inode_state& state, const wordvec& words){
   string dir_name = words[1];
   state.next_direct(dir_name);
   inode_ptr cwd = state.return_cwd();
   if(dir_name == ".."){
     state.pop_path();
   }
   if(dir_name == "."){

   }
   else{
     state.insert(dir_name);
   }
   wordvec path = state.return_path();
   for(auto it = path.begin(); it != path.end(); ++it){
     if(it != path.begin()){
       cout << "/" << *it;
     }
   }
   cout << endl;
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_echo (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   for(auto it = words.begin() + 1; it != words.end(); it++){
    cout << *it << " ";
   }
   cout << endl;
}

void fn_exit (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   int exit_status;
   if(words.size() == 1) {
      exit_status = 0;
   }
   else if(!std::isdigit(words.at(1))) {
      exit_status = 127;
   }
   else {
      exit_status = words.at(1);
   }
   cout << ": exit(" << exit_status << ")" << endl;
   //throw ysh_exit_exn();
}

void fn_ls (inode_state& state, const wordvec& words){
  if (words.size() >= 1){
    inode_ptr cwd = state.return_cwd();
    int enter_nr = state.return_nr();
    wordvec data = state.get_data();
    inode_ptr root = state.return_root();
    inode_t type = cwd->return_my_type();
    inode_ptr my{nullptr};
    cout << "      " << "1" << "      ";
    cout << root->get_inode_nr() <<"  ." << endl;
    cout << "      " << "1" << "      ";
    cout << cwd->get_inode_nr() << "  .." << endl;
    for(auto iterator = data.begin() + enter_nr + 1; 
      iterator != data.end(); ++iterator){
      my = state.it_cwd(*iterator);
      type = cwd->return_my_type();
      if(type == PLAIN_INODE){
        cout << "      " << "#" << "      " << 
        state.get_size(*iterator) << "  " << *iterator << endl;
      }
      if(type == DIR_INODE){
        cout << "      " << cwd->get_inode_nr() 
             << "      " << state.get_size(*iterator) 
             << "  " << *iterator << endl;
      }
    }
  }
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_lsr (inode_state& state, const wordvec& words){
  DEBUGF ('c', state);
  DEBUGF ('c', words);
}

void fn_make (inode_state& state, const wordvec& words){
  string file_name = "";
  wordvec data;
  file_name = words[1];
  for(auto it = words.begin() + 2; it != words.end(); ++it){
    data.push_back(*it);
  }
  inode_ptr curr_dir = state.return_cwd();
  state.make_file(curr_dir, file_name, data);;
  DEBUGF ('c', state);
  DEBUGF ('c', words);
}

void fn_mkdir (inode_state& state, const wordvec& words){
   string dir_name = "";
   dir_name = words[1];
   inode_ptr curr_dir = state.return_cwd();
   state.make_dir(curr_dir, dir_name);   
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_prompt (inode_state& state, const wordvec& words){   
  if(words.size() > 1){
    state.reset_prompt();
    for(unsigned int it = 1; it != words.size(); ++it){
      if( it != words.size() ){
        state.set_prompt(words[it]);
        state.set_prompt(" ");
      }
      else{
        state.set_prompt(words[it]);
      }
    }
  }
  DEBUGF ('c', state);
  DEBUGF ('c', words);   
}

void fn_pwd (inode_state& state, const wordvec& words){
  wordvec path = state.return_path();
  for(auto it = path.begin(); it != path.end(); ++it){
    if(it != path.begin()){
      cout << "/" << *it;
    }
  }
  cout << endl;
  DEBUGF ('c', state);
  DEBUGF ('c', words);

}

void fn_rm (inode_state& state, const wordvec& words){
  string del_file = words[1];
  state.delete_file(del_file);
  DEBUGF ('c', state);
  DEBUGF ('c', words);
}

void fn_rmr (inode_state& state, const wordvec& words){
/*Post-order
Traverse the left subtree by recursively calling the post-order function.
Traverse the right subtree by recursively calling the post-order function.
Display the data part of root element (or current element). */
   //fn_rmr(state, );
   //words = split(words.back(), "/");s
   //fn_rmr(state, words);
   //state.delete_file(words.back());
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

/*
int exit_status_message() {
  //int exit_status = exit_status::get();
  
  cout << execname() << ": exit(" << exit_status << ")" << endl;
  return exit_status;
}
*/

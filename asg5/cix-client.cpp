// Antony Robbins androbbi@ucsc.edu
// Brandon Gomez brlgomez@ucsc.edu
// $Id: cix-client.cpp,v 1.7 2014-07-25 12:12:51-07 - - $

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
using namespace std;

#include <stdio.h>
#include <libgen.h>
#include <sys/types.h>
#include <unistd.h>

#include "cix_protocol.h"
#include "logstream.h"
#include "signal_action.h"
#include "sockets.h"

logstream log (cout);

unordered_map<string,cix_command> command_map {
   {"exit", CIX_EXIT},
   {"get", CIX_GET},
   {"help", CIX_HELP},
   {"ls"  , CIX_LS  },
   {"put", CIX_PUT},
   {"rm", CIX_RM},
};

void cix_help() {
   static vector<string> help = {
      "exit         - Exit the program.  Equivalent to EOF.",
      "get filename - Copy remote file to local host.",
      "help         - Print help summary.",
      "ls           - List names of files on remote server.",
      "put filename - Copy local file to remote host.",
      "rm filename  - Remove file from remote server.",
   };
   for (const auto& line: help) cout << line << endl;
}

void cix_get(client_socket& server, string& file){
  cix_header header;
  header.cix_command = CIX_GET;
  for(size_t it = 0; it < file.size(); ++it){ 
    header.cix_filename[it] = file[it];
  }
  log << "sending header " << header << endl;
  send_packet (server, &header, sizeof header);
  recv_packet (server, &header, sizeof header);
  log << "received header " << header << endl;
  if (header.cix_command != CIX_FILE) {
    log << "sent CIX_LS, server did not return CIX_LSOUT" << endl;
    log << "server returned " << header << endl;
  }else {
    char buffer[header.cix_nbytes + 1];
    recv_packet (server, buffer, header.cix_nbytes);
    log << "received " << header.cix_nbytes << " bytes" << endl;
    buffer[header.cix_nbytes] = '\0';
    ofstream ofs {header.cix_filename};
    ofs.write (buffer, sizeof buffer);
    ofs.close();
    cout << header.cix_filename<<" has been created"<< endl;;
  }
}

void cix_ls (client_socket& server) {
   cix_header header;
   header.cix_command = CIX_LS;
   log << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   log << "received header " << header << endl;
   if (header.cix_command != CIX_LSOUT) {
      log << "sent CIX_LS, server did not return CIX_LSOUT" << endl;
      log << "server returned " << header << endl;
   }else {
      char buffer[header.cix_nbytes + 1];
      recv_packet (server, buffer, header.cix_nbytes);
      log << "received " << header.cix_nbytes << " bytes" << endl;
      buffer[header.cix_nbytes] = '\0';
      cout << buffer;
   }
}

void cix_put(client_socket& server, string& file){
   cix_header header;
   header.cix_command = CIX_PUT;
   
   for(size_t it = 0; it < file.size(); ++it){
     header.cix_filename[it] = file[it];
   }

   ifstream stream{header.cix_filename};
   string output;
   while(!stream.eof()){
     char buffer[0x1000];
     stream.read(buffer, sizeof buffer);
     output.append(buffer);
   }
   stream.close();
   header.cix_nbytes = output.size();

   log << "sender header " << header << endl;
   send_packet (server, &header, sizeof header);
   send_packet (server, output.c_str(), output.size());
   log << "sent " << output.size() << " bytes" << endl;
   recv_packet (server, &header, sizeof header);
   log << "received header " << header << endl;
   if (header.cix_command != CIX_ACK) {
     log << "sent CIX_PUT, server did not return CIX_AK" << endl;
     log << "server returned " << header << endl;
   }
}

void cix_rm(client_socket& server, string& file){
   cix_header header;
   header.cix_command = CIX_RM;
   
   strcpy (header.cix_filename, const_cast<char*> (file.c_str()));
   
   log << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   log << "received header " << header << endl;
   
   if (header.cix_command != CIX_ACK) {
     log << "insert stuff here yo " << endl;
     log << "insert stuff here " << header << endl;
   }

}

void usage() {
   cerr << "Usage: " << log.execname() << " [host] [port]" << endl;
   throw cix_exit();
}

bool SIGINT_throw_cix_exit {false};
void signal_handler (int signal) {
   log << "signal_handler: caught " << strsignal (signal) << endl;
   switch (signal) {
      case SIGINT: case SIGTERM: SIGINT_throw_cix_exit = true; break;
      default: break;
   }
}

int main (int argc, char** argv) {
   log.execname (basename (argv[0]));
   log << "starting" << endl;
   vector<string> args (&argv[1], &argv[argc]);
   signal_action (SIGINT, signal_handler);
   signal_action (SIGTERM, signal_handler);
   if (args.size() > 2) usage();
   string host = get_cix_server_host (args, 0);
   in_port_t port = get_cix_server_port (args, 1);
   log << to_string (hostinfo()) << endl;
   try {
      log << "connecting to " << host << " port " << port << endl;
      client_socket server (host, port);
      log << "connected to " << to_string (server) << endl;
      for (;;) {
         string line;
         string file;
         getline (cin, line);
         if (cin.eof()) throw cix_exit();
         if (SIGINT_throw_cix_exit) throw cix_exit();
    auto it = line.find_first_of(" ");
    if(it != string::npos){
      file = line.substr(++it);
      line = line.substr(0, it-1);
    }
         log << "command " << line << endl;
         const auto& itor = command_map.find (line);
         cix_command cmd = itor == command_map.end()
                         ? CIX_ERROR : itor->second;
         switch (cmd) {
            case CIX_EXIT:
               throw cix_exit();
               break;
            case CIX_HELP:
               cix_help();
               break;
            case CIX_LS:
               cix_ls (server);
               break;
           case CIX_GET:
              cix_get(server, file);
              break;
           case CIX_PUT:
              cix_put(server, file);
              break;
           case CIX_RM:
              cix_rm(server, file);
              break;
            default:
               log << line << ": invalid command" << endl;
               break;
         }
      }
   }catch (socket_error& error) {
      log << error.what() << endl;
   }catch (cix_exit& error) {
      log << "caught cix_exit" << endl;
   }
   log << "finishing" << endl;
   return 0;
}


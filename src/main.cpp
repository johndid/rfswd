#include <iostream>
#include <stdlib.h>
#include <signal.h>
#include <stdarg.h>
#include <netdb.h>
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

/// These are the programs exit reasons
enum exit_values {
    success = EXIT_SUCCESS, ///< Need I say more?
    bad_option_passed, ///< incorrect opion passed to the program
    socket_error, ////< the switchboard could not start a listening socket
};

/// the usage function prints the switchboards usage string.
void usage() {
    cout << "switchboard [option] argument" << endl;
    cout << "where:" << endl;
    cout << "-d domain optional argument will use $hostname if not present! " << endl;
    cout << "-h hostname optional argument will use $hostname if not present" << endl;
    cout << "-l logfile optional argument, will use switchboard.log if not present" << endl;
    cout << "-p port optional argument will use 8080 if not present" << endl;
}

/// The parse options function, extracts the arguments as given from the command line
/// Each argument for the switchboard is passed to the function, for creation
/// The individual argument have different meanings and must be checked against these.
///
/// all arguments are optional, and if not passed they'll obtain a default argument value
/// see the usage function for details!
///
/// @param argc int as presented by the main() function
/// @param argv char ** as presented by the main() function
/// @param host string to fill with the host name
/// @param domain string to fill with the domain
/// @param post string tp fill withe the port number
/// @param log string to fill with the log file
/// @return true if the options are passed correctly, false if there is an error
bool parse_program_options(int argc,char ** argv,string &host, string &domain, string &port, string &log) {
    // argument string for the getopt function, containing all legal options
    // the arguments follow the options explained above, the : is used by
    // getopt to indicate the the option needs an tailing argument
    // i.e. -h hostname 
    const char *switchboard_options = "h:e:l:p:d:"; // lmao
    int c; // temporary place holder used to extract the individual options with getopt, see: man getopt for details
    char len=80; // buffer length, used with buffer, to extract host and domain names from the machiene running the application
    char buffer[len]; 

    // read the machiene defaults for host and domain! These should be overwritten with care
    // as the switchboard should use the machiene name and current domain for this
    // The switchoard uses these values in the replies to its clients. They are 
    // inserted into the XML messages in the <!DOCTYPE/> tag to allow clients to validate
    // against the DTD's served by the switcboard
    if(0 == gethostname(buffer,len)) host = string(buffer);
    if(0 == getdomainname(buffer,len)) domain = string(buffer);

    opterr = 0;
    while ((c = getopt(argc, argv, switchboard_options)) != -1) {
 	switch (c) {
	case 'd':
	    domain = string(optarg);
	    break;
	case 'h':
	    host = string(optarg);
	    break;
	case 'l':
	    log = string(optarg);
	    break;
	case 'p':
	    port = string(optarg);
	    break;
	case '?':
	    if (optopt == 'd' || optopt == 'e' || optopt == 'h' || optopt == 'l' || optopt == 'p') {
		printf("Option -%c requires an argument.\n", optopt);
	    }
	    else if (isprint (optopt)) {
		printf("Unknown option `-%c'.\n", optopt);
	    }
	    else {
		printf("Unknown option character `\\x%x'.\n", optopt);
	    }
        /* Fall through */
	default:
	    return false;
	}
    }
    return true;
}

/// This function checks the existance of the engine model folder, and returns false if the folder does not
/// exist, or if the program does not have permissions to change the folder!
/// @param engine folder path
/// @return true if the engine folder exists and the program have permissions to write in it, false if not
bool check_engine_folder(const string& engine)
{
    struct stat sb;
    if(-1 < stat(engine.c_str(),&sb)) {
	if(S_IFDIR == (sb.st_mode & S_IFMT)) {
	    if (0 == access(engine.c_str(),F_OK|R_OK|W_OK|X_OK)) {
		return true;
	    }
	    else {
		cout << "process: " << getpid() << " don't have permissions to access: " << engine.c_str() << endl;
	    }
	}
	else {
	    cout << engine.c_str() << "Is not a directory!" << endl;
	}
    }
    else {
	cout << "No engine folder!" << endl;
    }
    return false;
}

// TODO: create signal handling in such a way that it is sane to exit with ctrl+c and kill!
void signal_int(int signum)
{
  // TODO: this is a test for shutdown, there are some stop issues with socket::accept and a leaking incoming connection thread
  // these issues have not yet been resolved! 

  // NOTICE: This exit is only present for development to cancel the application
  // When running with out attempting to resolve the stop issue keep this section
  // present, it leaks 136 bytes due to a thread not being joined correctly?
  // because the thread is waiting in socket::accept, and the socket is blocking
  exit(signum);
};

// TODO: this may be hanged to be in a configuration file !?!!?
/// Function reads the host_ip from the host name
/// @param host std::string containing the hostname
/// @return std::string containing the looked up host name
const char* get_host_ip(const char *host)
{
  if(host) {
    hostent * record = gethostbyname(host);
    if(record){
      in_addr* address = (in_addr*)record->h_addr;
      if(address) {
	return inet_ntoa(*address);
      }
    }
  }
  return "127.0.0.1";
}

/// This enumeration carries the number of arguments that can be passed to the main program.
enum arguments
{
  no_args = 0, ///< No arguments passed to the application (except the standard arg present in 0 namely the application name)
  host_name,
  port_number, ///< The portnumber that the socket should be bound to
  log_file,
  max_args ///< the number of args that must be passed for the program to work
};

string domain, host, port("8080"), log("rfswd.log");

///@b main
///This is the main application thread!
///it is responsible for parsing the options to the program,
///as well as starting and stopping the switchboard
///@param argc integer containing the number of arguments passed to the program
///@param argv char pointer pointer to a string that contains an array of strings passed to the program
///@return integer
int main(int argc, char **argv)
{
    signal(SIGINT, &signal_int);
    signal(SIGTERM, &signal_int);

    if(parse_program_options(argc,argv,host,domain,port,log)) {
      const int log_size = 100000;
      const int port_no = atoi(port.c_str());
      const unsigned int num_connections = 10;

      // TODO: all arguments passed should be checked for their sanity here!
      // TODO: directory and port sanity checks should be done here after the options has been passed!
      // TODO: create an argument parser to ensure that everything is a ok!
      
      cout << argv[0] << "@" << host.c_str() << "." << domain.c_str() << ":" << port << endl;
      cout << "logging to: " << log << endl;
      
    }
    else {
      usage();
      return bad_option_passed;
    }
    return success;
}

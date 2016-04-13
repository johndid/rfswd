#ifndef file_h_
#define file_h_

#include <sys/stat.h>
#include <time.h>
#include <string>

///@b File
///This class implements a simple file interface to files. I really disaprove of the c++ streams as they
///seem to add too much work for simpe file reads. hence this class
///To use this class to read a file and get it's results in a string simply:
///
///file f;
///string file_contents;
///if(f.read("path to my file",file_contents)) {
///  // file_contents is valid!
///}
///else {
///  // file_contents aren't valid!
///}
class file 
{
 public:
  file();
  ~file();
    
  ///@b read
  ///This function reads the specified file and returns it's contents to a string
  ///@param filename std::string containing a valid file name
  ///@param output std::string referece to an existing buffer ready to receive the file's contents a a c++ string
  ///@return boolean true if the file was read sucesfully false if not
  const bool read(const std::string &filename,std::string &output);

  ///@b write
  ///This function writes a string to a file. It erases the file's contents completely on write, so you need
  ///to make sure that you're creating a backup of the file's contents before writing.
  ///@param filename std::string containing a valid filename
  ///@param data string containing the contents to write
  ///@return bool true when the file was succesfully written, false when not
  const bool write(const std::string &filename,const std::string &data);

  ///@b append
  ///This function appends a string to the file.
  ///@param filename string containing a valid filename
  ///@param data string containing the contents to append
  ///@return bool true when the file was succesfully written, false when not
  const bool append(const std::string &filename,const std::string &data);

  /// This function checks for file existance
  /// @param filename std::string to test existance against
  /// @return true if the file exist false if not
  const bool exist(const std::string &filename);

  /// This function finde the file size in bytes from the file name
  /// @param filename std::string with a valid file name
  /// @return unsigned long carrying the number of bytes for the file
  const unsigned long size(const std::string &filename);

  // these functions require exist executed before they are used
    
  const unsigned long size() const; ///< see size above
  const time_t modified() const; ///< This function returns the file modification time
  inline const struct stat *stat_info() const; ///< This function returns the stat structure

  // these functions require one read or write before its non NULL
  inline FILE *const handler() const; ///< This creates a file handler

 private:
    
  /// This function opens a file in the specified mode
  /// @param filename std::string carrying the filename
  /// @param mode std::string carrying the file open mode, see (man fopen) for details on open operators
  /// @return true if the file was opened, false if not
  const bool open(const std::string &filename,const std::string &mode);

  /// This function fill's the file contents into a string (private edition)
  /// @param size unsigned long bytes that was read
  /// @return a std::string containing the file contents
  const std::string read(const unsigned long size)const; 

  /// This function writes the file contents
  /// @param data std::string of the data to write
  /// @return true if write was a success, false if write failed
  const bool write(const std::string &data);

  const unsigned long get_size()const ; ///< returns the file the size of a standard *nix FILE structure
  inline void to_start()const; ///< seeks file to start
  inline void to_end()const; ///< seeks file to end

  FILE *fhandler; ///< This member holds the standard *nix file handle  see: man FILE for details
  struct stat info; ///< This member holds the standard *nix file descriptor see: man stat for details
};
#endif // file_h_

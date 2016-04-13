#include <file.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>

#include <iostream>

using namespace std;

file::file():fhandler(NULL)
{
  memset(&info,0x00,sizeof(info));
}

file::~file() 
{
  if(fhandler) {
    fclose(fhandler);
  }
}

const bool file::read(const string &filename,string &output)
{
  if(open(filename.c_str(), "r")) {
    output = read(get_size());
    return (!output.empty());
  }
  cout << "file::read -> failed - Couldn't open: " << filename.c_str() << endl ;
  return false;
}

const bool file::write(const string &filename, const string &data)
{
  if(open(filename.c_str(), "w")) {
    const bool ret = write(data.c_str());
    to_end();
    return ret;
  }
  cout << "file::write -> failed - Couldn't open: " << filename.c_str() << endl;
  return false;
}

const bool file::append(const std::string &filename,const std::string &data)
{
  if(open(filename.c_str(), "a")) {
    const bool ret = write(data.c_str());
    to_end();
    return ret;
  }
  cout << "file::append -> failed - Couldn't open: " << filename << endl;
  return false;
}

const bool file::exist(const std::string &filename)  
{
  const bool ret = (-1 < stat(filename.c_str(),&info)) ? true:false;
  return ret;
}

const unsigned long file::size(const std::string &filename)
{
  if(exist(filename)) {
    return size();
  }
  cout << "file::size -> failed - no file named: " << filename.c_str() << endl;
  return 0;
}

const time_t file::modified() const
{
  return info.st_mtime;
}

const unsigned long file::size() const 
{
  return info.st_size;
}

const struct stat *file::stat_info() const 
{
  return &info;
}

FILE *const file::handler() const 
{
  return fhandler;
}

// Private function implementation starts here
const bool file::open(const std::string &filename,const std::string &mode) 
{
  fhandler = fopen(filename.c_str(), mode.c_str());
  return (NULL == fhandler)?false:true;
}

const string file::read(const unsigned long size) const 
{
  string ret;
  char *buffer = new char[size+1];
  if (buffer) {
    const unsigned long bytes = fread(buffer, size, 1, fhandler);
    if(bytes) {
      buffer[size] = '\0';
      ret = string(buffer);
    }
    delete [] buffer;
  }
  else {
    cout << "Memory error!" << endl;
  }
  return ret;
}

const bool file::write(const std::string &data) 
{
  const int data_size = static_cast<int>(data.length());
  if(data_size) {
    const int num_bytes = fwrite(data.c_str(),sizeof(char),data_size,fhandler);
    
    if(num_bytes == data_size) {
      return true;
    }
    cout << "file::write -> failed - Couldn't write: " << data_size << " bytes, only: " << num_bytes << " was written!" << endl;
    return false;
  }
  cout << "file::write -> failed - data string empty" << endl;
  return false;
}

const unsigned long file::get_size() const
{
  to_end();
  const unsigned long length = ftell(fhandler);
  to_start();

  return length;
}

void file::to_start() const
{
  fseek(fhandler, 0, SEEK_SET);
}

void file::to_end() const
{
  fseek(fhandler, 0, SEEK_END);
}

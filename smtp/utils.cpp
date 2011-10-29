#include "utils.h"


using namespace std;


string smtp::utils::read_file(string filename) {

  FILE *fp = fopen(filename.c_str(), "rb");
  if (!fp) {
    throw BaseException("File not found.");
    }
  char *buf = NULL;

  fseek(fp, 0, SEEK_END);
  int length=ftell(fp);
  fseek(fp, 0, SEEK_SET);

  buf = new char [length + 1];

  fread(buf, 1, length, fp);
  fclose(fp);

  string data = string(buf, length);
  delete [] buf;

  return data;
  }

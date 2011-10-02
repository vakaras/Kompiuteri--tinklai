#ifndef DICT_H
#define DICT_H 1


#include <cstdio>
#include <string>
#include <cstring>


namespace dict {

template<class T> class Dict {

private:

  std::string filename;
  FILE *fp;

public:

  Dict(std::string filename) {
    
    this->fp = fopen(filename.c_str(), "rb");

    }

  void search(std::string query, T &list) {
    fseek(this->fp, 0, 0);

    char row[MAX_CHAR << 2];

    int found = 0;

    while (1) {

      fgets(row, MAX_CHAR << 2, this->fp);

      bool match = true;
      char *i = row;
      for (int j = 0; j < query.length(); j++) {
        char a = tolower(*i);
        char b = tolower(query[j]);
        if (a < b) {
          match = false;
          break;
          }
        else if (a > b) {
          return;
          }
        i++;
        }

      if (match) {
        while (*i != '=')
          i++;
        *i = 0;
        i++;
        list.append(row, i);
        found ++;
        if (found >= 10) {
          return;
          }
        }
      }

    }

  };

};

#endif

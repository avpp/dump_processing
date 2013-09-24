#ifndef CFG_H
#define CFG_H

#include <vector>

using namespace std;

class Config
{
public:
  Config(char* fileName);
  ~Config();
  char* getProp(const char* propName);
private:
  char* s;
  vector<vector<char*> > m;
};

#endif

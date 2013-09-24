#include "cfg.h"
#include <fstream>
#include <stdlib.h>
#include "misc.h"
#include <vector>
#include <string.h>

using namespace std;

Config::Config(char* fileName)
{
  ifstream f(fileName);
  if (!f)
  {
    ofstream ff("./!ERROR.txt");
    ff<<"ERROR, no config file\n";
    ff.close();
    exit(1);
  }
  long sz = 0;
  f.seekg(0, ios_base::end);
  sz = f.tellg();
  s = new char [sz+1];
  f.seekg(0, ios_base::beg);
  f.getline(s, sz, EOF);
  vector<char*> v;
  createTokenVector(s, v, "\n");
  for (vector<char*>::iterator itr = v.begin(); itr != v.end(); itr++)
  {
    int i = 0;
    if (!(*itr)[i])
      continue;
    while (charInSet((*itr)[i], " =:"))
      i++;
    char* k = &((*itr)[i]);
    if(!(*itr)[i])
      continue;
    while (!charInSet((*itr)[i], " =:"))
      i++;
    (*itr)[i] = 0;
    i++;
    if (!(*itr)[i])
      continue;
    while (charInSet((*itr)[i], " =:"))
      i++;
    char* v = &((*itr)[i]);
    vector<char*> p;
    p.push_back(k);
    p.push_back(v);
    m.push_back(p);
  }
  f.close();
  
}

Config::~Config()
{
  delete [] s;
}

char* Config::getProp(const char* propName)
{
  for (vector<vector<char*> >::iterator itr = m.begin(); itr != m.end(); itr++)
  {
    if (!strcmp(propName, itr->front()))
      return itr->back();
  }
  return NULL;
}

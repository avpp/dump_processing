#include <iostream>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <algorithm>
#include "mac_description.h"
#include "misc.h"

using namespace std;

bool charInSet(char c, const char* set)
{
  int i = 0;
  while (set[i] && set[i] != c)
    i++;
  return set[i];
}

void createTokenVector(char* s, vector<char*> &v, const char* t)
{
  int i=0;
  while (true)
  {
    while (charInSet(s[i], t))
      if (s[i]) i++; else return;
    v.push_back(&(s[i]));
    while (!charInSet(s[i], t))
      if (s[i]) i++; else return;
    s[i]=0;
    i++;
  }
}

bool MACinList(MACaddr a, vector<MACaddr> l)
{
  vector<MACaddr>::iterator f = find(l.begin(), l.end(), a);
  return f != l.end();
}


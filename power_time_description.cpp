#include <stdio.h>
#include <string.h>
#include "power_time_description.h"
#include "misc.h"
#include <iostream>
#include <time.h>

using namespace std;

bool tryParsePower(char* s, Power &p)
{
  int i = 0;
  bool minus = false;
  if (s[0] == '-')
  {
    minus = true;
    i++;
  }
  int r = 0;
  while (charInSet(s[i], "0123456789"))
  {
    r *= 10;
    r +=s[i]-'0';
    i++;
  }
  if (s[i] == 'd' && s[i+1] == 'B' && s[i+2] == 0)
  {
    p = 0xFF & r;
    return true;
  }
  return false;
}
/*
ostream& operator << (ostream &s, Power p)
{
  s<<((int)p)<<"dB";
  return s;
}

ostream& operator << (ostream &s, Time t)
{
  s<<(long)t<<"."<<(long)((t-(long)t)*1000);
  return s;
}
*/

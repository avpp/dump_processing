#include <iostream>
#include <stdio.h>
#include "mac_description.h"

using namespace std;

  unsigned char& MACaddr::operator [ ] (int n)
  {
    return addr[n];
  }

  MACaddr::MACaddr()
  {
    for (int i = 0; i < 6; i++)
    {
      addr[i] = 0;
    }
  }
  
  MACaddr::MACaddr(int a)
  {
    for (int i = 0; i < 6; i++)
      addr[i] = (unsigned char) (0xFF && a>>8*(5-i));
  }

  MACaddr::MACaddr(char* s, bool *result)
  {
    bool rslt;
    if (result == NULL)
      result = &rslt;
    *result = true;
    unsigned char b[6];
    for (int j = 0; j < 6; j++)
    {
      int z;
      if (1 != sscanf(&(s[j*3]), "%x", &z))
        *result = false;
      b[j] = 0xFF & z;
    }
    if (*result)
      for (int i = 0; i < 6; i++)
        addr[i] = b[i];
  }

  MACaddr::operator int () const
  {
    int r = 0;
    for (int i = 0; i < 6; i++)
    {
      r <<= 8;
      r |= addr[i];
    }
    return r;
  }

  unsigned char* MACaddr::getCompress()
  {
    return addr;
  }
  void MACaddr::setCompress(char* a)
  {
    for (int i = 0; i < 6; i++)
      addr[i] = a[i];
  }

ostream& operator << (ostream &s, MACaddr a)
{
  s<<hex;
  for (int i = 0; i < 6; i++)
    s<<((i)?":":"")<<((a[i]<0x10)?"0":"")<<(int)a[i];
  s<<dec;
  return s;
}

bool tryParseMAC(char* s, const char *prefix, MACaddr &a)
{
  int i = 0;
  while (prefix[i])
    if (s[i] != prefix[i])
      return false;
    else i++;
  bool r;
  MACaddr b(&(s[i]), &r);
  if (r)
    a = b;
  return r;
}


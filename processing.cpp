#include <iostream>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <time.h>
#include <algorithm>

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

//typedef unsigned char MACaddr[6];
struct MACaddr
{
  unsigned char addr[6];
  unsigned char& operator [ ] (int n)
  {
    return addr[n];
  }
  MACaddr()
  {
    for (int i = 0; i < 6; i++)
    {
      addr[i] = 0;
    }
  }
  
  MACaddr(int a)
  {
    for (int i = 0; i < 6; i++)
      addr[i] = (unsigned char) (0xFF && a>>8*(5-i));
  }
  MACaddr(char* s, bool *result = NULL)
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
  operator int () const
  {
    int r = 0;
    for (int i = 0; i < 6; i++)
    {
      r <<= 8;
      r |= addr[i];
    }
    return r;
  }
};

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

bool MACinList(MACaddr a, vector<MACaddr> l)
{
  vector<MACaddr>::iterator f = find(l.begin(), l.end(), a);
  return f != l.end();
}

typedef char Power;

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

typedef double Time;

int main()
{
  clock_t t;
  vector<MACaddr> ignoreList;
  vector<MACaddr> trustedList;
  trustedList.push_back(MACaddr("84:c9:b2:07:9d:0a"));
  char *input = new char [1024];
  
  while (true)
  {
    //cout<<"CLOCK   "<<clock()<<"\n\n\n";
    cin.getline(input, 1023, '\n');
    vector<char*> tokens;
    createTokenVector(input, tokens, " \t");
    if (tokens[0] == "tcpdump:")
      continue;
    MACaddr a;
    Power p;
    Time t;
    sscanf(tokens[0], "%lf", &t);
    bool MACdetected = false, PowerDetected = false;
    for (vector<char*>::iterator itr = tokens.begin(); itr != tokens.end() && !(MACdetected && PowerDetected); itr++)
    {
      if (!MACdetected && tryParseMAC(*itr, "SA:", a))
        MACdetected = true;
      else
      if (!PowerDetected && tryParsePower(*itr, p))
        PowerDetected = true;
    }
    if (!PowerDetected || !MACdetected)
      continue;
    cout<<t<<" "<<a<<" "<<(int)p<<endl;
    if (MACinList(a, ignoreList))
      continue;
    else if (MACinList(a, trustedList))
    {
      cout<<"^^^ this is trusted MAC ^^^\n\n";
      
    }

  }
  delete [] input;
}

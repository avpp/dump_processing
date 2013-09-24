#ifndef MAC_DESCRIPTION_H
#define MAC_DESCRIPTION_H

#include <iostream>
#include <stdio.h>

using namespace std;

class MACaddr
{
private:
  unsigned char addr[6];
public:
  MACaddr();
  MACaddr(int a);
  MACaddr(char* s, bool *result = NULL);
  operator int () const;
  unsigned char& operator [ ] (int n);
  unsigned char* getCompress();
  unsigned char*& getCompress(unsigned char* m);
  void setCompress(char* a);
};

ostream& operator << (ostream &s, MACaddr a);

bool tryParseMAC(char* s, const char *prefix, MACaddr &a);

#endif

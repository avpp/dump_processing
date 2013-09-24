#ifndef POWER_TIME_DESCRIPTION_H
#define POWER_TIME_DESCRIPTION_H

#include <iostream>

using namespace std;

typedef char   Power;
typedef double Time;

bool tryParsePower(char* s, Power &p);

/*ostream& operator << (ostream &s, Power p);
ostream& operator << (ostream &s, Time t);*/
#endif

#ifndef DUMPDATA_H
#define DUMPDATA_H

#include "mac_description.h"
#include "power_time_description.h"
#include <vector>
#include <map>
#include <stdio.h>

class DumpData
{
  public:
    DumpData();
    bool flushToFile(char *fileName);
    bool loadFromFile(char *fileName);
    void clearData();
    vector<MACaddr> getAllMAC();
    vector<pair<Time, Power> > getInfoAboutMAC(MACaddr a);
    void addInfoAboutMAC(MACaddr a, Time t, Power p);

  private:
    map<MACaddr, vector<pair<Time, Power> > > m;
    void addInfoToFile(FILE *f);
    bool readInfoFromFile(FILE *f);
};

#endif

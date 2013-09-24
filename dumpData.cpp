#include "dumpData.h"
#include <stdio.h>
#include "mac_description.h"
#include <vector>
#include <map>
#include <errno.h>

DumpData::DumpData()
{
}

bool DumpData::flushToFile(char *fileName)
{
  FILE* f = fopen(fileName, "ab");
  if (f == NULL)
  {
    cout<<"\n!!!!!!!!!!!Error open file while write data.\n";
    cout<<"Errno: "<<errno<<endl;
    return false;
  }
  addInfoToFile(f);
  fclose(f);
  return true;
}

bool DumpData::loadFromFile(char *fileName)
{
  FILE* f = fopen(fileName, "rb");
  if (f == NULL)
  {
    cout<<"\n!!!!!!!!!!!Error open file while read data.\n";
    cout<<"Errno: "<<errno<<endl;
    return false;
  }
  readInfoFromFile(f);
  fclose(f);
  return true;
}

void DumpData::clearData()
{
  m.clear();
}

/*
all data represent as array of:
[
  MAC = 6 byte
  InitTime = double = 8 byte
  Amount of data records = unsigned long = 4 byte
  Array of datarecords:
  [
    delta time = long = 4 byte
    power value = unsigned char = 1 byte
  ]
]
*/
void DumpData::addInfoToFile(FILE *f)
{
  map<MACaddr, vector<pair<Time, Power> > >::iterator itr;
  for (itr = m.begin(); itr != m.end(); itr++)
  {
    vector<pair<Time, Power> >::iterator vitr;
    MACaddr a = itr->first;
    fwrite(a.getCompress(), 6, 1, f);
    double lastTime = itr->second.front().first;
    fwrite(&lastTime, sizeof(Time), 1, f);
    unsigned long sz = itr->second.size();
    fwrite(&sz, sizeof(sz), 1, f);
    for (vitr = itr->second.begin(); vitr != itr->second.end(); vitr++)
    {
      long dt = (long)((vitr->first - lastTime)*1000);
      lastTime = vitr->first;
      fwrite(&dt, sizeof(long), 1, f);
      fwrite(&(vitr->second), sizeof(Power), 1, f);
    }
  }
}

void DumpData::readInfoFromFile(FILE *f)
{
  while (true)
  {
    char pack_mac[6];
    if (fread(pack_mac, 6, 1, f) != 1)
      return;
    MACaddr a;
    a.setCompress(pack_mac);
    Time lastTime;
    unsigned long sz;
    fread(&lastTime, sizeof(Time), 1, f);
    fread(&sz, sizeof(unsigned long), 1, f);
    for (int i = 0; i < sz; i++)
    {
      long dt;
      Power p;
      fread(&dt, sizeof(long), 1, f);
      fread(&p, sizeof(Power), 1, f);
      lastTime += 0.001*dt;
      addInfoAboutMAC(a, lastTime, p);
    }
  }
}

vector<MACaddr> DumpData::getAllMAC()
{
  vector<MACaddr> v;
  for (map<MACaddr, vector<pair<Time, Power> > >::iterator itr = m.begin(); itr != m.end(); itr++)
    v.push_back(itr->first);
}

vector<pair<Time, Power> > DumpData::getInfoAboutMAC(MACaddr a)
{
  return m[a];
}

void DumpData::addInfoAboutMAC(MACaddr a, Time t, Power p)
{
  m[a].push_back(pair<Time, Power>(t, p));
}


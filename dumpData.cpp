#include "dumpData.h"
#include <stdio.h>
#include "mac_description.h"
#include <vector>
#include <map>
#include <errno.h>
#include <stdint.h>

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

template <typename T>
void var2charArray(T l, char *c)
{
  for (int i = 0; i < sizeof(T); i++)
  {
    c[i] = 0xFF & (l>>(i*8));
  }
}

template <typename T>
void  charArray2var(char *c, T &l)
{
  l = 0;
  for (int i = 0; i < sizeof(T); i++)
  {
    l |= (((T)(c[i]))&0xFF)<<(i*8);
  }
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
  char* buf = new char[8];
  for (itr = m.begin(); itr != m.end(); itr++)
  {
    vector<pair<Time, Power> >::iterator vitr;
    MACaddr a = itr->first;
    fwrite(a.getCompress(), 1, 6, f);
    double lastTime = itr->second.front().first;
    int64_t *lt = (int64_t*)(&lastTime);
    var2charArray(*lt, buf);
    fwrite(buf, 1, 8, f);
    uint32_t sz = itr->second.size();
    var2charArray(sz, buf);
    fwrite(buf, 1, 4, f);
    for (vitr = itr->second.begin(); vitr != itr->second.end(); vitr++)
    {
      int32_t dt = (int32_t)((vitr->first - lastTime)*1000);
      lastTime = vitr->first;
      var2charArray(dt, buf);
      fwrite(buf, 1, 4, f);
      fwrite(&(vitr->second), 1, 1, f);
    }
  }
  delete [] buf;
}

void DumpData::readInfoFromFile(FILE *f)
{
  char *buf = new char [8];
  while (true)
  {
    char pack_mac[6];
    if (fread(pack_mac, 1, 6, f) != 6)
      break;
    MACaddr a;
    a.setCompress(pack_mac);
    Time lastTime;
    uint32_t sz;
    fread(buf, 1, 8, f);
    int64_t *lt = (int64_t*)(&lastTime);
    charArray2var(buf, *lt);
    fread(buf, 1, 4, f);
    charArray2var(buf, sz);
    for (int i = 0; i < sz; i++)
    {
      int32_t dt;
      Power p;
      fread(buf, 1, 4, f);
      charArray2var(buf, dt);
      fread(&p, 1, 1, f);
      lastTime += 0.001*dt;
      addInfoAboutMAC(a, lastTime, p);
    }
  }
  delete [] buf;
}

vector<MACaddr> DumpData::getAllMAC()
{
  vector<MACaddr> v;
  for (map<MACaddr, vector<pair<Time, Power> > >::iterator itr = m.begin(); itr != m.end(); itr++)
    v.push_back(itr->first);
  return v;
}

vector<pair<Time, Power> > DumpData::getInfoAboutMAC(MACaddr a)
{
  return m[a];
}

void DumpData::addInfoAboutMAC(MACaddr a, Time t, Power p)
{
  m[a].push_back(pair<Time, Power>(t, p));
}


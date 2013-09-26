#include "mac_description.h"
#include "power_time_description.h"
#include "dumpData.h"
#include "misc.h"
#include <vector>
#include <iostream>

using namespace std;

int main(int argc, char** argv)
{
  if (argc != 2)
  {
    cout<<"You should send one argument - file name.";
    return 0;
  }
  DumpData dd;
  dd.loadFromFile(argv[1]);
  vector<MACaddr> v = dd.getAllMAC();
  for (vector<MACaddr>::iterator itr = v.begin(); itr != v.end(); itr++)
  {
    vector<pair<Time, Power> > i = dd.getInfoAboutMAC(*itr);
    cout<<"\n"<<*itr<<" : ";
    int cnt = 0;
    for (vector<pair<Time, Power> >::iterator vitr = i.begin(); vitr != i.end(); vitr++)
    {
      if (!((cnt++)%7)) cout<<endl;
      cout<<"{"<<(Time)(vitr->first)<<", "<<(int)(Power)(vitr->second)<<"dB}; ";
    }
    cout<<"\n\n********************************\n\n";
  }
}

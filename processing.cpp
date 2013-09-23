#include <iostream>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <time.h>
#include <algorithm>
#include "mac_description.h"
#include "power_time_description.h"
#include "misc.h"
#include "dumpData.h"

using namespace std;

void intDown()
{
}

void intUp()
{
}

int main()
{
  clock_t lastTime = time(NULL);
  
  vector<MACaddr> ignoreList;
  vector<MACaddr> trustedList;
  char *OUTPUT_FILE_NAME = NULL;
  char *OUTPUT_RESULT_NAME = NULL;
  double UP_PERIOD, FLUSH_PERIOD;

  DumpData dd;
  char *input = new char [1024];
  double timeToFlush = -1;
  double timeToIntDown = -1;

  while (true)
  {
    //Parse input
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

    //cout<<t<<" "<<a<<" "<<(int)p<<endl;

    //MAC check
    if (MACinList(a, ignoreList))
      continue;
    else if (MACinList(a, trustedList))
    {
    //  cout<<"^^^ this is trusted MAC ^^^\n\n";
      dd.flushToFile(OUTPUT_FILE_NAME);
      dd.clearData();
      remove(OUTPUT_RESULT_NAME);
      rename(OUTPUT_FILE_NAME, OUTPUT_RESULT_NAME);
      intUp();
      timeToIntDown = UP_PERIOD;
    }
    else
      dd.addInfoAboutMAC(a, t, p);
    
    //Time manager.
    time_t curTime = time(NULL);
    double dt = difftime(curTime, lastTime);
    lastTime = curTime;
    if (dt > 0)
    {
      if (timeToFlush > 0)
      {
        timeToFlush -= dt;
        if (timeToFlush <= 0)
        {
          dd.flushToFile(OUTPUT_FILE_NAME);
          dd.clearData();
          timeToFlush = FLUSH_PERIOD;
        }
      }
      if (timeToIntDown > 0)
      {
        timeToIntDown -= dt;
        if (timeToIntDown <= 0)
           intDown();
      }
    }
  }
  delete [] input;
}

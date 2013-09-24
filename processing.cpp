#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <time.h>
#include <algorithm>
#include "mac_description.h"
#include "power_time_description.h"
#include "misc.h"
#include "dumpData.h"
#include "cfg.h"

using namespace std;

void intDown()
{
}

void intUp()
{
}

void readListFromFile(char *fileName, vector<MACaddr> &v)
{
  v.clear();
  if (fileName == NULL)
    return;
  ifstream f(fileName);
  if (!f)
    return;
  long sz = 0;
  f.seekg(0, ios_base::end);
  sz = f.tellg();
  f.seekg(0, ios_base::beg);
  char* s = new char [sz+1];
  f.getline(s, sz, EOF);
  f.close();
  vector<char*> sv;
  createTokenVector(s, sv, "\n");
  for (vector<char*>::iterator itr = sv.begin(); itr != sv.end(); itr++)
  {
    MACaddr a(*itr);
    v.push_back(a);
  }
  delete [] s;
}

char *CONFIG_FILE_NAME="config.cfg";

int main()
{
  clock_t lastTime = time(NULL);
  
  Config cfg(CONFIG_FILE_NAME);
  
  char *OUTPUT_FILE_NAME = cfg.getProp("output");
  char *RESULT_FILE_NAME = cfg.getProp("result");
  char *ignoreFileName = cfg.getProp("ignore");
  char *trustedFileName = cfg.getProp("trusted");
  char *up_period_str = cfg.getProp("up_period");
  char *flush_period_str = cfg.getProp("flush_period");
  vector<MACaddr> ignoreList;
  vector<MACaddr> trustedList;
  readListFromFile(ignoreFileName, ignoreList);
  readListFromFile(trustedFileName, trustedList);
  double UP_PERIOD = 5*60, FLUSH_PERIOD = 60;
  if (up_period_str)
    sscanf(up_period_str, "%lf", &UP_PERIOD);
  if (flush_period_str)
    sscanf(flush_period_str, "%lf", &FLUSH_PERIOD);

  cout<<"Successfully read config file '"<<CONFIG_FILE_NAME<<"'.\n";
  cout<<"Temporary output file name is: ";
  if (OUTPUT_FILE_NAME == NULL)
  {
    cout<<"Error! No output file name.\n";
    exit(1);
  }
  cout<<"'"<<OUTPUT_FILE_NAME<<"'\n";
  
  cout<<"Result file name is: ";
  if (RESULT_FILE_NAME == NULL)
  {
    cout<<"Error! No result file name.\n";
    exit(1);
  }
  cout<<"'"<<RESULT_FILE_NAME<<"'\n";


  DumpData dd;
  char *input = new char [1024];
  double timeToFlush = FLUSH_PERIOD;
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
      remove(RESULT_FILE_NAME);
      rename(OUTPUT_FILE_NAME, RESULT_FILE_NAME);
      intUp();
      timeToIntDown = UP_PERIOD;
    }
    else
      dd.addInfoAboutMAC(a, t, p);
    
    //Time manager.
    time_t curTime = time(NULL);
    double dt = difftime(curTime, lastTime);
    lastTime = curTime;
    cout<<"\r"<<curTime;
    if (dt > 0)
    {
      if (timeToFlush > 0)
      {
        timeToFlush -= dt;
        if (timeToFlush <= 0)
        {
          cout<<"flush to file\n"<<flush;
          if (dd.flushToFile(OUTPUT_FILE_NAME))
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

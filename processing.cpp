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
cout<<"\nWi-Fi should down\n";
}

void intUp()
{
cout<<"\nWi-Fi should up\n";
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

MACaddr getCurMAC()
{
  return MACaddr("F8:1A:67:D9:F5:1E");
}

char* generateFileNamePrefix(char* directory)
{
  MACaddr a = getCurMAC();
  int prefixSize = strlen(directory);
  prefixSize += 6*2 + 5 + 1 + 4;
  char *prefix = new char[prefixSize];
  sprintf(prefix, "%s/%x-%x-%x-%x-%x-%x_", directory, a[0], a[1], a[2], a[3], a[4], a[5]);
  return prefix;
}

bool fileExist(char* fname)
{
  FILE* f = fopen(fname, "r");
  if (f == NULL)
    return false;
  fclose(f);
  return true;
}

int lastFileNum = 1;
char* RESULT_FILE_NAME;
char* getNextFileName(char* filePrefix)
{
  if (RESULT_FILE_NAME)
    delete [] RESULT_FILE_NAME;
  int endPrefix = strlen(filePrefix);
  RESULT_FILE_NAME = new char [endPrefix + 15];
  strcpy(RESULT_FILE_NAME, filePrefix);
  char* num = new char [15];
  sprintf(num, "%d.dump", 1);
  strcat(RESULT_FILE_NAME, num);
  if (!fileExist(RESULT_FILE_NAME))
    lastFileNum = 0;
  else
    lastFileNum--;
  do
  {
    lastFileNum++;
    sprintf(num, "%d.dump", lastFileNum);
    RESULT_FILE_NAME[endPrefix] = 0;
    strcat(RESULT_FILE_NAME, num);
  }
  while (fileExist(RESULT_FILE_NAME));
  delete [] num;
  return RESULT_FILE_NAME;
}

char *CONFIG_FILE_NAME="config.cfg";

int main()
{
  RESULT_FILE_NAME = NULL;
  bool pointUp = false;
  clock_t lastTime = time(NULL);
  
  Config cfg(CONFIG_FILE_NAME);
  
  char *DUMP_COMMAND_FILE = cfg.getProp("dump_command");

  char *OUTPUT_FILE_NAME = cfg.getProp("output");
  char *RESULT_FILE_DIR = cfg.getProp("result");
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
  
  cout<<"Result file name prefix is: ";
  if (RESULT_FILE_DIR == NULL)
  {
    cout<<"Error! No result file name.\n";
    exit(1);
  }
  char* RESULT_FILE_NAME_PREFIX = generateFileNamePrefix(RESULT_FILE_DIR);
  cout<<"'"<<RESULT_FILE_NAME_PREFIX<<"'\n";

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
    else if (MACinList(a, trustedList) || !remove(DUMP_COMMAND_FILE))/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
    {
    //  cout<<"^^^ this is trusted MAC ^^^\n\n";
      if (!pointUp)
      {
        cout<<" flush to file. And copy to "<<RESULT_FILE_DIR<<endl;
        dd.flushToFile(OUTPUT_FILE_NAME);
        dd.clearData();
        rename(OUTPUT_FILE_NAME, getNextFileName(RESULT_FILE_NAME_PREFIX));
        intUp();
        pointUp = true;
      }
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
        {
           intDown();
           pointUp = false;
        }
      }
    }
  }
  delete [] input;
}

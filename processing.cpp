#include <iostream>
#include <vector>

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

int main()
{
  char *input = new char [1024];
  while (true)
  {
    cin.getline(input, 1023, '\n');
    vector<char*> tokens;
    createTokenVector(input, tokens, " \t");
    for (vector<char*>::iterator itr = tokens.begin(); itr != tokens.end(); itr++)
      cout<<"\""<<(*itr)<<"\"\n";
    cout<<"\n**************************************************\n\n";
  }
  delete [] input;
}

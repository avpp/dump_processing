#include <vector>
#include "mac_description.h"

using namespace std;

bool charInSet(char c, const char* set);
void createTokenVector(char* s, vector<char*> &v, const char* t);
bool MACinList(MACaddr a, vector<MACaddr> l);


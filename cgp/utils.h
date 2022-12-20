#ifndef PCGP_UTILS_H
#define PCGP_UTILS_H

/** C headers */
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <cfloat>
#include <random>

/** CPP headers */
#include <iostream>
#include <string>
#include <iomanip>
#include <vector>
#include <limits>
#include <climits>
#include <ctime>
#include <sstream>
#include <fstream>
#include <algorithm>

#include "constants.h"


int randomFunction(Parameters *p);
int randomInput(Parameters *p, int index);
int randomInterval(int inf_bound, int sup_bound);
int randomOutputIndex();
int getFunctionInputs(int function);

void readDataset(Parameters* params, Dataset* fulldata, std::string filename);
void printDataset(Dataset *data);

std::string ToString( double t );
bool IsPowerOf2( int n );
int NextPowerOf2( int n );
bool stopCriteria(int it);

#endif //PCGP_UTILS_H

#include "DtorCounter.h"

int DtorCounter::n_destroyed = 0;
int DtorCounter::n_created = 0;
std::mutex DtorCounter::mut = {};
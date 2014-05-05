#include "logger.h"

#define DEBUG
#ifdef DEBUG
#define DBG(x) std::cerr<<x<<std:endl
#else
#define DBG(x)
#endif

#define COUT(x) std::cout<<x
#define COUTLN(x) std::cout<<x<<std::endl

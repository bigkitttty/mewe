
#include "mozilla/Compiler.h"
#if !defined(__arm__) && !defined(LINUX)
#error "This code is for Linux ARM only. Check that it works on your system, too.\nBeware that this code is highly compiler dependent."
#endif

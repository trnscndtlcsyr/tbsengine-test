#ifndef CORE_LIB_HEADER
#define CORE_LIB_HEADER

//unicode for winapi functions
#ifndef UNICODE
#define UNICODE
#endif


//lib and macros for memory dump leaks
//#define __CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <iostream>
//#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
//#define new DEBUG_NEW

//std and winapi libs
#include<Windows.h>
#include<wincodec.h>
#include<comdef.h>
#include<vector>
#include<cmath>
#include<exception>
#include<string>
#include<sstream>
#include<memory>
#include<random>
#include<limits>
#include<algorithm>

#endif

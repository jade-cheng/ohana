/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2017 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_SYSTEM_HPP__
#define JADE_SYSTEM_HPP__

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <exception>
#include <fstream>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <numeric>
#include <random>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>

#if defined(JADE_USE_ACCELERATE_FRAMEWORK)
   #include <Accelerate/Accelerate.h>
#elif defined(JADE_USE_NETLIB_PACKAGES)
   #include <cblas.h>
   #include <lapacke.h>
#else
   #error Unsupported build environment
#endif

#ifdef I
#undef I
#endif

#ifdef assert
#undef assert
#endif

#endif // JADE_SYSTEM_HPP__

/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2020 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#include "test.main.hpp"

// ----------------------------------------------------------------------------
int main(const int argc, const char * argv[])
{
  return test::execute(argc, argv, {
      test::neoscan
  });
}

/**
 ** \file libport/perror.cc
 ** \brief perror: implements file libport/perror.hh
 */

#include <cstdio>
#include <iostream>

#include "libport/windows.hh"
#include "libport/cstring"

namespace libport
{

  void
  perror (const char* s)
  {
#ifndef WIN32
    ::perror(s);
#else
    if (errno != 0)
      ::perror(s);
    else
    {
      const int errnum;
      const char* errstring;
      const char* colon;

      errnum = GetLastError();
      errstring = getWinErrorMessage();

      if (s == NULL || *s == '\0')
        s = colon = "";
      else
        colon = ": ";

      std::cerr << s << colon << errstring << std::endl;
    }
#endif
  }

}

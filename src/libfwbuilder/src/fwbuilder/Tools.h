/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id$


  This program is free software which we release under the GNU General Public
  License. You may redistribute and/or modify this program under the terms
  of that license as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  To get a copy of the GNU General Public License, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

/*
 * This file contains various utility methods
 */

#ifndef __FW_TOOLS_HH_FLAG__
#define __FW_TOOLS_HH_FLAG__

#ifndef SAME
# define SAME 0
#endif

#ifndef nil
# define nil nullptr
#endif


#include <string>
#include <map>
#include <list>
#include <vector>
#include <set>

#ifndef _WIN32
#  include <dirent.h>
#else
#  include <io.h>
#endif

#include "fwbuilder/FWException.h"

namespace libfwbuilder
{

    /**
     * This method should be called to initialize the library
     * before any calls to it are made. The argument is a full path
     * to the executable that wants to initialize the library. Just
     * pass argv[0] there. This method passes substring between 
     * the beginning of exec_path and position of the last '/' or '\'
     * to XMLTools::init; this information is later used to find DTD
     * if template directory is defined as relative path. This is especially
     * useful on Mac and Win32 where API is not installed in the absolute
     * predetermined path.
     */
    void init();

    /**
     * case insensitive string comparison. We reimplement it on win32
     */
    int   cxx_strcasecmp(const char *s1, const char *s2);

    char *cxx_strdup(const char *x);
    char *cxx_strdup(const std::string &x);

    std::string int2string(int n);

    /**
     * portable function that gets a list of files that reside in a given
     * directory and have specified extension
     */
    std::list<std::string> getDirList(const std::string &dir,
                                      const std::string &ext);
    /**
     * just a portable version of sleep
     */
    unsigned int cxx_sleep(unsigned int seconds);

    /**
     *  Tokenize given string 'str' with delimiters into string vector.
     */
    void tokenize(const std::string& str,
                  std::vector<std::string>& tokens,
                  const std::string& delimiters = " ");

    /**
     *  Concat all members of a vector together into a string
     *  if there is more than one member, they are separated by delimiter.
     */
    std::string stringify(const std::vector<std::string>& parts,
                          const std::string& delimiter = " ");

    /**
     *  Strip identifier from string 'in' and return as string.
     */
    std::string strip(const std::string& in, const std::string& identifier);

    /* Convert from string <-> set<string>, using comma as delim */
    std::set<std::string> stringToSet(const std::string &str);
    std::string setToString(const std::set<std::string> &s);
    
}

#endif

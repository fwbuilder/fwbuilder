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


#include "fwbuilder/Tools.h"
#include "fwbuilder/XMLTools.h"

#include <string.h>
#include <stdio.h>

#include <iostream>
#include <sstream>

/*
 * this is workaround, for some reason dirent.h defines DIR as a typedef
 * for struct __dirstream which is not defined anywhere.  07/12/02  vk
 */
#ifndef __dirstream
struct __dirstream {};
#endif

#ifndef _WIN32
#  include <unistd.h>
#  include <dirent.h>
#else
#  include <basetsd.h>
#  include <io.h>
//#  include <windows.h>
#  include <winsock2.h>
#endif


using namespace std;

namespace libfwbuilder
{

char *cxx_strdup(const string &x)
{
    char *res=new char[x.length()+1];
    strcpy(res, x.c_str());
    return res;
}

char *cxx_strdup(const char *x)
{
    if(!x)
        return (char*)nullptr;

    char *res=new char[strlen(x)+1];
    strcpy(res,x);
    return res;
}

int  cxx_strcasecmp(const char *s1, const char *s2)
{
#ifndef _WIN32
	return ::strcasecmp(s1,s2);
#else
	return _stricoll(s1,s2);
#endif
}


void init()
{
#ifdef _WIN32
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;
 
    wVersionRequested = MAKEWORD( 2, 0 );
 
    err = WSAStartup( wVersionRequested, &wsaData );
    if ( err != 0 )
    {
        // Could not find usable dll
        cerr << "Could not initialize winsock dll v2.0";
        return;
    }
 
    // Confirm that the WinSock DLL supports 2.0.
 
    if ( LOBYTE( wsaData.wVersion ) != 2 ||
         HIBYTE( wsaData.wVersion ) != 0 )
    {
        cerr << "Could not initialize winsock dll v2.0";
        WSACleanup( );
        return; 
    }
#endif

    /* Init XML parser */
    XMLTools::initXMLTools();
   
}

/**
 *  portable function that gets a list of files in the directory.
 *  dir - directory
 *  ext - file extension mask, should be just "xml" rather than "*.xml"
 */
list<string> getDirList(const std::string &dir,
                                      const std::string &ext)
{
    list<string> res;

#ifndef _WIN32
    DIR *d=opendir(dir.c_str());
    if (d!=nullptr)
    {
        struct dirent *de;
        while ( (de=readdir(d))!=nullptr ) 
        {
            if (strcmp(de->d_name,".")==SAME || strcmp(de->d_name,"..")==SAME)
                continue;

            string pfile=de->d_name;
            string rfile=dir+FS_SEPARATOR+pfile;
            if (rfile.rfind(string(".")+ext)==rfile.size()-ext.size()-1)
                res.push_back(rfile);
        }
    }
    closedir(d);
#else

    struct _finddata_t c_file;
    intptr_t hFile;
    string filepath=dir + FS_SEPARATOR + "*." + ext;
    /* Find first file in current directory */
    if( (hFile = _findfirst( filepath.c_str(), &c_file )) != -1L )
    {
        string rfile=dir+FS_SEPARATOR+c_file.name;
        res.push_back(rfile);
        /* Find the rest of the files */
        while( _findnext( hFile, &c_file ) == 0 )
        {
            string rfile=dir+FS_SEPARATOR+c_file.name;
            res.push_back(rfile);
        }
    }
#endif
    return res;
}

unsigned int cxx_sleep(unsigned int seconds)
{
#ifndef _WIN32
        return sleep(seconds);
#else
        Sleep(seconds*1000);
        return 0;
#endif
}

void tokenize(const string& str, vector<string>& tokens, const string& delimiters)
{
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}

string strip(const string& in, const string& identifier)
{
    string result;
    vector<string> parts;

    tokenize(in, parts, identifier);

    for (vector<string>::iterator it = parts.begin(); it != parts.end(); ++it)
    {
        result += *it;
    }

    return result;
}

string stringify(const vector<string>& parts, const string& delimiter)
{
    string result;

    vector<string>::const_iterator it = parts.begin();

    while (it != parts.end())
    {
        result += *it++;

        if (it != parts.end())
        {
            result += delimiter;
        }
    }

    return result;
}


set<string> stringToSet(const string &str)
{
    set<string> ret;
    size_t lastpos = 0;
    for ( ; ; ) {
        size_t pos = str.find(',', lastpos);
        if (pos == string::npos) {
            if (lastpos < str.size()) {
                ret.insert(str.substr(lastpos));
            }
            return ret;
        }
        ret.insert(str.substr(lastpos, pos - lastpos));
        lastpos = pos + 1;
    }
}


string setToString(const set<string> &s)
{
    string ret;
    set<string>::const_iterator iter;
    bool first = true;
    for (iter = s.begin(); iter != s.end(); ++iter) {
        if (first) {
            first = false;
        } else {
            ret += ",";
        }
        ret += *iter;
    }

    return ret;
}

}

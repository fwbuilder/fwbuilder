/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

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

#include <fwbuilder/libfwbuilder-config.h>
#include <fwbuilder/Tools.h>
#include <fwbuilder/XMLTools.h>

#include <string.h>
#include <stdio.h>

/*
 * this is workaround, by some reason dirent.h defines DIR as a typedef
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
#  include <windows.h>
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
        return (char*)NULL;

    char *res=new char[strlen(x)+1];
    strcpy(res,x);
    return res;
}

string int2string(int n)
{
    char x[32];
    sprintf(x,"%d", n);
    return x;
}

string substituteMacros(const string &source, const map<string, string> &macros, bool strict) throw(libfwbuilder::FWException)
{
    string name;
    string res;

    enum {
        MODE_TEXT,
        MODE_META,
        MODE_NAME
    } m = MODE_TEXT;
    
    for(size_t i=0;i<source.length();i++)
    {
        char c = source[i];
        switch(m)
        {
        case MODE_TEXT:
            if(c=='$')
                m=MODE_META;
            else
                res+=c;
            break;
        case MODE_META:
            if(c=='$')
            {
                // double '$' means '$'
                res+=c;
            } else
            {
                if(c=='{')
                {
                    m    = MODE_NAME;
                    name = "";
                } else
                    throw FWException(string("Unexpected symbol after '$' at postion ")+int2string(i));
            }
            break;
        case MODE_NAME:
            if(c=='}')
            {
                map<string, string>::const_iterator i=macros.find(name);
                if(i!=macros.end())
                    res+=(*i).second;
                else if(strict)
                    throw FWException(string("Undefined macro '")+name+"'");
                m = MODE_TEXT;
            } else
                name+=c;
            break;
        }
    }
    
    if(m!=MODE_TEXT)
        throw FWException("Unexpected end of string");

    return res;
}

char *cxx_strtok_r(char *s, const char *delim, char **save_ptr)
{
#ifndef HAVE_STRTOK_R
    char *token;

    token = 0;     /* Initialize to no token. */

    if (s == 0) {  			/* If not first time called... */
        s = *save_ptr;          /* restart from where we left off. */
    }
        
    if (s != 0) { 	/* If not finished... */
        *save_ptr = 0;

        s += strspn(s, delim); /* Skip past any leading delimiters. */
        if (*s != '\0') {      /* We have a token. */
            token = s;
            *save_ptr = strpbrk(token, delim); /* Find token's end. */
            if (*save_ptr != 0) {
                /* Terminate the token and make SAVE_PTR point past it.  */
                *(*save_ptr)++ = '\0';
            }
        }
    }
    
    return token;
#else
//    return ::strtok_r(s, delim, save_ptr);
    return strtok_r(s, delim, save_ptr);
#endif
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
    if (d!=NULL)
    {
        struct dirent *de;
        while ( (de=readdir(d))!=NULL ) 
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
    long hFile;
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

}

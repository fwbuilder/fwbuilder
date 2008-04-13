/* 

                          Firewall Builder

                 Copyright (C) 2001 NetCitadel, LLC

  Author:  Vadim Zaliva lord@crocodile.org

  $Id: HostsFile.cpp 808 2004-09-08 05:34:53Z vkurland $


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

#include <fwbuilder/HostsFile.h>
#include <fwbuilder/Tools.h>

#include <iostream>
#include <fstream>

using namespace std;
using namespace libfwbuilder;


void HostsFile::parse(const string &filename) throw(FWException)
{
    ifstream f(filename.c_str(), ios::in);
    if(!f)
        throw FWException("Can't open file '"+filename+"'");
    else
        parse(f);
}

/**
 * Does actuall parsing.
 */
void HostsFile::parse(istream &from) throw(FWException)
{
    enum
    {
        s_ip,
        s_iws,
        s_ws,
        s_name,
        s_rest
    } state;

    state = s_iws;
    
    InetAddr      ip    ;
    string         name  ;
    vector<string> names ;

    char c ;
    int  ln = 1;

    while(from.get(c))
    {
        switch(state)
        {
        case s_iws:
            if(c=='#')
            {
                state=s_rest;
                break;
            } else if(c!='\n')
            {
                state=s_ip;
            } else
                break;
        case s_ip:
            if(c=='#')
            {
                throw FWException("Comment started in IP address field at line "+int2string(ln));
            } else if(c==' ' || c=='\t')
            {
                // if IP address is invalid, it will 
                // throw exception here.
                try
                {
                    ip = InetAddr(name);
                    names.clear();
                    name="";
                    state=s_ws;
                } catch(FWNotSupportedException &v6)
                {
                    // IPv6 addresses are not supported
                    // thus, we just skip it.
                    state=s_rest;
                }
            } else
            {
                name+=c;
            }
            break;
            
        case s_ws:
            if(c=='#')
            {
                // IP ends with comment without name
                if(names.empty())
                    throw FWException("Address: '"+name+"' without host name at line "+int2string(ln));
                else
                {
                    state=s_rest;
                    break;
                }
            } else if(c==' ' || c=='\t')
            {
                break;
            } else if(c=='\n')
            {
                if(names.empty())
                    throw FWException("Address: '"+name+"' without host name at line "+int2string(ln));
            } else
            {
                state=s_name;
                name = "";
            }
            //Fallthrough
        case s_name:
            if(c==' ' || c=='\t' || c=='#' || c=='\n')
            {
                names.push_back(name);
                name="";
                if(c=='#')
                {
                    data[ip] = names;
                    state=s_rest;
                } else if(c=='\n')
                {
                    data[ip] = names;
                    state=s_iws;
                }
                break;
            } else
            {
                name+=c;
                break;
            }
        case s_rest:
            if(c=='\n')
            {
                ln++;
                name = "";
                state = s_iws;
            }
            break;
        }
    }
}



/* 

                          Firewall Builder

                 Copyright (C) 2005 NetCitadel, LLC

  Author:  Illiya Yalovoy <yalovoy@gmail.com>

  $Id: AddressTable.cpp 1002 2006-12-28 07:37:41Z vkurland $

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

#include <assert.h>

#include <fwbuilder/libfwbuilder-config.h>

#include <fwbuilder/AddressTable.h>
#include <fwbuilder/FWException.h>
#include <fwbuilder/FWObjectReference.h>
#include <fwbuilder/FWObjectDatabase.h>
#include <fwbuilder/Network.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace libfwbuilder;
using namespace std;


const char *AddressTable::TYPENAME={"AddressTable"};

AddressTable::AddressTable() : MultiAddress() 
{
    registerSourceAttributeName("filename");
    setSourceName("");
    setRunTime(false);
}

AddressTable::AddressTable(const FWObject *root,bool prepopulate) :
    MultiAddress(root,prepopulate) 
{
    registerSourceAttributeName("filename");
    setSourceName("");
    setRunTime(false);
}

void AddressTable::loadFromSource() throw(FWException)
{
    ifstream fs(getSourceName().c_str());
    ostringstream exmess;
    Network *net;
    FWObject *root = getParent();
    string buf;
    size_type pos;
    int line = 1;
    int cntr = 0;
    if(fs)
    {
       while(!fs.eof())
       {
            getline(fs,buf);
            pos=buf.find_first_not_of(" \t");
            if (pos!=string::npos)
            {
                buf=buf.substr(pos);
                pos=buf.find_first_not_of("0123456789/.");
                buf=buf.substr(0,pos);
            }
            else
            {
                buf="";
            }
            if (!buf.empty())
            {
                try
                {
                    net = Network::cast(getRoot()->create(Network::TYPENAME));
                    *net = buf;
                    root->add(net);
                    net->setName(buf);
                    if (validateChild(net))
                    {
                        addRef(net);
                        cntr++;
                    }
                    
                } catch (FWException &ex) 
                {
                    
                    exmess << "Invalid address: "
                           << getSourceName() << ":"
                           << line
                           << " \"" << buf << "\"";
                    throw FWException(exmess.str());
                }
            }
            line++;
       }
    }
    else
    {
        exmess << "File not found for Address Table: "
               << getName()
               << " (" << getSourceName() << ")";
        throw FWException(exmess.str());
    }
}


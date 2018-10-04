/* 

                          Firewall Builder

                 Copyright (C) 2005 NetCitadel, LLC

  Author:  Illiya Yalovoy <yalovoy@gmail.com>

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

#include <assert.h>



#include "fwbuilder/AddressTable.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/FWObjectReference.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/FWOptions.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/NetworkIPv6.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace libfwbuilder;
using namespace std;


const char *AddressTable::TYPENAME={"AddressTable"};

AddressTable::AddressTable() : MultiAddress() 
{
    setRunTime(false);
    setStr("filename", "");
}

string AddressTable::getSourceName()
{
    return getStr("filename");
}

void AddressTable::setSourceName(const std::string& source_name)
{
    setStr("filename", source_name);
}

void AddressTable::fromXML(xmlNodePtr root)
{
    FWObject::fromXML(root);
    const char *n;
    
    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("filename")));
    assert(n!=nullptr);
    setStr("filename", n);
    XMLTools::FreeXmlBuff(n);

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("run_time")));
    assert(n!=nullptr);
    setStr("run_time", n);
    XMLTools::FreeXmlBuff(n);
}

xmlNodePtr AddressTable::toXML(xmlNodePtr parent)
{
    xmlNodePtr me = FWObject::toXML(parent, false);

    xmlNewProp(me, XMLTools::ToXmlCast("name"), XMLTools::StrToXmlCast(getName()));
    xmlNewProp(me, XMLTools::ToXmlCast("comment"), XMLTools::StrToXmlCast(getComment()));
    xmlNewProp(me, XMLTools::ToXmlCast("ro"), XMLTools::ToXmlCast(((getRO()) ? "True" : "False")));

    return me;
}


string AddressTable::getFilename(FWOptions *options)
{
    string path = getStr("filename");
    size_t found = path.find("%DATADIR%");
    if (found == string::npos) return path;

    string dataDir;
    if (isRunTime()) {
        dataDir = options->getStr("data_dir");
        if (dataDir.empty()) {
            throw FWException("Firewall 'data directory' setting is blank");
        }
    } else {
        dataDir = FWObject::getDataDir();
        if (dataDir.empty()) {
            throw FWException("Global 'data directory' setting is blank");
        }
    }

    path.replace(found, 9, dataDir);
    return path;
}

/*
 * read file specified by the "filename" attribute and interpret lines
 * as addresses. Create corresponding address or network objects, add
 * them to the object database and add references to them to @this. If
 * file does not exist and we run in test mode, create dummy object
 * and add it to the database and referece to it, then throw
 * exception.
 *
 * TODO: new objects should be added to some kind of special group in
 * the object tree, something with the name "tmp" or similar.
 */
void AddressTable::loadFromSource(bool ipv6, FWOptions *options,
                                  bool test_mode)
{
    string path = getFilename(options);
    ifstream fs(path.c_str());
    ostringstream exmess;
    string buf;
    size_type pos;
    int line = 1;
    int cntr = 0;

    Address *new_addr;

    if(fs)
    {
       while(!fs.eof())
       {
            getline(fs,buf);

            pos = buf.find_first_not_of(" \t");
            if (pos!=string::npos)
            {
                buf = buf.substr(pos);
                pos = buf.find_first_not_of("0123456789abcdef:/.");
                buf = buf.substr(0,pos);
            }
            else
            {
                buf="";
            }
            if (!buf.empty())
            {
                new_addr = nullptr;
                if (ipv6 && buf.find(":")!=string::npos)
                {
                    try
                    {
                        InetAddr(AF_INET6, buf); // to test address

                        NetworkIPv6 *net = getRoot()->createNetworkIPv6();
                        net->setAddressNetmask(buf);
                        new_addr = net;
                    } catch (FWException &ex)
                    {
                        exmess << "Invalid address: "
                               << path << ":"
                               << line
                               << " \"" << buf << "\"";
                        throw FWException(exmess.str());
                    }
                }

                if (!ipv6 && buf.find(".")!=string::npos)
                {
                    try
                    {
                        InetAddr(AF_INET, buf); // to test address

                        Network *net = getRoot()->createNetwork();
                        net->setAddressNetmask(buf);
                        new_addr = net;
                    } catch (FWException &ex)
                    {
                        exmess << "Invalid address: "
                               << path << ":"
                               << line
                               << " \"" << buf << "\"";
                        throw FWException(exmess.str());
                    }
                }

                if (new_addr)
                {
                    new_addr->setName(buf);
                    if (validateChild(new_addr))
                    {
                        getRoot()->add(new_addr);
                        addRef(new_addr);
                        cntr++;
                    }
                }
            }
            line++;
       }
    }
    else
    {
        // in test mode we use dummy address but still throw exception.
        // Compiler should print error message but continue.
        exmess << "File not found for Address Table: "
               << getName()
               << " (" << path << ")";
        if (test_mode)
        {
            exmess << " Using dummy address in test mode";
            if (ipv6)
            {
                NetworkIPv6 *net = getRoot()->createNetworkIPv6();
                net->setAddressNetmask("2001:db8::/32");
                new_addr = net;
            } else
            {
                Network *net = getRoot()->createNetwork();
                net->setAddressNetmask("192.0.2.0/24");
                new_addr = net;
            }
            new_addr->setName(buf);
            if (validateChild(new_addr))
            {
                getRoot()->add(new_addr);
                addRef(new_addr);
                cntr++;
            }
            new_addr->setBool(".rule_error", true);
            new_addr->setStr(".error_msg", exmess.str());
        }
        throw FWException(exmess.str());
    }
}


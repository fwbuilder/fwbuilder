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



#include <sys/types.h>

#include <sys/stat.h>
#ifndef _WIN32
#  include <pwd.h>
#  include <unistd.h>
#endif
#include <fcntl.h>
#include <errno.h>
#include <assert.h>

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>

#include "fwbuilder/Resources.h"
#include "fwbuilder/Constants.h"
#include "fwbuilder/FWObject.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/FWOptions.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Host.h"
#include "fwbuilder/Tools.h"

using namespace libfwbuilder;
using namespace std;

const string Resources::PLATFORM_RES_DIR_NAME = "platform";
const string Resources::OS_RES_DIR_NAME = "os";

Resources*  Resources::global_res = nullptr;
map<string,Resources*>  Resources::platform_res;
map<string,Resources*>  Resources::os_res;


Resources::Resources()
{
    doc=nullptr;
}

Resources::Resources(const string &_resF)
{
    doc = nullptr;
    resfile = _resF;

    if (global_res==nullptr) 
    {
        global_res = this;
        loadRes(_resF);
        loadSystemResources();
    } else
        loadRes(_resF);
}

Resources::~Resources()
{
    if (doc) xmlFreeDoc(doc);
}

void Resources::clear()
{
    for (map<string,Resources*>::iterator it=platform_res.begin(); 
         it!=platform_res.end(); ++it)
    {
        delete it->second;
    }
    platform_res.clear();

    for (map<string,Resources*>::iterator it=os_res.begin(); 
         it!=os_res.end(); ++it)
    {
        delete it->second;
    }
    os_res.clear();
}

string Resources::getXmlNodeContent(xmlNodePtr node)
{
    string res;
    char* cptr= (char*)( xmlNodeGetContent(node) );
    if (cptr!=nullptr) 
    {
        res=cptr;
        XMLTools::FreeXmlBuff(cptr);
    }
    return res;
}

string Resources::getXmlNodeProp(xmlNodePtr node,string prop)
{
    string res;
    char* cptr=(char*)( xmlGetProp(node,XMLTools::ToXmlCast(prop.c_str())));
    if (cptr!=nullptr) 
    {
        res=cptr;
        XMLTools::FreeXmlBuff(cptr);
    }
    return res;
}

void Resources::loadRes(const std::string &rfile )
{

    string buffer = XMLTools::readFile(rfile);
    doc = xmlParseMemory(buffer.c_str(), buffer.length());

//    doc = xmlParseFile(rfile.c_str()); 
//    doc = XMLTools::loadAndParseFile(rfile);

    if (!doc) throw FWException("Error parsing "+rfile);

    root = xmlDocGetRootElement(doc);
   
    if(!root || !root->name || strcmp(XMLTools::FromXmlCast(root->name), "FWBuilderResources")!=0) 
    {
        xmlFreeDoc(doc);
        throw FWException("Invalid resources file "+rfile);
    }
}

void Resources::loadSystemResources()
{
/*
 * Find and open resources for individual firewall platforms and OS.
 * If corresponding directory does not exist or is empty, then no
 * target platform support package is installed. So be it, not my
 * problem
 */
//    string resDir=Constants::getTemplateDirectory();

    string::size_type n=resfile.find_last_of("/\\");
    string resDir = resfile.substr(0,n);

    list<string> pllist = getDirList( resDir + FS_SEPARATOR +PLATFORM_RES_DIR_NAME,
                                      "xml" );

    for (list<string>::iterator lsi1=pllist.begin(); lsi1!=pllist.end(); lsi1++)
    {
        string::size_type n=lsi1->find_last_of("/\\")+1;
        string platform=lsi1->substr(n, lsi1->rfind(".xml")-n);
        Resources *tr=new Resources(*lsi1);	
        platform_res[platform]=tr;
    }


    list<string> oslist = getDirList( resDir + FS_SEPARATOR +OS_RES_DIR_NAME,
                                      "xml" );
    for (list<string>::iterator lsi2=oslist.begin(); lsi2!=oslist.end(); lsi2++)
    {
        string::size_type n=lsi2->find_last_of("/\\")+1;
        string os=lsi2->substr(n, lsi2->rfind(".xml")-n);
        Resources *tr=new Resources(*lsi2);	
        os_res[os]=tr;
    }

#if 0
    cerr << "Loaded resources for the following modules :\n";

    map<string,string> p=getPlatforms();
    map<string,string>::iterator i1;
    for (i1=p.begin(); i1!=p.end(); ++i1)
    {
        cerr << (*i1).first << "  " << (*i1).second << endl;
    }

    p=getOS();
    for (i1=p.begin(); i1!=p.end(); ++i1)
    {
        cerr << (*i1).first << "  " << (*i1).second << endl;
    }
#endif
}

xmlNodePtr Resources::getXmlNode(const string& path)
{
    return XMLTools::getXmlNodeByPath(root,path);
}


string  Resources::getIconPath(const char* icon)
{
    string icn;

    icn= getResourceStr("/FWBuilderResources/Paths/Icndir");
    icn += "/";
    icn += getResourceStr(string("/FWBuilderResources/UI/Icons/")+icon);

    return icn;
}

string  Resources::getIconPath(const string& icon)
{
    return getIconPath(icon.c_str());
}

string  Resources::getResourceStr(const string& resource_path)
{
    xmlNodePtr node=XMLTools::getXmlNodeByPath(root,resource_path.c_str());
    if (node)  return getXmlNodeContent(node);
    return "";
}

int     Resources::getResourceInt(const string& resource_path)
{
    return atoi(getResourceStr(resource_path).c_str());
}

bool    Resources::getResourceBool(const string& resource_path)
{
    string res=getResourceStr(resource_path);
    return (res=="true" || res=="True");
}

/*
 * Adds bodies of xml elements found directly under resource_path to
 * the list<string> res
 *
 * <element1>
 *   <element2>
 *      <string>value1</string>
 *      <string>value2</string>
 *      <string>value3</string>
 *      <string>value4</string>
 *   </element2>
 * </element1>
 *
 * here resorce_path="/element1/element2", returned list consists of
 * strings value1,value2,value3,value4
 */
void Resources::getResourceStrList(const string& resource_path, list<string> &res)
{
    xmlNodePtr node = XMLTools::getXmlNodeByPath(root, resource_path.c_str());
    if (node)
    {
        xmlNodePtr c;
        for(c=node->xmlChildrenNode; c; c=c->next) 
        {
            if ( xmlIsBlankNode(c) ) continue;
            res.push_back(getXmlNodeContent(c));
        }
    }
}

string  Resources::getObjResourceStr(const FWObject *obj,
                                     const string& resource_name)
{
    string objid   = FWObjectDatabase::getStringId(obj->getId());
    string objtype = obj->getTypeName();
    string res;
    string resource="/FWBuilderResources/Object/"+objid+"/"+resource_name;
    res= getResourceStr(resource);
    if (res.empty()) 
    {
        string resource="/FWBuilderResources/Type/"+objtype+"/"+resource_name;
        res= getResourceStr(resource);
        if (res.empty()) 
        {
            resource="/FWBuilderResources/Type/DEFAULT/"+resource_name;
            res= getResourceStr(resource);
        }
    }

    if (res.empty())
        cerr << "Failed to locate resource for object " << obj->getName() 
             << " (type=" << objtype << "), "
             << " (id=" << obj->getId() << "), resource name: "
             << resource_name << endl;

    return res;
}

bool  Resources::getObjResourceBool(const FWObject *obj,
				    const string&  resource_name)
{
    string res=getObjResourceStr(obj,resource_name);
    return (res=="true" || res=="True");
}

/*
 *  call this using appropriate Resources object, like this:
 *  
 *  platform_res["iptables"]->getVersion()
 */
string  Resources::getVersion()
{
    xmlNodePtr pn=getXmlNode("/FWBuilderResources/");
    return getXmlNodeProp(pn,"version");
}

/*
 *  call this using appropriate Resources object, like this:
 *  
 *  platform_res["iptables"]->getCompiler()
 */
string  Resources::getCompiler()
{
    return getResourceStr("/FWBuilderResources/Target/compiler");
}

string  Resources::getInstaller()
{
    return getResourceStr("/FWBuilderResources/Target/installer");
}

string  Resources::getTransferAgent()
{
    return getResourceStr("/FWBuilderResources/Target/transfer_agent");
}

vector<string> Resources::getListOfPlatforms()
{
    vector<string> vs;
    for (map<string,Resources*>::iterator i1=platform_res.begin();
         i1!=platform_res.end(); ++i1)
    {
        vs.push_back( (*i1).first );
    }
    return vs;
}



map<string,string> Resources::getPlatforms()
{
    map<string,string> vs;
    for (map<string,Resources*>::iterator i1=platform_res.begin();
         i1!=platform_res.end(); ++i1)
    {
        Resources *res = (*i1).second;
        if (res)
        {
            string desc = res->getResourceStr(
                "/FWBuilderResources/Target/description");
            vs[ (*i1).first ]=desc;
        }
    }
    return vs;
}


map<string,string> Resources::getOS()
{
    map<string,string> vs;
    for (map<string,Resources*>::iterator i1=os_res.begin(); i1!=os_res.end(); ++i1)
    {
        Resources *res = (*i1).second;
        if (res)
        {
            string desc = res->getResourceStr(
                "/FWBuilderResources/Target/description");
            vs[ (*i1).first ]=desc;
        }
    }
    return vs;
}


string Resources::getRuleElementResourceStr(const string &rel,
                                            const string &resource_name)

{
    xmlNodePtr  c,d;

    xmlNodePtr  dptr=Resources::global_res->getXmlNode("FWBuilderResources/RuleElements");

    assert (dptr!=nullptr);

    for(c=dptr->xmlChildrenNode; c; c=c->next) 
    {
	if ( xmlIsBlankNode(c) ) continue;
        if (rel==getXmlNodeProp(c,"RuleElement")) 
        {
            d=XMLTools::getXmlChildNode(c,resource_name.c_str());
            if (d) 
            {
                return getXmlNodeContent(d);
            }
	}
    }
    return string("");
}

bool Resources::isSystem(const FWObject *o)
{
    return global_res->getObjResourceBool(o, "system");
}

string  Resources::getIconFileName(const FWObject *o)
{ 
    string res;
    
    res=global_res->getResourceStr("/FWBuilderResources/Paths/Icndir");
    res += "/";
    res += global_res->getObjResourceStr(o, "icon");

    return res;
}

string  Resources::getNegIconFileName(const FWObject *o)
{ 
    string res;
    
    res=global_res->getResourceStr("/FWBuilderResources/Paths/Icndir");
    res += "/";
    res += global_res->getObjResourceStr(o, "icon-neg");
    
    return res;
}

string  Resources::getRefIconFileName(const FWObject *o)
{ 
    string res;
    
    res=global_res->getResourceStr("/FWBuilderResources/Paths/Icndir");
    res += "/";
    res += global_res->getObjResourceStr(o, "icon-ref");
    
    return res;
}

string  Resources::getTreeIconFileName(const FWObject *o)
{ 
    string res;
    
    res=global_res->getResourceStr("/FWBuilderResources/Paths/Icndir");
    res += "/";
    res += global_res->getObjResourceStr(o, "icon-tree");
    
    return res;
}

void    Resources::setDefaultOption(FWObject *o,const string &xml_node)
{
    xmlNodePtr pn = XMLTools::getXmlNodeByPath(root,xml_node.c_str());
    if (pn==nullptr) return;

    string optname=XMLTools::FromXmlCast(pn->name);
    string optval =getXmlNodeContent(pn);
    o->setStr(optname , optval);
}

void    Resources::setDefaultOptionsAll(FWObject *o,const string &xml_node)
{
    xmlNodePtr pn = XMLTools::getXmlNodeByPath(root , xml_node.c_str() );
    if (pn==nullptr) return;

    xmlNodePtr opt;

    for(opt=pn->xmlChildrenNode; opt; opt=opt->next) 
    {
        if ( xmlIsBlankNode(opt) ) continue;
        setDefaultOption(o,xml_node+"/"+XMLTools::FromXmlCast(opt->name));
    }
}


void    Resources::setDefaultTargetOptions(const string &target,Firewall *fw)
{
    FWOptions *opt=fw->getOptionsObject();
    Resources *r=nullptr;

    if (platform_res.count(target)!=0)      r=platform_res[target];
    if (r==nullptr && os_res.count(target)!=0) r=os_res[target];
    if (r==nullptr)
        throw FWException("Support module for target '"+target+"' is not available");

    r->setDefaultOptionsAll(opt,"/FWBuilderResources/Target/options/default");
}

void    Resources::setDefaultIfaceOptions(const string &target,Interface *iface)
{
    FWOptions *opt=iface->getOptionsObject();
    /* if InterfaceOptions object does not yet exist -> create one */
    if (opt == nullptr) {
        iface->add(iface->getRoot()->create(InterfaceOptions::TYPENAME));
        opt = iface->getOptionsObject();
    }

    Resources *r=nullptr;

    if (platform_res.count(target)!=0)      r=platform_res[target];
    if (r==nullptr && os_res.count(target)!=0) r=os_res[target];
    if (r==nullptr)
        throw FWException("Support module for target '"+target+"' is not available");

    r->setDefaultOptionsAll(opt,"/FWBuilderResources/Target/options/interface");
}

void    Resources::setDefaultOptions(Host *h)
{
    FWOptions *opt=h->getOptionsObject();

    global_res->setDefaultOptionsAll(opt,
      "/FWBuilderResources/Type/"+h->getTypeName()+"/options");
}

void    Resources::setDefaultProperties(FWObject *obj)
{
    global_res->setDefaultOptionsAll(obj,
      "/FWBuilderResources/Type/"+obj->getTypeName()+"/properties");
}

string Resources::getTargetCapabilityStr(const string &target,
                                         const string &cap_name)
{
    Resources *r=nullptr;

    if (platform_res.count(target)!=0)      r=platform_res[target];
    if (r==nullptr && os_res.count(target)!=0) r=os_res[target];
    if (r==nullptr)
        throw FWException("Support module for target '"+target+"' is not available");

    return r->getResourceStr("/FWBuilderResources/Target/capabilities/"+cap_name);
}

bool Resources::getTargetCapabilityBool(const string &target,
                                        const string &cap_name)
{
    string s=getTargetCapabilityStr(target,cap_name);
    return (s=="true" || s=="True");
}

bool Resources::isTargetActionSupported(const string &target, const string &action)
{
    bool res=false;
    try  
    {
         res = getTargetCapabilityBool(target, "actions/" + action + "/supported");
    } catch (FWException &ex) { }
    return res;
}

string Resources::getActionEditor(const string &target, const string &action)
{
    string res="None";
    try  
    {
         res = getTargetCapabilityStr(target, "actions/" + action + "/parameter");
    } catch (FWException &ex) { }
    return res;
}

string Resources::getTargetOptionStr(const string &target,
                                     const string &opt_name)
{
    Resources *r=nullptr;

    if (platform_res.count(target)!=0)      r=platform_res[target];
    if (r==nullptr && os_res.count(target)!=0) r=os_res[target];
    if (r==nullptr)
        throw FWException("Support module for target '"+target+"' is not available");

    return r->getResourceStr("/FWBuilderResources/Target/options/"+opt_name);
}

bool  Resources::getTargetOptionBool(const string &target,
                                     const string &opt_name)
{
    string s=getTargetOptionStr(target,opt_name);
    return (s=="true" || s=="True");
}



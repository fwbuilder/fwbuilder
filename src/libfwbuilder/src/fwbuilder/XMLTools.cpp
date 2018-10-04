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




#include "fwbuilder/XMLTools.h"
#include "fwbuilder/ThreadTools.h"

#include <zlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>     // for va_start and friends
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>

#ifndef _WIN32
#  include <unistd.h>   // need this for read(2)
#else
#  include <io.h>       // for access
#  define R_OK 4        // for access
#endif


#include <libxslt/xsltconfig.h>

#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>

#include <libxml/xmlmemory.h>

#include <zlib.h>

#include <iostream>
#include <memory>

#undef FW_XMLTOOLS_VERBOSE
// #define FW_XMLTOOLS_VERBOSE 1

#define DTD_LOAD_BITS (1|XML_DETECT_IDS|XML_COMPLETE_ATTRS)

using namespace std;
using namespace libfwbuilder;

#ifndef __MINGW32__
extern int xmlDoValidityCheckingDefaultValue ;
extern int xmlLoadExtDtdDefaultValue         ;
#else
extern __declspec(dllimport) int xmlDoValidityCheckingDefaultValue ;
extern __declspec(dllimport) int xmlLoadExtDtdDefaultValue         ;
#endif

/*
 * This mutex protects access to XML parser.
 * since we change DTD validation flags and error
 * handling function pointers, access should be
 * synchronized.
 */
Mutex xml_parser_mutex;

/*
 * This mutex protects access to XSLT processor.
 * since we error handling function pointers, access should be
 * synchronized.
 */
Mutex xslt_processor_mutex;

static void xslt_error_handler(void *ctx, const char *msg, ...)
{
    char buf[4096];
    va_list args;

    assert(ctx!=nullptr);
    va_start(args, msg);

#ifdef _WIN32
    _vsnprintf(buf, sizeof(buf)-1, msg, args);
#else
    vsnprintf(buf, sizeof(buf)-1, msg, args);
#endif

    va_end(args);
    
#ifdef FW_XMLTOOLS_VERBOSE
    cerr << "XSLT ERR: " << buf << endl;
#endif

    *((string*)ctx)+=buf;
}

xmlNodePtr XMLTools::getXmlChildNode(xmlNodePtr r,const char *child_name)
{
    xmlNodePtr  cur;

    for(cur=r->xmlChildrenNode; cur; cur=cur->next) {
	if ( xmlIsBlankNode(cur) ) continue;
	if (strcmp(child_name,XMLTools::FromXmlCast(cur->name))==SAME)
	    return cur;
    }
    return nullptr;
}


xmlNodePtr XMLTools::getXmlNodeByPath(xmlNodePtr r, const string &path)
{
    return getXmlNodeByPath(r, path.c_str());
}

xmlNodePtr XMLTools::getXmlNodeByPath(xmlNodePtr r, const char *path)
{
    char *s1, *cptr;
    char *path_copy;
    xmlNodePtr  cur, res;

    res=nullptr;
    
    path_copy= cxx_strdup( path );

    s1=path_copy+strlen(path_copy)-1;
    while (*s1=='/') { *s1='\0'; s1--; }

    s1=path_copy;
    if (*s1=='/') {
	res=getXmlNodeByPath(r,s1+1);
	delete[] path_copy;
	return(res);
    }

    cptr=strchr(s1,'/');
    if (cptr!=nullptr) {
	*cptr='\0';
	cptr++;
    }
    if (strcmp(XMLTools::FromXmlCast(r->name), s1)==0) {
	if (cptr) {
	    for(cur=r->xmlChildrenNode; cur; cur=cur->next) {
		if ( xmlIsBlankNode(cur) ) continue;
		res=getXmlNodeByPath(cur,cptr);
		if (res) {
		    delete[] path_copy;
		    return(res);
		}
	    }
	} else
	    res=r;
    }
    delete[] path_copy;
    return(res);
}


xmlExternalEntityLoader XMLTools::defaultLoader = nullptr;

/** 
 * This is global variable used in 'fwbExternalEntityLoader'
 * parser callback. It is protected by 'xml_parser_mutex'.
 */

static char* current_template_dir=nullptr;

xmlParserInputPtr fwbExternalEntityLoader(const char *URL, 
                                          const char *ID,
                                          xmlParserCtxtPtr ctxt) 
{
    xmlParserInputPtr ret;

#ifdef FW_XMLTOOLS_VERBOSE
    cerr << "ENTITY: " << URL << " " << string((ID)?ID:"(null)") << endl;
#endif

    string fname;

    fname=string(current_template_dir) + FS_SEPARATOR;

    string url=URL;
    string::size_type pos=url.find_last_of("/\\");
    fname+=(pos==string::npos)?url:url.substr(pos+1);

#ifdef FW_XMLTOOLS_VERBOSE
    cerr << "ENTITY FNAME: " << fname << endl;
#endif    
    ret = xmlNewInputFromFile(ctxt, fname.c_str());
    if(ret)
        return(ret);
    else if(XMLTools::defaultLoader)
        return XMLTools::defaultLoader(URL, ID, ctxt);
    else
        return nullptr;
}

void XMLTools::initXMLTools()
{
    xmlInitMemory();
    xmlInitParser();
    defaultLoader = xmlGetExternalEntityLoader();
    current_template_dir=cxx_strdup("");
    xmlSetExternalEntityLoader(fwbExternalEntityLoader);
}

void XMLTools::close()
{
    xmlCleanupParser();
}

string XMLTools::readFile(const std::string &rfile)
{
    string buf;

    if (rfile=="-")
    {
        string s;
        while (!cin.eof()) 
        {
            getline(cin,s);
            buf += s;
            buf += '\n';
        }
        return buf;
    }

    gzFile gzf = gzopen(rfile.c_str(), "rb9");
    if (gzf == nullptr) throw FWException("Could not read file "+rfile);

    unsigned int chunk_size = 65536;
    std::unique_ptr<char[]> chunk_storage(new char[chunk_size]);
    char *chunk = chunk_storage.get();
    if (!chunk) throw FWException("Out of memory");

    int  n = 0;
    while(1)
    {
        n = gzread(gzf, chunk, chunk_size-1);
        if (n<=0) break;
        chunk[n] = '\0';
        buf = buf + chunk;
    }
    int errn = errno;
    gzclose(gzf);

    if (n<0)
    {
        string s;
        s = "Error reading from file " + rfile + " : " + string(strerror(errn));
        throw FWException(s);
    }

    return buf;
}

xmlDocPtr XMLTools::parseFile(const string &file_name, 
                              const string &buffer,
                              bool use_dtd,
                              const string &template_dir)
{
    LockGuard lock(xml_parser_mutex);

    if (current_template_dir!=nullptr) delete[] current_template_dir;
    current_template_dir = cxx_strdup(template_dir.c_str());
    
    xmlDoValidityCheckingDefaultValue = use_dtd ? 1 : 0;
    xmlLoadExtDtdDefaultValue = use_dtd ? DTD_LOAD_BITS : 0;
    
    string errors;
    xmlSetGenericErrorFunc(&errors, xslt_error_handler);
//    xmlDocPtr doc = xmlParseFile(file_name.c_str()); 

    xmlDocPtr doc = xmlParseMemory(buffer.c_str(), buffer.length());

    xmlSetGenericErrorFunc(nullptr, nullptr);

    if (!doc || errors.length())
    {
        throw FWException(
            "Error parsing XML from file '"+file_name+ "' "+
            "(use_dtd="+ (use_dtd?string("1"):string("0"))+") "+
            (errors.length() ? (
                string("\nXML Parser reported:\n")+errors):string(""))
        );
    }

    return doc;
}

xmlDocPtr XMLTools::loadFile(const string &data_file , 
                             const string &type      ,
                             const string &dtd_file  ,
                             const UpgradePredicate *upgrade,
                             const string &template_dir,
                             const string &current_version
                             )
{
#ifdef FW_XMLTOOLS_VERBOSE
    cerr << "Loading file:        " << data_file      << endl
         << "    type:            " << type           << endl
         << "    dtd_file:        " << dtd_file       << endl
         << "    template_dir:    " << template_dir   << endl
         << "    current_version: " << current_version
         << endl;
#endif
    int access_err = 0;
    if (data_file!="-" && (access_err=access(data_file.c_str() , R_OK ))!=0)
    {
        string access_err_str;
        switch (access_err)
        {
        case EACCES: access_err_str = "EACCES"; break;
        case EFAULT: access_err_str = "EFAULT"; break;
        case EIO: access_err_str = "EIO"; break;
#ifdef ELOOP
        case ELOOP: access_err_str = "ELOOP"; break;
#endif
        case ENAMETOOLONG: access_err_str = "ENAMETOOLONG"; break;
        case ENOENT: access_err_str = "ENOENT"; break;
        case ENOTDIR: access_err_str = "ENOTDIR"; break;
        default: access_err_str = "Unknown"; break;
        }
        throw FWException(
            string("Could not access data file '") + data_file + "'" +
            " error code " + access_err_str);
    }

    string buf = readFile(data_file);

    // First load without using DTD to check version
    xmlDocPtr doc = parseFile(data_file, buf, false, template_dir); 

#ifdef FW_XMLTOOLS_VERBOSE
    cerr << "Parsed file: " << data_file << endl;
#endif

    // normally we load the file twice, first time to check the version and
    // upgrade it and the second time to generate doc that will be
    // used in the program. We can't do this if data_file is '-' (stdin)
    // 'cause we can't read stdin twice. So in this case we do not 
    // upgrade. 
   
    if (data_file=="-") return doc;


    xmlDocPtr newdoc = convert(doc, data_file, type,
                               template_dir, current_version);
    if(newdoc)
    {
        const string upgrade_msg = "The file '" + data_file +
            "' was saved with\n\
an older version of Firewall Builder.  Opening it in this version will\n\
cause it to be upgraded, which may prevent older versions of the program\n\
from reading it. Backup copy of your file in the old format will be made\n\
in the same directory with extension '.bak'. Are you sure you want to open it?";


        if(!(*upgrade)(upgrade_msg))
        {
            xmlFreeDoc(newdoc);
            //xmlCleanupParser();
            throw FWException("Load operation cancelled for file: '"+data_file);
        }
     
#ifdef FW_XMLTOOLS_VERBOSE
        cerr << "Saving updated file: " << data_file << endl;
#endif
        // file was changed save it
        doc=newdoc;
        string backup_file = data_file+".bak";

// on windows rename fails if target file already exists
        unlink(backup_file.c_str());

        if(rename(data_file.c_str(), backup_file.c_str()))
        {
            xmlFreeDoc(doc);
            //xmlCleanupParser();
            throw FWException("Error making backup copy of file: '" + 
                              data_file + "' as '" + backup_file + "'");
        }

        try
        {
            saveFile(doc, data_file, type, dtd_file);
        } catch(FWException &ex)
        {
            // Saving converted copy failed
            // let's restore backup
            if(rename(backup_file.c_str(), data_file.c_str()))
            {
                throw FWException(ex.toString() +
                                  "\nRestoring backup copy failed " +
                                  "your old data could be found in the file: '"+
                                  backup_file+"'");
            } else
                throw;
        }
    } 
    assert(doc!=nullptr);
    xmlFreeDoc(doc);
    //xmlCleanupParser();

    // Now we know the version is OK,
    // let us load for real, checking DTD.
    doc = parseFile(data_file, readFile(data_file), true, template_dir); 
    
    return doc;
}

void XMLTools::setDTD(xmlDocPtr doc, 
                      const string &type_name, 
                      const string &dtd_file)
{
#ifdef FW_XMLTOOLS_VERBOSE
    cerr << "XMLTools::setDTD: type_name=" << type_name << " dtd_file=" << dtd_file << endl;
#endif
  

    xmlCreateIntSubset(doc, XMLTools::StrToXmlCast(type_name), 
                       nullptr, 
                       XMLTools::StrToXmlCast(dtd_file)
    );
    
    LockGuard lock(xml_parser_mutex);

    xmlDoValidityCheckingDefaultValue = 1;
    xmlLoadExtDtdDefaultValue         = DTD_LOAD_BITS;
    xmlSubstituteEntitiesDefaultValue = 1;

    string errors;
    xmlSetGenericErrorFunc (&errors, xslt_error_handler);

    try
    {
/*
 * This broke with libxml 2.6.4. Tests seem to rule out bug inside
 * libxml2 (used their example program "tree2.c" and added similar
 * fragment for validation, it worked), so it must be something in our
 * code. I can't seem to find the problem though. 
 *
 * We recreate the tree from the objects in the memory, so doing
 * validation here is mostly a double check. It should be relatively
 * safe to just skip validation until I figure out what's wrong with
 * it.

        xmlValidCtxt vctxt;
        vctxt.userData = &errors;
        vctxt.error    = xslt_error_handler;
        vctxt.warning  = xslt_error_handler;
        if(xmlValidateDocument(&vctxt, doc)!=1)
            throw FWException(string("DTD validation stage 2 failed with following errors:\n")+errors);
*/
        xmlSetGenericErrorFunc (nullptr, nullptr);
    } catch(...)
    {
        xmlSetGenericErrorFunc (nullptr, nullptr);  
        throw;
    }
}

void XMLTools::saveFile(xmlDocPtr doc, 
                        const string &file_name, 
                        const string &type_name,
                        const string &dtd_file)
{
    
#ifdef FW_XMLTOOLS_VERBOSE
    cerr << "SAVE: " << file_name << " " <<dtd_file << endl;
#endif
    
    setDTD(doc, type_name, dtd_file);

    if(xmlSaveFormatFileEnc(file_name.c_str(), doc, "utf-8", 1)==-1)
        throw FWException("Error saving XML file: "+file_name);
}


void XMLTools::dumpToMemory(xmlDocPtr doc, 
                            xmlChar **buffer,
                            int      *size,
                            const string &type_name,
                            const string &dtd_file)
{
    setDTD(doc, type_name, dtd_file);

#ifdef HAVE_XMLSAVEFORMATFILEENC
    xmlDocDumpFormatMemoryEnc(doc,buffer,size,"utf-8",1);
#else
    xmlDocDumpMemoryEnc(doc,buffer,size,"utf-8");
#endif
}


void XMLTools::transformFileToFile(const string &src_file,
				   const string &stylesheet_file,
				   const char **params,
				   const string &dst_file)
{
    string xslt_errors;
    xsltStylesheetPtr ss = nullptr;
    xmlDocPtr doc, res;

    LockGuard xslt_processor_mutex_lock(xslt_processor_mutex);
    LockGuard xml_parser_mutex_lock(xml_parser_mutex);
    
    xsltSetGenericErrorFunc(&xslt_errors, xslt_error_handler);
    xmlSetGenericErrorFunc (&xslt_errors, xslt_error_handler);

    // Following line is workaround for bug #73088 in Gnome
    // bugzilla. To be removed than it will be fixed.
    xsltSetGenericDebugFunc (&xslt_errors, xslt_error_handler);

    xmlSubstituteEntitiesDefault(1);
    xmlLoadExtDtdDefaultValue = DTD_LOAD_BITS;
    ss = xsltParseStylesheetFile(XMLTools::StrToXmlCast(stylesheet_file));

    if(!ss)
    {
        xsltSetGenericErrorFunc(nullptr, nullptr);
        xmlSetGenericErrorFunc (nullptr, nullptr);

        // Following line is workaround for bug #73088 in Gnome
        // bugzilla. To be removed than it will be fixed.
        xsltSetGenericDebugFunc (nullptr, nullptr);
        throw FWException("File conversion error: Error loading stylesheet: " + 
                          stylesheet_file+
                          (xslt_errors.length()?(string("\nXSLT reports: \n") +
                                                 xslt_errors):string(""))
        );
    }
    
    string buffer = readFile(src_file);
    doc = xmlParseMemory(buffer.c_str(), buffer.length());

    res = xsltApplyStylesheet(ss, doc, params);
    xsltSaveResultToFilename(dst_file.c_str(), res, ss, 0);

    xsltSetGenericErrorFunc(nullptr, nullptr);
    xmlSetGenericErrorFunc (nullptr, nullptr);

    // Following line is workaround for bug #73088 in Gnome
    // bugzilla. To be removed than it will be fixed.
    xsltSetGenericDebugFunc (nullptr, nullptr);
    
    if(!res)
    {
        xsltFreeStylesheet(ss);
        throw FWException("File conversion Error: Error during conversion: " +
                          stylesheet_file+
                          (xslt_errors.length()?(string("XSLT reports: \n") +
                                                 xslt_errors):string(""))
        );
    }

    xsltFreeStylesheet(ss);
    xmlFreeDoc(res);
    xmlFreeDoc(doc);

    xsltCleanupGlobals();
    //xmlCleanupParser();
}

void XMLTools::transformDocumentToFile(xmlDocPtr doc, 
                                       const string &stylesheet_file,
                                       const char **params,
                                       const string &dst_file
)
{
    string xslt_errors;

    LockGuard xslt_processor_mutex_lock(xslt_processor_mutex);
    LockGuard xml_parser_mutex_lock(xml_parser_mutex);
    
    xsltSetGenericErrorFunc(&xslt_errors, xslt_error_handler);
    xmlSetGenericErrorFunc (&xslt_errors, xslt_error_handler);

    // Following line is workaround for bug #73088 in Gnome
    // bugzilla. To be removed when it is fixed.
    xsltSetGenericDebugFunc (&xslt_errors, xslt_error_handler);
    
#ifdef FW_XMLTOOLS_VERBOSE
    xmlSaveFormatFileEnc(".backup_copy.xml", doc, "utf-8", 1);
    cerr << "Backup copy of XML tree is saved to file .backup_copy.xml" << endl;
#endif

    xmlDoValidityCheckingDefaultValue = 0;
    xmlLoadExtDtdDefaultValue = 0;
    xsltStylesheetPtr ss = xsltParseStylesheetFile(
        XMLTools::StrToXmlCast(stylesheet_file));
    xmlDoValidityCheckingDefaultValue = 1;
    xmlLoadExtDtdDefaultValue = DTD_LOAD_BITS;

    if (!ss)
    {
        xsltSetGenericErrorFunc(nullptr, nullptr);
        xmlSetGenericErrorFunc (nullptr, nullptr);

        // Following line is workaround for bug #73088 in Gnome
        // bugzilla. To be removed than it will be fixed.
        xsltSetGenericDebugFunc (nullptr, nullptr);  
        throw FWException("File conversion error: Error loading stylesheet: " + 
                          stylesheet_file+
                          (xslt_errors.length()?(string("\nXSLT reports: \n") +
                                                 xslt_errors) : string(""))
        );
    }
    
    xmlDocPtr res = xsltApplyStylesheet(ss, doc, params);

    xsltSetGenericErrorFunc(nullptr, nullptr);
    xmlSetGenericErrorFunc (nullptr, nullptr);

    // Following line is workaround for bug #73088 in Gnome
    // bugzilla. To be removed than it will be fixed.
    xsltSetGenericDebugFunc (nullptr, nullptr);  

    if (!res)
    {
        xsltFreeStylesheet(ss);
        throw FWException("File conversion Error: Error during conversion: " + 
                          stylesheet_file +
                          (xslt_errors.length()?(string("XSLT reports: \n") +
                                                 xslt_errors) : string(""))
        );
    }

    if (dst_file=="-")
        xsltSaveResultToFile(stdout, res, ss);
    else
        xsltSaveResultToFilename(dst_file.c_str(), res, ss, 0 /* compression */ );

    xmlFreeDoc(res);
    //xmlCleanupParser();
    xsltFreeStylesheet(ss);
}

xmlDocPtr XMLTools::transformDocument(xmlDocPtr doc, 
                                      const string &stylesheet_file,
                                      const char **params)
{
    string xslt_errors;

    LockGuard xslt_processor_mutex_lock(xslt_processor_mutex);
    LockGuard xml_parser_mutex_lock(xml_parser_mutex);
        
    xsltSetGenericErrorFunc(&xslt_errors, xslt_error_handler);
    xmlSetGenericErrorFunc (&xslt_errors, xslt_error_handler);

    // Following line is workaround for bug #73088 in Gnome
    // bugzilla. To be removed than it is fixed.
    xsltSetGenericDebugFunc(&xslt_errors, xslt_error_handler);

    xmlDoValidityCheckingDefaultValue = 0;
    xmlLoadExtDtdDefaultValue         = 0;
    xsltStylesheetPtr ss = xsltParseStylesheetFile(
        XMLTools::StrToXmlCast(stylesheet_file));
    xmlDoValidityCheckingDefaultValue = 1;
    xmlLoadExtDtdDefaultValue         = DTD_LOAD_BITS;

    if (!ss)
    {
        xsltSetGenericErrorFunc(nullptr, nullptr);
        xmlSetGenericErrorFunc (nullptr, nullptr);
        // Following line is workaround for bug #73088 in Gnome
        // bugzilla. To be removed than it will be fixed.
        xsltSetGenericDebugFunc (nullptr, nullptr);  
        throw FWException("File conversion error: Error loading stylesheet: "+stylesheet_file+
                          (xslt_errors.length()?(string("\nXSLT reports: \n")+xslt_errors):string(""))
        );
    }
    
    xmlDocPtr res = xsltApplyStylesheet(ss, doc, params);

    xsltFreeStylesheet(ss);
    xsltSetGenericErrorFunc(nullptr, nullptr);
    xmlSetGenericErrorFunc (nullptr, nullptr);
    // Following line is workaround for bug #73088 in Gnome
    // bugzilla. To be removed than it will be fixed.
    xsltSetGenericDebugFunc (nullptr, nullptr);   

    if (!res)
    {
        throw FWException("File conversion Error: Error during conversion: "+stylesheet_file+
                          (xslt_errors.length()?(string("XSLT reports: \n")+xslt_errors):string(""))
        );
    }
    
    return res;
}

xmlDocPtr XMLTools::convert(xmlDocPtr doc, 
                            const string &file_name, 
                            const string &type_name, 
                            const string &template_dir,
                            const string &current_version)
{
    xmlDocPtr  res = nullptr;
    
    xmlNodePtr root = xmlDocGetRootElement(doc);
    if (!root || !root->name || type_name!=XMLTools::FromXmlCast(root->name))
    {
        xmlFreeDoc(doc);
        //xmlCleanupParser();
        throw FWException("XML file '"+file_name+ "' has invalid structure.");
    }

    string vers;
    const char *v = XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("version")));
    if (v==nullptr)
    {
        // no version.
        v="0.8.7"; // at this version attribute has been introduced
        xmlNewProp(root, 
                   XMLTools::ToXmlCast("version") , 
                   XMLTools::ToXmlCast(v));
        res=doc; // changed
        vers=v;
    } else
    {
        vers=v;
        XMLTools::FreeXmlBuff(v);
    }

#ifdef FW_XMLTOOLS_VERBOSE
        cerr << "File reports version : " << vers << endl;
#endif

    int c;
    while (!vers.empty() && (c=version_compare(current_version,vers))!=0)
    {
        if(c<0)
        {
            string err;
            err += string("The file '");
            err += file_name;
            err += string("' ");
            err += string("was created by a newer version of "
                          "Firewall Builder, please upgrade in order to "
                          "open this file.");
            err += string("\n");
            err += string("\n");
            err += string("DTD version in the file: ");
            err += vers;
            err += string(" current: ");
            err += current_version;
            throw FWException(err);
        }

        string oldversion = vers;
        
#ifdef FW_XMLTOOLS_VERBOSE
        cerr << "Converting from version: " << oldversion << endl;
#endif

        string fname;

        fname = template_dir;

        fname = fname+FS_SEPARATOR+"migration"+FS_SEPARATOR+type_name+"_"+vers+".xslt";

        if (access(fname.c_str() , R_OK )!=0) 
        {
            xmlFreeDoc(doc);
            //xmlCleanupParser();
            throw FWException(
                string("File '" + file_name +
                       "' conversion error: no converter found for version: ") +
                oldversion+".\n" + string("Supposed to be a file ")+fname );
        }
        
        try
        {
            res = transformDocument(doc, fname, nullptr);
        } catch(FWException &ex)
        {
            ex.getProperties()["failed_transformation"]=fname;
            xmlFreeDoc(doc);
            //xmlCleanupParser();
            throw;
        }
        xmlFreeDoc(doc);
        //xmlCleanupParser();
        doc = res;
        
        root = xmlDocGetRootElement(doc);
        if (!root || !root->name || type_name!=XMLTools::FromXmlCast(root->name))
        {
            xmlFreeDoc(doc);
            //xmlCleanupParser();
            throw FWException("File '" + file_name +
                              "' conversion Error: conversion produced file with invalid structure.");
        }

        v = XMLTools::FromXmlCast(xmlGetProp(root, XMLTools::ToXmlCast("version")));
        if (v==nullptr)
        {
            xmlFreeDoc(doc);
            //xmlCleanupParser();
            throw FWException("File '" + file_name +
                              "' conversion error: converted to unknown version.");
        } 

        vers=v;
        XMLTools::FreeXmlBuff(v);
        
        if (version_compare(vers, oldversion) <= 0)
        {
            xmlFreeDoc(doc);
            //xmlCleanupParser();
            throw FWException("File '" + file_name +
                              "' conversion error: conversion did not advance version number!.");
        }
    }

    return res;
}

int XMLTools::major_number(const string &v, string &rest)
{
    string a;
    string::size_type pos=v.find('.');
    if(pos==string::npos)
    {
        a    = v;
        rest = "";
    } else
    {
        a    = v.substr(0,pos);
        rest = v.substr(pos+1);
    }
    //TODO: handle conversion errors, by using 'strtol'
    if (a.empty()) a = "0";
    return atoi(v.c_str());
}

/*
 * Compare two version numbers.
 * If versions have different length (different total number of components)
 * consider  missing components equal to zero. That is,
 * 1.2.3 is equal to 1.2.3.0 so, if we have to compare "1.2.3.4" to "1.2.3", then
 * this is equivalent to comparing "1.2.3.4" to "1.2.3.0". The opposite
 * is also true.
 */
int XMLTools::version_compare(const string &v1, const string &v2)
{
    string rest1, rest2;
    int x1=major_number(v1, rest1);
    int x2=major_number(v2, rest2);
    if (rest1.empty() && rest2.empty()) return x1-x2;
    if (rest1.empty() && !rest2.empty()) rest1 = "0";
    if (rest2.empty() && !rest1.empty()) rest2 = "0";
    if (x1!=x2) return x1-x2;
    else return version_compare(rest1, rest2);
}

string XMLTools::quote_linefeeds(const string &s)
{
    string res;

    for(string::size_type i=0;i<s.size();i++)
        if(s[i]=='\n')
            res.append("\\n");
        else
            res.append(1, s[i]);
    
    return res;
}

string XMLTools::unquote_linefeeds(const string &s)
{
    string res;
    
    for(string::size_type i=0;i<s.size();i++)
    {
        char c=s[i];
        if(c=='\\')
            if(i<(s.size()-1))
                if(s[i+1]=='n')
                {
                    c='\n';
                    i++;
                }
        res.append(1, c);
    }
    
    return res;
}

string XMLTools::cleanForUTF8(const string &str)
{
    string res;
    
    if ( xmlCheckUTF8(XMLTools::ToXmlCast(str.c_str()))==0 )
    {
        for (string::size_type i=0; i<str.size(); ++i)
        {
            char c[2];
            c[1]='\0';
            c[0]=str[i];
            if ( xmlCheckUTF8(XMLTools::ToXmlCast(c))==0 )
            {
                res += "?";
            } else  res += c;
        }

    } else res=str;

    return res;
}

string XMLTools::cleanForNVTASCII(const string &str)
{
    string res=str;
    for (string::size_type i=0; i<res.size(); ++i)
    {
        unsigned int u=res[i];
        if (u>127) res[i]='?';
    }
    return res;
}

#undef DTD_LOAD_BITS

#undef FW_XMLTOOLS_VERBOSE

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
 * This file contains assorted XML handling code
 */

#ifndef __XML_TOOLS_HH_FLAG__
#define __XML_TOOLS_HH_FLAG__


#include "fwbuilder/Tools.h"
#include "fwbuilder/FWException.h"

#include <string>
#include <functional>
#include <type_traits>

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlIO.h>
#include <libxml/parserInternals.h>
#include <libxml/xmlmemory.h>


namespace libfwbuilder
{

class XMLTools
{
    public:

    static const char * FromXmlCast(xmlChar * c) { return reinterpret_cast<const char *>(c); }
    static const char * FromXmlCast(const xmlChar * c) { return reinterpret_cast<const char *>(c); }
    static xmlChar * ToXmlCast(char * c) { return reinterpret_cast<xmlChar *>(c); }
    static xmlChar * ToXmlCast(const char * c) { return ToXmlCast(const_cast<char *>(c)); }
    static xmlChar * StrToXmlCast(const std::string s) { return ToXmlCast(s.c_str()); }
    template<class T>
    static void FreeXmlBuff(const T t) {
        xmlFree(reinterpret_cast<void *>(const_cast< typename std::remove_const<T>::type>(t) >(t)));
    }

    static xmlNodePtr getXmlNodeByPath(xmlNodePtr r,const char   *path       );
    static xmlNodePtr getXmlNodeByPath(xmlNodePtr r,const std::string &path  );

    static xmlNodePtr getXmlChildNode (xmlNodePtr r,const char   *child_name );

    static void initXMLTools();
    static void close();

    class UpgradePredicate
    {
        public:

        virtual ~UpgradePredicate() {}

        virtual bool operator()(const std::string&) const 
        { 
            return true;
        }
    };

    static std::string readFile(const std::string &file_name);
    
    /**
     * Loads given file, performing version conversion
     * if neccessary.
     */
    static xmlDocPtr loadFile(const std::string &file_name, 
                              const std::string &type_name, 
                              const std::string &dtd_file,
                              const UpgradePredicate *upgrade,
                              const std::string &template_dir,
                              const std::string &current_version = std::string(FWBUILDER_XML_VERSION)
    );

    static void setDTD(xmlDocPtr doc, 
                       const std::string &type_name, 
                       const std::string &dtd_file);
    
    /**
     * Saves to file with setting DTD.
     */
    static void saveFile(xmlDocPtr doc, 
                         const std::string &file_name, 
                         const std::string &type_name,
                         const std::string &dtd_file);

    /**
     * Saves XML document to the memory buffer
     */
    static void dumpToMemory(xmlDocPtr doc, 
                             xmlChar **buffer,
                             int      *size,
                             const std::string &type_name,
                             const std::string &dtd_file);
        
    static xmlExternalEntityLoader defaultLoader;

    /**
     * parses contents of the file file_name which is preloaded into
     * buffer, without version conversion. File name is passed for
     * pretty error printing.
     *
     * @return document pointer
     */
    static xmlDocPtr parseFile(const std::string &file_name,
                               const std::string &buffer, 
                               bool use_dtd, const std::string &template_dir
    );
    
    /**
     * Performs XSLT transformation of the document in memory
     * @return new document
     */
    static xmlDocPtr transformDocument(xmlDocPtr doc, 
                                       const std::string &stylesheet_file,
                                       const char **params
    );

    /**
     * Performs XSLT transformation of the document. Results are
     * stored in dst file.
     */
    static void transformDocumentToFile(xmlDocPtr doc, 
                                        const std::string &stylesheet_file,
                                        const char **params,
                                        const std::string &dst_file
    );

    /**
     * Performs XSLT transformation of the src file. Results are
     * stored in dst file.
     */
    static void transformFileToFile(const std::string &src_file,
				    const std::string &stylesheet_file,
				    const char **params,
				    const std::string &dst_file
    );


    static std::string quote_linefeeds  (const std::string &s);
    static std::string unquote_linefeeds(const std::string &s);

    /** 
     * checks all characters in str and makes sure they all conform
     * with UTF8 encoding. Replaces unconforming characters with '?'
     */
    static std::string cleanForUTF8(const std::string &str);

    /** 
     * checks all characters in str and makes sure they all conform
     * to NVT ASCII encoding. Replaces unconforming characters with '?'
     */
    static std::string cleanForNVTASCII(const std::string &str);


    /**
     * Compares 2 product version numbers in dotted notation
     * @return 0 if equal, positive if v1>v2, negative if v1<v2
     */
    static int version_compare(const std::string &v1, const std::string &v2);

    
    private:
    
    /**
     * Convert file from older version to current one
     * @return pointer to new document or nullptr if doc is unchanged.
     *         if pointer to new document is returned, doc parameter
     *         becomes invalid and should not be used.
     */
    static xmlDocPtr convert(xmlDocPtr doc, 
                             const std::string &file_name, 
                             const std::string &type_name,
                             const std::string &template_dir,
                             const std::string &current_version = std::string(FWBUILDER_XML_VERSION)
    );

    /**
     * returns first component of dotted notation.
     */
    static int major_number(const std::string &v, std::string &rest);

};

}

#endif



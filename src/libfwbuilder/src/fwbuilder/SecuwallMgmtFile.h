/*
 * SecuwallMgmtFile.h - hosts(5) file format parser
 *
 * Copyright (c) 2008 secunet Security Networks AG
 * Copyright (c) 2008 Adrian-Ken Rueegsegger <rueegsegger@swiss-it.ch>
 * Copyright (c) 2008 Reto Buerki <buerki@swiss-it.ch>
 *
 * This work is dual-licensed under:
 *
 * o The terms of the GNU General Public License as published by the Free
 *   Software Foundation, either version 2 of the License, or (at your option)
 *   any later version.
 *
 * o The terms of NetCitadel End User License Agreement
 */

#ifndef __SECUWALLMGMT_FILE_H_
#define __SECUWALLMGMT_FILE_H_

#include "FWException.h"

#include <iostream>
#include <string>
#include <map>

namespace libfwbuilder
{

    /**
     * This class is parser for file in hosts(5) format
     * (e.g. /etc/hosts)
     */
    class SecuwallMgmtFile
    {
    public:

        void parse(const std::string &filename);
        void parse(std::istream &from);

        /* Return data */
        std::map<std::string, std::string> getData() { return data; }

    private:

        std::map<std::string, std::string> data;
    };

}

#endif


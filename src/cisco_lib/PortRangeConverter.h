/* 

                          Firewall Builder

                 Copyright (C) 2011 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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

#ifndef _PORT_RANGE_CONVERTER_HH
#define _PORT_RANGE_CONVERTER_HH

#include <string>
#include <sstream>


class PortRangeConverter
{
    int rs;
    int re;

public:

    PortRangeConverter(int range_start, int range_end)
    { rs = range_start; re = range_end; }

    std::string toString()
    {
        std::ostringstream  str;

        if (rs<0) rs = 0;
        if (re<0) re = 0;

        if (rs>0 || re>0)
        {
            if (rs==re)  str << "eq " << rs << " ";
            else
            {
                if (rs==0 && re!=0)
                {
                    str << "lt " << re + 1 << " ";
                } else
                {
                    if (rs!=0 && re==65535)
                    {
                        str << "gt " << rs - 1 << " ";
                    } else
                    {
                        str << "range " << rs << " " << re << " ";
                    }
                }
            }
        }

        return str.str();
    }
};

#endif


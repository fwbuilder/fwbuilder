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


#ifndef __FW_EXCEPTION_HH_FLAG__
#define __FW_EXCEPTION_HH_FLAG__

#include <string>
#include <map>

namespace libfwbuilder
{

/**
 * Base class for all Exceptions used in FWBuilder project
 */
class FWException
{
    private:
    
    protected:
		std::string reason;
		std::map<std::string, std::string> properties;
    
    public:

    /**
     * Creates exception with given reason
     */
    FWException(const std::string &reason);
    virtual ~FWException() {};

    /**
     * Returns textual representation of this exception
     */
    virtual const std::string &toString() const;

    std::map<std::string, std::string>& getProperties();
    
};

class FWNotSupportedException: public FWException
{
    public:

    FWNotSupportedException(const std::string &reason):FWException(reason) {};
};

}

#endif


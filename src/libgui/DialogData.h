/* 

                          Firewall Builder

                 Copyright (C) 2004 NetCitadel, LLC

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


#ifndef __DIALOGOPTIONS_H_
#define __DIALOGOPTIONS_H_


#include <list>

class QWidget;
#include <qstring.h>
#include <qstringlist.h>

namespace libfwbuilder {
    class FWObject;
};

class DialogData;

class DialogOption
{
    friend class DialogData;

 public:
    enum DataType { Unknown, String, Int, Bool };

 private:
    QWidget *w;
    libfwbuilder::FWObject *obj;
    QString attr;
    QStringList mapping;
    QString override_str_val;
    int override_int_val;
    DataType dtype;

    DialogOption(QWidget *widget, libfwbuilder::FWObject *obj, const char* attr);
    DialogOption(QWidget *widget, libfwbuilder::FWObject *obj, const char* attr, QStringList mapping);

    void overrideValue(const QString &val) { override_str_val=val; }
    void overrideValue(int val)  { override_int_val=val; }

    DataType type() { return dtype; }
};


class DialogData
{
    std::list<DialogOption>  options;

    void loadToWidget( DialogOption &dopt , bool override=false );
    
 public:
    DialogData();
    ~DialogData();

/**
 * destroy all registered options, prepare for reuse
 */
    void clear();
    
/**
 * registers an option that is stored in object 'obj' as an attribute
 * 'attr' and controlled by widget 'widget' in the dialog. 
 */
    void registerOption(QWidget *widget,
                        libfwbuilder::FWObject *obj,
                        const char *attr);

/**
 * like the method above, plus adds a maping between option value
 * rendered in the widget and value stored in the object
 * attribute. This is mostly used for combo boxes.  Mapping is defined
 * by an array of strings in the following format:
 *
 *     "Linux 2.4" , "linux24",
 *     "IPFilter"  , "ipf",
 *     "Cisco PIX" , "pix",
 *     nullptr,         nullptr
 *
 *  Odd strings correspond to the data in the widget, while even
 *  strings define what is stored in the object (counting strings in
 *  the array from 1).
 */
    void registerOption(QWidget *widget,
                        libfwbuilder::FWObject *obj,
                        const char *attr,
                        QStringList map);

/**
 * sets value 'val' in the widget that corresponds to attribute 'attr'
 */
    void setWidgetValue(const char *attr,const QString &val);
    void setWidgetValue(const char *attr,int val);
    
/**
 * loads all options from their respective object attributes to widgets
 */
    void loadAll();

/**
 * saves all options from their widgets to object attributes
 */
    void saveAll(libfwbuilder::FWObject *new_obj=nullptr);
};

#endif

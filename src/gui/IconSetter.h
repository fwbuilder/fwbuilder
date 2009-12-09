#ifndef ICONSETTER_H
#define ICONSETTER_H

#include "../../config.h"
#include "global.h"
#include "fwbuilder/FWObject.h"
#include <QPixmap>

class IconSetter
{
public:
    virtual bool isStandardFolder(libfwbuilder::FWObject*)
    {
        return true;
    }

    void setObjectIcon(libfwbuilder::FWObject *obj, QPixmap *pm, int icon_size);
};

#endif // ICONSETTER_H

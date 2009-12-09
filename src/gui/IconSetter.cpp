#include "IconSetter.h"
#include "utils.h"

void IconSetter::setObjectIcon(libfwbuilder::FWObject *obj, QPixmap *pm, int icon_size)
{
    QString icn_alias;
    QString icn_sfx;

    switch (icon_size)
    {
    case 0: icn_sfx = "icon-tree"; break;
    case 2: icn_sfx = "icon-big"; break;
    default: icn_sfx = "icon"; break;
    }

    if (obj->getRO())
        icn_alias = ":/Icons/lock";
    else
    {
        if (isStandardFolder(obj))
            icn_alias = ":/Icons/SystemGroup/" + icn_sfx;
        else
            icn_alias = QString(":/Icons/") + obj->getTypeName().c_str() + "/" + icn_sfx;
    }

    LoadPixmap(icn_alias, *pm);
}

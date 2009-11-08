#ifndef FWOBJECTSTATE_H
#define FWOBJECTSTATE_H

#include "fwbuilder/FWObject.h"
#include <QString>

/********************************************************
 * FWObjectState
 ********************************************************/

class FWObjectState
{
public:
    QString name;
    QString comment;

    FWObjectState() {}

    FWObjectState(libfwbuilder::FWObject* object) {save(object);}

    virtual void save(libfwbuilder::FWObject* object)
    {
        name = QString::fromUtf8(object->getName().c_str());
        comment = QString::fromUtf8(object->getComment().c_str());
    }

    virtual void restore(libfwbuilder::FWObject* object)
    {
        object->setName(std::string(name.toUtf8().constData()));
        object->setComment(std::string(comment.toUtf8().constData()));
    }
};

class FWObjectStateTime : public FWObjectState
{
public:
    FWObjectStateTime() {}

    FWObjectStateTime(libfwbuilder::FWObject* object) : FWObjectState(object) {}
};

#endif // FWOBJECTSTATE_H

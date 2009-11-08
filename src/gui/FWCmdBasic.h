#ifndef FWCMDBASIC_H
#define FWCMDBASIC_H

#include <QUndoCommand>

#include "ProjectPanel.h"

#include "fwbuilder/FWObject.h"

class FWCmdBasic : public QUndoCommand
{
    int id;
protected:

    ProjectPanel *project;
public:
    FWCmdBasic(ProjectPanel *project);
    int objectId() {return id;}
    void setObjectId(libfwbuilder::FWObject *object) {id = object->getId();}
};

#endif // FWCMDBASIC_H

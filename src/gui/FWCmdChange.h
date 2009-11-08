#ifndef FWCMDCHANGE_H
#define FWCMDCHANGE_H

#include "FWCmdBasic.h"
#include "FWObjectState.h"

/********************************************************
 * FWCmdChange
 ********************************************************/

class FWCmdChange : public FWCmdBasic
{
protected:
    FWObjectState *oldState;
    FWObjectState *newState;

public:
    FWCmdChange(ProjectPanel *project, FWObjectState *oldState, FWObjectState *newState);
    ~FWCmdChange();

};

/********************************************************
 * FWCmdChangeTime
 ********************************************************/

class FWCmdChangeTime : public FWCmdChange
{

    FWObjectState* createState(libfwbuilder::FWObject *object);
public:
    FWCmdChangeTime(ProjectPanel *project, libfwbuilder::FWObject *object, FWObjectState *newState):
            FWCmdChange(project, createState(object), newState) {setText("edit object");}

    void redo() {}
    void undo() {}

};


#endif // FWCMDCHANGE_H

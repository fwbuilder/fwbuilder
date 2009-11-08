#include "FWCmdChange.h"


FWCmdChange::FWCmdChange(ProjectPanel *project, FWObjectState *oldState, FWObjectState *newState):
            FWCmdBasic(project)
{
    this->oldState = oldState;
    this->newState = newState;
}

FWCmdChange::~FWCmdChange()
{
    delete (oldState);
    delete (newState);
}

FWObjectState* FWCmdChangeTime::createState(libfwbuilder::FWObject *object)
{
    return new FWObjectStateTime(object);
}

#ifndef FWOBJECTSELECTIONMODEL_H
#define FWOBJECTSELECTIONMODEL_H

#include <QModelIndex>

namespace libfwbuilder
{
    class FWObject;
}

class FWObjectSelectionModel
{
public:
    libfwbuilder::FWObject *selectedObject;
    libfwbuilder::FWObject *selectedObjectOld;
    QModelIndex index;
    FWObjectSelectionModel();
    void setSelected(libfwbuilder::FWObject *, const QModelIndex &index);
    void reset();
};


#endif // FWOBJECTSELECTIONMODEL_H

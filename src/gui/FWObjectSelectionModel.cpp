#include "FWObjectSelectionModel.h"
#include "fwbuilder/Firewall.h"

using namespace libfwbuilder;

FWObjectSelectionModel::FWObjectSelectionModel()
{
}

void FWObjectSelectionModel::setSelected(FWObject * so, const QModelIndex &index)
{
    this->selectedObjectOld = this->selectedObject;
    this->selectedObject = so;

    this->index = index;
}

void FWObjectSelectionModel::reset()
{
    QModelIndex index;
    setSelected(NULL, index);
}

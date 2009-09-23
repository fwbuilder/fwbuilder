#include "ColDesc.h"
#include "platforms.h"

ColDesc::ColDesc()
{
    this->type = Unknown;
}

ColDesc::ColDesc(QString origin, ColumnType type)
{
    this->origin = origin;
    this->name = getReadableRuleElementName(origin.toStdString());
    this->type = type;
}

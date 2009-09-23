#ifndef COLDESC_H
#define COLDESC_H

 #include <QMetaType>

class ColDesc
{
public:
     enum ColumnType
     {
         GroupHandle,
          RuleOp,
          Object,
          Action,
          Direction,
          Options,
          Time,
          Comment,
          Metric,
          Unknown
      };
    ColDesc(QString origin, ColumnType type);
    ColDesc();
    QString name;
    QString origin;
    ColumnType type;

};

Q_DECLARE_METATYPE(ColDesc)

#endif // COLDESC_H

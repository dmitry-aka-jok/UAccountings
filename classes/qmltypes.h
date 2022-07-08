#ifndef QMLTYPES_H
#define QMLTYPES_H

#include <QObject>

namespace SqlType
{
Q_NAMESPACE
enum Value {
    NaT=0,
    PK,
    FK,
    String,
    Int,
    Bool,
    Numeric,
    Date,
    DateTime,
    Time,

    /*service column*/
    ServiceColumn,
    AppendRow,
    TotalsRow
};
Q_ENUM_NS(Value)
}

Q_DECLARE_METATYPE(SqlType::Value)



#endif // QMLTYPES_H

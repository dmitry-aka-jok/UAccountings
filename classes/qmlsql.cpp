#include "qmlsql.h"

#include <QVariant>


QString QmlSql::formatToSQL(QVariant val)
{
    QString type = val.metaType().name();
    if (type==u"QString"_qs)
        return u"'%1'"_qs.arg(val.toString());
    if (type==u"QDate"_qs || type==u"QDateTime"_qs || type==u"QTime"_qs)
        return u"'%1'"_qs.arg(val.toString());

    return val.toString();
}

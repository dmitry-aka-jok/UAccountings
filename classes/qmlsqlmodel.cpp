#include "qmlsqlmodel.h"


QmlSqlModel::QmlSqlModel(Datapipe *datapipe, QObject *parent) :
    QSqlQueryModel(parent), datapipe (datapipe)
{

}


QString QmlSqlModel::queryString() const
{
    return  m_queryString;
}

void QmlSqlModel::setQueryString(const QString &queryString)
{
    if ( m_queryString == queryString )
        return;
    m_queryString = queryString ;
    emit queryStringChanged();
}


QStringList QmlSqlModel::rolesList() const
{
    return m_roleList;
}

QStringList QmlSqlModel::typesList() const
{
    return m_typeList;
}


QString QmlSqlModel::errorString() const
{
    return m_errorString;
}


void QmlSqlModel::exec()
{
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQueryModel::setQuery(m_queryString, db);
    if ( this->lastError().isValid() )
    {
        qCritical()<<"Error in query";
        qCritical()<<m_queryString;
        qCritical()<<this->lastError().text();

        m_errorString = this->lastError().text();
        emit errorStringChanged();

        return ;
    }

    m_roleList.clear();
    m_typeList.clear();
    for( int i = 0; i < record().count(); i++)
    {
        m_roleList.append(record().fieldName(i).toLatin1());
        m_typeList.append(record().field(i).metaType().name());
    }
    emit rolesListChanged();

}

void QmlSqlModel::clearModel()
{
    this->clear();
}

QHash<int, QByteArray>QmlSqlModel::roleNames() const
{
    QHash<int,QByteArray> hash;
    for( int i = 0; i < record().count(); i++)
    {
        hash.insert(Qt::UserRole + i + 1, "Field."_qba+QByteArray::number(i)); // QByteArray(record().fieldName(i).toLatin1())
    }
    return hash;
}


QVariant QmlSqlModel::data(const QModelIndex &index, int role)const
{
    QVariant value = QSqlQueryModel::data(index, role);
    if(role < Qt::UserRole)
    {
        value = QSqlQueryModel::data(index, role);
    }
    else
    {
        int columnIdx = role - Qt::UserRole - 1;
        QModelIndex modelIndex = this->index(index.row(), columnIdx);
        value = QSqlQueryModel::data(modelIndex, Qt::DisplayRole);
    }
    return value;
}

QVariant QmlSqlModel::value(int row, int column, QVariant deflt) const
{
    QModelIndex modelIndex = this->index(row, column);
    QVariant val = QSqlQueryModel::data(modelIndex, Qt::DisplayRole);
    if(val.isValid())
        return val;
    else
        return deflt;
}

QVariant QmlSqlModel::value(int row, QString column, QVariant deflt) const
{
    return value(row, m_roleList.indexOf(column), deflt);
}

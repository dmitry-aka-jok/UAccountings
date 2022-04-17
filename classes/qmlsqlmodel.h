#ifndef QMLSQLMODEL_H
#define QMLSQLMODEL_H

#include <QSqlQueryModel>
#include <QSortFilterProxyModel>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>

#include <QDebug>
#include <QVariant>
#include <QStringList>

#include "datapipe.h"

class QmlSqlModel : public QSortFilterProxyModel
{
    Q_OBJECT
//    Q_PROPERTY(QStringList rolesList READ rolesList  NOTIFY rolesListChanged)
//    Q_PROPERTY(QString filterString READ filterString WRITE setFilterString NOTIFY filterStringChanged)
//    Q_PROPERTY(int filterColumn READ filterColumn WRITE setFilterColumn NOTIFY filterColumnChanged)

public:
    explicit QmlSqlModel(QObject *parent = nullptr);
    ~QmlSqlModel();

    Q_INVOKABLE QStringList rolesList() const;
    Q_INVOKABLE QStringList typesList() const;

    QString errorString ()const;

    bool exec(const QString &queryString);

    QVariant data(const QModelIndex &index, int role)const;

    Q_INVOKABLE QVariant value(int row, int column, QVariant deflt = QVariant()) const;
    Q_INVOKABLE QVariant value(int row, QString column, QVariant deflt = QVariant()) const;

    QHash<int, QByteArray>roleNames() const;

    Q_INVOKABLE QString filterString() const;
    Q_INVOKABLE void setFilterString(const QString &filterString);

    Q_INVOKABLE int filterColumn();
    Q_INVOKABLE void setFilterColumn(int filterColumn);

    Q_INVOKABLE void setSort(int row,  Qt::SortOrder order);

signals:
    void filterStringChanged(QString filterString);
    void filterColumnChanged(int filterColumn);

    void rolesListChanged();
    void errorOccurred();

protected:
    bool filterAcceptsRow(int sourceRow,const QModelIndex &sourceParent) const;

private:
    QString m_queryString;
    QStringList m_roleList;
    QStringList m_typeList;
    QString m_errorString;
    QString m_filterString;
    int m_filterColumn;

    Datapipe *datapipe;
    QSqlQueryModel sqlmodel;
};
#endif // QMLSQLMODEL_H

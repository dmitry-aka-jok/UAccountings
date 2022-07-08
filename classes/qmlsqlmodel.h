#ifndef QMLSQLMODEL_H
#define QMLSQLMODEL_H

#include <QSqlQueryModel>
#include <QSortFilterProxyModel>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>

#include <QVariant>
#include <QStringList>

#include "macro.h"
#include "qmltypes.h"
#include "qmlsqlquery.h"

class QmlSqlModel : public QSortFilterProxyModel
{
    Q_OBJECT

    UA_PROP_RW(bool, serviceColumn, setServiceColumn)
    UA_PROP_RW(bool, totalsRow,     setTotalsRow)
    UA_PROP_RW(bool, appendRow,     setAppendRow)


    UA_PROP_RW(QString, filterString, setFilterString)
    UA_PROP_RW(int, filterColumn, setFilterColumn)

    UA_PROP_RO(QString, errorString)

public:
    explicit QmlSqlModel(QObject *parent = nullptr);
    ~QmlSqlModel();

    bool exec(const QString &table, QVariantMap queryMap);
    Q_INVOKABLE bool reread();

    QVariant data(const QModelIndex &index, int role) const override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    Q_INVOKABLE QVariantMap sqlIndex(int row) const;

    QHash<int, QByteArray>roleNames() const override;

    Q_INVOKABLE void setSort(int row,  Qt::SortOrder order);

signals:
    void errorOccurred();

protected:
    bool filterAcceptsRow(int sourceRow,const QModelIndex &sourceParent) const override;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

private:
    QString m_table;
    QVariantMap m_query;
    //QVariantMap m_fields;
    QStringList m_fieldsPK;

    PrepearedQuery m_prepQuery;
    QSqlQueryModel sqlmodel;
};
#endif // QMLSQLMODEL_H

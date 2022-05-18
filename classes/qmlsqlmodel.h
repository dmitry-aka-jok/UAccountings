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


class QmlSqlModel : public QSortFilterProxyModel
{
    Q_OBJECT

    Q_PROPERTY(bool serviceColumn READ serviceColumn WRITE setServiceColumn NOTIFY serviceColumnChanged)
    Q_PROPERTY(bool totalsRow READ totalsRow WRITE setTotalsRow NOTIFY totalsRowChanged)
    Q_PROPERTY(bool appendRow READ appendRow WRITE setAppendRow NOTIFY appendRowChanged)

public:
    explicit QmlSqlModel(QObject *parent = nullptr);
    ~QmlSqlModel();

    Q_INVOKABLE QStringList rolesList() const;
    Q_INVOKABLE QStringList typesList() const;

    QString errorString ()const;

    bool exec(const QString &table, const QVariantMap &fields, const QVariantMap &query);
    Q_INVOKABLE bool reread(); //


    QVariant data(const QModelIndex &index, int role) const override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    Q_INVOKABLE QVariantMap sqlIndex(int row) const;
    Q_INVOKABLE QVariant value(int row, int column, QVariant deflt = QVariant()) const;
    Q_INVOKABLE QVariant value(int row, QString column, QVariant deflt = QVariant()) const;

    QHash<int, QByteArray>roleNames() const override;

    Q_INVOKABLE QString filterString() const;
    Q_INVOKABLE void setFilterString(const QString &filterString);

    Q_INVOKABLE int filterColumn();
    Q_INVOKABLE void setFilterColumn(int filterColumn);

    Q_INVOKABLE void setSort(int row,  Qt::SortOrder order);


    void setServiceColumn(bool serviceColumn);
    bool serviceColumn();
    void setTotalsRow(bool totalsRow);
    bool totalsRow();
    void setAppendRow(bool appendRow);
    bool appendRow();



//    QModelIndex index(int row, int column, const QModelIndex& parent=QModelIndex()) const override;
//    QModelIndex parent(const QModelIndex &index) const override;
//    QModelIndex mapFromSource(const QModelIndex &source) const override;
//    QModelIndex mapToSource(const QModelIndex &proxy) const override;


signals:
    void filterStringChanged(QString filterString);
    void filterColumnChanged(int filterColumn);

    void serviceColumnChanged();
    void totalsRowChanged();
    void appendRowChanged();


    void rolesListChanged();
    void errorOccurred();

protected:
    bool filterAcceptsRow(int sourceRow,const QModelIndex &sourceParent) const override;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
private:
    QString m_table;
    QVariantMap m_query;
    QVariantMap m_fields;
    QVariantMap m_fieldsPK;

    QStringList m_roleList;
    QStringList m_typeList;
    QList<int> m_digitsList;
    QString m_errorString;
    QString m_filterString;
    int m_filterColumn;

    bool m_serviceColumn;
    bool m_totalsRow;
    bool m_appendRow;

    QString m_queryString;
    QSqlQueryModel sqlmodel;
};
#endif // QMLSQLMODEL_H

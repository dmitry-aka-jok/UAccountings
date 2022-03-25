#ifndef QMLSQLMODEL_H
#define QMLSQLMODEL_H

#include <QSqlQueryModel>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>

#include <QDebug>
#include <QVariant>
#include <QStringList>

#include "datapipe.h"

class QmlSqlModel : public QSqlQueryModel
{
    Q_OBJECT
    Q_PROPERTY(QString queryString READ queryString WRITE setQueryString NOTIFY queryStringChanged)
    Q_PROPERTY(QStringList rolesList READ rolesList  NOTIFY rolesListChanged)
//    Q_PROPERTY(QStringList typesList READ typesList  NOTIFY rolesListChanged)
    Q_PROPERTY(QString errorString READ errorString NOTIFY errorStringChanged)

public:

    explicit QmlSqlModel(Datapipe *datapipe, QObject *parent = nullptr);

    QString queryString()const;
    void setQueryString(const QString &queryString);

    QStringList rolesList() const;
    Q_INVOKABLE QStringList typesList() const;

    QString errorString ()const;

    Q_INVOKABLE void exec();
    Q_INVOKABLE void clearModel();

    QVariant data(const QModelIndex &index, int role)const;

    Q_INVOKABLE QVariant value(int row, int column, QVariant deflt = QVariant()) const;
    Q_INVOKABLE QVariant value(int row, QString column, QVariant deflt = QVariant()) const;

    QHash<int, QByteArray>roleNames() const;


signals:
    void completed();

    void queryStringChanged();
    void rolesListChanged();
    void errorStringChanged();

private:
        QString m_queryString;
        QStringList m_roleList;
        QStringList m_typeList;
        QString m_errorString;
        Datapipe *datapipe;
};
#endif // QMLSQLMODEL_H

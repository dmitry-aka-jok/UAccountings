#ifndef DATAPIPE_H
#define DATAPIPE_H

#include <QObject>
#include <QSettings>

#include <QJSValue>
#include <QQuickItem>

#include "qmltreeelement.h"
#include "qmltreemodel.h"
#include "qmlsqltable.h"
#include "qmlsqlmodel.h"


#define SERVICETABLE_SETTINGS u"settings"_qs
#define SERVICETABLE_USERS u"users"_qs

#define DEFAULTS_TABLE_MINIMAL_WEIGHT 50
#define DEFAULTS_TABLE_SERVICE_WEIGHT 30


class Datapipe : public QObject
{
    Q_OBJECT
public:
//    explicit Datapipe(QObject *parent = nullptr);
    static Datapipe* instance()
    {
        static Datapipe singleton;
        return &singleton;
    }

    ~Datapipe();

    Q_INVOKABLE QSettings* getSettings();
    Q_INVOKABLE QVariant settings(const QString &name, const QVariant &deflt = QVariant());

    Q_INVOKABLE QVariant variable(const QString &name, QVariant deflt = QVariant());
    Q_INVOKABLE void setVariable(const QString &name, QVariant var);

    Q_INVOKABLE QStringList tables();
    Q_INVOKABLE void defineTable(const QString &name, QVariantMap fields);
    QVariantMap tableDefinition(const QString &name);
    Q_INVOKABLE QmlSqlTable *table(const QString &name);


    Q_INVOKABLE void setJsonMenu(const QByteArray &json);
    Q_INVOKABLE QString jsonMenu();

    Q_INVOKABLE QQuickItem *findTableField(QQuickItem *parent, const QString& name);

    Q_INVOKABLE QString inetAdresses();

    bool isSame(const QVariantMap &v1, const QVariantMap &v2);

private:
    explicit Datapipe();
    Datapipe& operator=(const Datapipe& rhs);


    QString menuFromArray(const QJsonArray& ar, int deep);

    QSettings* m_settings;
    QVariantMap m_variables;
    QVariantMap m_tables;

    QByteArray m_jsonMenu;
};

#endif // DATAPIPE_H

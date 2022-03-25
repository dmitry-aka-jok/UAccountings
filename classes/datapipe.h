#ifndef DATAPIPE_H
#define DATAPIPE_H

#include <QObject>
#include <QSettings>

class Datapipe : public QObject
{
    Q_OBJECT
public:
    explicit Datapipe(QObject *parent = nullptr);
    ~Datapipe();

    Q_INVOKABLE QSettings* getSettings();
    Q_INVOKABLE QVariant settings(const QString &name, const QVariant &deflt = QVariant());

    Q_INVOKABLE QVariant variable(const QString &name, QVariant deflt = QVariant());
    Q_INVOKABLE void setVariable(const QString &name, QVariant var);

    Q_INVOKABLE QStringList tables();
    Q_INVOKABLE QVariantMap table(const QString &name);
    Q_INVOKABLE void addTable(const QString &name, QVariantMap fields);

    Q_INVOKABLE QString inetAdresses();

private:
    QSettings* m_settings;

    QVariantMap m_variables;

    QVariantMap m_tables;

};

#endif // DATAPIPE_H

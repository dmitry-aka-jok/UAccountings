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

    Q_INVOKABLE QVariant variable(const QString &name);
    Q_INVOKABLE void setVariable(const QString &name, QVariant var);

    Q_INVOKABLE QString inetAdresses();

private:
    QSettings* m_settings;
    QMap<QString, QVariant> m_variables;

};

#endif // DATAPIPE_H

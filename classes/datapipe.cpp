#include "datapipe.h"

#include <QCoreApplication>
#include <QNetworkInterface>

Datapipe::Datapipe(QObject *parent) : QObject(parent)
{
    QString s = QCoreApplication::applicationDirPath() + "/" + QCoreApplication::applicationName();

    m_settings = new QSettings(s + ".ini", QSettings::IniFormat);
   // m_settings->setIniCodec(QTextCodec::codecForName("UTF-8"));
}

Datapipe::~Datapipe()
{
    delete m_settings;
}

QSettings* Datapipe::getSettings()
{
    return m_settings;
}

QVariant Datapipe::settings(const QString &name, const QVariant &deflt)
{
    return m_settings->value(name, deflt);
}

QVariant Datapipe::variable(const QString &name, QVariant deflt)
{
    if(m_variables.value(name).isValid())
        return m_variables.value(name);
    else
        return deflt;
}

void Datapipe::setVariable(const QString &name, QVariant var)
{
    m_variables[name] = var;
}

QStringList Datapipe::tables()
{
    return m_tables.keys();
}

QVariantMap Datapipe::table(const QString &name)
{
    return m_tables.value(name).toMap();
}

void Datapipe::addTable(const QString &name, QVariantMap fields)
{
    m_tables[name] = fields;
}

QString Datapipe::inetAdresses()
{
    QString addr;
    const QHostAddress &localhost = QHostAddress(QHostAddress::LocalHost);
    for (const QHostAddress &address: QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost)
             addr += (addr.isEmpty()?"":", ") + address.toString();
    }
    return addr;
}





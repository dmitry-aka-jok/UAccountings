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

QVariant Datapipe::variable(const QString &name)
{
    return m_variables.value(name);
}

void Datapipe::setVariable(const QString &name, QVariant var)
{
    m_variables[name] = var;
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





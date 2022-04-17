#include "datapipe.h"

#include <QCoreApplication>
#include <QNetworkInterface>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

Datapipe::Datapipe() : QObject(nullptr)
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

void Datapipe::setJsonMenu(const QByteArray &json)
{
    m_jsonMenu = json;
}

QString Datapipe::menuFromArray(const QJsonArray &ar, int deep)
{
    QString result;
    QString deeps;
    deeps.fill(' ',deep*3);

    for (auto menu : ar) {
        QJsonObject element = menu.toObject();
        if(element.value(u"type"_qs).toString()==u"Item"_qs){
            result += u"\n%1MenuItem{\n   %2text:\"%3\"\n\n   %4onTriggered:{windowManager.push(\"file:%5\")}\n%6}"_qs.arg(
                        deeps,
                        deeps,
                        element.value(u"name"_qs).toString(),
                        deeps,
                        element.value(u"qml"_qs).toString(),
                        deeps
                        );
        }
        if(element.value(u"type"_qs).toString()==u"Separator"_qs){
            result += u"\n%1MenuSeparator{}"_qs.arg(
                        deeps);
        }
        if(element.value(u"type"_qs).toString()==u"Menu"_qs){
            result += u"\n%1Menu{\n   %2title:\"%3\"\n %4%5\n%6}"_qs.arg(
                        deeps, deeps,
                        element.value(u"name"_qs).toString(),
                        deeps,
                        menuFromArray(element.value(u"items"_qs).toArray(), deep+1),
                        deeps);
        }
    }

    return result;
}


QString Datapipe::jsonMenu()
{
    QJsonParseError perr;
    QJsonDocument jdoc = QJsonDocument::fromJson(m_jsonMenu, &perr);
    if (jdoc.isNull()){
        qCritical()<<u"Error loading json menu"_qs;
        qCritical()<<perr.errorString();
        qCritical()<<m_jsonMenu;
        return QString();
    }

    QJsonObject obj = jdoc.object();

    QString menu;
    if (obj.contains(u"items"_qs)){
        menu = menuFromArray(obj.value(u"items"_qs).toArray(),1);
    }
    QString result = u"import QtQuick\nimport QtQuick.Controls\nMenuBar{\n%1\n}"_qs.arg(menu);

    return result;
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





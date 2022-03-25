#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QCommandLineParser>

#include <QSqlDatabase>
#include <QSqlError>

#include <QQmlContext>

#include <QDebug>
#include <QFile>

#include "build_defs.h"
#include "classes/datapipe.h"
#include "classes/qmlsql.h"
#include "classes/qmlsqltable.h"
#include "classes/qmlsqlmodel.h"
#include "classes/qmlcellproperty.h"


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

//    app.setWindowIcon(QIcon(QT_ICON_PATH)); // iconless on taskbar? not anymore.

    app.setOrganizationName("Majister");
    app.setOrganizationDomain("uvelirsoft.com.ua");
    app.setApplicationName("UAccounting");

    QString version = QString::number(BUILD_YEAR)+""
            +QString::number(BUILD_MONTH).rightJustified(2, '0')+""
            +QString::number(BUILD_DAY).rightJustified(2, '0')+"."
            +QString::number(BUILD_HOUR).rightJustified(2, '0')+""
            +QString::number(BUILD_MIN).rightJustified(2, '0');

    app.setApplicationVersion(version);
#ifdef QT_DEBUG
    qSetMessagePattern("[%{type}] %{appname} (%{file}:%{line}) - %{message}");
#else
    qSetMessagePattern("[%{type}] %{message}");
#endif


    QCommandLineParser parser;
    parser.setApplicationDescription("Database for ");
    parser.addHelpOption();
    parser.addVersionOption();

    //parser.addPositionalArgument("file", "File with rules to sync.");

    QCommandLineOption verboseOption(QStringList() << "b" << "verbose", "Verbose mode. Prints a lot of debug information.");
    parser.addOption(verboseOption);
    QCommandLineOption silentOption(QStringList() << "i" << "silent", "Silent mode. Prints only errors.");
    parser.addOption(silentOption);
    QCommandLineOption replaceOption(QStringList() << "r" << "replacesettings", "Replace settings file with defaults");
    parser.addOption(replaceOption);

    // An option with a value
    QCommandLineOption paramsOption(QStringList() << "p" << "params", "List of overrided params","params like param1=value,param2=value");
    parser.addOption(paramsOption);

    // Process the actual command line arguments given by the user
    parser.process(app);

//    const QStringList args = parser.positionalArguments();
//    if(args.length()==0){
//        qCritical()<<"No file to sync ";
//        return -1;
//    }
//    QString filename = args.at(0);

    bool replaceSettings = parser.isSet(verboseOption);


    Datapipe* datapipe = new Datapipe();

    QSettings *settings = datapipe->getSettings();

    QStringList currentKeys = settings->allKeys();


    QSettings defaultSettings(u":/defaults/settings.ini"_qs, QSettings::IniFormat);
    QStringList defaultKeys = defaultSettings.allKeys();

    for (int i = 0; i < defaultKeys.count(); i++)
    {
        if (!currentKeys.contains(defaultKeys.at(i)))
            settings->setValue(defaultKeys.at(i), defaultSettings.value(defaultKeys.at(i)));

        if (defaultKeys.at(i).startsWith("_description") || replaceSettings)
        {
            if(defaultSettings.value(defaultKeys.at(i)) != settings->value(defaultKeys.at(i)))
                settings->setValue(defaultKeys.at(i), defaultSettings.value(defaultKeys.at(i)));
        }
    }
    // copy all keys
    for (int i = 0; i < currentKeys.count(); i++)
        datapipe->setVariable(currentKeys.at(i), settings->value(currentKeys.at(i)));

    // change loglevel from commandline
    if (parser.isSet(verboseOption))
        datapipe->getSettings()->setValue("global/loglevel", 3);
    if (parser.isSet(silentOption))
        datapipe->getSettings()->setValue("global/loglevel", 1);


    QStringList paramsList = parser.value(paramsOption).split(",");
    for(const QString &paramSingle : qAsConst(paramsList))
    {
        QStringList paramSingleList = paramSingle.split("=");
        if (paramSingleList.count() == 2)
            datapipe->getSettings()->setValue(paramSingleList.at(0), paramSingleList.at(1));

    }



    int loglevel      = datapipe->settings("global/loglevel").toInt();

    QString dbdriver = datapipe->settings("database/driver").toString();
    QString username = datapipe->settings("database/username").toString();
    QString password = datapipe->settings("database/password").toString();
    QString hostname = datapipe->settings("database/hostname").toString();
    QString database = datapipe->settings("database/database").toString();
    int dbport   = datapipe->settings("database/port").toInt();


    QSqlDatabase db;
    db = QSqlDatabase::addDatabase(dbdriver);
    db.setUserName(username);
    db.setPassword(password);
    db.setHostName(hostname);
    db.setDatabaseName(database);
    db.setPort(dbport);

    if(!db.isOpen())
        if (!db.open()){
            if(loglevel >= 1){
                qCritical()<<"Error connecting to database";
                qCritical()<<db.lastError().text();
            }
            return -1;
        }



    QQmlApplicationEngine engine;
    const QUrl url(u"qrc:/UAccounting/main.qml"_qs);
//    engine.addImportPath("qrc:////modules"); // modules

    //qmlRegisterSingletonInstance("UA.Settings",1,0, "Datapipe", datapipe);
    engine.rootContext()->setContextProperty("datapipe", datapipe);


    QmlSql qmlsql(datapipe);
    qmlRegisterSingletonInstance("UA.SQL",1,0, "SQL", &qmlsql);

    qmlRegisterType<QmlSqlTable>("UA.SQL",1,0, "SQLTable");
    qmlRegisterType<QmlSqlModel>("UA.SQL",1,0, "SQLModel");


    qmlRegisterType<QmlCellProperty>("UA.Cell",1,0, "Property");


//    qmlRegisterUncreatableMetaObject(MetaTypeNamespace::staticMetaObject, "UA.Types", 0, 1, "MetaType", "Access to enums & flags only");

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}


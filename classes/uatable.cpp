#include "uatable.h"

#include "datapipe.h"

#include "qmlsqltable.h"
#include "qmldefaults.h"



UATable::UATable(QQuickItem *parent) : QQuickItem(parent)
  ,m_sortField(0), m_sortOrder(0), m_filterColumn(-1)
  ,saveSortField(false), saveSortOrder(false), saveFilterColumn(false),saveColumnWidth(false)
  ,m_totalsRow(false), m_appendRow(false), m_serviceColumn(false)
{
    connect(this, &UATable::filterColumnChanged,  [=](){saveFilterColumn = true; applyFilter();});
    connect(this, &UATable::filterStringChanged,  [=](){applyFilter();});
    connect(this, &UATable::sortFieldChanged,     [=](){saveSortField   = true; applySort();});
    connect(this, &UATable::sortOrderChanged,     [=](){saveSortOrder   = true; applySort();});


    connect(this, &UATable::totalsRowChanged,      [=](){m_sortFilterModel.setTotalsRow(m_totalsRow);});
    connect(this, &UATable::appendRowChanged,      [=](){m_sortFilterModel.setAppendRow(m_appendRow);});
    connect(this, &UATable::serviceColumnChanged,  [=](){m_sortFilterModel.setServiceColumn(m_serviceColumn);});

    setTotalsRow(false);
    setAppendRow(true);
    setServiceColumn(true);
}

UATable::~UATable()
{
    saveSettings();
}

void UATable::tableInit()
{
    Datapipe *datapipe = Datapipe::instance();


    m_selectionFields.clear();
    m_editorFields.clear();


    foreach (const auto& field,  m_fields){
        QVariantMap fieldMap = field.toMap();

        QString foreignTable = fieldMap.value("foreign_table").toString();

        m_editorFields.append(fieldMap.value("field").toString());
        if(foreignTable.isEmpty()){
            m_selectionFields.append(fieldMap.value("field").toString());
        }else{
            m_selectionFields.append(u"%1.%2"_qs.arg(
                                         fieldMap.value("foreign_table").toString(),
                                         fieldMap.value("foreign_name").toString()
                                         ));
            m_selectionJoins.append(fieldMap.value("foreign_table").toString());
        }
    }


    m_sortFilterModel.exec(m_tableName,
                           QVariantMap({
                                           {"fields", m_selectionFields},
                                           {"joins", m_selectionJoins}
                                       })
                           );


    QPointer<QmlSqlTable> serviceTable = datapipe->table(TABLE_SETTINGS);

    QPointer<QmlSqlQuery>sqlSettings =
            serviceTable->select(QVariantMap({
                                                 {"fields","value"},
                                                 {"where", QVariantMap({
                                                      {"section", m_tableName},
                                                      {"option","columnsWidth"},
                                                      {"user_id", datapipe->variable("user_id",-1)}}
                                                  )}
                                             }));
    QStringList columnWidths = sqlSettings->value(0).toString().split(",");

    // 0-service-column not needed in m_columnWidths
    for (int i = 0, cnt = m_fields.count(); i < cnt; i++) {
        if (!m_fields.at(i).toMap().value("visible", true).toBool())
            m_columnWidths.append(0);
        else
            if(i < columnWidths.length())
                m_columnWidths.append(qMax(columnWidths.at(i).toInt(), TABLE_MINIMAL_WIDTH));
            else
                m_columnWidths.append(TABLE_MINIMAL_WIDTH);
    }

    QPointer<QmlSqlQuery>sqlFilterColumn =
            serviceTable->select(QVariantMap({
                                                 {"fields","value"},
                                                 {"where", QVariantMap({
                                                      {"section", m_tableName},
                                                      {"option","filterColumn"},
                                                      {"user_id", datapipe->variable("user_id",-1)}}
                                                  )}
                                             }));
    m_filterColumn = sqlFilterColumn->value(0).toInt();

    QPointer<QmlSqlQuery>sqlSortField =
            serviceTable->select(QVariantMap({
                                                 {"fields","value"},
                                                 {"where", QVariantMap({
                                                      {"section", m_tableName},
                                                      {"option","sortField"},
                                                      {"user_id", datapipe->variable("user_id",-1)}}
                                                  )}
                                             }));
    m_sortField = sqlSortField->value(0).toInt();

    QPointer<QmlSqlQuery>sqlSortOrder =
            serviceTable->select(QVariantMap({
                                                 {"fields","value"},
                                                 {"where", QVariantMap({
                                                      {"section", m_tableName},
                                                      {"option","sortOrder"},
                                                      {"user_id", datapipe->variable("user_id",-1)}}
                                                  )}
                                             }));
    m_sortOrder = sqlSortOrder->value(0).toInt();

    applyFilter();
    applySort();
}


QVariant UATable::fieldProperty(int column, const QString &propertyName)
{
    // empty property for 0-service-column
    if(column==0 || column-1 >= m_fields.count())
        return QString("");

    //qDebug()<<m_fields.at(column-1).toMap().value(propertyName);
    return m_fields.at(column-1).toMap().value(propertyName);
}


QmlSqlModel *UATable::tableModel()
{
    //qobject_cast<QAbstractItemModel*>(
    return &m_sortFilterModel;
}

QVariantMap UATable::sqlIndex(int row)
{
    return m_sortFilterModel.sqlIndex(row);
}



void UATable::setColumnWidth(int column, int width)
{
    if(m_serviceColumn && column==0)
        return;

    m_columnWidths[column - (m_serviceColumn?1:0)] = width;
    saveColumnWidth = true;
}

int UATable::getColumnWidth(int column)
{
    if(m_serviceColumn && column==0)
        return TABLE_SERVICE_WIDTH;

    return m_columnWidths.at(column - (m_serviceColumn?1:0));
}


void UATable::applySort()
{
    if(m_sortField < m_sortFilterModel.columnCount())
    {
        if (m_sortOrder==0)
            m_sortFilterModel.setSort(-1, Qt::AscendingOrder);
        if (m_sortOrder==1)
            m_sortFilterModel.setSort(m_sortField, Qt::AscendingOrder);
        if (m_sortOrder==2)
            m_sortFilterModel.setSort(m_sortField, Qt::DescendingOrder);
    }
}

void UATable::applyFilter()
{
    m_sortFilterModel.setFilterString(m_filterString);
    m_sortFilterModel.setFilterColumn(m_filterColumn);
}



void UATable::saveSettings() {
    Datapipe *datapipe = Datapipe::instance();

    QPointer<QmlSqlTable> serviceTable = datapipe->table(TABLE_SETTINGS);

    if(saveColumnWidth){
        QString sss;
        for (int i = 0, cnt = m_fields.length() ; i < cnt; i++) { // 0-column is service
            sss += (i==0?"":",")+QString::number(m_columnWidths.at(i));
        }

        serviceTable->insert_or_update(
                    QVariantMap({{"value",sss}}),
                    QVariantMap ({
                                     {"section",m_tableName},
                                     {"option","columnsWidth"},
                                     {"user_id",datapipe->variable("user_id",-1)}
                                 })
                    );
    }

    if(saveFilterColumn)
        serviceTable->insert_or_update(
                    QVariantMap({{"value",m_filterColumn}}),
                    QVariantMap ({
                                     {"section",m_tableName},
                                     {"option","filterField"},
                                     {"user_id",datapipe->variable("user_id",-1)}
                                 })
                    );

    if(saveSortField)
        serviceTable->insert_or_update(
                    QVariantMap({{"value",m_sortField}}),
                    QVariantMap ({
                                     {"section",m_tableName},
                                     {"option","sortField"},
                                     {"user_id",datapipe->variable("user_id",-1)}
                                 })
                    );

    if(saveSortOrder)
        serviceTable->insert_or_update(
                    QVariantMap({{"value",m_sortOrder}}),
                    QVariantMap ({
                                     {"section",m_tableName},
                                     {"option","sortOrder"},
                                     {"user_id",datapipe->variable("user_id",-1)}
                                 })
                    );

}


void UATable::findCurrentRow()
{
    Datapipe *datapipe = Datapipe::instance();
    if(datapipe->isSame(m_editStatement, sqlIndex(m_currentRow))){
        return;
    }

    for(int i=0,cnt=m_sortFilterModel.rowCount(); i<cnt; i++ ){
        if(datapipe->isSame(m_editStatement, sqlIndex(i))){
            m_currentRow = i;
            emit currentRowChanged();
            return;
        }
    }


    m_currentRow = 0;
    emit currentRowChanged();
}


void UATable::prepareEdit(QQuickItem *editor, int row)
{
    m_beforeEdit.clear();

    Datapipe *datapipe = Datapipe::instance();

    QPointer<QmlSqlTable> editTable = datapipe->table(m_tableName);

    m_editStatement = sqlIndex(row);

    QPointer<QmlSqlQuery> editRecord =
            editTable->select(QVariantMap({
                                              {"fields", m_editorFields},
                                              {"where", m_editStatement}
                                          })
                              );


    for(int i=0, cnt=m_fields.count(); i<cnt; i++){
        m_beforeEdit.insert(m_fields.at(i).toMap().value("field").toString(), editRecord->value(0,-1));
    }

    loadToEditor(editor);
}

void UATable::prepareAdd(QQuickItem *editor)
{
    m_beforeEdit.clear();
    m_editStatement.clear();
    loadToEditor(editor);
}


void UATable::loadToEditor(QQuickItem *parent)
{
    if (parent==nullptr)
        return;

    const QList<QQuickItem *> &children = parent->childItems();
    QQuickItem *obj;
    for (int i = 0; i < children.size(); ++i) {
        obj = children.at(i);
        QVariant fieldProperty = obj->property("field");
        if(fieldProperty.isValid()){
            if (m_beforeEdit.contains(fieldProperty.toString())){
                // TODO work with types
                if (obj->property("digits").toInt()!=0)
                    obj->setProperty("value",
                                     QString::number(
                                         m_beforeEdit.value(
                                             obj->property("field").toString()
                                             ).toDouble(),'f',obj->property("digits").toInt())
                                     );
                else
                    obj->setProperty("value", m_beforeEdit.value(obj->property("field").toString()));
            }else
                obj->setProperty("value", "");
        }

        loadToEditor(obj);
    }
}

void UATable::commitEdit(QQuickItem *editor)
{
    Datapipe *datapipe = Datapipe::instance();

    QPointer<QmlSqlTable> editTable = datapipe->table(m_tableName);

    QVariantMap changes;

    compareToCommit(editor, &changes);

    editTable->update(changes, m_editStatement);

    m_sortFilterModel.reread();

    findCurrentRow();
}

void UATable::commitAdd(QQuickItem *editor)
{
    Datapipe *datapipe = Datapipe::instance();

    QPointer<QmlSqlTable> editTable = datapipe->table(m_tableName);

    QVariantMap changes;

    compareToCommit(editor, &changes);

    QVariantList returns;
    editTable->insert(changes, returns);
    m_sortFilterModel.reread();
}

void UATable::remove(int row)
{
    Datapipe *datapipe = Datapipe::instance();

    QPointer<QmlSqlTable> editTable = datapipe->table(m_tableName);

    editTable->remove(sqlIndex(row));

    m_sortFilterModel.reread();
}


void UATable::compareToCommit(QQuickItem *parent, QVariantMap *changes)
{
    if (parent==nullptr)
        return;

    const QList<QQuickItem *> &children = parent->childItems();
    QQuickItem *obj;
    for (int i = 0; i < children.size(); ++i) {
        obj = children.at(i);
        QVariant propObject = obj->property("field");

        if(propObject.isValid()){
            /*for update - look at old variant*/
            if (m_beforeEdit.contains(obj->property("field").toString())){
                if(obj->property("value") != m_beforeEdit.value(obj->property("field").toString())){
                    changes->insert(obj->property("field").toString(), obj->property("value"));
                }
            }
            else // for insert - add to list anytime
            {
                changes->insert(obj->property("field").toString(), obj->property("value"));
            }
        }

        compareToCommit(obj, changes);
    }
}


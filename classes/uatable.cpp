#include "uatable.h"

#include "datapipe.h"

#include "qmlsqltable.h"


UATable::UATable(QQuickItem *parent) : QQuickItem(parent)
  ,m_sortField(0), m_sortOrder(0), m_filterField(0)
  ,isSortFieldChanged(false), isSortOrderChanged(false), isFilterFieldChanged(false)
  ,isColumnWidthChanged(false)
{

}

UATable::~UATable()
{
    saveSettings();
}

void UATable::tableInit()
{
    Datapipe *datapipe = Datapipe::instance();

    m_selectionFields.clear();
    for(int i=0, cnt=m_fields.count(); i<cnt; i++){
        QVariantMap fieldMap = m_fields.at(i).toMap();
        QString foreignTable = fieldMap.value("foreign_table").toString();

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


    m_sqlModel = datapipe->table(m_tableName)->select(
                QVariantMap({
                                {"fields", m_selectionFields},
                                {"joins", m_selectionJoins}
                            })
                );

    connect(m_sqlModel, &QmlSqlModel::serviceColumnChanged, this, &UATable::serviceColumnChanged);
    connect(m_sqlModel, &QmlSqlModel::appendRowChanged, this, &UATable::appendRowChanged);
    connect(m_sqlModel, &QmlSqlModel::totalsRowChanged, this, &UATable::totalsRowChanged);

    m_sqlModel->setServiceColumn(true);
    m_sqlModel->setAppendRow(true);

    QPointer<QmlSqlTable> serviceTable = datapipe->table(SERVICETABLE_SETTINGS);

    QPointer<QmlSqlModel>sqlSettings =
            serviceTable->select(QVariantMap({
                                                 {"fields","value"},
                                                 {"where", QVariantMap({
                                                      {"section", m_tableName},
                                                      {"option","columnsWidth"},
                                                      {"user_id", datapipe->variable("user_id",-1)}}
                                                  )}
                                             }));
    QStringList columnWidths = sqlSettings->value(0,0).toString().split(",");

    // 0-service-column not needed in m_columnWidths
    for (int i = 0, cnt = m_fields.count(); i < cnt; i++) {
        if (!m_fields.at(i).toMap().value("visible", true).toBool())
            m_columnWidths.append(0);
        else
            if(i < columnWidths.length())
              m_columnWidths.append(qMax(columnWidths.at(i).toInt(), DEFAULTS_TABLE_MINIMAL_WEIGHT));
            else
              m_columnWidths.append(DEFAULTS_TABLE_MINIMAL_WEIGHT);
    }

    QPointer<QmlSqlModel>sqlFilterField =
            serviceTable->select(QVariantMap({
                                                 {"fields","value"},
                                                 {"where", QVariantMap({
                                                      {"section", m_tableName},
                                                      {"option","filterField"},
                                                      {"user_id", datapipe->variable("user_id",-1)}}
                                                  )}
                                             }));
    m_filterField = sqlFilterField->value(0,0).toInt();

    QPointer<QmlSqlModel>sqlSortField =
            serviceTable->select(QVariantMap({
                                                 {"fields","value"},
                                                 {"where", QVariantMap({
                                                      {"section", m_tableName},
                                                      {"option","sortField"},
                                                      {"user_id", datapipe->variable("user_id",-1)}}
                                                  )}
                                             }));
    m_sortField = sqlSortField->value(0,0).toInt();

    QPointer<QmlSqlModel>sqlSortOrder =
            serviceTable->select(QVariantMap({
                                                 {"fields","value"},
                                                 {"where", QVariantMap({
                                                      {"section", m_tableName},
                                                      {"option","sortOrder"},
                                                      {"user_id", datapipe->variable("user_id",-1)}}
                                                  )}
                                             }));
    m_sortOrder = sqlSortOrder->value(0,0).toInt();


    applyFilter();
    applySort();

    //    emit modelChanged();

    //   qDebug()<<"init complete";

}

QString UATable::tableName() const
{
    return m_tableName;
}

void UATable::setTableName(const QString &tableName)
{
    m_tableName = tableName;
    emit tableNameChanged();
}

QVariantList UATable::fields()
{
    return m_fields;
}

void UATable::setFields(const QVariantList &fields)
{
    m_fields = fields;
    emit fieldsChanged();
}

QVariant UATable::fieldProperty(int column, const QString &propertyName)
{
    // empty property for 0-service-column
    if(column==0 || column-1 >= m_fields.count())
        return QString("");

    //qDebug()<<m_fields.at(column-1).toMap().value(propertyName);
    return m_fields.at(column-1).toMap().value(propertyName);
}

QVariantList UATable::linkedTables()
{
    return m_linkedTables;
}

void UATable::setLinkedTables(const QVariantList &linkedTables)
{
    m_linkedTables = linkedTables;
}

QAbstractItemModel *UATable::sqlModel()
{
    return qobject_cast<QAbstractItemModel*>(m_sqlModel);
}

QVariantMap UATable::sqlIndex(int row)
{
    return m_sqlModel->sqlIndex(row);
}

int UATable::currentRow()
{
    return m_currentRow;
}

void UATable::setCurrentRow(int currentRow)
{
    if(m_currentRow == currentRow)
        return;

    m_currentRow = currentRow;
    emit currentRowChanged();
}

//QVariantMap UATable::editStatement() const
//{
//    return m_editStatement;
//}

//void UATable::setEditStatement(QVariantMap editStatement)
//{
//    m_editStatement = editStatement;
//}

int UATable::filterField()
{
    return m_filterField;
}

void UATable::setFilterField(int filterField)
{
    m_filterField = filterField;
    isFilterFieldChanged = true;
    applyFilter();

    emit filterFieldChanged();
}

int UATable::sortField()
{
    return m_sortField;
}

void UATable::setSortField(int sortField)
{
    m_sortField = sortField;
    isSortFieldChanged = true;
    applySort();

    emit sortChanged();
}

int UATable::sortOrder()
{
    return m_sortOrder;
}

void UATable::setSortOrder(int sortOrder)
{
    m_sortOrder = sortOrder;
    isSortOrderChanged = true;
    applySort();

    emit sortChanged();
}

void UATable::setColumnWidth(int column, int width)
{
    m_columnWidths[column-1] = width;
    isColumnWidthChanged =  true;
}

int UATable::getColumnWidth(int column)
{
    if(column==0 || column-1 >= m_columnWidths.count())
        return DEFAULTS_TABLE_SERVICE_WEIGHT;

    return m_columnWidths.at(column-1);
}


void UATable::applySort()
{
    if (m_sortOrder==0)
        m_sqlModel->setSort(-1, Qt::AscendingOrder);
    if (m_sortOrder==1)
        m_sqlModel->setSort(m_sortField, Qt::AscendingOrder);
    if (m_sortOrder==2)
        m_sqlModel->setSort(m_sortField, Qt::DescendingOrder);
}

void UATable::applyFilter()
{
    m_sqlModel->setFilterColumn(m_filterField);
}



void UATable::saveSettings() {
    Datapipe *datapipe = Datapipe::instance();

    QPointer<QmlSqlTable> serviceTable = datapipe->table(SERVICETABLE_SETTINGS);

    if(isColumnWidthChanged){
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

    if(isFilterFieldChanged)
        serviceTable->insert_or_update(
                    QVariantMap({{"value",m_filterField}}),
                    QVariantMap ({
                                     {"section",m_tableName},
                                     {"option","filterField"},
                                     {"user_id",datapipe->variable("user_id",-1)}
                                 })
                    );

    if(isSortFieldChanged)
        serviceTable->insert_or_update(
                    QVariantMap({{"value",m_sortField}}),
                    QVariantMap ({
                                     {"section",m_tableName},
                                     {"option","sortField"},
                                     {"user_id",datapipe->variable("user_id",-1)}
                                 })
                    );

    if(isSortOrderChanged)
        serviceTable->insert_or_update(
                    QVariantMap({{"value",m_sortOrder}}),
                    QVariantMap ({
                                     {"section",m_tableName},
                                     {"option","sortOrder"},
                                     {"user_id",datapipe->variable("user_id",-1)}
                                 })
                    );

}

void UATable::setServiceColumn(bool serviceColumn)
{
    m_sqlModel->setServiceColumn(serviceColumn);
}

bool UATable::serviceColumn()
{
    return m_sqlModel->serviceColumn();
}

void UATable::setTotalsRow(bool totalsRow)
{
    m_sqlModel->setTotalsRow(totalsRow);
}

bool UATable::totalsRow()
{
    return m_sqlModel->totalsRow();
}

void UATable::setAppendRow(bool appendRow)
{
    m_sqlModel->setAppendRow(appendRow);
}

bool UATable::appendRow()
{
    return m_sqlModel->appendRow();
}

void UATable::findCurrentRow()
{
    Datapipe *datapipe = Datapipe::instance();

    if(datapipe->isSame(m_editStatement, sqlIndex(m_currentRow)))
        return;

    for(int i=0,cnt=m_sqlModel->rowCount(); i<cnt; i++ ){
        if(datapipe->isSame(m_editStatement, sqlIndex(i))){
            m_currentRow = i;
            emit currentRowChanged();

        }
    }
}


void UATable::prepareEdit(QQuickItem *editor, int row)
{
    m_beforeEdit.clear();

    Datapipe *datapipe = Datapipe::instance();

    QPointer<QmlSqlTable> editTable = datapipe->table(m_tableName);

    m_editStatement = sqlIndex(row);

    QPointer<QmlSqlModel> editRecord =
            editTable->select(QVariantMap({
                                              {"fields", m_selectionFields},
                                              {"joins", m_selectionJoins},
                                              {"where", m_editStatement}
                                          })
                              );

    for(int i=0, cnt=m_fields.count(); i<cnt; i++){
        m_beforeEdit.insert(m_fields.at(i).toMap().value("field").toString(), editRecord->value(0,i,-1));
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

    m_sqlModel->reread();
    findCurrentRow();
}

void UATable::commitAdd(QQuickItem *editor)
{
    Datapipe *datapipe = Datapipe::instance();

    QPointer<QmlSqlTable> editTable = datapipe->table(m_tableName);

    QVariantMap changes;

    compareToCommit(editor, &changes);

//    qDebug()<<changes;
    QVariantList returns;
    editTable->insert(changes, returns);
    m_sqlModel->reread();
}

void UATable::remove(int row)
{
    Datapipe *datapipe = Datapipe::instance();

    QPointer<QmlSqlTable> editTable = datapipe->table(m_tableName);

    editTable->remove(sqlIndex(row));


//    QPointer<QmlSqlModel> editRecord =
//            editTable->select(QVariantMap({
//                                              {"fields", m_selectionFields},
//                                              {"joins", m_selectionJoins},
//                                              {"where", m_editStatement}
//                                          })
//                              );

//    for(int i=0, cnt=m_fields.count(); i<cnt; i++){
//        m_beforeEdit.insert(m_fields.at(i).toMap().value("field").toString(), editRecord->value(0,i,-1));
//    }

    m_sqlModel->reread();


}


void UATable::compareToCommit(QQuickItem *parent, QVariantMap *changes)
{
    if (parent==nullptr)
        return;

    const QList<QQuickItem *> &children = parent->childItems();
    QQuickItem *obj;
    for (int i = 0; i < children.size(); ++i) {
        obj = children.at(i);
        if (m_beforeEdit.contains(obj->property("field").toString())){
            if(obj->property("value") != m_beforeEdit.value(obj->property("field").toString())){
                changes->insert(obj->property("field").toString(), obj->property("value"));
            }
        }
        else{
            if(!obj->property("field").toString().isEmpty())
                changes->insert(obj->property("field").toString(), obj->property("value"));
            compareToCommit(obj, changes);
        }
    }
}


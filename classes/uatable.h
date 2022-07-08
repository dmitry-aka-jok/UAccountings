#ifndef UATABLE_H
#define UATABLE_H

#include <QQuickItem>

#include "qmlsqlmodel.h"
#include "macro.h"

class UATable : public QQuickItem
{
    Q_OBJECT

    UA_PROP_RW(QString, tableName, setTableName)
    UA_PROP_RW(QVariantList, fields, setFields)
    UA_PROP_RW(QVariantList, linkedTables, setLinkedTables)

    UA_PROP_RW(int, currentRow, setCurrentRow)

    UA_PROP_RW(bool, serviceColumn, setServiceColumn)
    UA_PROP_RW(bool, totalsRow, setTotalsRow)
    UA_PROP_RW(bool, appendRow, setAppendRow)

    UA_PROP_RW(int, filterColumn, setFilterColumn)
    UA_PROP_RW(QString, filterString, setFilterString)
    UA_PROP_RW(int, sortField, setSortField)
    UA_PROP_RW(int, sortOrder, setSortOrder)

    QML_ELEMENT

public:
    UATable(QQuickItem *parent = 0);
    ~UATable();

    Q_INVOKABLE void tableInit();

    Q_INVOKABLE QmlSqlModel *tableModel();


    Q_INVOKABLE QVariantMap sqlIndex(int row);

    Q_INVOKABLE QVariant fieldProperty(int column, const QString &propertyName);
    Q_INVOKABLE void setColumnWidth(int column, int width);
    Q_INVOKABLE int getColumnWidth(int column);


    void applySort();
    void applyFilter();
    void saveSettings();

    void findCurrentRow();

    Q_INVOKABLE void prepareEdit(QQuickItem *editor, int row);
    Q_INVOKABLE void commitEdit(QQuickItem *editor);
    Q_INVOKABLE void prepareAdd(QQuickItem *editor);
    Q_INVOKABLE void commitAdd(QQuickItem *editor);
    Q_INVOKABLE void remove(int row);

private:
    void loadToEditor(QQuickItem *parent);
    void compareToCommit(QQuickItem *parent, QVariantMap *changes);

private:

    QVariantList m_selectionFields;
    QVariantList m_selectionJoins;

    QVariantList m_editorFields;

    QmlSqlModel m_sortFilterModel;

    QVariantMap m_editStatement;
    QVariantMap m_beforeEdit;

    QVector<int> m_columnWidths;
    bool saveColumnWidth;
    bool saveFilterColumn;
    bool saveSortField;
    bool saveSortOrder;
};


#endif // UATABLE_H

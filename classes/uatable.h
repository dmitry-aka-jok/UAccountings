#ifndef UATABLE_H
#define UATABLE_H

#include <QQuickItem>

#include "qmlsqlmodel.h"

class UATable : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString tableName READ tableName WRITE setTableName NOTIFY tableNameChanged)
    Q_PROPERTY(QVariantList fields READ fields WRITE setFields NOTIFY fieldsChanged)
    Q_PROPERTY(QVariantList linkedTables READ linkedTables WRITE setLinkedTables NOTIFY linkedTablesChanged)

    Q_PROPERTY(QAbstractItemModel* sqlModel READ sqlModel NOTIFY sqlModelChanged)

    Q_PROPERTY(int currentRow READ currentRow WRITE setCurrentRow NOTIFY currentRowChanged)

    Q_PROPERTY(int filterField READ filterField WRITE setFilterField NOTIFY filterFieldChanged)
    Q_PROPERTY(int sortField READ sortField WRITE setSortField NOTIFY sortChanged)
    Q_PROPERTY(int sortOrder READ sortOrder WRITE setSortOrder NOTIFY sortChanged)

    Q_PROPERTY(bool serviceColumn READ serviceColumn WRITE setServiceColumn NOTIFY serviceColumnChanged)
    Q_PROPERTY(bool totalsRow READ totalsRow WRITE setTotalsRow NOTIFY totalsRowChanged)
    Q_PROPERTY(bool appendRow READ appendRow WRITE setAppendRow NOTIFY appendRowChanged)

    QML_ELEMENT

public:
    UATable(QQuickItem *parent = 0);
    ~UATable();

    Q_INVOKABLE void tableInit();

    QString tableName() const;
    void setTableName(const QString &tableName);

    QVariantList fields();
    void setFields(const QVariantList &fields);
    Q_INVOKABLE QVariant fieldProperty(int column, const QString &propertyName);

    QVariantList linkedTables();
    void setLinkedTables(const QVariantList &linkedTables);


    QAbstractItemModel *sqlModel();
    Q_INVOKABLE QVariantMap sqlIndex(int row);

    int currentRow();
    void setCurrentRow(int currentRow);


    int filterField();
    void setFilterField(int filterField);
    int sortField();
    void setSortField(int sortField);
    int sortOrder();
    void setSortOrder(int sortOrder);

    Q_INVOKABLE void setColumnWidth(int column, int width);
    Q_INVOKABLE int getColumnWidth(int column);

    void applySort();
    void applyFilter();
    void saveSettings();

    void setServiceColumn(bool serviceColumn);
    bool serviceColumn();
    void setTotalsRow(bool totalsRow);
    bool totalsRow();
    void setAppendRow(bool appendRow);
    bool appendRow();

    void findCurrentRow();

    Q_INVOKABLE void prepareEdit(QQuickItem *editor, int row);
    Q_INVOKABLE void commitEdit(QQuickItem *editor);
    Q_INVOKABLE void prepareAdd(QQuickItem *editor);
    Q_INVOKABLE void commitAdd(QQuickItem *editor);
    Q_INVOKABLE void remove(int row);

private:
    void loadToEditor(QQuickItem *parent);
    void compareToCommit(QQuickItem *parent, QVariantMap *changes);

signals:
    void tableNameChanged();
    void fieldsChanged();
    void linkedTablesChanged();
    void sqlModelChanged();
    void currentRowChanged();
    void filterFieldChanged();
    void sortChanged();
//    void editStatementChanged();
    void serviceColumnChanged();
    void totalsRowChanged();
    void appendRowChanged();


private:
    QString m_tableName;
    QVariantList m_fields;
    QVariantList m_linkedTables;

    int m_currentRow;

    QVariantList m_selectionFields;
    QVariantList m_selectionJoins;

    QPointer<QmlSqlModel> m_sqlModel;

    QVariantMap m_editStatement;
    QVariantMap m_beforeEdit;

    QVector<int> m_columnWidths;
    bool isColumnWidthChanged;
    int m_filterField;
    bool isFilterFieldChanged;
    int m_sortField;
    bool isSortFieldChanged;
    int m_sortOrder;
    bool isSortOrderChanged;


};


#endif // UATABLE_H

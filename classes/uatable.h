#ifndef UATABLE_H
#define UATABLE_H

#include <QQuickItem>

#include "qmlsqlmodel.h"

class UATable : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString tableName READ tableName WRITE setTableName NOTIFY tableNameChanged)
    Q_PROPERTY(QVariantList fields READ fields WRITE setFields NOTIFY fieldsChanged)

    Q_PROPERTY(QAbstractItemModel* sqlModel READ sqlModel NOTIFY sqlModelChanged)

    Q_PROPERTY(QVariantMap editStatement READ editStatement WRITE setEditStatement NOTIFY editStatementChanged)

    Q_PROPERTY(int filterField READ filterField WRITE setFilterField NOTIFY filterFieldChanged)
    Q_PROPERTY(int sortField READ sortField WRITE setSortField NOTIFY sortChanged)
    Q_PROPERTY(int sortOrder READ sortOrder WRITE setSortOrder NOTIFY sortChanged)


    QML_ELEMENT

public:
    UATable(QQuickItem *parent = 0);
    ~UATable();

    Q_INVOKABLE void init();

    QString tableName() const;
    void setTableName(const QString &tableName);

    QVariantList fields();
    void setFields(const QVariantList &fields);

    QAbstractItemModel *sqlModel();
    Q_INVOKABLE QVariantMap sqlIndex(int row);

    QVariantMap editStatement() const;
    void setEditStatement(QVariantMap editStatement);

    int filterField();
    void setFilterField(int filterField);
    int sortField();
    void setSortField(int sortField);
    int sortOrder();
    void setSortOrder(int sortOrder);

    Q_INVOKABLE void setColumnWidth(int column, int width);
    Q_INVOKABLE int getColumnWidth(int column);

    Q_INVOKABLE void applySort();
    Q_INVOKABLE void applyFilter();
    void saveSettings();

    Q_INVOKABLE void prepareEditor(QQuickItem *editor);
    Q_INVOKABLE void commitEditor(QQuickItem *editor);
private:
    void loadToEditor(QQuickItem *parent);
    void compareToCommit(QQuickItem *parent, QVariantMap *changes);

signals:
    void tableNameChanged();
    void fieldsChanged();
    void sqlModelChanged();
    void filterFieldChanged();
    void sortChanged();
    void editStatementChanged();


private:
    QString m_tableName;
    QVariantList m_fields;


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

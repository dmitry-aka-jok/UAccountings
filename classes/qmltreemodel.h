#ifndef QMLTREEMODEL_H
#define QMLTREEMODEL_H

#include <QAbstractItemModel>
#include <QQmlListProperty>

class QmlTreeElement;

class QmlTreeModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    Q_PROPERTY(QQmlListProperty<QmlTreeElement> items READ items NOTIFY itemsChanged)
    Q_PROPERTY(QVariantList roles READ roles WRITE setRoles NOTIFY rolesChanged)
    Q_CLASSINFO("DefaultProperty", "items")

    QmlTreeModel(QObject *parent = Q_NULLPTR);
    ~QmlTreeModel() override;

    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QQmlListProperty<QmlTreeElement> items();

    QVariantList roles() const;
    void setRoles(const QVariantList &roles);

    Q_INVOKABLE QModelIndex indexFromElement(QmlTreeElement *item);
    Q_INVOKABLE bool insertElement(QmlTreeElement *item, const QModelIndex &parent = QModelIndex(), int pos = -1);

    QmlTreeElement *elementFromIndex(const QModelIndex &index) const;

private:
    QmlTreeElement *m_root;
    QHash<int, QByteArray> m_roles;

signals:
    void rolesChanged();
    void itemsChanged();
};

#endif // TreeModel_H

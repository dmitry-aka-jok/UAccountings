#include "qmltreemodel.h"
#include "qmltreeelement.h"

QmlTreeModel::QmlTreeModel(QObject *parent) :
    QAbstractItemModel(parent){
    m_root = new QmlTreeElement;
}
QmlTreeModel::~QmlTreeModel(){
    delete m_root;
}

QHash<int, QByteArray> QmlTreeModel::roleNames() const{
    return m_roles;
}

QVariant QmlTreeModel::data(const QModelIndex &index, int role) const{
    if (!index.isValid())
        return QVariant();
    QmlTreeElement *item = static_cast<QmlTreeElement*>(index.internalPointer());
    QByteArray roleName = m_roles[role];
    QVariant name = item->property(roleName.data());
    return name;
}

Qt::ItemFlags QmlTreeModel::flags(const QModelIndex &index) const{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index);
}

QModelIndex QmlTreeModel::index(int row, int column, const QModelIndex &parent) const{
    if (!hasIndex(row, column, parent))
        return QModelIndex();
    QmlTreeElement *parentItem = elementFromIndex(parent);
    QmlTreeElement *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex QmlTreeModel::parent(const QModelIndex &index) const{
    if (!index.isValid())
        return QModelIndex();
    QmlTreeElement *childItem = static_cast<QmlTreeElement*>(index.internalPointer());
    QmlTreeElement *parentItem = static_cast<QmlTreeElement *>(childItem->parentItem());
    if (parentItem == m_root)
        return QModelIndex();
    return createIndex(parentItem->pos(), 0, parentItem);
}

int QmlTreeModel::rowCount(const QModelIndex &parent) const{
    if (parent.column() > 0)
        return 0;
    QmlTreeElement *parentItem = elementFromIndex(parent);
    return parentItem->count();
}

int QmlTreeModel::columnCount(const QModelIndex &parent) const{
    Q_UNUSED(parent)
    return 1;
}

QQmlListProperty<QmlTreeElement> QmlTreeModel::items(){
    return m_root->items();
}

QVariantList QmlTreeModel::roles() const{
    QVariantList list;
    QHashIterator<int, QByteArray> i(m_roles);
    while (i.hasNext()) {
        i.next();
        list.append(i.value());
    }

    return list;
}

void QmlTreeModel::setRoles(const QVariantList &roles){
    static int nextRole = Qt::UserRole + 1;
    for(const QVariant &role : roles) {
        m_roles.insert(nextRole, role.toByteArray());
        nextRole ++;
    }
    emit rolesChanged();
}

QModelIndex QmlTreeModel::indexFromElement(QmlTreeElement *item){
    QVector<int> positions;
    QModelIndex result;
    if(item) {
        do{
            int pos = item->pos();
            positions.append(pos);
            item = item->parentItem();
        } while(item != nullptr);

        for (int i = positions.size() - 2; i >= 0 ; i--)
            result = index(positions[i], 0, result);
    }
    return result;
}


bool QmlTreeModel::insertElement(QmlTreeElement *item, const QModelIndex &parent, int pos){
    QmlTreeElement *parentElement = elementFromIndex(parent);
    if(pos >= parentElement->count())
        return false;
    if(pos < 0)
        pos = parentElement->count();
    beginInsertRows(parent, pos, pos);
    bool retValue = parentElement->insertItem(item, pos);
    endInsertRows();
    emit itemsChanged();
    return retValue;
}

QmlTreeElement *QmlTreeModel::elementFromIndex(const QModelIndex &index) const{
    if(index.isValid())
        return static_cast<QmlTreeElement *>(index.internalPointer());
    return m_root;
}

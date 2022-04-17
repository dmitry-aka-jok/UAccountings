#include "qmltreeelement.h"

QmlTreeElement::QmlTreeElement(QObject *parent) :
    QObject(parent),
    m_parent(nullptr)
{

}

QmlTreeElement *QmlTreeElement::parentItem() const
{
    return m_parent;
}


QQmlListProperty<QmlTreeElement> QmlTreeElement::items()
{
    return  QQmlListProperty<QmlTreeElement> (this,
                                           this,
                                           &QmlTreeElement::appendElement,
                                           &QmlTreeElement::countElement,
                                           &QmlTreeElement::atElement,
                                           &QmlTreeElement::clearElement);
}

QmlTreeElement *QmlTreeElement::child(int index) const
{
    if(index < 0 || index >= m_childs.length())
        return nullptr;
    return m_childs.at(index);
}

void QmlTreeElement::clear()
{
    qDeleteAll(m_childs);
    m_childs.clear();
    emit itemsChanged();
}

bool QmlTreeElement::insertItem(QmlTreeElement *item, int pos)
{
    if(pos > m_childs.count())
        return false;
    if(pos < 0)
        pos = m_childs.count();
    item->m_parent = this;
    item->setParent(this);
    m_childs.insert(pos, item);
    emit itemsChanged();
    return true;
}

int QmlTreeElement::pos() const
{
    QmlTreeElement *parent = parentItem();
    if(parent)
        return parent->m_childs.indexOf(const_cast<QmlTreeElement *>(this));
    return 0;
}

int QmlTreeElement::count() const
{
    return m_childs.size();
}

void QmlTreeElement::appendElement(QQmlListProperty<QmlTreeElement> *property, QmlTreeElement *value)
{
    QmlTreeElement *parent = qobject_cast<QmlTreeElement *>(property->object);
    parent->insertItem(value);
}

long long QmlTreeElement::countElement(QQmlListProperty<QmlTreeElement> *property)
{
    QmlTreeElement *parent = qobject_cast<QmlTreeElement *>(property->object);
    return parent->count();
}

void QmlTreeElement::clearElement(QQmlListProperty<QmlTreeElement> *property)
{
    QmlTreeElement *parent = qobject_cast<QmlTreeElement *>(property->object);
    parent->clear();
}

QmlTreeElement *QmlTreeElement::atElement(QQmlListProperty<QmlTreeElement> *property, long long index)
{
    QmlTreeElement *parent = qobject_cast<QmlTreeElement *>(property->object);
    if(index < 0 || index >= parent->count())
        return nullptr;
    return parent->child(index);
}

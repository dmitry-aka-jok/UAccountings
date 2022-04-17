#ifndef QMLTREEELEMENT_H
#define QMLTREEELEMENT_H

#include <QObject>
#include <QQmlListProperty>

class QmlTreeElement : public QObject
{
    Q_OBJECT
public:
    Q_PROPERTY(QQmlListProperty<QmlTreeElement> items READ items NOTIFY itemsChanged)
    Q_CLASSINFO("DefaultProperty", "items")
    QmlTreeElement(QObject *parent = nullptr);

    Q_INVOKABLE QmlTreeElement *parentItem() const;
    bool insertItem(QmlTreeElement *item, int pos = -1);
    QQmlListProperty<QmlTreeElement> items();

    QmlTreeElement *child(int index) const;
    void clear();

    Q_INVOKABLE int pos() const;
    Q_INVOKABLE int count() const;
signals:
    void itemsChanged();

private:
    static void appendElement(QQmlListProperty<QmlTreeElement> *property, QmlTreeElement *value);
    static long long countElement(QQmlListProperty<QmlTreeElement> *property);
    static void clearElement(QQmlListProperty<QmlTreeElement> *property);
    static QmlTreeElement *atElement(QQmlListProperty<QmlTreeElement> *property, long long index);

    QList<QmlTreeElement *> m_childs;
    QmlTreeElement *m_parent;
};
#endif // QMLTREEELEMENT_H

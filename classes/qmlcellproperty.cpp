#include "qmlcellproperty.h"

#include <QtGlobal>



QmlCellProperty::QmlCellProperty()
    : QObject(), m_name(u""_qs), m_value(u""_qs)
{
}

QmlCellProperty::QmlCellProperty(const QString &name, const QVariant &value)
    : QObject(), m_name(name), m_value(value)
{
    m_editorType=suitableEditor();
}

QmlCellProperty::QmlCellProperty(const QString &name, const QVariant &value, const QVariantMap &options)
    : QmlCellProperty (name, value)
{
    m_options = options;
}

QmlCellProperty::QmlCellProperty(const QString &name, int value, int min, int max, bool readonly)
    : QmlCellProperty(name, value,  {{"min", min},{"max", max},{"readonly", readonly}})
{
    m_editorType = NumberEditor;
}

QmlCellProperty::QmlCellProperty(const QString &name, double value, double min, double max, bool readonly)
    : QmlCellProperty(name, value,  {{"min", min},{"max", max},{"readonly", readonly}})
{
    m_editorType = NumberEditor;
}

QString QmlCellProperty::name() const  {
    return m_name;
}
QVariant QmlCellProperty::value() const {
    return m_value;
}
QMetaType QmlCellProperty::type() const   {
    return m_value.metaType();
}
int QmlCellProperty::editorType() const {
    return m_editorType;
}
QVariantMap QmlCellProperty::options() const {
    return m_options;
}

void QmlCellProperty::setName(QString name)    {
    if (m_name != name) emit nameChanged(m_name = name);
}
void QmlCellProperty::setValue(QVariant value) {
    if (m_value != value)    emit valueChanged(m_value = value);
}
void QmlCellProperty::setEditorType(int EditorType) {
    if (m_editorType != EditorType)     emit editorTypeChanged(m_editorType = EditorType);
}
void QmlCellProperty::setOptions(QVariantMap option) {
    if (m_options != option)
        emit optionsChanged(m_options = option);
}

int QmlCellProperty::suitableEditor() const {
    if (m_value.typeId() == QMetaType::Bool)
        return BoolEditor;
    if (m_value.typeId() == QMetaType::QColor)
        return ColorEditor;

    static QVector<unsigned int> numerical { QMetaType::Int,QMetaType::UInt,QMetaType::LongLong ,QMetaType::ULongLong, QMetaType::Double,QMetaType::Long,QMetaType::Short,QMetaType::ULong,QMetaType::UShort,QMetaType::Float};
    if (numerical.contains(m_value.typeId()))
        return NumberEditor;

    return TextEditor;
}


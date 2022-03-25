#ifndef QMLCELLPROPERTY_H
#define QMLCELLPROPERTY_H

#include <QObject>
#include <QVariant>

class QmlCellProperty : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString      name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QVariant     value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(QVariantMap  options READ options WRITE setOptions NOTIFY optionsChanged)
    Q_PROPERTY(int          editorType READ editorType WRITE setEditorType NOTIFY editorTypeChanged)
public:
    enum        EditorType {TextEditor,NumberEditor,BoolEditor,ColorEditor,ListEditor};
    Q_ENUM(EditorType)

    explicit    QmlCellProperty();
    explicit    QmlCellProperty(const QString &name, const QVariant &value);
    explicit    QmlCellProperty(const QString &name, const QVariant &value, const QVariantMap& options);
    explicit    QmlCellProperty(const QString& name, int value, int min, int max, bool readonly=false);
    explicit    QmlCellProperty(const QString& name, double value, double min, double max, bool readonly=false);

    QString     name() const;
    QVariant    value() const;
    QMetaType   type() const;
    int         editorType() const;
    QVariantMap options() const;

signals:
    void        nameChanged(QString name);
    void        valueChanged(QVariant value);
    void        editorTypeChanged(int editorType);
    void        optionsChanged(QVariantMap option);

public slots:
    void        setName(QString name);
    void        setValue(QVariant value);
    void        setEditorType(int editorType);
    void        setOptions(QVariantMap option);

protected:
    int         suitableEditor() const;

private:
    QString     m_name;
    QVariant    m_value;
    int         m_editorType;
    QVariantMap m_options;
};

#endif // QMLCELLPROPERTY_H

#ifndef MACROS_H
#define MACROS_H

#include <QObject>


#define UA_PROP_RW(type, name, setter)                                 \
private:                                                               \
    Q_PROPERTY(type name READ name WRITE setter NOTIFY name##Changed)  \
public:                                                                \
    type name() const { return m_##name ; }                            \
Q_SIGNALS:                                                             \
    void name##Changed();                                              \
public Q_SLOTS:                                                        \
    void setter(type name) {                                           \
        if (m_##name == name) return;                                  \
         m_##name = name;                                              \
         emit name##Changed();                                         \
    }                                                                  \
private:                                                               \
    type m_##name;


#define UA_PROP_RO(type, name)                                         \
private:                                                               \
    Q_PROPERTY(type name READ name NOTIFY name##Changed)               \
public:                                                                \
    type name() const { return m_##name ; }                            \
Q_SIGNALS:                                                             \
    void name##Changed();                                              \
private:                                                               \
    type m_##name;



#endif // MACROS_H

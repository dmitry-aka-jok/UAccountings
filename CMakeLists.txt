cmake_minimum_required(VERSION 3.16)

project(UAccounting VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTORCC ON)
#set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.3 COMPONENTS Quick Sql REQUIRED)


qt_add_executable(appUAccounting
    build_defs.h
    classes/datapipe.h
    classes/datapipe.cpp
    classes/qmlsql.h
    classes/qmlsql.cpp
    classes/qmlsqltable.h
    classes/qmlsqltable.cpp
    classes/qmlsqlmodel.h
    classes/qmlsqlmodel.cpp
    classes/qmltreeelement.h
    classes/qmltreeelement.cpp
    classes/qmltreemodel.h
    classes/qmltreemodel.cpp

    classes/uatable.h
    classes/uatable.cpp

    resources.qrc
    main.cpp
)



#qt_add_qml_module(appUAccounting
#    URI UAccounting
#    VERSION 1.0
#    QML_FILES

#    main.qml
#)


set_target_properties(appUAccounting PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_compile_definitions(appUAccounting
    PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(appUAccounting
    PRIVATE Qt6::Quick Qt6::Sql)

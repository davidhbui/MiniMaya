QT += core widgets opengl

TARGET = 277starter3d
TEMPLATE = app

INCLUDEPATH += ./include

SOURCES += main.cpp \
    myglwidget.cpp \
    mymainwindow.cpp \
    camera.cpp \
    face.cpp \
    halfedge.cpp \
    vertex.cpp \
    mesh.cpp

HEADERS += \
    myglwidget.h \
    mymainwindow.h \
    camera.h \
    face.h \
    halfedge.h \
    vertex.h \
    mesh.h

FORMS += \
    mymainwindow.ui

RESOURCES += \
    shaders.qrc

OTHER_FILES += \
    diffuse.frag.glsl \
    diffuse.vert.glsl \
    wire.frag.glsl \
    wire.vert.glsl \
    diff.frag.glsl \
    diff.vert.glsl

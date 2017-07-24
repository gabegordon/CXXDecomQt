TEMPLATE = app

QT += qml quick gui
CONFIG += c++14 static
INCLUDEPATH += ./include

SOURCES += src/main.cpp \
    src/backend.cpp \
    src/ByteManipulation.cpp \
    src/CSVRow.cpp \
    src/DatabaseReader.cpp \
    src/DataDecode.cpp \
    src/Decom.cpp \
    src/h5Decode.cpp \
    src/HeaderDecode.cpp \
    src/InstrumentFormat.cpp \
    src/LogFile.cpp \
    src/ProgressBar.cpp \
    src/ThreadPoolServer.cpp \
    src/ThreadSafeListenerQueue.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    include/backend.h \
    include/ByteManipulation.h \
    include/CSVRow.h \
    include/DatabaseReader.h \
    include/DataDecode.h \
    include/DataTypes.h \
    include/Decom.h \
    include/getFiles.h \
    include/h5Decode.h \
    include/hdf_wrapper.h \
    include/HeaderDecode.h \
    include/InstrumentFormat.h \
    include/LogFile.h \
    include/mingw.condition_variable.h \
    include/mingw.mutex.h \
    include/mingw.thread.h \
    include/ProgressBar.h \
    include/progressqueue.h \
    include/ReadFile.h \
    include/ThreadPoolServer.h \
    include/ThreadSafeListenerQueue.h

win32: LIBS += -L$$PWD/../mxe/usr/i686-w64-mingw32.static/lib/ -lhdf5

INCLUDEPATH += $$PWD/../mxe/usr/i686-w64-mingw32.static/include
DEPENDPATH += $$PWD/../mxe/usr/i686-w64-mingw32.static/include

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../mxe/usr/i686-w64-mingw32.static/lib/hdf5.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/../mxe/usr/i686-w64-mingw32.static/lib/libhdf5.a


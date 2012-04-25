
TEMPLATE = app
TARGET = qt-google-places

QT += gui network webkit

INCLUDEPATH += ../qjson/include


linux* {
    LIBS += -L../qjson/build/lib -lqjson
}

win* {
    LIBS += ../qjson/build/lib/qjson0.dll
}

macx* {
    LIBS += -F../qjson/build/lib -framework qjson
}



SOURCES += \
    main.cpp \
    mainwindow.cpp \
    form.cpp \
    placesdatamanager.cpp \
    settingsdialog.cpp \
    placesjsmanager.cpp \
    datamanagerhelper.cpp \
    variantlistmodel.cpp \
    placedetailsdialog.cpp

HEADERS += \
    mainwindow.h \
    form.h \
    placesdatamanager.h \
    settingsdialog.h \
    placesjsmanager.h \
    datamanagerhelper.h \
    variantlistmodel.h \
    placedetailsdialog.h

FORMS += \
    mainwindow.ui \
    form.ui \
    settingsdialog.ui \
    placedetailsdialog.ui

RESOURCES += \
    resources.qrc

OTHER_FILES += \
    html_js/index.html \
    html_js/init.js \
    html_js/tools.js




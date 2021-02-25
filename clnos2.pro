#-------------------------------------------------
#
# Project created by QtCreator 2017-08-05T11:36:49
#
#-------------------------------------------------

QT       += core gui
QT       += network


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = clnos2
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#INCLUDEPATH += D:\libs\lua
#LIBS += -LD:\libs\lua -llua51

win32:LIBS += -luser32
win32:LIBS += -lpsapi
win32:LIBS += -lkernel32

SOURCES += \
    botcontroller.cpp \
    complimentui.cpp \
    gfnetworklogin.cpp \
        main.cpp \
        mainwindow.cpp \
    logger.cpp \
    nosobjectmanager.cpp \
    objects/skill.cpp \
    settings.cpp \
    newbotdialog.cpp \
    player.cpp \
    playercontroller.cpp \
    playerui.cpp \
    flowlayout.cpp \
    noscrypto.cpp \
    networkmanager.cpp \
    networker.cpp \
    map.cpp \
    minimapscene.cpp \
    minimap.cpp \
    nosstrings.cpp \
    misc.cpp \
    settingsui.cpp \
    packetlogger.cpp \
    bazarmanager.cpp \
    inventory.cpp \
    moneyui.cpp \
    itemlist.cpp \
    bitem.cpp \
    skillz.cpp \
    versionloader.cpp \
    runguard.cpp \
    akt4status.cpp \
    gsettingsui.cpp \
    notificationsystem.cpp \
    stattracker.cpp \
    chatmng.cpp \
    chatui.cpp \
    downloadmanager.cpp \
    memory.cpp \
    gflogin.cpp \
    session.cpp \
    adtui.cpp \
    delayedpacket.cpp \
    testingwindow.cpp \
    battlemanager.cpp

HEADERS += \
    botcontroller.h \
    complimentui.h \
    gfnetworklogin.h \
        mainwindow.h \
    logger.h \
    nosobjectmanager.h \
    objects/skill.h \
    settings.h \
    newbotdialog.h \
    player.h \
    playercontroller.h \
    playerui.h \
    flowlayout.h \
    noscrypto.h \
    networkmanager.h \
    networker.h \
    map.h \
    minimapscene.h \
    minimap.h \
    nosstrings.h \
    misc.h \
    settingsui.h \
    packetlogger.h \
    bazarmanager.h \
    inventory.h \
    moneyui.h \
    itemlist.h \
    bitem.h \
    skillz.h \
    versionloader.h \
    runguard.h \
    akt4status.h \
    gsettingsui.h \
    notificationsystem.h \
    stattracker.h \
    chatmng.h \
    chatui.h \
    downloadmanager.h \
    memory.h \
    gflogin.h \
    session.h \
    adtui.h \
    delayedpacket.h \
    testingwindow.h \
    battlemanager.h

FORMS += \
    botcontroller.ui \
    complimentui.ui \
        mainwindow.ui \
    newbotdialog.ui \
    playerui.ui \
    minimap.ui \
    settingsui.ui \
    packetlogger.ui \
    moneyui.ui \
    versionloader.ui \
    akt4status.ui \
    gsettingsui.ui \
    chatui.ui \
    session.ui \
    adtui.ui \
    testingwindow.ui

RESOURCES += images.qrc

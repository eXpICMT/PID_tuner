QT -= gui
QT += sql

CONFIG += c++17 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        database.cpp \
        main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
	combined_model \
	dups_sql09_22.db \
	dups_sql11_22_hppc_2d_try_delta_gel_12_15.db \
	main_tevenin_model \
	preparedTable.csv \
	readme.md

HEADERS += \
	alphabetafilter.h \
	database.h \
	hppc_to_model.hpp \
	medianfilter.h

INCLUDEPATH += /home/expicmt/DevLibs/eigen-3.4.0/Eigen


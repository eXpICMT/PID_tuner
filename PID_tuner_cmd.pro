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
	db_pid_tuner_sql10_22.db \
	dups_sql09_22.db \
	dups_sql09_22_093022_SoC.db \
	hppc_test_from_112122_rev.3.csv \
	hppc_test_from_112122_v2.csv \
	main \
	main_tevenin_model \
	readme.md \
	t1_for_Up.txt \
	t2_for_Up.txt \
	test.csv \
	test.txt

HEADERS += \
	alphabetafilter.h \
	database.h \
	hppc_to_model.hpp \
	medianfilter.h

INCLUDEPATH += /home/expicmt/DevLibs/eigen-3.4.0/Eigen


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
	db_pid_tuner_sql10_22.db \
	dups_sql09_22.db \
	dups_sql09_22_093022_SoC.db \
	hppc_test_from_112122_rev.3.csv \
	hppc_test_from_112122_v2.csv \
	main \
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

INCLUDEPATH += D:\alexicmt\DevLibs\Eigen\Eigen\src\AccelerateSupport
INCLUDEPATH += D:\alexicmt\DevLibs\Eigen\Eigen\src\Cholesky
INCLUDEPATH += D:\alexicmt\DevLibs\Eigen\Eigen\src\CholmodSupport
INCLUDEPATH += D:\alexicmt\DevLibs\Eigen\Eigen\src\Core
INCLUDEPATH += D:\alexicmt\DevLibs\Eigen\Eigen\src\Eigenvalues
INCLUDEPATH += D:\alexicmt\DevLibs\Eigen\Eigen\src\Geometry
INCLUDEPATH += D:\alexicmt\DevLibs\Eigen\Eigen\src\Householder
INCLUDEPATH += D:\alexicmt\DevLibs\Eigen\Eigen\src\IterativeLinearSolvers
INCLUDEPATH += D:\alexicmt\DevLibs\Eigen\Eigen\src\Jacobi
INCLUDEPATH += D:\alexicmt\DevLibs\Eigen\Eigen\src\KLUSupport
INCLUDEPATH += D:\alexicmt\DevLibs\Eigen\Eigen\src\LU
INCLUDEPATH += D:\alexicmt\DevLibs\Eigen\Eigen\src\MetisSupport
INCLUDEPATH += D:\alexicmt\DevLibs\Eigen\Eigen\src\misc
INCLUDEPATH += D:\alexicmt\DevLibs\Eigen\Eigen\src\OrderingMethods
INCLUDEPATH += D:\alexicmt\DevLibs\Eigen\Eigen\src\PardisoSupport
INCLUDEPATH += D:\alexicmt\DevLibs\Eigen\Eigen\src\PaStiXSupport
INCLUDEPATH += D:\alexicmt\DevLibs\Eigen\Eigen\src\plugins
INCLUDEPATH += D:\alexicmt\DevLibs\Eigen\Eigen\src\QR
INCLUDEPATH += D:\alexicmt\DevLibs\Eigen\Eigen\src\SparseCholesky
INCLUDEPATH += D:\alexicmt\DevLibs\Eigen\Eigen\src\SparseCore
INCLUDEPATH += D:\alexicmt\DevLibs\Eigen\Eigen\src\SparseLU
INCLUDEPATH += D:\alexicmt\DevLibs\Eigen\Eigen\src\SparseQR
INCLUDEPATH += D:\alexicmt\DevLibs\Eigen\Eigen\src\SPQRSupport
INCLUDEPATH += D:\alexicmt\DevLibs\Eigen\Eigen\src\StlSupport
INCLUDEPATH += D:\alexicmt\DevLibs\Eigen\Eigen\src\SuperLUSupport
INCLUDEPATH += D:\alexicmt\DevLibs\Eigen\Eigen\src\SVD
INCLUDEPATH += D:\alexicmt\DevLibs\Eigen\Eigen\src\UmfPackSupport


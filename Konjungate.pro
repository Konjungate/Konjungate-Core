TEMPLATE = app
TARGET = Konjungate-qt
VERSION = 1.1.7.6
INCLUDEPATH += src src/json src/qt src/qt/plugins/mrichtexteditor
QT += core gui widgets network printsupport
DEFINES += ENABLE_WALLET
DEFINES += QT_GUI BOOST_THREAD_USE_LIB BOOST_SPIRIT_THREADSAFE
CONFIG += no_include_pwd
CONFIG += thread
CONFIG += widgets
CONFIG += static
CONFIG += openssl

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
    DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
}

win32{
BOOST_LIB_SUFFIX=-mgw8-mt-s-x32-1_74
BOOST_INCLUDE_PATH=C:/deps/boost_1_74_0
BOOST_LIB_PATH=C:/deps/boost_1_74_0/stage/lib
BDB_INCLUDE_PATH=C:/deps/db-6.2.38.NC/build_unix
BDB_LIB_PATH=C:/deps/db-6.2.38.NC/build_unix
OPENSSL_INCLUDE_PATH=C:/deps/openssl-1.0.2u/include
OPENSSL_LIB_PATH=C:/deps/openssl-1.0.2u
MINIUPNPC_INCLUDE_PATH=C:/deps/
MINIUPNPC_LIB_PATH=C:/deps/miniupnpc-2.1
QRENCODE_INCLUDE_PATH=C:/deps/qrencode-4.1.1
QRENCODE_LIB_PATH=C:/deps/qrencode-4.1.1/.libs
SECP256K1_INCLUDE_PATH=C:/deps/secp256k1/include
SECP256K1_LIB_PATH=C:/deps/secp256k1
}

# for boost 1.37, add -mt to the boost libraries
# use: qmake BOOST_LIB_SUFFIX=-mt
# for boost thread win32 with _win32 sufix
# use: BOOST_THREAD_LIB_SUFFIX=_win32-...
# or when linking against a specific BerkelyDB version: BDB_LIB_SUFFIX=-4.8

# Dependency library locations can be customized with:
#    BOOST_INCLUDE_PATH, BOOST_LIB_PATH, BDB_INCLUDE_PATH,
#    BDB_LIB_PATH, OPENSSL_INCLUDE_PATH and OPENSSL_LIB_PATH respectively

# workaround for boost 1.58
DEFINES += BOOST_VARIANT_USE_RELAXED_GET_BY_DEFAULT

OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build

# Mac: compile for maximum compatibility (10.14, 64-bit)
macx:QMAKE_CXXFLAGS += -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk
macx:QMAKE_CFLAGS += -isysroot  /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk
macx:QMAKE_LFLAGS +=  -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk
macx:QMAKE_OBJECTIVE_CFLAGS += -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk

# use: qmake "RELEASE=1"
contains(RELEASE, 1) {
    !windows:!macx {
        # Linux: static link
        # LIBS += -Wl,-Bstatic
    }
}

!win32 {
# for extra security against potential buffer overflows: enable GCCs Stack Smashing Protection
QMAKE_CXXFLAGS *= -fstack-protector-all --param ssp-buffer-size=1
QMAKE_LFLAGS *= -fstack-protector-all --param ssp-buffer-size=1
# We need to exclude this for Windows cross compile with MinGW 4.2.x, as it will result in a non-working executable!
# This can be enabled for Windows, when we switch to MinGW >= 4.4.x.
}
# for extra security (see: https://wiki.debian.org/Hardening): this flag is GCC compiler-specific
QMAKE_CXXFLAGS += -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=2
# for extra security on Windows: enable ASLR and DEP via GCC linker flags
win32:QMAKE_LFLAGS *= -Wl,--dynamicbase -Wl,--nxcompat
# on Windows: enable GCC large address aware linker flag
win32:QMAKE_LFLAGS *= -Wl,--large-address-aware -static
# i686-w64-mingw32
win32:QMAKE_LFLAGS *= -static-libgcc -static-libstdc++
# PFX Certificate signing
wince*: {
win32:SIGNATURE_FILE = C:/Users/PepperedCrumpets/Documents/CERTIFICATE_KONJ/certificate_konj.pfx
}
# use: qmake "USE_QRCODE=1"
# libqrencode (http://fukuchi.org/works/qrencode/index.en.html) must be installed for support
USE_QRCODE=1
contains(USE_QRCODE, 1) {
    message(Building with QRCode support)
    DEFINES += USE_QRCODE
    LIBS += -lqrencode
}

# use: qmake "USE_UPNP=1" ( enabled by default; default)
#  or: qmake "USE_UPNP=0" (disabled by default)
#  or: qmake "USE_UPNP=-" (not supported)
# miniupnpc (http://miniupnp.free.fr/files/) must be installed for support
contains(USE_UPNP, -) {
message(Building without UPNP support)
} else {
message(Building with UPNP support)
count(USE_UPNP, 0) {
USE_UPNP=1
}
DEFINES += DMINIUPNP_STATICLIB
INCLUDEPATH += $$MINIUPNPC_INCLUDE_PATH
LIBS += $$join(MINIUPNPC_LIB_PATH,,-L,) -lminiupnpc
win32:LIBS += -liphlpapi
}

USE_DBUS=0
# use: qmake "USE_DBUS=1" or qmake "USE_DBUS=0"
linux:count(USE_DBUS, 0) {
    USE_DBUS=1
}
contains(USE_DBUS, 1) {
    message(Building with DBUS (Freedesktop notifications) support)
    DEFINES += USE_DBUS
    QT += dbus
}

contains(BITCOIN_NEED_QT_PLUGINS, 1) {
    DEFINES += BITCOIN_NEED_QT_PLUGINS
    QTPLUGIN += qcncodecs qjpcodecs qtwcodecs qkrcodecs qtaccessiblewidgets
}


#Build Leveldb
INCLUDEPATH += src/leveldb/include src/leveldb/helpers
LIBS += $$PWD/src/leveldb/libleveldb.a $$PWD/src/leveldb/libmemenv.a
SOURCES += src/txdb-leveldb.cpp
!win32 {
    # we use QMAKE_CXXFLAGS_RELEASE even without RELEASE=1 because we use RELEASE to indicate linking preferences not -O preferences
    genleveldb.commands = cd $$PWD/src/leveldb && CC=$$QMAKE_CC CXX=$$QMAKE_CXX $(MAKE) OPT=\"$$QMAKE_CXXFLAGS $$QMAKE_CXXFLAGS_RELEASE\" libleveldb.a libmemenv.a
} else {
    # make an educated guess about what the ranlib command is called
    isEmpty(QMAKE_RANLIB) {
        QMAKE_RANLIB = $$replace(QMAKE_STRIP, strip, ranlib)
    }
    LIBS += -lshlwapi
    # genleveldb.commands = cd $$PWD/src/leveldb && chmod 755 ./build_detect_platform && CC=$$QMAKE_CC CXX=$$QMAKE_CXX TARGET_OS=OS_WINDOWS_CROSSCOMPILE $(MAKE) OPT=\"$$QMAKE_CXXFLAGS $$QMAKE_CXXFLAGS_RELEASE\" libleveldb.a libmemenv.a && $$QMAKE_RANLIB $$PWD/src/leveldb/libleveldb.a && $$QMAKE_RANLIB $$PWD/src/leveldb/libmemenv.a
}
genleveldb.target = $$PWD/src/leveldb/libleveldb.a
genleveldb.depends = FORCE
PRE_TARGETDEPS += $$PWD/src/leveldb/libleveldb.a
QMAKE_EXTRA_TARGETS += genleveldb
# Gross ugly hack that depends on qmake internals, unfortunately there is no other way to do it.
# QMAKE_CLEAN += $$PWD/src/leveldb/libleveldb.a; cd $$PWD/src/leveldb ; $(MAKE) clean


#Build Secp256k1
INCLUDEPATH += src/secp256k1/include
LIBS += $$PWD/src/secp256k1/src/libsecp256k1_la-secp256k1.o
!win32 {
    # we use QMAKE_CXXFLAGS_RELEASE even without RELEASE=1 because we use RELEASE to indicate linking preferences not -O preferences
    gensecp256k1.commands = cd $$PWD/src/secp256k1 && chmod 755 ./autogen.sh && ./autogen.sh && ./configure --enable-module-recovery && CC=$$QMAKE_CC CXX=$$QMAKE_CXX $(MAKE) OPT=\"$$QMAKE_CXXFLAGS $$QMAKE_CXXFLAGS_RELEASE\"
}
gensecp256k1.target = $$PWD/src/secp256k1/src/libsecp256k1_la-secp256k1.o
gensecp256k1.depends = FORCE
PRE_TARGETDEPS += $$PWD/src/secp256k1/src/libsecp256k1_la-secp256k1.o
QMAKE_EXTRA_TARGETS += gensecp256k1

# Gross ugly hack that depends on qmake internals, unfortunately there is no other way to do it.
# QMAKE_CLEAN += $$PWD/src/secp256k1/src/libsecp256k1_la-secp256k1.o; cd $$PWD/src/secp256k1; $(MAKE) clean

# regenerate src/build.h
!windows|contains(USE_BUILD_INFO, 1) {
    genbuild.depends = FORCE
    genbuild.commands = cd $$PWD; /bin/sh share/genbuild.sh $$OUT_PWD/build/build.h
    genbuild.target = $$OUT_PWD/build/build.h
    PRE_TARGETDEPS += $$OUT_PWD/build/build.h
    QMAKE_EXTRA_TARGETS += genbuild
    DEFINES += HAVE_BUILD_INFO
}

contains(USE_O3, 1) {
    message(Building O3 optimization flag)
    QMAKE_CXXFLAGS_RELEASE -= -O2
    QMAKE_CFLAGS_RELEASE -= -O2
    QMAKE_CXXFLAGS += -O3
    QMAKE_CFLAGS += -O3
}

contains(USE_O0, 1) {
    message(Building O0 optimization flag)
    QMAKE_CXXFLAGS_RELEASE -= -O2
    QMAKE_CFLAGS_RELEASE -= -O2
    QMAKE_CXXFLAGS += -O0
    QMAKE_CFLAGS += -O0
}

*-g++-32 {
    message("32 platform, adding -msse2 flag")

    QMAKE_CXXFLAGS += -msse2
    QMAKE_CFLAGS += -msse2
}

QMAKE_CXXFLAGS_WARN_ON = -fdiagnostics-show-option -Wall -Wextra -Wno-ignored-qualifiers -Wformat -Wformat-security -Wno-unused-parameter -Wstack-protector
QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-variable -fpermissive

windows:QMAKE_CXXFLAGS_WARN_ON += -Wno-cpp -Wno-maybe-uninitialized
!macx:QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-local-typedefs
macx:QMAKE_CXXFLAGS_WARN_ON += -Wno-deprecated-declarations

# Input
DEPENDPATH += src src/json src/qt
HEADERS += src/qt/bitcoingui.h \
    src/qt/transactiontablemodel.h \
    src/qt/addresstablemodel.h \
    src/qt/bantablemodel.h \
    src/qt/optionsdialog.h \
    src/qt/coincontroldialog.h \
    src/qt/coincontroltreewidget.h \
    src/qt/sendcoinsdialog.h \
    src/qt/addressbookpage.h \
    src/qt/signverifymessagedialog.h \
    src/qt/aboutdialog.h \
    src/qt/editaddressdialog.h \
    src/qt/importprivatekeydialog.h \
    src/qt/editconfigdialog.h \
    src/qt/bitcoinaddressvalidator.h \
    src/alert.h \
    src/blocksizecalculator.h \
    src/allocators.h \
    src/addrman.h \
    src/base58.h \
    src/bignum.h \
    src/blockparams.h \
    src/chainparams.h \
    src/chainparamsseeds.h \
    src/checkpoints.h \
    src/compat.h \
    src/coincontrol.h \
    src/fork.h \
    src/genesis.h \
    src/mining.h \
    src/sync.h \
    src/util.h \
    src/hash.h \
    src/uint256.h \
    src/kernel.h \
    src/pbkdf2.h \
    src/serialize.h \
    src/support/cleanse.h \
    src/chain.h \
    src/main.h \
    src/miner.h \
    src/net.h \
    src/ecwrapper.h \
    src/key.h \
    src/pubkey.h \
    src/db.h \
    src/txdb.h \
    src/txmempool.h \
    src/walletdb.h \
    src/script.h \
    src/scrypt.h \
    src/init.h \
    src/mruset.h \
    src/json/json_spirit_writer_template.h \
    src/json/json_spirit_writer.h \
    src/json/json_spirit_value.h \
    src/json/json_spirit_utils.h \
    src/json/json_spirit_stream_reader.h \
    src/json/json_spirit_reader_template.h \
    src/json/json_spirit_reader.h \
    src/json/json_spirit_error_position.h \
    src/json/json_spirit.h \
    src/qt/clientmodel.h \
    src/qt/guiutil.h \
    src/qt/transactionrecord.h \
    src/qt/guiconstants.h \
    src/qt/optionsmodel.h \
    src/qt/monitoreddatamapper.h \
    src/qt/peertablemodel.h \
    src/qt/trafficgraphwidget.h \
    src/qt/transactiondesc.h \
    src/qt/transactiondescdialog.h \
    src/qt/bitcoinamountfield.h \
    src/wallet.h \
    src/keystore.h \
    src/qt/transactionfilterproxy.h \
    src/qt/transactionview.h \
    src/qt/walletmodel.h \
    src/qt/walletmodeltransaction.h \
    src/rpcclient.h \
    src/rpcprotocol.h \
    src/rpcserver.h \
    src/rpcvelocity.h \
    src/limitedmap.h \
    src/qt/fractalui.h \
    src/qt/tokenui.h \
    src/qt/nftui.h \
    src/qt/contractui.h \
    src/qt/overviewpage.h \
    src/qt/csvmodelwriter.h \
    src/crypter.h \
    src/qt/sendcoinsentry.h \
    src/qt/qvalidatedlineedit.h \
    src/qt/bitcoinunits.h \
    src/qt/qvaluecombobox.h \
    src/qt/askpassphrasedialog.h \
    src/protocol.h \
    src/qt/notificator.h \
    src/qt/paymentserver.h \
    src/ui_interface.h \
    src/qt/rpcconsole.h \
    src/qt/rpcconsolesettings.h \
    src/version.h \
    src/velocity.h \
    src/netbase.h \
    src/clientversion.h \
    src/threadsafety.h \
    src/tinyformat.h \
    src/stealth.h \
    src/qt/flowlayout.h \
    src/masternode.h \
    src/mnengine.h \
    src/instantx.h \
    src/activemasternode.h \
    src/masternodeconfig.h \
    src/masternodeman.h \
    src/masternode-payments.h \
    src/spork.h \
    src/crypto/common/common.h \
    src/crypto/common/hmac_sha256.h \
    src/crypto/common/hmac_sha512.h \
    src/crypto/common/ripemd160.h \
    src/crypto/common/sha1.h \
    src/crypto/common/sha256.h \
    src/crypto/common/sha512.h \
    src/stb/stb_image.h \
    src/stb/stb_image_write.h \
    src/fractal/fractalengine.h \
    src/fractal/fractalcontract.h \
    src/fractal/fractalparams.h \
    src/fractal/fractaldataob.h \
    src/fractal/fractalnftbase.h \
    src/fractal/fractalnft.h \
    src/fractal/fractalbvac.h \
    src/deminode/demimodule.h \
    src/deminode/deminet.h \
    src/deminode/demisync.h \
    src/qt/masternodemanager.h \
    src/qt/addeditadrenalinenode.h \
    src/qt/adrenalinenodeconfigdialog.h \
    src/qt/qcustomplot.h \
    src/smessage.h \
    src/qt/messagepage.h \
    src/qt/messagemodel.h \
    src/qt/sendmessagesdialog.h \
    src/qt/sendmessagesentry.h \
    src/qt/blockbrowser.h \
    src/qt/settingspage.h \
    src/qt/plugins/mrichtexteditor/mrichtextedit.h \
    src/qt/qvalidatedtextedit.h \
    src/crypto/common/sph_bmw.h \
    src/crypto/common/sph_echo.h \
    src/crypto/common/sph_types.h \
    src/crypto/bmw/bmw512.h \
    src/crypto/echo/echo512.h \
    src/limitedmap.h

SOURCES += src/qt/bitcoin.cpp src/qt/bitcoingui.cpp \
    src/qt/transactiontablemodel.cpp \
    src/qt/addresstablemodel.cpp \
    src/qt/bantablemodel.cpp \
    src/qt/optionsdialog.cpp \
    src/qt/sendcoinsdialog.cpp \
    src/qt/coincontroldialog.cpp \
    src/qt/coincontroltreewidget.cpp \
    src/qt/addressbookpage.cpp \
    src/qt/signverifymessagedialog.cpp \
    src/qt/aboutdialog.cpp \
    src/qt/editaddressdialog.cpp \
    src/qt/importprivatekeydialog.cpp \
    src/qt/editconfigdialog.cpp \
    src/qt/bitcoinaddressvalidator.cpp \
    src/alert.cpp \
    src/blocksizecalculator.cpp \
    src/allocators.cpp \
    src/base58.cpp \
    src/blockparams.cpp \
    src/chainparams.cpp \
    src/version.cpp \
    src/velocity.cpp \
    src/sync.cpp \
    src/txmempool.cpp \
    src/util.cpp \
    src/hash.cpp \
    src/netbase.cpp \
    src/ecwrapper.cpp \
    src/key.cpp \
    src/pubkey.cpp \
    src/script.cpp \
    src/scrypt.cpp \
    src/chain.cpp \
    src/main.cpp \
    src/miner.cpp \
    src/init.cpp \
    src/net.cpp \
    src/checkpoints.cpp \
    src/addrman.cpp \
    src/db.cpp \
    src/walletdb.cpp \
    src/qt/clientmodel.cpp \
    src/qt/guiutil.cpp \
    src/qt/transactionrecord.cpp \
    src/qt/optionsmodel.cpp \
    src/qt/monitoreddatamapper.cpp \
    src/qt/peertablemodel.cpp \
    src/qt/trafficgraphwidget.cpp \
    src/qt/transactiondesc.cpp \
    src/qt/transactiondescdialog.cpp \
    src/qt/bitcoinstrings.cpp \
    src/qt/bitcoinamountfield.cpp \
    src/wallet.cpp \
    src/keystore.cpp \
    src/qt/transactionfilterproxy.cpp \
    src/qt/transactionview.cpp \
    src/qt/walletmodel.cpp \
    src/qt/walletmodeltransaction.cpp \
    src/rpcclient.cpp \
    src/rpcprotocol.cpp \
    src/rpcserver.cpp \
    src/rpcdump.cpp \
    src/rpcmisc.cpp \
    src/rpcnet.cpp \
    src/rpcmining.cpp \
    src/rpcvelocity.cpp \
    src/rpcwallet.cpp \
    src/rpcblockchain.cpp \
    src/rpcrawtransaction.cpp \
    src/qt/overviewpage.cpp \
    src/qt/csvmodelwriter.cpp \
    src/crypter.cpp \
    src/qt/sendcoinsentry.cpp \
    src/qt/qvalidatedlineedit.cpp \
    src/qt/bitcoinunits.cpp \
    src/qt/qvaluecombobox.cpp \
    src/qt/askpassphrasedialog.cpp \
    src/protocol.cpp \
    src/qt/notificator.cpp \
    src/qt/paymentserver.cpp \
    src/qt/rpcconsole.cpp \
    src/qt/rpcconsolesettings.cpp \
    src/noui.cpp \
    src/kernel.cpp \
    src/pbkdf2.cpp \
    src/qt/fractalui.cpp \
    src/qt/tokenui.cpp \
    src/qt/nftui.cpp \
    src/qt/contractui.cpp \
    src/fractal/fractalengine.cpp \
    src/fractal/fractalcontract.cpp \
    src/fractal/fractalparams.cpp \
    src/fractal/fractaldataob.cpp \
    src/fractal/fractalnftbase.cpp \
    src/fractal/fractalnft.cpp \
    src/fractal/fractalbvac.cpp \
    src/deminode/demimodule.cpp \
    src/deminode/deminet.cpp \
    src/deminode/demisync.cpp \
    src/support/cleanse.cpp \
    src/stealth.cpp \
    src/qt/flowlayout.cpp \
    src/masternode.cpp \
    src/mnengine.cpp \
    src/rpcmnengine.cpp \
    src/instantx.cpp \
    src/activemasternode.cpp \
    src/masternodeman.cpp \
    src/masternode-payments.cpp \
    src/spork.cpp \
    src/masternodeconfig.cpp \
    src/crypto/common/hmac_sha256.cpp \
    src/crypto/common/hmac_sha512.cpp \
    src/crypto/common/ripemd160.cpp \
    src/crypto/common/sha1.cpp \
    src/crypto/common/sha256.cpp \
    src/crypto/common/sha512.cpp \
    src/qt/masternodemanager.cpp \
    src/qt/addeditadrenalinenode.cpp \
    src/qt/adrenalinenodeconfigdialog.cpp \
    src/qt/qcustomplot.cpp \
    src/smessage.cpp \
    src/qt/messagepage.cpp \
    src/qt/messagemodel.cpp \
    src/qt/sendmessagesdialog.cpp \
    src/qt/sendmessagesentry.cpp \
    src/qt/blockbrowser.cpp \
    src/qt/settingspage.cpp \
    src/qt/qvalidatedtextedit.cpp \
    src/qt/plugins/mrichtexteditor/mrichtextedit.cpp \
    src/rpcsmessage.cpp \
    src/crypto/common/aes_helper.c \
    src/crypto/common/bmw.c \
    src/crypto/common/echo.c

RESOURCES += \
    src/qt/bitcoin.qrc

FORMS += \
    src/qt/forms/coincontroldialog.ui \
    src/qt/forms/sendcoinsdialog.ui \
    src/qt/forms/addressbookpage.ui \
    src/qt/forms/signverifymessagedialog.ui \
    src/qt/forms/aboutdialog.ui \
    src/qt/forms/editaddressdialog.ui \
    src/qt/forms/importprivatekeydialog.ui \
    src/qt/forms/editconfigdialog.ui \
    src/qt/forms/transactiondescdialog.ui \
    src/qt/forms/overviewpage.ui \
    src/qt/forms/sendcoinsentry.ui \
    src/qt/forms/askpassphrasedialog.ui \
    src/qt/forms/rpcconsole.ui \
    src/qt/forms/rpcconsolesettings.ui \
    src/qt/forms/optionsdialog.ui \
    src/qt/forms/masternodemanager.ui \
    src/qt/forms/addeditadrenalinenode.ui \
    src/qt/forms/adrenalinenodeconfigdialog.ui \
    src/qt/forms/messagepage.ui \
    src/qt/forms/sendmessagesentry.ui \
    src/qt/forms/sendmessagesdialog.ui \
    src/qt/forms/blockbrowser.ui \
    src/qt/forms/settingspage.ui \
    src/qt/forms/fractalui.ui \
    src/qt/forms/tokenui.ui \
    src/qt/forms/nftui.ui \
    src/qt/forms/contractui.ui \
    src/qt/plugins/mrichtexteditor/mrichtextedit.ui

contains(USE_QRCODE, 1) {
HEADERS += src/qt/qrcodedialog.h
SOURCES += src/qt/qrcodedialog.cpp
FORMS += src/qt/forms/qrcodedialog.ui
}

CODECFORTR = UTF-8

# for lrelease/lupdate
# also add new translations to src/qt/bitcoin.qrc under translations/
TRANSLATIONS = $$files(src/qt/locale/bitcoin_*.ts)

isEmpty(QMAKE_LRELEASE) {
    win32:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]\\lrelease.exe
    else:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
}
isEmpty(QM_DIR):QM_DIR = $$PWD/src/qt/locale
# automatically build translations, so they can be included in resource file
TSQM.name = lrelease ${QMAKE_FILE_IN}
TSQM.input = TRANSLATIONS
TSQM.output = $$QM_DIR/${QMAKE_FILE_BASE}.qm
TSQM.commands = $$QMAKE_LRELEASE ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_OUT}
TSQM.CONFIG = no_link
QMAKE_EXTRA_COMPILERS += TSQM

# "Other files" to show in Qt Creator
OTHER_FILES += \
    doc/*.rst doc/*.txt doc/README README.md res/bitcoin-qt.rc

# platform specific defaults, if not overridden on command line
isEmpty(BOOST_LIB_SUFFIX) {
    macx:BOOST_LIB_SUFFIX = -mt-x64
    windows:BOOST_LIB_SUFFIX=-mgw49-mt-s-1_57
}

isEmpty(BOOST_THREAD_LIB_SUFFIX) {
    BOOST_THREAD_LIB_SUFFIX = $$BOOST_LIB_SUFFIX
    #win32:BOOST_THREAD_LIB_SUFFIX = _win32$$BOOST_LIB_SUFFIX
    #else:BOOST_THREAD_LIB_SUFFIX = $$BOOST_LIB_SUFFIX
}

equals(QMAKE_HOST.arch, x86_64) {
    message( "It's x86" )
    LIBOSXPATH=/Users/m1chl/Dev
}

equals(QMAKE_HOST.arch, arm64) {
    message( "It's M1" )
    LIBOSXPATH=/Users/m1chl/Dev
}

isEmpty(BDB_LIB_PATH) {
    macx:BDB_LIB_PATH = $${LIBOSXPATH}/db62/lib
    windows:BDB_LIB_PATH=C:/dev/coindeps32/bdb-4.8/lib
}

isEmpty(BDB_LIB_SUFFIX) {
    macx:BDB_LIB_SUFFIX = -6.2
}

isEmpty(BDB_INCLUDE_PATH) {
    macx:BDB_INCLUDE_PATH = $${LIBOSXPATH}/db62/include
    windows:BDB_INCLUDE_PATH=C:/dev/coindeps32/bdb-4.8/include
}

isEmpty(BOOST_LIB_PATH) {
    macx:BOOST_LIB_PATH = $${LIBOSXPATH}/lib
    windows:BOOST_LIB_PATH=C:/dev/coindeps32/boost_1_57_0/lib
}

isEmpty(BOOST_INCLUDE_PATH) {
    macx:BOOST_INCLUDE_PATH = $${LIBOSXPATH}/include
    windows:BOOST_INCLUDE_PATH=C:/dev/coindeps32/boost_1_57_0/include
}

isEmpty(QRENCODE_LIB_PATH) {
    macx:QRENCODE_LIB_PATH = $${LIBOSXPATH}/qrencode/lib
	windows:QRENCODE_LIB_PATH=C:/deps/qrencode-3.4.4/.libs	
}

isEmpty(QRENCODE_INCLUDE_PATH) {
    macx:QRENCODE_INCLUDE_PATH = $${LIBOSXPATH}/qrencode/include
	windows:QRENCODE_INCLUDE_PATH=C:/deps/qrencode-3.4.4/
}

isEmpty(MINIUPNPC_LIB_SUFFIX) {
    windows:MINIUPNPC_LIB_SUFFIX=-miniupnpc
}

isEmpty(MINIUPNPC_INCLUDE_PATH) {
    macx:MINIUPNPC_INCLUDE_PATH= $${LIBOSXPATH}/include/miniupnpc
    windows:MINIUPNPC_INCLUDE_PATH=C:/dev/coindeps32/miniupnpc-1.9
}

isEmpty(MINIUPNPC_LIB_PATH) {
    macx:MINIUPNPC_LIB_PATH= $${LIBOSXPATH}/lib
    windows:MINIUPNPC_LIB_PATH=C:/dev/coindeps32/miniupnpc-1.9
}

isEmpty(OPENSSL_INCLUDE_PATH) {
    macx:OPENSSL_INCLUDE_PATH = $${LIBOSXPATH}/include/openssl
    windows:OPENSSL_INCLUDE_PATH=C:/dev/coindeps32/openssl-1.0.1p/include
}

isEmpty(OPENSSL_LIB_PATH) {
    macx:OPENSSL_LIB_PATH = $${LIBOSXPATH}/lib
    windows:OPENSSL_LIB_PATH=C:/dev/coindeps32/openssl-1.0.1p/lib
}

windows:DEFINES += WIN32
windows:RC_FILE = src/qt/res/bitcoin-qt.rc

windows:!contains(MINGW_THREAD_BUGFIX, 0) {
    # At least qmake's win32-g++-cross profile is missing the -lmingwthrd
    # thread-safety flag. GCC has -mthreads to enable this, but it doesn't
    # work with static linking. -lmingwthrd must come BEFORE -lmingw, so
    # it is prepended to QMAKE_LIBS_QT_ENTRY.
    # It can be turned off with MINGW_THREAD_BUGFIX=0, just in case it causes
    # any problems on some untested qmake profile now or in the future.
    DEFINES += _MT BOOST_THREAD_PROVIDES_GENERIC_SHARED_MUTEX_ON_WIN
    QMAKE_LIBS_QT_ENTRY = -lmingwthrd $$QMAKE_LIBS_QT_ENTRY
}

macx:HEADERS += src/qt/macdockiconhandler.h src/qt/macnotificationhandler.h
macx:OBJECTIVE_SOURCES += src/qt/macdockiconhandler.mm src/qt/macnotificationhandler.mm
macx:LIBS += -framework Foundation -framework ApplicationServices -framework AppKit -framework CoreServices
macx:DEFINES += MAC_OSX MSG_NOSIGNAL=0
macx:ICON = src/qt/res/icons/bitcoin.icns
macx:TARGET = "Konjungate-Qt"
macx:QMAKE_CFLAGS_THREAD += -pthread
macx:QMAKE_LFLAGS_THREAD += -pthread
macx:QMAKE_CXXFLAGS_THREAD += -pthread
macx:QMAKE_INFO_PLIST = share/qt/Info.plist

# Set libraries and includes at end, to use platform-defined defaults if not overridden
INCLUDEPATH += $$BOOST_INCLUDE_PATH $$BDB_INCLUDE_PATH $$OPENSSL_INCLUDE_PATH $$QRENCODE_INCLUDE_PATH $$BDB_INCLUDE_PATH
LIBS += $$join(BOOST_LIB_PATH,,-L,) $$join(BDB_LIB_PATH,,-L,) $$join(OPENSSL_LIB_PATH,,-L,) $$join(QRENCODE_LIB_PATH,,-L,)
LIBS += /Users/m1chl/Dev/lib/libssl.a /Users/m1chl/Dev/lib/libcrypto.a -lz /Users/m1chl/Dev/lib/libdb_cxx-6.2.a
# -lgdi32 has to happen after -lcrypto (see  #681)
windows:LIBS += -lws2_32 -lshlwapi -lmswsock -lole32 -loleaut32 -luuid -lgdi32 -pthread
!windows: {
    LIBS += -lgmp -levent
}
LIBS += -lboost_system$$BOOST_LIB_SUFFIX -lboost_filesystem$$BOOST_LIB_SUFFIX -lboost_program_options$$BOOST_LIB_SUFFIX -lboost_thread$$BOOST_THREAD_LIB_SUFFIX
LIBS += -lboost_chrono$$BOOST_LIB_SUFFIX

contains(RELEASE, 1) {
    !windows:!macx {
        # Linux: turn dynamic linking back on for c/c++ runtime libraries
        LIBS += -Wl,-Bdynamic
    }
}

!windows:!macx {
    DEFINES += LINUX
    LIBS += -lrt -ldl
}

system($$QMAKE_LRELEASE -silent $$_PRO_FILE_)

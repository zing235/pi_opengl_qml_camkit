QT += qml quick gui opengl

TARGET = openglunderqml
target.path = /home/pi/qt_examples/oo4
INSTALLS += target

DEPENDPATH += . camkit/include
INCLUDEPATH += . camkit/include

INCLUDEPATH += $(SDKSTAGE)/opt/vc/include/ $(SDKSTAGE)/opt/vc/include/interface/vcos/pthreads $(SDKSTAGE)/opt/vc/include/interface/vmcs_host/linux ./
INCLUDEPATH += $(SDKSTAGE)/opt/vc/src/hello_pi/libs/ilclient/ $(SDKSTAGE)/opt/vc/src/hello_pi/libs/vgfont/
QMAKE_CFLAGS +='-DSTANDALONE -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -DTARGET_POSIX -D_LINUX -fPIC -DPIC -D_REENTRANT -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -U_FORTIFY_SOURCE -Wall -g -DHAVE_LIBOPENMAX=2 -DOMX -DOMX_SKIP64BIT -ftree-vectorize -pipe -DUSE_EXTERNAL_OMX -DHAVE_LIBBCM_HOST -DUSE_EXTERNAL_LIBBCM_HOST -DUSE_VCHIQ_ARM -Wno-psabi'
LIBS+= -lilclient -L$(SDKSTAGE)/opt/vc/lib/ -lGLESv2 -lEGL -lopenmaxil -lbcm_host -lvcos -lvchiq_arm -lpthread -lavcodec  -lavformat -lavutil  -lswscale -lrt -lm -L/opt/vc/src/hello_pi/libs/ilclient -L/opt/vc/src/hello_pi/libs/vgfont


HEADERS += squircle.h \
    camerakit.h \
    camkit/include/camkit.h \
    camkit/include/capture.h \
    camkit/include/comdef.h \
    camkit/include/config.h \
    camkit/include/convert.h \
    camkit/include/encode.h \
    camkit/include/ilclient.h \
    camkit/include/ffmpeg_common.h
SOURCES += squircle.cpp main.cpp \
    camerakit.c \
    camkit/ffmpeg_convert.c \
    camkit/ilclient.c \
    camkit/omx_encode.c \
    camkit/v4l_capture.c
RESOURCES += openglunderqml.qrc




qml_folder.source = /scenegraph/openglunderqml
DEPLOYMENTFOLDERS = qml_folder

for(deploymentfolder, DEPLOYMENTFOLDERS) {
    item = item$${deploymentfolder}
    itemsources = $${item}.sources
    $$itemsources = $$eval($${deploymentfolder}.source)
    itempath = $${item}.path
    $$itempath= $$eval($${deploymentfolder}.target)
    export($$itemsources)
    export($$itempath)
    DEPLOYMENT += $$item
}

installPrefix = /home/pi/$${TARGET}

for(deploymentfolder, DEPLOYMENTFOLDERS) {
    item = item$${deploymentfolder}
    itemfiles = $${item}.files
    $$itemfiles = $$eval($${deploymentfolder}.source)
    itempath = $${item}.path
    $$itempath = $${installPrefix}/$$eval($${deploymentfolder}.target)
    export($$itemfiles)
    export($$itempath)
    INSTALLS += $$item
}


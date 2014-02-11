# Common build configuration for all plug-ins
# The default build mode is "release".
# Generate a debug makefile with "qmake MODE=debug".
#
# All one needs to do in a plugin-specific .pro file is this:
# PLUGIN = Texture
# include(../piiplugin.pri)
#
# Use class naming conventions for the plug-in name, without the "Pii"
# prefix.

isEmpty(PLUGIN) {
  error(You must define PLUGIN before including piiplugin.pri)
}

# Build a relative path to Into root. Not necessary, but looks nicer and
# avoids problems with non-regular absolute pathnames.
isEmpty(INTODIR) {
  INTODIR = ../..
  # There's no while loop in qmake. Hope 10 levels is enough.
  levels = 0 1 2 3 4 5 6 7 8 9
  plugindir = $$PLUGIN
  for(level, levels) {
    plugindir = $$dirname(plugindir)
    isEmpty(plugindir) {
      break()
    }
    INTODIR = $$INTODIR/..
  }
}

TARGET = pii$$lower($$basename(PLUGIN))

include(../base.pri)

TEMPLATE        = lib
unix:VERSION    = $$INTO_LIB_VERSION
HEADERS         += *.h
SOURCES         += *.cc
DEFINES         += PII_BUILD_$$upper($$basename(PLUGIN))
DEFINES         += PII_LOG_MODULE=$$basename(PLUGIN)

win32-msvc*: QMAKE_LFLAGS_WINDOWS_DLL += /OPT:NOREF

include(../plugindeps.pri)

defined(PLUGIN_INSTALL_PATH, var) {
  INSTALL_PATH = $$PLUGIN_INSTALL_PATH
} else {
  !defined(INSTALL_PATH, var) {
    INSTALL_PATH = /usr/lib/into/plugins
  } else {
    INSTALL_PATH = $$INSTALL_PATH/plugins
  }
}
include(../libinstall.pri)

LIBS += -lpiiydin$$INTO_LIBV -lpiicore$$INTO_LIBV

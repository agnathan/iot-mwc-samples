AC_PREREQ(2.61)

# package version number (not shared library version)
# odd micro numbers indicate in-progress development
# even micro numbers indicate released versions
m4_define(arduinoLEDBlink_version_major, 0)
m4_define(arduinoLEDBlink_version_minor, 1)
m4_define(arduinoLEDBlink_version_micro, 0)

m4_define([arduinoLEDBlink_version],
          [arduinoLEDBlink_version_major.arduinoLEDBlink_version_minor.arduinoLEDBlink_version_micro])
m4_define([arduinoLEDBlink_api_version],
          [arduinoLEDBlink_version_major.arduinoLEDBlink_version_minor])

AC_INIT(arduinoLEDBlink, arduinoLEDBlink_version)
AM_INIT_AUTOMAKE(arduinoLEDBlink, arduinoLEDBlink_version)
AC_CONFIG_HEADERS(config.h)

# Checks for programs.
AC_PROG_LIBTOOL
AC_PROG_CC_C_O
AC_PROG_CXX
AC_PROG_INSTALL

AC_SUBST(arduinoLEDBlink_CFLAGS)
AC_SUBST(arduinoLEDBlink_LIBS)

AC_OUTPUT([
Makefile
src/Makefile
])

echo ""
echo "   arduinoLEDBlink $VERSION"
echo "   ====================="
echo ""
echo "   To build the project, run \"make\""
echo ""

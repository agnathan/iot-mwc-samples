AC_PREREQ(2.61)

# package version number (not shared library version)
# odd micro numbers indicate in-progress development
# even micro numbers indicate released versions
m4_define(mashery_version_major, 0)
m4_define(mashery_version_minor, 1)
m4_define(mashery_version_micro, 0)

m4_define([mashery_version],
          [mashery_version_major.mashery_version_minor.mashery_version_micro])
m4_define([mashery_api_version],
          [mashery_version_major.mashery_version_minor])

AC_INIT(mashery, mashery_version)
AM_INIT_AUTOMAKE(mashery, mashery_version)
AC_CONFIG_HEADERS(config.h)

# Checks for programs.
AC_PROG_LIBTOOL
AM_PROG_CC_C_O
AC_PROG_INSTALL

AC_SUBST(mashery_CFLAGS)
AC_SUBST(mashery_LIBS)

AC_OUTPUT([
Makefile
src/Makefile
])

echo ""
echo "   mashery $VERSION"
echo "   ====================="
echo ""
echo "   To build the project, run \"make\""
echo ""

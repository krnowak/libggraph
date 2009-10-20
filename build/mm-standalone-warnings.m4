## Copyright (c) 2009  Openismus GmbH  <http://www.openismus.com/>
## Modified by Krzesimir Nowak.
##
## This file is part of mm-common.
##
## mm-common is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published
## by the Free Software Foundation, either version 2 of the License,
## or (at your option) any later version.
##
## mm-common is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with mm-common.  If not, see <http://www.gnu.org/licenses/>.

#serial 20091020

## _MMSA_PRE_INIT
##
## Private pre-initialization macro for use with AC_REQUIRE().  For now,
## all it does is register a forbidden token pattern with autom4te, so
## that unexpanded macro calls in the output can be caught.
##
AC_DEFUN([_MMSA_PRE_INIT], [m4_pattern_forbid([^_?MMSA_])])

## _MMSA_ARG_ENABLE_WARNINGS_OPTION
##
## Implementation helper macro of MMSA_ARG_ENABLE_WARNINGS().  Pulled in
## through AC_REQUIRE() so that it is only expanded once.
##
m4_define([_MMSA_ARG_ENABLE_WARNINGS_OPTION],
[dnl
AC_PROVIDE([$0])[]dnl
AC_ARG_ENABLE([warnings],
              [AS_HELP_STRING([[--enable-warnings[=min|max|fatal|no]]],
                              [set compiler pedantry level [default=min]])],
              [mmsa_enable_warnings=$enableval],
              [mmsa_enable_warnings=min])[]dnl
])

## MMSA_ARG_ENABLE_WARNINGS(variable, min-flags, max-flags, [deprecation-prefixes])
##
## Provide the --enable-warnings configure argument, set to "min" by default.
## <min-flags> and <max-flags> should be space-separated lists of compiler
## warning flags to use with --enable-warnings=min or --enable-warnings=max,
## respectively.  Warning level "fatal" is the same as "max" but in addition
## enables -Werror mode.
##
## If not empty, <deprecation-prefixes> should be a list of module prefixes
## which is expanded to -D<module>_DISABLE_DEPRECATED flags if fatal warnings
## are enabled, too.
##
## For instance, your configure.ac file might use the macro like this:
##   MMSA_ARG_ENABLE_WARNINGS([MYAPP_C_WARNING_FLAGS],
##                            [-Wall],
##                            [-pedantic -Wall -Wextra]
##                            [G PANGO ATK GDK GDK_PIXBUF GTK])
##
## Your Makefile.am could then contain a line such as this:
##   AM_CFLAGS = $(MYAPP_C_WARNING_FLAGS)
##
## Note that you may call MMSA_ARG_ENABLE_WARNINGS twice, once for CFLAGS, and once for CXXFLAGS (for C++).
##
## You may force people to fix warnings when creating release tarballs by 
## adding this line to your Makefile.am:
##   DISTCHECK_CONFIGURE_FLAGS = --enable-warnings=fatal
##
AC_DEFUN([MMSA_ARG_ENABLE_WARNINGS],
[dnl
m4_assert([$# >= 3])[]dnl
AC_REQUIRE([_MMSA_PRE_INIT])[]dnl
AC_REQUIRE([_MMSA_ARG_ENABLE_WARNINGS_OPTION])[]dnl
dnl
AS_CASE([$ac_compile],
        [[*'$CXXFLAGS '*]], [mmsa_lang='C++' mmsa_cc=$CXX mmsa_conftest="conftest.[$]{ac_ext-cc}"],
        [[*'$CFLAGS '*]],   [mmsa_lang=C mmsa_cc=$CC mmsa_conftest="conftest.[$]{ac_ext-c}"],
        [AC_MSG_ERROR([[current language is neither C nor C++]])])
dnl
AC_MSG_CHECKING([which $mmsa_lang compiler warning flags to use])
m4_ifval([$4], [mmsa_deprecation_flags=
])mmsa_tested_flags=
dnl
AS_CASE([$mmsa_enable_warnings],
        [no],    [mmsa_warning_flags=],
        [max],   [mmsa_warning_flags="$3"],
        [fatal], [mmsa_warning_flags="$3 -Werror"[]m4_ifval([$4], [
         for mmsa_prefix in $4
         do
           mmsa_deprecation_flags="$mmsa_deprecation_flags-D[$]{mmsa_prefix}_DISABLE_DEPRECATED "
         done])],
        [mmsa_warning_flags="$2"])
dnl
AS_IF([test "x$mmsa_warning_flags" != x],
[
  # Keep in mind that the dummy source must be devoid of any
  # problems that might cause diagnostics.
  AC_LANG_CONFTEST([AC_LANG_SOURCE([[
int main(int argc, char** argv) { return (argv != 0) ? argc : 0; }
]])])
  for mmsa_flag in $mmsa_warning_flags
  do
    # Test whether the compiler accepts the flag.  Look at standard output,
    # since GCC only shows a warning message if an option is not supported.
    mmsa_cc_out=`$mmsa_cc $mmsa_tested_flags $mmsa_flag -c "$mmsa_conftest" 2>&1 || echo failed`
    rm -f "conftest.[$]{OBJEXT-o}"

    AS_IF([test "x$mmsa_cc_out" = x],
          [AS_IF([test "x$mmsa_tested_flags" = x],
                 [mmsa_tested_flags=$mmsa_flag],
                 [mmsa_tested_flags="$mmsa_tested_flags $mmsa_flag"])],
[cat <<_MMEOF >&AS_MESSAGE_LOG_FD
$mmsa_cc: $mmsa_cc_out
_MMEOF
])
  done
  rm -f "$mmsa_conftest"
])
mmsa_all_flags=m4_ifval([$4], [$mmsa_deprecation_flags])$mmsa_tested_flags
AC_SUBST([$1], [$mmsa_all_flags])
dnl
test "x$mmsa_all_flags" != x || mmsa_all_flags=none
AC_MSG_RESULT([$mmsa_all_flags])[]dnl
])

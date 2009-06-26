#!/bin/sh

#aclocal -I m4 || exit $?
#libtoolize || exit $?
#gtkdocize --flavour no-tmpl || exit $?
#autoconf --warnings=all || exit $?
#autoheader --warnings=all || exit $?
#automake --add-missing --no-force --warnings=all || exit $?
echo "autogen.sh: running: gtkdocize --flavour no-tmpl"
gtkdocize --flavour no-tmpl || exit $?
echo "autogen.sh: running: autoreconf -visW all"
autoreconf -visW all || exit $?
echo "running ./configure $@"
./configure "$@" || exit $?

echo "autogen.sh: run \`make'"

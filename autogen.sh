#!/bin/sh

PROJECT="libggraph"

echo "autogen.sh: running: gtkdocize --flavour no-tmpl"
gtkdocize --flavour no-tmpl || exit $?
echo "autogen.sh: running: autoreconf -visW all"
autoreconf -visW all || exit $?
if test "x$NOCONFIGURE" = "x"
then
	echo "autogen.sh: running: ./configure $@"
	./configure "$@" || exit $?
	echo "autogen.sh: run \`make' to compile $PROJECT"
else
	echo "autogen.sh: not running configure"
fi

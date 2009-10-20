#!/bin/sh

gtkdocizecommand="gtkdocize --flavour no-tmpl"
autoreconfcommand="autoreconf -visfW all"
configurecommand="./configure $@"

echo "autogen.sh: running: $gtkdocizecommand"
$gtkdocizecommand || exit $?
echo "autogen.sh: running: $autoreconfcommand"
$autoreconfcommand || exit $?
if test "x$NOCONFIGURE" = "x"
then
	echo "autogen.sh: running: $configurecommand"
	$configurecommand || exit $?
	echo "autogen.sh: run \`make' to compile project"
else
	echo "autogen.sh: not running configure"
fi

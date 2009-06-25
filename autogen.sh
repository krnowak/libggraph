#!/bin/sh

autoreconf -v -i -s || exit $?

./configure "$@" || exit $?

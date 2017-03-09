#! /bin/sh
$EXTRACTRC `find . -name \*.ui -o -iname \*.kcfg` >> rc.cpp || exit 11
$XGETTEXT `find . -name '*.cpp' | grep -v '/tests/' | grep -v '/autotests/'` -o $podir/libmailtransport5.pot

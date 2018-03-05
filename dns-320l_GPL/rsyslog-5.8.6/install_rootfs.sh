#!/bin/sh
mkdir ${ROOT_FS}/usr/lib/rsyslog

find -name *.so | xargs -i cp -vf {} ${ROOT_FS}/usr/lib/rsyslog
cp -vf ./tools/rsyslogd ${ROOT_FS}/usr/bin

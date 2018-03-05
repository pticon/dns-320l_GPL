#!/bin/bash

#################################################################################
# Hope that it is useful for you. At least, it works for DNS-320L and DNS-340L
# This script works on Debian Wheezy x86. 
# If it doesn't works for your Linux distribution, two choices:
# method 1:
# Please build a native version mysql for your Linux host envrionment and replace
# comp_sql.x86, comp_err.x86 and gen_lex_hash.x86. Re-try this script
# Because mysql's some source codes are generated at compilation time by these
# three commands, comp_sql, comp_err and gen_lex_hash which are also compiled at
# compilation time by Makefile. They have to be x86 version to execute on Host
# environment.
# method 2:
# setup a scratchbox2 environment. It may work but I didn't try.
################################################################################ 
unset CFLAGS
unset CXXFLAGS
unset LDFLAGS
unset LIBS

CFLAGS="-I${PWD}/../_xinstall/${PROJECT_NAME}/include" 
CXXFLAGS="-I${PWD}/../_xinstall/${PROJECT_NAME}/include" 
LDFLAGS="-L${PWD}/../_xinstall/${PROJECT_NAME}/lib"
export CFLAGS
export LDFLAGS
export CXXFLAGS

source ../xcp.sh

xbuild()
{
make distclean

./configure \
--build=i386-linux \
--host=arm-linux \
--prefix=/usr \
--with-charset=utf8 \
--without-debug \
--without-docs \
--without-man \
--with-pthread \
--with-zlib-dir=${PWD}/../_xinstall/${PROJECT_NAME} \
--without-named-curses-libs \
ac_cv_sys_restartable_syscalls=yes \
ac_cv_func_gethostbyname_r=no \
mysql_cv_btype_last_arg_accept=socklen_t \
ac_cv_c_stack_direction=-1 

#--with-named-curses-libs=${PWD}/../_xinstall/${PROJECT_NAME}/lib/libcurl.so \
#--with-named-curses-libs=${PWD}/../_xinstall/${PROJECT_NAME}/lib/libcurl.so \
#--with-ssl=${PWD}/../_xinstall/${PROJECT_NAME} \
#--without-bench \

make
cp -f comp_sql.x86 ./scripts/comp_sql
make
cp -f comp_err.x86 ./extra/comp_err
make
cp -f gen_lex_hash.x86 ./sql/gen_lex_hash
make

}

xinstall()
{
${CROSS_COMPILE}strip -scp ./libmysql/.libs/libmysqlclient.so.16
${CROSS_COMPILE}strip -scp ./libmysql_r/.libs/libmysqlclient_r.so.16
${CROSS_COMPILE}strip ./extra/my_print_defaults
xcp ./libmysql/.libs/libmysqlclient.so.16 ${ROOT_FS}/lib/libmysqlclient.so.16
xcp ./libmysql_r/.libs/libmysqlclient_r.so.16 ${ROOT_FS}/lib/libmysqlclient_r.so.16
xcp ./libmysql/.libs/libmysqlclient.so.16 ${PWD}/../_xinstall/${PROJECT_NAME}/lib/libmysqlclient.so.16
xcp ./libmysql_r/.libs/libmysqlclient_r.so.16 ${PWD}/../_xinstall/${PROJECT_NAME}/lib/libmysqlclient_r.so.16
cp -a ./libmysql/.libs/libmysqlclient.so* ${XLIB_DIR}/

xcp ./scripts/mysql_system_tables_data.sql ${ROOT_FS}/mysql/share/mysql/mysql_system_tables_data.sql
xcp ./scripts/mysql_test_data_timezone.sql ${ROOT_FS}/mysql/share/mysql/mysql_test_data_timezone.sql
xcp ./scripts/mysql_fix_privilege_tables.sql ${ROOT_FS}/mysql/share/mysql/mysql_fix_privilege_tables.sql
xcp ./scripts/mysql_system_tables.sql ${ROOT_FS}/mysql/share/mysql/mysql_system_tables.sql

xcp ./sql/mysqld ${ROOT_FS}/mysql/libexec/mysqld
xcp ./client/.libs/mysql ${ROOT_FS}/bin/mysql
xcp ./scripts/mysql_install_db ${ROOT_FS}/bin/mysql_install_db
xcp ./scripts/mysqld_safe ${ROOT_FS}/bin/mysqld_safe
xcp ./scripts/mysql_secure_installation ${ROOT_FS}/bin/mysql_secure_installation
xcp ./client/.libs/mysqladmin ${ROOT_FS}/bin/mysqladmin
xcp ./extra/my_print_defaults ${ROOT_FS}/bin/my_print_defaults

}

xclean()
{
	make clean
	make distclean
}

if [ "$1" = "build" ]; then
	xbuild
elif [ "$1" = "install" ]; then
	xinstall
elif [ "$1" = "clean" ]; then
	xclean
else
   echo "Usage : xbuild.sh build or xbuild.sh install or xbuild.sh clean"
fi

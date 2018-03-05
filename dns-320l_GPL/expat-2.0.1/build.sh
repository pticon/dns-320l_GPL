
unset CFLAGS
unset LDFLAGS
unset LIBS


./configure --host=${CC%-*} --prefix='$(shell while [ ! -e "configure" ] ; do cd .. ; done ; echo `pwd` )/../tmp_install/'
make buildlib
make installlib
${CC%-*}-strip ../tmp_install/lib/libexpat.so.1.5.2

source ./colortab.sh

echo -e $fcolor_green$color_bright
echo '		##############################################'
echo '		# 1.Run "sh clean.sh" to clean all           #'
echo '		#   dependancy files                         #'
echo '		# 2.Run "sh .installfile.sh" to install      #'
echo '		#    files to /home/tmp/user                 #'
echo '		# 3.Run "sh .installfile.sh nas" to install  #'
echo '		#    files to nas folder                     #'
echo '		##############################################'
echo -ne $color_default

PATH=/bin:/usr/bin
rm -rf t042-dir
mkdir t042-dir
echo prepend-path PATH /042 > t042-dir/042
echo prepend-path PATH /043 > t042-dir/043
ln -s 043 t042-dir/default
PATH= MODULEPATH=. ../cmod sh load t042-dir

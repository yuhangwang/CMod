PATH=/bin:/usr/bin
export PATH
rm -rf t041-dir
mkdir t041-dir
echo module add t041-dir/foo > t041-dir/default
echo prepend-path path foo > t041-dir/foo
MODULEPATH=. ../cmod sh add t041-dir

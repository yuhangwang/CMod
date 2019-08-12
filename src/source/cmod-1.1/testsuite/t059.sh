PATH=/bin:/usr/bin
export PATH
rm -rf t059-dir
mkdir t059-dir
mkdir t059-dir/a
mkdir t059-dir/a/w3-tools
mkdir t059-dir/b
echo "append-path PATH /usr/local" > t059-dir/a/local
echo "append-path PATH /usr/18.57" > t059-dir/a/18.57
echo "append-path PATH /usr/19.25.94" > t059-dir/a/19.25.94
echo "append-path PATH /usr/19.25.95" > t059-dir/a/19.25.95
echo "append-path PATH /usr/19.26" > t059-dir/a/19.26
echo "append-path PATH /usr/19.27" > t059-dir/a/19.27
echo "append-path PATH /usr/19.29" > t059-dir/a/19.29
echo "append-path PATH /usr/19.30" > t059-dir/a/19.30
echo "append-path PATH /usr/19.31" > t059-dir/a/19.31
echo "append-path PATH /usr/19.33" > t059-dir/a/19.33
echo "append-path PATH /usr/19.34" > t059-dir/a/19.34
echo "append-path PATH /usr/20.2" > t059-dir/a/20.2
echo "append-path PATH /usr/latest" > t059-dir/a/latest
echo "append-path PATH /usr/old" > t059-dir/a/old
echo "append-path PATH /usr/test" > t059-dir/a/test
echo "append-path PATH /usr/z" > t059-dir/a/z
echo "append-path PATH /usr/1.0" > t059-dir/a/w3-tools/1.0
echo "append-path PATH /sw/x" > t059-dir/b/x
echo "append-path PATH /sw/y" > t059-dir/b/y
echo "append-path PATH /sw/z" > t059-dir/b/z

MODULEPATH=t059-dir/a:t059-dir/b
export MODULEPATH
LOADEDMODULES=y
export LOADEDMODULES

../cmod sh avail

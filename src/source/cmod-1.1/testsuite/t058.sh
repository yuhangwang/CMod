PATH=/bin:/usr/bin
export PATH
rm -rf t058-dir
mkdir t058-dir
mkdir t058-dir/emacs
mkdir t058-dir/w3-tools
echo "append-path PATH /usr/local" > t058-dir/local
echo "append-path PATH /usr/18.57" > t058-dir/emacs/18.57
echo "append-path PATH /usr/19.25.94" > t058-dir/emacs/19.25.94
echo "append-path PATH /usr/19.25.95" > t058-dir/emacs/19.25.95
echo "append-path PATH /usr/19.26" > t058-dir/emacs/19.26
echo "append-path PATH /usr/19.27" > t058-dir/emacs/19.27
echo "append-path PATH /usr/19.29" > t058-dir/emacs/19.29
echo "append-path PATH /usr/19.30" > t058-dir/emacs/19.30
echo "append-path PATH /usr/19.31" > t058-dir/emacs/19.31
echo "append-path PATH /usr/19.33" > t058-dir/emacs/19.33
echo "append-path PATH /usr/19.34" > t058-dir/emacs/19.34
echo "append-path PATH /usr/20.2" > t058-dir/emacs/20.2
echo "append-path PATH /usr/latest" > t058-dir/emacs/latest
echo "append-path PATH /usr/old" > t058-dir/emacs/old
echo "append-path PATH /usr/test" > t058-dir/emacs/test
echo "append-path PATH /usr/z" > t058-dir/emacs/z
echo "append-path PATH /usr/1.0" > t058-dir/w3-tools/1.0

MODULEPATH=t058-dir
export MODULEPATH
../cmod sh avail

PATH=/bin:/usr/bin
export PATH
rm -rf t057-dir
mkdir t057-dir
mkdir t057-dir/emacs
mkdir t057-dir/w3-tools
echo "append-path PATH /usr/18.57" > t057-dir/emacs/18.57
echo "append-path PATH /usr/19.25.94" > t057-dir/emacs/19.25.94
echo "append-path PATH /usr/19.25.95" > t057-dir/emacs/19.25.95
echo "append-path PATH /usr/19.26" > t057-dir/emacs/19.26
echo "append-path PATH /usr/19.27" > t057-dir/emacs/19.27
echo "append-path PATH /usr/19.29" > t057-dir/emacs/19.29
echo "append-path PATH /usr/19.30" > t057-dir/emacs/19.30
echo "append-path PATH /usr/19.31" > t057-dir/emacs/19.31
echo "append-path PATH /usr/19.33" > t057-dir/emacs/19.33
echo "append-path PATH /usr/19.34" > t057-dir/emacs/19.34
echo "append-path PATH /usr/20.2" > t057-dir/emacs/20.2
echo "append-path PATH /usr/latest" > t057-dir/emacs/latest
echo "append-path PATH /usr/old" > t057-dir/emacs/old
echo "append-path PATH /usr/test" > t057-dir/emacs/test
echo "append-path PATH /usr/z" > t057-dir/emacs/z
echo "append-path PATH /usr/1.0" > t057-dir/w3-tools/1.0

MODULEPATH=t057-dir
export MODULEPATH
../cmod sh avail

PATH=/bin:/usr/bin
export PATH
rm -rf t055-dir
mkdir t055-dir
mkdir t055-dir/empty
mkdir t055-dir/fallback
mkdir t055-dir/system

cd t055-dir/system

sed 's/^X//' << 'SHAR_EOF' > 'X11'
X#%Module
Xprepend-path PATH /usr/bin/X11
SHAR_EOF

sed 's/^X//' << 'SHAR_EOF' > 'X11-append'
X#%Module
Xappend-path PATH /usr/bin/X11
SHAR_EOF

sed 's/^X//' << 'SHAR_EOF' > 'cfengine'
X#%Module
Xprepend-path PATH /sw/cfengine/sbin
SHAR_EOF

sed 's/^X//' << 'SHAR_EOF' > 'cmod'
Xprepend-path INFOPATH /sw/modules/info
SHAR_EOF

sed 's/^X//' << 'SHAR_EOF' > 'default'
X#%Module
Xmodule add ucb
Xmodule add cmod
Xmodule add unix
Xmodule add X11
Xmodule add emacs
Xmodule add local
Xmodule add user
SHAR_EOF

sed 's/^X//' << 'SHAR_EOF' > 'demos'
Xprepend-path PATH /usr/demos/bin
SHAR_EOF

sed 's/^X//' << 'SHAR_EOF' > 'emacs'
Xprepend-path INFOPATH /sw/common/share/emacs/19.34/info:/sw/common/share/emacs/site-info
SHAR_EOF

sed 's/^X//' << 'SHAR_EOF' > 'local'
X#%Module
Xprepend-path PATH /sw/local/bin
Xprepend-path MANPATH /sw/local/man
Xprepend-path INFOPATH /sw/local/info:/sw/common/info
SHAR_EOF

sed 's/^X//' << 'SHAR_EOF' > 'local-append'
X#%Module
Xappend-path PATH /sw/local/bin
Xappend-path MANPATH /sw/local/man
Xappend-path INFOPATH /sw/local/info:/sw/common/info
SHAR_EOF

> 'sysadm'

sed 's/^X//' << 'SHAR_EOF' > 'tex'
Xprepend-path MANPATH /sw/tex/teTeX/man
Xprepend-path INFOPATH /sw/tex/teTeX/info
SHAR_EOF

sed 's/^X//' << 'SHAR_EOF' > 'ucb'
Xprepend-path PATH /usr/bsd
SHAR_EOF

sed 's/^X//' << 'SHAR_EOF' > 'ucb-append'
Xappend-path PATH /usr/bsd
SHAR_EOF

sed 's/^X//' << 'SHAR_EOF' > 'unix'
X#%Module
Xprepend-path PATH /usr/bin:/usr/sbin:/usr/etc
Xprepend-path MANPATH /usr/share/catman:/usr/share/man:/usr/catman:/usr/man
SHAR_EOF

sed 's/^X//' << 'SHAR_EOF' > 'unix-append'
X#%Module
Xappend-path PATH /usr/bin:/usr/sbin:/usr/etc
Xappend-path MANPATH /usr/share/catman:/usr/share/man:/usr/catman:/usr/man
SHAR_EOF

sed 's/^X//' << 'SHAR_EOF' > 'user'
X#%Module
Xprepend-path PATH $env(HOME)/bin/IP12
Xprepend-path PATH $env(HOME)/bin/sh
Xprepend-path MANPATH $env(HOME)/man
SHAR_EOF

sed 's/^X//' << 'SHAR_EOF' > 'wheel'
X#%Module
Xmodule add cfengine
Xsetenv CFINPUTS /sw/wheel/cfengine
SHAR_EOF

cd ../fallback

sed 's/^X//' << 'SHAR_EOF' > 'beta'
X#%Module
Xsetenv       pfxbeta  ${HOME}/pfxbeta/${CEDER_ARCH}
Xprepend-path PATH     ${HOME}/pfxbeta/${CEDER_ARCH}/bin
Xprepend-path INFOPATH ${HOME}/pfxbeta/${CEDER_ARCH}/info
SHAR_EOF

sed 's/^X//' << 'SHAR_EOF' > 'ceder'
X#%Module
Xmodule add default
Xmodule add user
Xmodule add beta
SHAR_EOF

sed 's/^X//' << 'SHAR_EOF' > 'user'
X#%Module
Xprepend-path PATH ${HOME}/common/scripts
Xsetenv pfx ${HOME}/pfx/${CEDER_ARCH}
Xprepend-path PATH ${pfx}/bin
Xprepend-path INFOPATH ${pfx}/info
SHAR_EOF

cd ../..

MODULEPATH=t055-dir/empty:t055-dir/fallback:t055-dir/system
export MODULEPATH
# /home/ceder/.modules/mips--sgi-irix5.4:/home/ceder/.modules/fallback:/sw/modules/etc/cmod/modulefiles

CEDER_ARCH=mips--sgi-irix5.4
export CEDER_ARCH

HOME=/home/ceder
export HOME

PATH=:/usr/sbin:/usr/bsd:/sbin:/usr/bin:/bin:/usr/bin/X11
export PATH

INFOPATH=
export INFOPATH

MANPATH=
export MANPATH

../cmod sh add default

cmod_echo=:
if [ -n "$BASH" ]; then
  $cmod_echo cmod: detected bash
  . /home/steven/Install/cmod/1.1/etc/cmod/bash.init
elif [ -n "$ZSH_VERSION" ]; then
  $cmod_echo cmod: detected zsh
  . /home/steven/Install/cmod/1.1/etc/cmod/zsh.init
elif [ -n "$FCEDIT" ]; then
  case x"$VERSION" in
    xzsh*)
      $cmod_echo cmod: detected old zsh
      . /home/steven/Install/cmod/1.1/etc/cmod/zsh.init;;
    *)
      $cmod_echo cmod: detected ksh
      . /home/steven/Install/cmod/1.1/etc/cmod/ksh.init;;
  esac
else
  $cmod_echo cmod: detected sh
  . /home/steven/Install/cmod/1.1/etc/cmod/sh.init
fi

if [ -z "$LOADEDMODULES" ]; then
  $cmod_echo cmod: setting up environment...
  PATH=
  MANPATH=
  INFOPATH=
  export PATH MANPATH INFOPATH
  module add default
else
  $cmod_echo cmod: retaining environment...
fi

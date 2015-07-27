set cmod_echo=:
if ($?tcsh == 1) then
  $cmod_echo cmod: detected tcsh
  source /home/steven/Install/cmod/1.1/etc/cmod/tcsh.init
else
  $cmod_echo cmod: detected csh
  source /home/steven/Install/cmod/1.1/etc/cmod/csh.init
endif

if ( $?LOADEDMODULES == 0 ) then
  $cmod_echo cmod: setting up environment...
  setenv PATH ""
  setenv MANPATH ""
  setenv INFOPATH ""
  module add default
else
  $cmod_echo cmod: retaining environment...
endif

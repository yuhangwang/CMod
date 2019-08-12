# CMod
Module written in C

This is a verbatim redistribution (fair use under GPL v2.0) of cmod written by the following people:
- Per Cederqvist
- Kent Engström
- Henrik Rindlöw
- David Kågedal

The source file can be found at http://www.lysator.liu.se/cmod/

# Installation
- `git clone https://github.com/yuhangwang/CMod.git`
- `cd CMod/src/source`
- `tar xvf cmod-1.1.tar.gz`
- `cd cmod-1.1`
- `mkdir -p ~/install/cmod/1.1`
- `./configure --prefix=$HOME/install/cmod/1.1`
- `make`
- `make check`
- `make install`

# Post installation
- First, you need to make a folder containing the CMod scripts.
   `mkdir -p ~/modules/python`
- Then create a new file `~/modules/python/default`   
  with the following content.
  ```
  prepend-path PATH /Users/steven/install/miniconda/3/bin
  prepend-path DYLD_LIBRARY_PATH /Users/steven/install/miniconda/3/lib
  ```
 - Add the following lines in your `~/.bashrc` file
   ```
   source $HOME/install/cmod/1.1/etc/cmod/bash.init
   module use $HOME/modules
   ```
 - Refresh your BASH shell using `source ~/.bashrc`
 - Now you can see the available modules with `module avail`
 - To load a module into your current bash shell, use `module add python`
 - To remove the module, use `module rm python`
 - To see the currently loaded modules, use `module list`
 - You can add more module files to `~/modules` similar to the python example.

# Documentation
A complete documentation can be found in the `info` folder of your cmod installation path.  
You can also browse the documentation contained in the [usage/cmod.md](https://github.com/yuhangwang/CMod/blob/master/usage/cmod.md)
file of this Github repository.
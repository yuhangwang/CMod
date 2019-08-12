This is Info file cmod.info, produced by Makeinfo version 1.68 from the
input file cmod.texi.

   This file documents cmod, which provides modularized initialization
of environment variables such as `$PATH'.

   Copyright (C) 1998 Per Cederqvist.

   Permission is granted to make and distribute verbatim copies of this
manual provided the copyright notice and this permission notice are
preserved on all copies.

   Permission is granted to copy and distribute modified versions of
this manual under the conditions for verbatim copying, provided also
that the sections entitled "Copying" and "GNU General Public License"
are included exactly as in the original, and provided that the entire
resulting derived work is distributed under the terms of a permission
notice identical to this one.

   Permission is granted to copy and distribute translations of this
manual into another language, under the above conditions for modified
versions, except that this permission notice may be stated in a
translation approved by the Free Software Foundation.

INFO-DIR-SECTION Shells and utilities
START-INFO-DIR-ENTRY
* Cmod: (cmod).                         Setting up your environment.
* module: (cmod)Invoking module.        Modifying your environment.
END-INFO-DIR-ENTRY


File: cmod.info,  Node: Top,  Next: Introduction,  Up: (dir)

Cmod
****

   This manual documents cmod version 1.1.

* Menu:

* Introduction::                What is cmod good for?
* Initializing cmod::           How to get the `module' alias.
* Invoking module::             How to invoke the `module' alias.
* Module files::                Writing module files.
* Environment variables::       Environment variables used by cmod.
* Bugs::                        Reporting bugs in cmod.
* New releases::                How to download cmod.
* Function index::              Function, file and variable index.
* Concept index::               Concept index.


File: cmod.info,  Node: Introduction,  Next: Initializing cmod,  Prev: Top,  Up: Top

Introduction to cmod
********************

   So, you think that you know how to set your path?  You think that it
is as easy as putting something like this in your `.bashrc' file?

     export PATH=/usr/local/bin:/usr/bin:/bin

   This manual documents an alternative way to set your `PATH' (and
other environment variables).  It may not be as simple as the above
statement, but it is in many ways more flexible.  Cmod allows system
administrators to provide a good default setup for users of a system,
while at the same time allowing the users to make alterations to the
environment without having to take on the entire burden of setting up
the environment.

   Cmod introduces the concept of "module files".  A module file can
alter the environment.  For instance, the module file `X11' may add
`/usr/X11/bin' to `PATH' and `/usr/X11/man' to `MANPATH'.  A module
file can also do certain other things, such as load other module
files-`default' may for instance be set up to load the `X11', `unix'
and `user' modules, which in a typical setup would add `/usr/X11/bin',
`/usr/bin:/bin' and `$HOME/ARCH/bin' to `PATH'.

   Cmod searches for module files in a list of directories, which can be
modified by the user.  A user can for instance prepend his own directory
of module files to the predefined list, and override module definitions
which he doesn't like.

   Cmod owes a lot, such as the "module" concept and some (but not all)
of the syntax of the `module' alias and module files, to `modules' (see
`http://www.modules.org/').  Unlike `modules', cmod is implemented in a
portable subset of C, and it is self contained, which makes it very
easy to install.  To unpack cmod, compile it, and install the binaries
should very seldom take more than five minutes, and often considerably
less.  (Writing good module files that suit your environment is another
matter.)


File: cmod.info,  Node: Initializing cmod,  Next: Invoking module,  Prev: Introduction,  Up: Top

Initializing cmod
*****************

   Before you can use `module', you have to install it in your
environment.  How you do that depends on which shell you are using, and
whether you want to have full control over your environment or trust
your system administrator to provide a good default environment.

* Menu:

* Using system defaults::       Using `path.sh' or `path.csh'.
* Defining module::             Using `sh.init', `csh.init'...


File: cmod.info,  Node: Using system defaults,  Next: Defining module,  Up: Initializing cmod

Using system defaults
=====================

   Cmod provides two files named `path.sh' and `path.csh'.  They are
typically located in `/usr/local/etc/cmod/', but the directory can be
specified at compile-time by giving the options `--prefix' or
`--sysconfdir' to `configure'.

   These files will detect which shell you are actually using, and load
the appropriate initialization file (*note Defining module::.).  They
will also load the module `default', unless some module already has been
loaded into the environment.

   If you use `sh', `bash', `zsh' or `ksh' you should put this in one
of your initialization files to enable cmod:

     . /usr/local/etc/cmod/path.sh

   If you use `csh' or `tcsh', you should put this in `.cshrc' or
`.tcshrc':

     source /usr/local/etc/cmod/path.csh

   After that point, `PATH' will be set to the default value provided
by your system administrator.  You can than add more modules using the
`module' alias.  The following example loads the `sysadm' module.  The
same syntax is used regardless of which shell you are using.

     module add sysadm


File: cmod.info,  Node: Defining module,  Prev: Using system defaults,  Up: Initializing cmod

Defining module
===============

   If you want complete control over your environment, you can source
one of the following files instead of `path.sh' or `path.csh':
     sh.init
     csh.init
     bash.init
     tcsh.init
     zsh.init
     ksh.init

   They are installed in the same directory as `path.sh' and `path.csh'
(*note Using system defaults::.).  They all define the `module' alias
in a way appropriate for the shell.  They will also set `MODULEPATH',
unless it already was set.  They will not load any module, so your init
file will have to do that by itself.


File: cmod.info,  Node: Invoking module,  Next: Module files,  Prev: Initializing cmod,  Up: Top

Invoking module
***************

   Once cmod is initialized you can use it interactively or from shell
scripts.  This chapter lists the commands that are available.

* Menu:

* Adding modules::              Adding modules to your environment.
* Search path::                 Altering the cmod search path.
* Displaying information::      Displaying loaded and available modules.


File: cmod.info,  Node: Adding modules,  Next: Search path,  Up: Invoking module

Adding modules
==============

     module add `module'...
     module try-add `module'...
     module rm `module'...
     module load `module'...
     module unload `module'...

   You can use `module add' to add one or more modules to your
environment.  The command will fail if the module cannot be found.  You
can add several modules with the same command.  `module load' is an
alias for `module add'.

   If you want to load a module if it is available, use `module
try-add'.  It will silently ignore any attempt to load a module that
doesn't exist.  It can be handy to add `module try-add spice' to your
`.bashrc' file to add the `spice' module whenever it is present, but
avoid complaints on systems where it doesn't exist.

   Use `module rm' to remove a module from your environment.  This will
parse the module file and do the opposite of what is said there.  For
example, if the module file adds `/usr/local/bin' to your `PATH' this
command will remove `/usr/local/bin' from `PATH'.  An attempt to remove
a module that isn't present in your environment will result in a
warning message.  Depending on how the module file is written it may
sometimes be impossible to unload it.  This command does its best, but
that is sometimes not enough.

   `module unload' is an alias for `module rm'.


File: cmod.info,  Node: Search path,  Next: Displaying information,  Prev: Adding modules,  Up: Invoking module

Altering where modules are found
================================

     module use `dir'
     module unuse `dir'

   `module use' will prepend `dir' to the list of directories that are
searched for module files.  When writing a module file this is
equivalent to `prepend-path MODULEPATH `dir''.

   `module unuse' will remove `dir' from the list.  When writing a
module file the same effect can be achieved with `delete-path
MODULEPATH `dir''.


File: cmod.info,  Node: Displaying information,  Prev: Search path,  Up: Invoking module

Displaying information to the user
==================================

     module list
     module avail
     module --version
     module --help

   These commands display information to the user.  The format of the
output is designed so that it should be easy to read for a human.
Programs should not attempt to parse the output, since it may change
even in bug-fix releases of cmod.

   `module list' lists the modules that are currently loaded.  The
environment variable `LOADEDMODULES' also contains this information.

   `module avail' traverses all the directories in the current module
load path (which is held in `MODULEPATH') and prints all the modules
that are available.  Modules that are loaded are preceded by a plus
sign, others by a minus sign.  This makes it easy to see at a glance if
there are some exiting new modules that you may wish to add to your
environment.

   `module --version' outputs the version number of cmod.  `module
--help' prints a few lines of help for the commands that can be used
interactively.


File: cmod.info,  Node: Module files,  Next: Environment variables,  Prev: Invoking module,  Up: Top

Writing module files
********************

   A module is defined in a "module file".  The module files are
typically installed in `/usr/local/etc/cmod/modulefiles', but the
directory can be specified at compile-time by giving the options
`--prefix' or `--sysconfdir' to `configure', or by specifying
`moduledir=LOCATION' to `make' and `make install'.

* Menu:

* Module syntax::               Syntax of module files.
* Directories::                 Using directories as module files.
* Altering the environment::    Commands that alter the environment.


File: cmod.info,  Node: Module syntax,  Next: Directories,  Up: Module files

Syntax of module files.
=======================

   The parsing is performed by three major components:

   * The "preprocessor" performs variable lookup and shell escapes.

   * The "tokenizer" receives a stream of characters from the
     preprocessor.  It splits them into tokens, gather tokens until the
     end of line, and feeds each line to the evaluator.  The tokenizer
     is also responsible for comment removal.

   * The "evaluator" receives one command at a time from the tokenizer.
     It evaluates the command.

   It is important to note that both the preprocessor and the tokenizer
are lazy; they always do as little as possible.  Whenever possible, a
line is evaluated by the evaluator before the next line is even parsed.
That way, variable assignments made on one line can affect the
preprocessor directives on the next line.

* Menu:

* Preprocessor::                The preprocessor.
* Tokenizer::                   The tokenizer.
* Evaluator::                   The evaluator.


File: cmod.info,  Node: Preprocessor,  Next: Tokenizer,  Up: Module syntax

The preprocessor
----------------

                                                      Well I'm not sure
                                                   what I'm looking for
                                                            --- Garbage

   The preprocessor looks for `$' signs.  All other characters are
passed unmodified to the tokenizer.

`$$'
     Pass a literal `$' sign to the tokenizer.

`${VAR}'
     Variable expansion.  See below.

`$(SHELLSCRIPT)'
     Evaluate SHELLSCRIPT in a subshell, and pass whatever the shell
     script generates on stdout to the tokenizer.  The output is not
     examined by the preprocessor.

     If the shellscript exits with a non-zero exit status the module
     will be aborted.  (You can easily circumvent this restriction by
     appending `; true' to the end of the shell script.)

     SHELLSCRIPT may contain any character, including newline.  While
     scanning for the terminating close-parenthesis `)' the following
     actions are performed, in order:

    `\$'
          Replaced by a dollar sign `$'.

    `\)'
          Replaced by a right-parenthesis `)'.

    `\\'
          Replaced by a single backslash `\'.

    `\ newline'
          A backslash followed by a newline is removed.

    `\('
          Replaced by a left-parenthesis `('.  This is not really
          necessary, but this way, at least there is symmetry.

    `\ C'
          A backslash followed by any other character is an error.

    `${VAR}'
          Variable expansion.  See below.

    `$ C'
          A dollar sign followed by any other character is an error,
          and will cause the module to be aborted.

     The resulting string is passed to `/bin/sh -c STRING' for
     evaluation.  If the result contains a trailing newline it will be
     removed.

`$ C'
     A dollar sign followed by any other character is an error, and will
     cause the module to be aborted.

Variable expansion
..................

   Variable expansion is modeled after the constructs found in
`/bin/sh', but since the variable expansion is performed by cmod the
same syntax is used even if you are using `csh'.

`${VAR}'
     This is the simplest form.  It expands to the value of the
     environment variable VAR.  (The value may be affected by commands
     that have been evaluated on previous lines.)  The variable name
     VAR cannot contain newlines or any of these characters: `{:-=?+'.
     The value of the variable may contain any character, and it isn't
     examined by the preprocessor, but passed straight to the tokenizer
     (or used as a shell command if the variable was used inside a
     shellscript construct).

     The module will be aborted if the environment variable was unset.

`${VAR:-`string'}'
     If the environment variable VAR is unset or empty, `string' is
     recursively sent through the preprocessor and the result is used
     instead of VAR.  Otherwise, the value of the environment variable
     VAR is used.

`${VAR-`string'}'
     Similar to `:-', but test if VAR is unset instead of testing if it
     is unset or empty.

`${VAR:=`string'}'
     Similar to `:-', but if `string' is used it is also assigned to
     VAR.

`${VAR=`string'}'
     Similar to `:=', but test if VAR is unset instead of testing if it
     is unset or empty.

`${VAR:+`string'}'
     If the environment variable VAR is unset or empty this expands to
     the empty string.  Otherwise, `string' is recursively sent through
     the preprocessor and the result is used.

`${VAR+`string'}'
     Similar to `:+', but test if VAR is unset instead of testing if it
     is unset or empty.

`${VAR:?`string'}'
     If the environment variable VAR is unset or empty `string' is
     recursively sent through the preprocessor, the result is printed to
     stderr, and this module is aborted.  Otherwise, the value of the
     environment variable VAR is used.

`${VAR?`string'}'
     Similar to `:?', but test if VAR is unset instead of testing if it
     is unset or empty.


File: cmod.info,  Node: Tokenizer,  Next: Evaluator,  Prev: Preprocessor,  Up: Module syntax

The tokenizer
-------------

   The tokenizer is responsible for creating tokens from the character
stream it receives from the preprocessor.

`\ newline'
     Each occurrence of a backslash character \ followed by a newline is
     deleted.

`" string "'
     A string within quotes `"' is a token.  Any character, including
     newlines, may be present in the string.  The following backslash
     substitutions are performed within the string:

    `\"'
          Replaced by a quote.

    `\#'
          Replaced by a hashmark.

    `\n'
          Replaced by a newline.

    `\ space'
          Replaced by a space.

    `\ DDD'
          DDD are three octal digits.  Replaced by the character they
          represent.

    `\\'
          Replaced by a single backslash.

    `\ C'
          A backslash followed by any other character is an error.

`token'
     A token is a sequence of any character except space, newline, tab,
     quote `"' or hash-mark `#'.  The same backslash substitutions are
     performed within a token as within a string.  (Thus, even the
     forbidden characters may be inserted into a token.)

`# comment newline'
     Text from a hash through the end of the line is ignored as a
     comment.

`horizontal whitespace'
     One or more horizontal whitespace character separates tokens.
     Space and tab are the only two characters that count as horizontal
     whitespace.

`newline'
     The newline character separates lines.  As soon as the tokenizer
     receives a complete line the tokens on the line will be passed to
     the evaluator for evaluation.  (Note that leading and trailing
     horizontal whitespace on a line is ignored.)


File: cmod.info,  Node: Evaluator,  Prev: Tokenizer,  Up: Module syntax

The evaluator
-------------

   The evaluator recognizes tokens, and evaluates them.  For instance,
if the first token is `prepend-path' the evaluator expects two more
tokens: VARIABLE and VALUE.

   If a command fails the module is aborted.  That means that the
parsing of the file stops, and the effects on the environment produced
by any command that had already been evaluated in this module file will
be undone.  Side effects (such as the output produced by `module
--version') can of course not be undone.

   All cmod commands that can be used from the shell (such as `module
add sysadm') can also be used in module files.  *Note Invoking module::.
In addition to those there are a few extra commands that alter the
environment; *note Altering the environment::..


File: cmod.info,  Node: Directories,  Next: Altering the environment,  Prev: Module syntax,  Up: Module files

Using directories as module files
=================================

   All files in the directories on the module search path can be used as
modules.  You can also have directories that in turn contains module
files.  This feature can be used to provide several versions of a
module.

   Say that your site has tree versions of Emacs installed: Emacs 19.30
in `/sw/emacs-19.30', Emacs 19.34 in `/sw/emacs-19.34' and Emacs 20.3
in `/sw/emacs-20.3'.  You could provide three modules named
`emacs-19.30', `emacs-19.34' and `emacs-20.3' to handle the situation.

   An alternative way would be to create the directory `emacs' and
populate it with three module files `19.30', `19.34' and `20.3'.  You
can then use `emacs/19.34' as a module name.

   If you create a file named `default' in the `emacs' directory, any
attempt to load the module `emacs' will actually load `emacs/default'.
By putting the line `module add emacs/19.34' in `emacs/default' you can
provide a default version; in that way users that want a specific
version of emacs can load that module file, while other users can rely
on the administrator to provide a stable version of Emacs.

   On systems that support symbolic links the file `emacs/default' can
be a symbolic link to the appropriate file to load.  Cmod understands
the symbolic link and will make the output of `module avail' look nicer
if the `default' module in a subdirectory is a symbolic link.


File: cmod.info,  Node: Altering the environment,  Prev: Directories,  Up: Module files

Altering the environment
========================

   There are several commands that can alter the value of environment
variables in different ways.

   `append-path', `prepend-path' and `remove-path' treats the value as
a colon-separated list of fragments.  This convention is often used;
`MANPATH' is for instance a colon-separated list of directories that
the command `man' should look in when it searches for man pages.

* Menu:

* append-path::                 Appending a directory to a path.
* prepend-path::                Prepending a directory to a path.
* remove-path::                 Removing a directory from a path.
* setenv::                      Setting a variable to a fixed value.
* unsetenv::                    Unsetting a variable.


File: cmod.info,  Node: append-path,  Next: prepend-path,  Up: Altering the environment

append-path: Appending a directory to a path
--------------------------------------------

     append-path `var' `path'

   This command appends `path' to the tail of VAR.  If VAR already
contained `path' it will be left unmodified.


File: cmod.info,  Node: prepend-path,  Next: remove-path,  Prev: append-path,  Up: Altering the environment

prepend-path: Prepending a directory to a path
----------------------------------------------

     prepend-path `var' `path'

   This command prepends `path' to the tail of VAR.  If VAR already
contained `path' the old occurance will be removed, so the net result
is that the `path' component is moved to the front of VAR.


File: cmod.info,  Node: remove-path,  Next: setenv,  Prev: prepend-path,  Up: Altering the environment

prepend-path: Removing a directory from a path
----------------------------------------------

     remove-path `var' `path'

   This command removes all occurances of `path' from VAR.  Nothing
happens if VAR does not contain `path'.


File: cmod.info,  Node: setenv,  Next: unsetenv,  Prev: remove-path,  Up: Altering the environment

setenv: Setting a variable to a fixed value
-------------------------------------------

     setenv `var' `value'

   Set environment variable VAR to `value'.


File: cmod.info,  Node: unsetenv,  Prev: setenv,  Up: Altering the environment

unsetenv: Clearing a variable
-----------------------------

     unsetenv `var'

   Unset the environment variable VAR.


File: cmod.info,  Node: Environment variables,  Next: Bugs,  Prev: Module files,  Up: Top

Environment variables used by cmod
**********************************

   Cmod can be used to modify all environment variables.  This is a
list of the environment variables that cmod itself uses.

`MODULEPATH'
     A colon-separated list of directories that should be searched for
     modules.  You can use `module use' and `module unuse' to modify
     this variable.

`LOADEDMODULES'
     A colon-separated list of loaded modules.


File: cmod.info,  Node: Bugs,  Next: New releases,  Prev: Environment variables,  Up: Top

Reporting bugs
**************

                                                         When I grow up
                                                         I'll be stable
                                                            --- Garbage

   Bugs can be reported to ceder@lysator.liu.se or via the LysLysKOM
conference "cmod (-) modules gjort rätt".

   Ideally, a bug report should provide:

   * A short description of the bug: how it can be reproduced, what
     behavior you expected, and what happened instead.

   * A test case of the same type as can be found in the `testsuite'
     directory in the cmod distribution that demonstrates the bug.

   * A patch that fixes the bug.

   * Written and PGP-signed permission to release the code under GNU
     General Public License version 2 or later.

   You don't have to provide all of the above to report a bug, however.
If there is a problem with cmod I want to know about it so that it can
be fixed.


File: cmod.info,  Node: New releases,  Next: Function index,  Prev: Bugs,  Up: Top

New releases
************

                                                           I'm waiting,
                                                           I'm waiting,
                                                               for You.
                                                            --- Garbage

   `http://www.lysator.liu.se/~ceder/cmod/' contains the cmod home
page.  New releases of cmod are announced there.  They are also
announced in the LysLysKOM conference "cmod (-) modules gjort rätt".

   There are two kind of cmod releases: regular releases and snapshot
releases.  A regular releases has received a fair amount of testing, and
the documentation and supplementary files such as `NEWS' and `README'
should be up to date.  They are believed to be stable, and all known
bugs are documented when they are made.  The version number for a
regular release consists of two decimal numbers separated by a point.
Example: 1.0.

   Snapshot releases, on the other hand, are made whenever there has
been some significant development on cmod.  They receive minimal
testing, at best, and the only file you can count on to be up-to-date
in them is `ChangeLog'.  If you want to be on the cutting edge of cmod
development you should use snapshot releases, but otherwise you are
better of sticking to the regular releases.

   The name of a snapshot release is based on the previous regular
release, with an appended `.post.N', where N is a sequence number.  So,
for example, the third snapshot release after cmod version 1.0 was
named 1.0.post.3.

   In the future, there may also be a third kind of release: bug fix
releases.  If 1.0 should contain a serious enough error I might release
a fix.  Bug fix releases are as stable as regular releases.  They have
version numbers such as 1.0.1.


File: cmod.info,  Node: Function index,  Next: Concept index,  Prev: New releases,  Up: Top

Function, file and variable index
*********************************

* Menu:

* --help:                                Displaying information.
* --version:                             Displaying information.
* add:                                   Adding modules.
* append-path:                           append-path.
* avail:                                 Displaying information.
* bash.init:                             Defining module.
* csh.init:                              Defining module.
* default:                               Directories.
* ksh.init:                              Defining module.
* list:                                  Displaying information.
* load:                                  Adding modules.
* LOADEDMODULES, definition:             Environment variables.
* LOADEDMODULES, used by module list:    Displaying information.
* module --help:                         Displaying information.
* module --version:                      Displaying information.
* module add:                            Adding modules.
* module avail:                          Displaying information.
* module list:                           Displaying information.
* module load:                           Adding modules.
* module rm:                             Adding modules.
* module try-add:                        Adding modules.
* module unload:                         Adding modules.
* module unuse:                          Search path.
* module use:                            Search path.
* modulefiles:                           Module files.
* MODULEPATH, affects module avail:      Displaying information.
* MODULEPATH, altering with module use:  Search path.
* MODULEPATH, definition:                Environment variables.
* path.csh:                              Using system defaults.
* path.sh:                               Using system defaults.
* prepend-path:                          prepend-path.
* remove-path:                           remove-path.
* rm:                                    Adding modules.
* setenv:                                setenv.
* sh.init:                               Defining module.
* tcsh.init:                             Defining module.
* try-add:                               Adding modules.
* unload:                                Adding modules.
* unsetenv:                              unsetenv.
* unuse:                                 Search path.
* use:                                   Search path.
* zsh.init:                              Defining module.


File: cmod.info,  Node: Concept index,  Prev: Function index,  Up: Top

Concept index
*************

* Menu:

* $, quoting:                            Preprocessor.
* $, quoting in shell escape:            Preprocessor.
* : used as fragment separator:          Altering the environment.
* \, quoting in shell escape:            Preprocessor.
* abort:                                 Evaluator.
* accessing variables in module files:   Preprocessor.
* active modules, listing:               Displaying information.
* adding a directory to the search path: Search path.
* adding a module:                       Adding modules.
* altering the module search path:       Search path.
* announcements of new releases:         New releases.
* appending a fragment to a path:        append-path.
* available module, listing:             Displaying information.
* backslash substitution:                Tokenizer.
* backslash substitution in shell escape: Preprocessor.
* backslash, quoting in shell escape:    Preprocessor.
* bug-fix releases:                      New releases.
* bugs, reporting:                       Bugs.
* clearing an environment variable:      unsetenv.
* cmod:                                  Introduction.
* cmod home page:                        New releases.
* cmod version number, displaying:       Displaying information.
* colon-separated lists:                 Altering the environment.
* comment:                               Tokenizer.
* component, appending:                  append-path.
* component, prepending:                 prepend-path.
* component, removing:                   remove-path.
* credit to modules:                     Introduction.
* default module of subdirectory:        Directories.
* directory, using as module files:      Directories.
* display cmod version number:           Displaying information.
* display help:                          Displaying information.
* display loaded modules:                Displaying information.
* dollar sign, quoting:                  Preprocessor.
* dollar, quoting in shell escape:       Preprocessor.
* email address to cmod:                 New releases.
* environment variable, setting an:      setenv.
* environment variable, unsetting an:    unsetenv.
* environment variables, accessing:      Preprocessor.
* error handling:                        Evaluator.
* evaluator:                             Evaluator.
* feedback:                              New releases.
* fragment:                              Altering the environment.
* fragment, appending:                   append-path.
* fragment, prepending:                  prepend-path.
* fragment, removing:                    remove-path.
* getting in touch:                      New releases.
* help:                                  Displaying information.
* home page for cmod:                    New releases.
* http://www.lysator.liu.se/~ceder/cmod/: New releases.
* http://www.modules.org/:               Introduction.
* introduction:                          Introduction.
* list available modules:                Displaying information.
* list loaded modules:                   Displaying information.
* loaded modules, listing:               Displaying information.
* loading a module:                      Adding modules.
* LysKOM:                                New releases.
* module file:                           Introduction.
* module file parsing:                   Module syntax.
* module file, writing:                  Module files.
* module search path, altering:          Search path.
* module, abort loading of:              Evaluator.
* module, list available:                Displaying information.
* module, loading:                       Adding modules.
* module, unloading:                     Adding modules.
* modules, list loaded:                  Displaying information.
* modules, the ancestor of cmod:         Introduction.
* new releases:                          New releases.
* octal digits:                          Tokenizer.
* parsing module files:                  Module syntax.
* passes:                                Module syntax.
* path separator:                        Altering the environment.
* path, appending to:                    append-path.
* path, prepending to:                   prepend-path.
* path, removing parts of:               remove-path.
* prepending a fragment to a path:       prepend-path.
* preprocessor:                          Preprocessor.
* quoted string:                         Tokenizer.
* quoting dollar signs:                  Preprocessor.
* releases, different kinds of:          New releases.
* removing a directory from the search path: Search path.
* removing a fragment to a path:         remove-path.
* removing a module:                     Adding modules.
* reporting bugs:                        Bugs.
* search path, altering:                 Search path.
* setting an environment variable:       setenv.
* shell escape:                          Preprocessor.
* shellscript:                           Preprocessor.
* snapshot releases:                     New releases.
* special characters:                    Tokenizer.
* stable releases:                       New releases.
* symbolic links:                        Directories.
* syntax of module files:                Module syntax.
* token:                                 Tokenizer.
* tokenizer:                             Tokenizer.
* unloading a module:                    Adding modules.
* unsetting an environment variable:     unsetenv.
* unusing a module directory:            Search path.
* usage message:                         Displaying information.
* using a module directory:              Search path.
* variable expansion:                    Preprocessor.
* variables, setting:                    setenv.
* variables, unsetting:                  unsetenv.
* version number, displaying:            Displaying information.
* version numbers:                       New releases.
* versioned modules:                     Directories.
* web address to cmod:                   New releases.



Tag Table:
Node: Top1267
Node: Introduction1926
Node: Initializing cmod3884
Node: Using system defaults4427
Node: Defining module5617
Node: Invoking module6288
Node: Adding modules6769
Node: Search path8154
Node: Displaying information8714
Node: Module files9844
Node: Module syntax10503
Node: Preprocessor11587
Node: Tokenizer15696
Node: Evaluator17477
Node: Directories18324
Node: Altering the environment19866
Node: append-path20713
Node: prepend-path21039
Node: remove-path21475
Node: setenv21816
Node: unsetenv22079
Node: Environment variables22283
Node: Bugs22811
Node: New releases23873
Node: Function index25766
Node: Concept index28406

End Tag Table

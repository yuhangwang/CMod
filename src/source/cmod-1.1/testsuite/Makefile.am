# This file was generated by gen-makefile.py.  Do not edit.

#  cmod - set up the user environment with module files.
#  Copyright (C) 1997, 1998 by the authors of cmod (see AUTHORS).
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. */

## Process this file with mkmf.py to produce Makefile.am.
## Process Makefile.am with automake to produce Makefile.in.
## Process Makefile.in with configure to produce Makefile.
## Process Makefile with make to produce the output string "All tests passed".

AUTOMAKE_OPTIONS =  gnu

PYTHON = @PYTHON@
RM = rm -f

# Test suite support.
noinst_PROGRAMS = clean-sh
MODULES = t002-1 t004-1 t005-1 t006-1 t008-1 t009-1 t010-1 t015-1 t018-1 t019-1 t024-1 t026-1 t029-1 t029-2 t029-3 t029-4 t029-5 t034-1 t035-1 t036-1 t037-1 t039-1 t045-1 t046-1 t047-1 t048-1 t049-1 t050-1 t051-1 t052-1 t053-1 t054-1 t060-1 t061-1 t062-1 t063-1 t064-1 t065-1 t066-1 t067-1 t068-1 t069-1 t070-1 t071-1 t072-1 t073-1 t074-1 t075-1 t076-1

SIMPLE_SH_TESTS = t001.sh t002.sh t003.sh t004.sh t005.sh t006.sh t007.sh t008.sh t009.sh t010.sh t011.sh t012.sh t013.sh t014.sh t015.sh t016.sh t017.sh t018.sh t019.sh t020.sh t021.sh t022.sh t023.sh t024.sh t025.sh t026.sh t027.sh t028.sh t029.sh t031.sh t032.sh t033.sh t034.sh t035.sh t036.sh t037.sh t038.sh t039.sh t040.sh t041.sh t042.sh t043.sh t045.sh t046.sh t047.sh t048.sh t049.sh t050.sh t051.sh t052.sh t053.sh t054.sh t055.sh t056.sh t057.sh t058.sh t059.sh t060.sh t061.sh t062.sh t063.sh t064.sh t065.sh t066.sh t067.sh t068.sh t069.sh t070.sh t071.sh t072.sh t073.sh t074.sh t075.sh t076.sh

SIMPLE_OK_FILES = t001.ok t002.ok t003.ok t004.ok t006.ok t007.ok t008.ok t009.ok t010.ok t011.ok t012.ok t013.ok t014.ok t015.ok t016.ok t017.ok t018.ok t019.ok t020.ok t021.ok t022.ok t023.ok t024.ok t026.ok t027.ok t028.ok t029.ok t030.ok t031.ok t032.ok t033.ok t037.ok t038.ok t039.ok t040.ok t041.ok t042.ok t045.ok t046.ok t047.ok t050.ok t051.ok t052.ok t053.ok t054.ok t055.ok t056.ok t057.ok t058.ok t059.ok t060.ok t061.ok t062.ok t063.ok t065.ok t066.ok t067.ok t069.ok t070.ok t071.ok t072.ok

GENERATED_OK_FILES_SRC = t005.ok.in t025.ok.in t034.ok.in t035.ok.in t036.ok.in t043.ok.in t044.ok.in t048.ok.in t049.ok.in t064.ok.in t068.ok.in t073.ok.in t074.ok.in t075.ok.in t076.ok.in
GENERATED_OK_FILES = t005.ok.gen t025.ok.gen t034.ok.gen t035.ok.gen t036.ok.gen t043.ok.gen t044.ok.gen t048.ok.gen t049.ok.gen t064.ok.gen t068.ok.gen t073.ok.gen t074.ok.gen t075.ok.gen t076.ok.gen

GENERATED_SH_TESTS_SRC = t030.sh.in t044.sh.in
GENERATED_SH_TESTS = t030.sh.gen t044.sh.gen

clean_sh_SOURCES = clean-sh.c

INCLUDES = -I$(top_srcdir)

MOSTLYCLEANFILES = $(GENERATED_OK_FILES) $(GENERATED_SH_TESTS) \
	*.outss *.outss1 *.outsg *.outsg1 \
	*.outgs *.outgs1 *.outgg *.outgg1


MAINTAINERCLEANFILES = Makefile.am Makefile.in

MOSTLYCLEANDIRS = t041-dir t042-dir t055-dir t057-dir t058-dir t059-dir

mostlyclean-local:
	$(RM) -r $(MOSTLYCLEANDIRS)

SH_TESTS = $(SIMPLE_SH_TESTS) $(GENERATED_SH_TESTS)

TEST_RESULTS = $(TEST_SS) $(TEST_SG) $(TEST_GS) $(TEST_GG)

TEST_SS = t001.outss t002.outss t003.outss t004.outss t006.outss t007.outss t008.outss t009.outss t010.outss t011.outss t012.outss t013.outss t014.outss t015.outss t016.outss t017.outss t018.outss t019.outss t020.outss t021.outss t022.outss t023.outss t024.outss t026.outss t027.outss t028.outss t029.outss t031.outss t032.outss t033.outss t037.outss t038.outss t039.outss t040.outss t041.outss t042.outss t045.outss t046.outss t047.outss t050.outss t051.outss t052.outss t053.outss t054.outss t055.outss t056.outss t057.outss t058.outss t059.outss t060.outss t061.outss t062.outss t063.outss t065.outss t066.outss t067.outss t069.outss t070.outss t071.outss t072.outss
TEST_SG = t005.outsg t025.outsg t034.outsg t035.outsg t036.outsg t043.outsg t048.outsg t049.outsg t064.outsg t068.outsg t073.outsg t074.outsg t075.outsg t076.outsg
TEST_GS = t030.outgs
TEST_GG = t044.outgg

CMODTEST = ./clean-sh MODULEPATH=$(srcdir)

check-local: $(TEST_RESULTS)
	@echo All tests passed

SUFFIXES = .outss .outsg .outgs .sh .ok .gen .in

# The "(if ... ; then echo $? ; else echo $? ; fi)" construct used in
# targets TEST_SS, TEST_SG, TEST_GS and TEST_GG may seem strange, but
# it is needed on SunOS 4.1.4 with a native make and /bin/sh.
# Rewriting the command "(... ; echo $?)" will cause make to complain
# if the "..." command exits with a non-zero exit status.

$(TEST_SS):
	@$(RM) $@ $@1
	(base=`basename $@ .outss` ;\
	(if $(CMODTEST) $(srcdir)/$$base.sh ; \
	 then echo $$? ; else echo $$? ; fi) > $@1 2>&1 ;\
	if cmp $@1 $(srcdir)/$$base.ok ; then \
	  mv $@1 $@ ; \
	else \
	  echo $@1:1:Generated file differ 2>&1; \
	  echo $(srcdir)/$$base.ok:1:from the expected result. 2>&1; \
	  diff -u $(srcdir)/$$base.ok $@1; \
	  exit 1 ; \
	fi)
	@echo PASS: $@

$(TEST_SG):
	@$(RM) $@ $@1
	(base=`basename $@ .outsg` ;\
	(if $(CMODTEST) $(srcdir)/$$base.sh ; \
	 then echo $$? ; else echo $$? ; fi) > $@1 2>&1 ;\
	if cmp $@1 $$base.ok.gen ; then \
	  mv $@1 $@ ; \
	else \
	  echo $@1:1:Generated file differ 2>&1; \
	  echo $$base.ok.gen:1:from the expected result. 2>&1; \
	  diff -u $$base.ok.gen $@1; \
	  exit 1 ; \
	fi)
	@echo PASS: $@

$(TEST_GS):
	@$(RM) $@ $@1
	(base=`basename $@ .outgs` ;\
	(if $(CMODTEST) $$base.sh.gen ; \
	 then echo $$? ; else echo $$? ; fi) > $@1 2>&1 ;\
	if cmp $@1 $(srcdir)/$$base.ok ; then \
	  mv $@1 $@ ; \
	else \
	  echo $@1:1:Generated file differ 2>&1; \
	  echo $(srcdir)/$$base.ok:1:from the expected result. 2>&1; \
	  diff -u $(srcdir)/$$base.ok $@1; \
	  exit 1 ; \
	fi)
	@echo PASS: $@

$(TEST_GG):
	@$(RM) $@ $@1
	(base=`basename $@ .outgg` ;\
	(if $(CMODTEST) $$base.sh.gen ; \
	 then echo $$? ; else echo $$? ; fi) > $@1 2>&1 ;\
	if cmp $@1 $$base.ok.gen ; then \
	  mv $@1 $@ ; \
	else \
	  echo $@1:1:Generated file differ 2>&1; \
	  echo $$base.ok.gen:1:from the expected result. 2>&1; \
	  diff -u $$base.ok.gen $@1; \
	  exit 1 ; \
	fi)
	@echo PASS: $@

$(GENERATED_OK_FILES): Makefile
	   $(RM) $@.tmp $@
	   sed -e 's%@MODULEPATH@%$(srcdir)%g' \
		   < $(srcdir)/`basename $@ .gen`.in > $@.tmp
	   mv $@.tmp $@

$(GENERATED_SH_TESTS): Makefile
	   $(RM) $@.tmp $@
	   sed -e 's%@MODULEPATH@%$(srcdir)%g' \
		   < $(srcdir)/`basename $@ .gen`.in > $@.tmp
	   mv $@.tmp $@

EXTRA_DIST = $(SIMPLE_SH_TESTS) $(GENERATED_SH_TESTS_SRC) $(MODULES) \
	.cvsignore $(GENERATED_OK_FILES_SRC) $(SIMPLE_OK_FILES) \
	Makefile.tmpl mkmf.py

dist-hook:
	for dir in t030-fb t030-vi t030-sy ; do \
	  cp -a $(srcdir)/$$dir $(distdir) || exit 1; \
	  find $(distdir)/$$dir -name CVS -print | xargs $(RM) -r ; \
	done

$(srcdir)/Makefile.am: Makefile.tmpl mkmf.py
	cd $(srcdir) && $(PYTHON) mkmf.py Makefile.tmpl Makefile.am

t045.outss: t045.sh t045.ok
t068.outsg: t068.sh t068.ok.gen
t068.ok.gen: t068.ok.in
t004.outss: t004.sh t004.ok
t057.outss: t057.sh t057.ok
t059.outss: t059.sh t059.ok
t005.outsg: t005.sh t005.ok.gen
t005.ok.gen: t005.ok.in
t076.outsg: t076.sh t076.ok.gen
t076.ok.gen: t076.ok.in
t007.outss: t007.sh t007.ok
t002.outss: t002.sh t002.ok
t006.outss: t006.sh t006.ok
t062.outss: t062.sh t062.ok
t026.outss: t026.sh t026.ok
t027.outss: t027.sh t027.ok
t024.outss: t024.sh t024.ok
t025.outsg: t025.sh t025.ok.gen
t025.ok.gen: t025.ok.in
t022.outss: t022.sh t022.ok
t023.outss: t023.sh t023.ok
t020.outss: t020.sh t020.ok
t021.outss: t021.sh t021.ok
t037.outss: t037.sh t037.ok
t041.outss: t041.sh t041.ok
t039.outss: t039.sh t039.ok
t038.outss: t038.sh t038.ok
t008.outss: t008.sh t008.ok
t009.outss: t009.sh t009.ok
t028.outss: t028.sh t028.ok
t029.outss: t029.sh t029.ok
t019.outss: t019.sh t019.ok
t018.outss: t018.sh t018.ok
t075.outsg: t075.sh t075.ok.gen
t075.ok.gen: t075.ok.in
t074.outsg: t074.sh t074.ok.gen
t074.ok.gen: t074.ok.in
t052.outss: t052.sh t052.ok
t001.outss: t001.sh t001.ok
t013.outss: t013.sh t013.ok
t012.outss: t012.sh t012.ok
t011.outss: t011.sh t011.ok
t010.outss: t010.sh t010.ok
t017.outss: t017.sh t017.ok
t016.outss: t016.sh t016.ok
t015.outss: t015.sh t015.ok
t014.outss: t014.sh t014.ok
t056.outss: t056.sh t056.ok
t067.outss: t067.sh t067.ok
t070.outss: t070.sh t070.ok
t046.outss: t046.sh t046.ok
t064.outsg: t064.sh t064.ok.gen
t064.ok.gen: t064.ok.in
t066.outss: t066.sh t066.ok
t031.outss: t031.sh t031.ok
t030.outgs: t030.sh.gen t030.ok
t030.sh.gen: t030.sh.in
t047.outss: t047.sh t047.ok
t033.outss: t033.sh t033.ok
t032.outss: t032.sh t032.ok
t055.outss: t055.sh t055.ok
t035.outsg: t035.sh t035.ok.gen
t035.ok.gen: t035.ok.in
t034.outsg: t034.sh t034.ok.gen
t034.ok.gen: t034.ok.in
t063.outss: t063.sh t063.ok
t040.outss: t040.sh t040.ok
t054.outss: t054.sh t054.ok
t036.outsg: t036.sh t036.ok.gen
t036.ok.gen: t036.ok.in
t042.outss: t042.sh t042.ok
t069.outss: t069.sh t069.ok
t053.outss: t053.sh t053.ok
t043.outsg: t043.sh t043.ok.gen
t043.ok.gen: t043.ok.in
t060.outss: t060.sh t060.ok
t051.outss: t051.sh t051.ok
t050.outss: t050.sh t050.ok
t061.outss: t061.sh t061.ok
t058.outss: t058.sh t058.ok
t073.outsg: t073.sh t073.ok.gen
t073.ok.gen: t073.ok.in
t065.outss: t065.sh t065.ok
t071.outss: t071.sh t071.ok
t044.outgg: t044.sh.gen t044.ok.gen
t044.sh.gen: t044.sh.in
t044.ok.gen: t044.ok.in
t048.outsg: t048.sh t048.ok.gen
t048.ok.gen: t048.ok.in
t072.outss: t072.sh t072.ok
t049.outsg: t049.sh t049.ok.gen
t049.ok.gen: t049.ok.in
t003.outss: t003.sh t003.ok


noinst_DATA = .made_mode

../../sysp/lib/libsysp.a:
	${MAKE} -C ../../sysp/lib all

.made_mode: ../../sysp/lib/libsysp.a PLog_wrap.c Makefile.perl
	$(MAKE) -f Makefile.perl
	touch $@

PLog_wrap.c: PLog.i
	swig -perl5 PLog.i

Makefile.perl: Makefile.PL
	perl $< MAKEFILE=$@

install:
	$(MAKE) -fMakefile.perl DESTDIR=$(prefix) install_vendor

CLEANFILES = *~ .made_mode PLog_wrap.c PLog.pm
clean-local:
	if test -f Makefile.perl; then $(MAKE) -fMakefile.perl realclean; fi
	rm -f Makefile.perl Makefile.perl.old

# /.../
# Copyright (c) 2001 SuSE GmbH Nuernberg, Germany. All rights reserved.
# Author: Marcus Schaefer <sax@suse.de>, 2001 
#
# Makefile for SaX2
# X-Server configuration program
#
# Version 8.1
# Status: Up-to-date
#
# Build init
buildroot      = /
mandir         = /usr/share/man
bindlib        = lib

ASK_FOR_CHANGELOG=no
TEMPLATE_CHECK=no
ARCH=`/bin/arch`

export

#============================================
# Prefixs...
#--------------------------------------------
etc_prefix     = ${buildroot}/etc
usr_prefix     = ${buildroot}/usr
sax_prefix     = ${buildroot}/usr/share/sax
xfi_prefix     = ${buildroot}/usr/share/xfine
var_prefix     = ${buildroot}/var/cache
lib_prefix     = ${buildroot}/usr/lib
doc_prefix     = ${buildroot}/usr/share/doc/packages
man_prefix     = ${buildroot}/usr/share/man

#============================================
# Variables... 
#--------------------------------------------
MANVZ          = ${man_prefix}/man1
PACKDOCVZ      = ${doc_prefix}/sax2
SBINVZ         = ${buildroot}/usr/sbin
LIBSAXVZ       = ${buildroot}/var/lib/sax
ICERCVZ        = ${buildroot}/etc/icewm
SAXBINVZ       = ${sax_prefix}
APIVZ          = ${sax_prefix}/api
APIDATAVZ      = ${sax_prefix}/api/data
CDBVZ          = ${sax_prefix}/api/data/cdb
PIXVZ          = ${sax_prefix}/api/figures
MODULEVZ       = ${sax_prefix}/modules
DETECTVZ       = ${sax_prefix}/modules/detect
CREATEVZ       = ${sax_prefix}/modules/create
SYSPVZ         = ${sax_prefix}/sysp
MAPVZ          = ${sax_prefix}/sysp/maps
PROFILE        = ${sax_prefix}/profile
SAXCACHEVZ     = ${var_prefix}/sax
XFICACHEVZ     = ${var_prefix}/xfine
RDBMS          = ${var_prefix}/sax/sysp/rdbms
FILES          = ${var_prefix}/sax/files
SCRIPTVZ       = ${sax_prefix}/sysp/script
SAXTOOLS       = ${sax_prefix}/tools
APITOOLS       = ${sax_prefix}/api/macros
USRLIBVZ       = ${lib_prefix}
INCVZ          = ${usr_prefix}/include/sax
LOCALEVZ       = ${usr_prefix}/share/locale
XFINEBINVZ     = ${xfi_prefix}
XFINEPIXMAPS   = ${xfi_prefix}/figures
LIBDOCDIR      = ${doc_prefix}/libsax
LIBTOOLDIR     = ${sax_prefix}/libsax
UDEVDIR        = ${etc_prefix}/udev/rules.d
HWIDSDIR       = ${buildroot}/var/lib/hardware/ids

#============================================
# Helpers
#--------------------------------------------
ARCH           = `arch`
PVERSION       = `rpm -q perl --queryformat '%{VERSION}' | tr -d .`
LC             = LC_MESSAGES

all:
	#============================================
	# checking basic font path
	#--------------------------------------------
	( ./.fontpath )

	#============================================
	# building parsers and system profiler
	#--------------------------------------------
	${MAKE} -C ./parse -f Makefile.Linux all
	${MAKE} -C ./spp   -f Makefile.swig  all
	${MAKE} -C ./parse all
	${MAKE} -C ./spp   all
	${MAKE} -C ./sysp  all

	#============================================
	# building framebuffer info module
	#--------------------------------------------
	${MAKE} -C ./sysp/lib/fbset -f Makefile.swig swig
	${MAKE} -C ./sysp/lib/fbset all

	#============================================
	# building CVT modeline module
	#--------------------------------------------
	${MAKE} -C ./sysp/lib/cvt -f Makefile.swig
	${MAKE} -C ./sysp/lib/cvt all

	#============================================
	# building libsax and bindings...
	#--------------------------------------------
	( test -f /usr/lib/graphviz/libgvplugin_dot_layout.so && dot -c || true )
	( cd libsax && doxygen sax.dox )
	( cd libsax && ./.make )
	${MAKE} -C ./libsax all
	( cd libsax && \
	for i in `ls -1 ./bindings`; do \
		if [ -d ./bindings/$$i ];then \
		( \
			cd ./bindings/$$i && \
			make -f Makefile.swig && make BINDLIB=${bindlib} \
		) ;\
		fi \
	done )

	#============================================
	# building GUI and tools...
	#--------------------------------------------
	( cd api && ./.make )
	( cd api/xcmd && ./.make )
	( cd tools && make -f Makefile.ac buildroot=${buildroot} )
	${MAKE} -C ./api/xcmd all
	${MAKE} -C ./api all
	${MAKE} -C ./tools all

	#============================================
	# building XFine2...
	#--------------------------------------------
	( cd xfine && ./.make )
	${MAKE} -C ./xfine all

install:install-docs
	#============================================
	# Install base directories
	#--------------------------------------------
	install -d -m 755 ${APIVZ} ${APIDATAVZ} ${USRLIBVZ}
	install -d -m 755 ${CDBVZ} ${PIXVZ} ${INCVZ} ${ICERCVZ}
	install -d -m 755 ${MODULEVZ} ${SYSPVZ} ${MAPVZ} ${FILES} ${DETECTVZ}
	install -d -m 755 ${RDBMS} ${SCRIPTVZ} ${SAXTOOLS} ${LIBVZ}
	install -d -m 755 ${PROFILE} ${SBINVZ} ${SAXCACHEVZ} ${XFICACHEVZ}
	install -d -m 755 ${CREATEVZ} ${APITOOLS} ${XFINEBINVZ} ${XFINEPIXMAPS}
	install -d -m 755 ${LIBDOCDIR} ${LIBTOOLDIR} ${LIBSAXVZ}
	install -d -m 755 ${HWIDSDIR}
	#install -d -m 755 ${UDEVDIR}

	#============================================
	# hwinfo data
	#--------------------------------------------
	echo  "vendor.id         usb 0x04e7" >  ${HWIDSDIR}/sax2-ident
	echo  "&device.id        usb 0x0030" >> ${HWIDSDIR}/sax2-ident
	echo  "+hwclass          mouse"      >> ${HWIDSDIR}/sax2-ident

	#============================================
	# udev rules
	#--------------------------------------------
	#install -m 644 udev/* ${UDEVDIR}

	#============================================
	# install X11 parser...
	#--------------------------------------------
	( cd parse && make DESTDIR=${buildroot} install_vendor )

	#============================================
	# install SaX Profile parser...
	#--------------------------------------------
	( cd spp && make DESTDIR=${buildroot} install_vendor )

	#============================================
	# install SaX Framebuffer info module...
	#--------------------------------------------
	( cd sysp/lib/fbset && make DESTDIR=${buildroot} install_vendor )

	#============================================
	# install SaX CVT modeline module...
	#--------------------------------------------
	( cd sysp/lib/cvt && make DESTDIR=${buildroot} install_vendor )

	#============================================
	# install language bindings...
	#--------------------------------------------
	( cd libsax && \
	for i in `ls -1 ./bindings`; do \
		if [ -d ./bindings/$$i ];then \
		( \
			cd ./bindings/$$i && \
			make DESTDIR=${buildroot} BINDLIB=${bindlib} install_vendor \
		) ;\
		fi \
	done )

	#============================================
	# install NLS support (translations)...
	#--------------------------------------------
	( cd ./locale && ./.locale )
	for i in `ls -1 ./locale`; do \
		if [ -d ./locale/$$i ];then \
		if [ ! "$$i" = "sax-help" ] && [ ! "$$i" == "sax-template" ];then \
		install -d -m 755 ${LOCALEVZ}/$$i/${LC} ;\
		( cd ./locale/$$i/${LC}  && msgfmt -o sax.mo sax.po ) ;\
		install -m 644 ./locale/$$i/${LC}/sax.mo   ${LOCALEVZ}/$$i/${LC} ;\
		fi \
		fi \
	done

	#============================================
	# install core files...
	#--------------------------------------------
	for i in `find ./profile -type f | grep -v TREE`; do \
		echo $$i | grep -q .pl$$		&& \
		install -m 755 $$i ${PROFILE}	|| \
		install -m 644 $$i ${PROFILE}	;\
	done
	install -m 755 ./init.pl                                ${SAXBINVZ}
	install -m 644 ./svnbuild                               ${SAXBINVZ}
	install -m 755 ./xc.pl                                  ${SAXBINVZ}
	install -m 755 ./tools/xwrapper/xw                      ${SBINVZ}
	install -m 755 ./api/xapi                               ${SBINVZ}
	install -m 755 ./api/xcmd/xcmd                          ${SBINVZ}
	install -m 755 ./api/macros/*                           ${APITOOLS}
	install -m 755 ./startup/pci.pl                         ${SAXBINVZ}
	install -m 755 ./sysp/sysp                              ${SBINVZ}
	install -m 644 ./modules/*.pm                           ${MODULEVZ}
	install -m 644 ./modules/detect/*                       ${DETECTVZ}
	install -m 644 ./modules/create/*                       ${CREATEVZ}
	install -m 644 ./modules/export/*                       ${MODULEVZ}
	install -m 644 ./parse/XFree.pm                         ${MODULEVZ}
	install -m 644 ./api/data/cdb/*                         ${CDBVZ}
	cp -l ./api/figures/*                                   ${PIXVZ}
	install -m 755 ./startup/sax.sh                         ${SBINVZ}
	install -m 755 ./startup/sax2-vesa                      ${SBINVZ}
	install -m 755 ./startup/SaX2                           ${SBINVZ}
	install -m 644 ./sysp/maps/Identity.map                 ${MAPVZ}
	install -m 644 ./sysp/maps/Keyboard.map                 ${MAPVZ}
	install -m 644 ./sysp/maps/Vendor.map                   ${MAPVZ}
	install -m 644 ./sysp/maps/Input.map                    ${MAPVZ}
	install -m 644 ./sysp/maps/Driver.map                   ${MAPVZ}
	install -m 644 ./sysp/maps/IntelPatch.map               ${MAPVZ}
	install -m 755 ./sysp/script/*                          ${SCRIPTVZ}

	#============================================
	# install architecture dependant Identity
	#--------------------------------------------
	if [ -f ./sysp/maps/arch/Identity.map.${ARCH} ];then \
		install -m 644 ./sysp/maps/arch/Identity.map.${ARCH} \
			${MAPVZ}/Identity.map ;\
	fi

	#============================================
	# transform Identitiy info to Cards file
	#--------------------------------------------
	./.cards.pl -f ${MAPVZ}/Identity.map > ${CDBVZ}/Cards

	#============================================
	# install library files...
	#--------------------------------------------
	cp -a ./libsax/doc/doxygen/html ${LIBDOCDIR}
	cp -a ./libsax/libsax.so*       ${USRLIBVZ}
	cp ./libsax/*.h                 ${INCVZ}
	cp ./libsax/tools/*             ${LIBTOOLDIR}

	#============================================
	# install API data files...
	#--------------------------------------------
	for i in `find ./api/data -maxdepth 1 -name "*" | grep -v .orig`; do \
	if [ -f $$i ];then \
		install -m 644 $$i  ${APIDATAVZ} ;\
	fi \
	done

	#============================================
	# move icerc.sax file to /etc/icewm
	#--------------------------------------------
	mv ${APIDATAVZ}/icerc.sax ${ICERCVZ}

	#============================================
	# install tools...
	#--------------------------------------------
	${MAKE} -C ./tools install

	#============================================
	# install XFine2...
	#--------------------------------------------
	install -m 644 ./xfine/figures/*   ${XFINEPIXMAPS}
	install -m 755 ./xfine/xfine       ${SBINVZ}
	install -m 644 ./xfine/xfine.gtx   ${XFINEBINVZ}

	#============================================
	# create startup links...
	#--------------------------------------------
	( rm -f ${SBINVZ}/sax2 && cd ${SBINVZ} && ln -s SaX2 sax2 )

	#=============================================
	# install desktop icon...
	#---------------------------------------------
	install -d -m 755 ${buildroot}/usr/share/pixmaps
	install -m 644 api/figures/sax2.png ${buildroot}/usr/share/pixmaps
	
	@echo "remember to have fun..."

install-docs:
	#============================================
	# install documentation...
	#--------------------------------------------
	install -d -m 755 ${PACKDOCVZ} ${MANVZ}

	#============================================
	# SaX2 system draft and LICENSE
	#--------------------------------------------
	install -m 644 ./LICENSE           ${PACKDOCVZ}
	install -m 644 ./doc/README        ${PACKDOCVZ}
	install -m 644 ./doc/sax.pdf       ${PACKDOCVZ}

	#============================================
	# SaX2 tools manual pages
	#--------------------------------------------
	for i in `ls -1 ./doc/sax-man`;do \
		install -m 644 ./doc/sax-man/$$i ${MANVZ}/$$i.1 ;\
	done

build:
	./.doit -p --local

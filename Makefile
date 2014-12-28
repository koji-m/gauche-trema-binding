# General info
SHELL       = /bin/bash
prefix      = /usr
exec_prefix = ${prefix}
bindir      = ${exec_prefix}/bin
libdir      = ${exec_prefix}/lib
datadir     = ${datarootdir}
datarootdir = ${prefix}/share
srcdir      = .


# These may be overridden by make invocators
DESTDIR        =
GOSH           = "/usr/bin/gosh"
GAUCHE_CONFIG  = "/usr/bin/gauche-config"
GAUCHE_PACKAGE = "/usr/bin/gauche-package"
INSTALL        = "/usr/bin/gauche-install" -C

# For Gauche-Trema parameters
GAUCHE_LIBDIR     = -L/usr/lib/gauche-0.9/0.9.1/i686-pc-linux-gnu
GAUCHE_TREMA_LIBS = -lgauche-0.9 -ldl -lcrypt -lm  -lpthread -ltrema -lrt -lsqlite3
GAUCHE_CC         = "gcc -std=gnu99"

# For Trema parameters
trema_rebuild_dir = $(srcdir)/trm_rebuild
TREMA_LDFLAGS  = -L$(trema_rebuild_dir)
#this should be modified before release. (CFLAGS)
TREMA_CFLAGS   = "-I$(trema_rebuild_dir)/lib -I/var/lib/gems/1.8/gems/trema-0.3.20/objects/openflow"
TREMA_LIBS     = "-ltrema -lrt -lpthread -lsqlite3 -ldl"
TREMA_HOME     = "/var/lib/gems/1.8/gems/trema-0.3.20"

# Other parameters
SOEXT  = so
OBJEXT = o
EXEEXT = 
LOCAL_PATHS = ""

# Module-specific stuff
PACKAGE   = gauche-trema

ARCHFILES = gauche_trema.$(SOEXT)
SCMFILES  = $(srcdir)/net/trema.scm
HEADERS   = 

TARGET    = $(ARCHFILES)
GENERATED = 
CONFIG_GENERATED = Makefile config.cache config.log config.status \
		   configure.lineno autom4te*.cache $(PACKAGE).gpd

GAUCHE_PKGINCDIR  = "$(DESTDIR)${libdir}/gauche-0.9/site/include"
GAUCHE_PKGLIBDIR  = "$(DESTDIR)${datadir}/gauche-0.9/site/lib"
GAUCHE_PKGARCHDIR = "$(DESTDIR)${libdir}/gauche-0.9/site/i686-pc-linux-gnu"

gauche_trema_OBJS  = $(srcdir)/gauche_trema.o $(srcdir)/gauche_tremalib.o
trema_rebuild_SRCS = $(trema_rebuild_dir)/lib/log.c \
                     $(trema_rebuild_dir)/lib/log.h
libtrema           = $(trema_rebuild_dir)/libtrema.a

all : $(TARGET)

gauche_trema.$(SOEXT): $(gauche_trema_OBJS) $(libtrema)
	gcc -std=gnu99 -g -shared \
	  $(GAUCHE_LIBDIR) $(TREMA_LDFLAGS) \
	  -o gauche_trema.$(SOEXT) $(gauche_trema_OBJS) \
	  $(GAUCHE_TREMA_LIBS)

gauche_trema.$(OBJEXT): gauche_trema.c
	$(GAUCHE_PACKAGE) compile -c --verbose \
	 --cflags=$(TREMA_CFLAGS) gauche_trema.c

gauche_tremalib.$(OBJEXT): gauche_tremalib.stub
	$(GAUCHE_PACKAGE) compile -c --verbose \
	 --cflags=$(TREMA_CFLAGS) gauche_tremalib.stub

#$(libtrema): $(trema_rebuild_SRCS)
#	cp $(TREMA_HOME)/objects/lib/libtrema.a $(trema_rebuild_dir)
#	cp -r $(TREMA_HOME)/src/lib $(srcdir)/trm_rebuild
#	patch -u -p1 -d lib < $(trema_rebuild_dir)/libtrema.patch
#	gcc -c
#	ar rv $(libtrema) $(trema_rebuild_dir)/lib/log.o \
#	 $(trema_rebuild_dir)/lib/messenger.o

check : all
	@rm -f test.log
	$(GOSH) -I. -I$(srcdir) $(srcdir)/test.scm > test.log

install : all
	$(INSTALL) -m 444 -T $(GAUCHE_PKGINCDIR) $(HEADERS)
	$(INSTALL) -m 444 -T $(GAUCHE_PKGLIBDIR) $(SCMFILES) 
	$(INSTALL) -m 555 -T $(GAUCHE_PKGARCHDIR) $(ARCHFILES)
	$(INSTALL) -m 444 -T $(GAUCHE_PKGLIBDIR)/.packages $(PACKAGE).gpd

uninstall :
	$(INSTALL) -U $(GAUCHE_PKGINCDIR) $(HEADERS)
	$(INSTALL) -U $(GAUCHE_PKGLIBDIR) $(SCMFILES) 
	$(INSTALL) -U $(GAUCHE_PKGARCHDIR) $(ARCHFILES)
	$(INSTALL) -U $(GAUCHE_PKGLIBDIR)/.packages $(PACKAGE).gpd

clean :
	$(GAUCHE_PACKAGE) compile --clean gauche_trema $(gauche_trema_SRCS)
	rm -rf core $(TARGET) $(GENERATED) *~ test.log so_locations

distclean : clean
	rm -rf $(CONFIG_GENERATED)

maintainer-clean : clean
	rm -rf $(CONFIG_GENERATED) configure VERSION


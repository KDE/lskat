####### kdevelop will overwrite this part!!! (begin)##########
bin_PROGRAMS = lskat
lskat_SOURCES = msgdlg.cpp aboutdlgdata.cpp aboutdlg.cpp networkdlg.cpp namedlg.cpp KRemoteConnect.cpp KRSocket.cpp KProcessConnect.cpp KMessageEntry.cpp KInteractiveConnect.cpp KInputChildProcess.cpp KEMessage.cpp KEInput.cpp KConnectEntry.cpp KChildConnect.cpp lskatview.cpp lskatdoc.cpp lskat.cpp main.cpp 
lskat_LDADD   = -lkfile -lkdeui -lkdecore -lqt -lXext -lX11 $(LIBSOCKET)

INCLUDES = $(all_includes)

lskat_METASOURCES = AUTO

lskat_LDFLAGS = $(all_libraries) $(KDE_RPATH)

DISTCLEANFILES = $(lskat_METASOURCES)

messages:


EXTRA_DIST = index.html main.cpp lskat.cpp lskat.h lskatdoc.cpp lskatdoc.h \
lskatview.cpp lskatview.h resource.h lskat.desktop large-lskat.xpm \
lskat.xpm mini-lskat.xpm KChildConnect.cpp KChildConnect.h KConnectEntry.cpp \
KConnectEntry.h KConnectTypes.h KEInput.cpp KEInput.h KEMessage.cpp \
KEMessage.h KInputChildProcess.cpp KInputChildProcess.h \
KInteractiveConnect.cpp KInteractiveConnect.h KMessageEntry.cpp \
KMessageEntry.h KProcessConnect.cpp KProcessConnect.h KRSocket.cpp \
KRSocket.h KRemoteConnect.cpp KRemoteConnect.h namedlg.cpp namedlg.h \
networkdlg.cpp networkdlg.h aboutdlg.cpp aboutdlg.h aboutdlgdata.cpp \
msgdlg.cpp msgdlg.h 

install-data-local:
	$(mkinstalldirs) $(kde_htmldir)/en
	$(mkinstalldirs) $(kde_htmldir)/en/lskat
	$(INSTALL_DATA) index.html $(kde_htmldir)/en/lskat/index.html

	$(mkinstalldirs) $(kde_appsdir)/Games/
	$(INSTALL_DATA) lskat.desktop $(kde_appsdir)/Games/lskat.desktop
	$(mkinstalldirs) $(kde_icondir)/locolor/32x32/apps/
	$(INSTALL_DATA) lskat.xpm $(kde_icondir)/locolor/32x32/apps/lskat.xpm
	$(mkinstalldirs) $(kde_icondir)/locolor/16x16/apps/
	$(INSTALL_DATA) mini-lskat.xpm $(kde_icondir)/locolor/16x16/apps/lskat.xpm
	$(mkinstalldirs) $(kde_icondir)/locolor/48x48/apps/
	$(INSTALL_DATA) large-lskat.xpm $(kde_icondir)/locolor/48x48/apps/lskat.xpm

	

uninstall-local:
	-rm -rf $(kde_htmldir)/en/lskat
	-rm -f $(kde_appsdir)/Games/lskat.desktop
	-rm -f $(kde_icondir)/locolor/16x16/apps/lskat.xpm
	-rm -f $(kde_icondir)/locolor/32x32/apps/lskat.xpm
	-rm -f $(kde_icondir)/locolor/48x48/apps/lskat.xpm

####### kdevelop will overwrite this part!!! (end)############
# this 10 paths are KDE specific. Use them:
# kde_htmldir       Where your docs should go to. (contains lang subdirs)
# kde_appsdir       Where your application file (.kdelnk) should go to. 
# kde_icondir       Where your icon should go to.
# kde_minidir       Where your mini icon should go to.
# kde_datadir       Where you install application data. (Use a subdir)
# kde_locale        Where translation files should go to.(contains lang subdirs)
# kde_cgidir        Where cgi-bin executables should go to.
# kde_confdir       Where config files should go to.
# kde_mimedir       Where mimetypes should go to.
# kde_toolbardir    Where general toolbar icons should go to.
# kde_wallpaperdir  Where general wallpapers should go to.

# set the include path for X, qt and KDE
INCLUDES= $(all_includes)
# claim, which subdirectories you want to install
# you can add here more. This one gets installed 
bin_PROGRAMS = lskat 

lskat_METASOURCES = AUTO

# the library search path. 
lskat_LDFLAGS = $(all_libraries) $(KDE_RPATH)

# them while "make clean", use CLEANFILES
DISTCLEANFILES = $(lskat_METASOURCES)

# make messages.po. Move this one to ../po/ and "make merge" in po
# the -x is for skipping messages already translated in kdelibs
messages:
	$(XGETTEXT) -C -ki18n -x $(KDEDIR)/include/kde.pot `find . -name \*.h -o -name \*.cpp -o -name \*.cc` -o ../po/lskat.pot 



####### kdevelop will overwrite this part!!! (begin)##########
bin_PROGRAMS = lskat
lskat_SOURCES = msgdlg.cpp networkdlg.cpp namedlg.cpp KRemoteConnect.cpp KRSocket.cpp KProcessConnect.cpp KMessageEntry.cpp KInteractiveConnect.cpp KInputChildProcess.cpp KEMessage.cpp KEInput.cpp KConnectEntry.cpp KChildConnect.cpp lskatview.cpp lskatdoc.cpp lskat.cpp main.cpp 

lskat_LDADD   = $(LIB_KFILE) $(LIB_KDEGAMES)

INCLUDES = -I$(top_srcdir)/libkdegames $(all_includes)

METASOURCES = AUTO

lskat_LDFLAGS = $(all_libraries) $(KDE_RPATH)

rcdir = $(kde_datadir)/lskat
rc_DATA = lskatui.rc


messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/lskat.pot 


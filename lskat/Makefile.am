
####### kdevelop will overwrite this part!!! (begin)##########
bin_PROGRAMS = lskat
lskat_SOURCES = msgdlg.cpp aboutdlgdata.cpp aboutdlg.cpp networkdlg.cpp namedlg.cpp KRemoteConnect.cpp KRSocket.cpp KProcessConnect.cpp KMessageEntry.cpp KInteractiveConnect.cpp KInputChildProcess.cpp KEMessage.cpp KEInput.cpp KConnectEntry.cpp KChildConnect.cpp lskatview.cpp lskatdoc.cpp lskat.cpp main.cpp 
lskat_LDADD   = $(LIB_KFILE) -lkdegames

INCLUDES = -I$(top_srcdir)/libkdegames $(all_includes)

METASOURCES = AUTO

lskat_LDFLAGS = -L$(top_builddir)/libkdegames $(all_libraries) $(KDE_RPATH)



messages:
	$(XGETTEXT) `find . -name \*.h -o -name \*.cpp -o -name \*.cc` -o $(podir)/lskat.pot 


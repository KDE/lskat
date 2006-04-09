AM_CPPFLAGS = -DSRCDIR="\"/home/kde/kde35/KDE/kdegames/lskat\""

bin_PROGRAMS = lskat 
lskat_SOURCES = playerstatusform.ui playerstatuswidget.cpp  \
                namedialog.ui namedialogwidget.cpp \
                main.cpp mainwindow.cpp gameview.cpp canvasview.cpp  \
                deck.cpp abstractdisplay.cpp display_two.cpp player.cpp \
                cardsprite.cpp abstractinput.cpp mouseinput.cpp \
                abstractengine.cpp engine_two.cpp textsprite.cpp \
                display_intro.cpp aiinput.cpp rectanglesprite.cpp \
                pixmapsprite.cpp config_two.cpp

lskat_LDADD   = $(LIB_KFILE) $(LIB_KDEGAMES)
lskat_DEPENDENCIES = $(LIB_KDEGAMES_DEP)

INCLUDES = -I$(top_srcdir)/libkdegames -I$(top_srcdir)/libkdegames/kgame $(all_includes)
#INCLUDES = $(all_includes)

METASOURCES = AUTO

rcdir = $(kde_datadir)/lskat
rc_DATA = lskatui.rc

lskat_LDFLAGS = $(all_libraries) $(KDE_RPATH)
lskatproc_LDFLAGS = $(all_libraries) $(KDE_RPATH)

kde_kcfg_DATA = lskat.kcfg

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/lskat.pot 


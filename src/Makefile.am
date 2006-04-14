#AM_CPPFLAGS = -DSRCDIR="\"/home/kde/kde35/KDE/kdegames/lskat\""
AM_CPPFLAGS = -DSRCDIR="\"$(srcdir)/..\""

bin_PROGRAMS = lskat 
lskat_SOURCES = playerstatusform.ui playerstatuswidget.cpp  \
                namedialog.ui namedialogwidget.cpp \
                gameview.cpp canvasview.cpp  \
                mainwindow.cpp main.cpp deck.cpp config_two.cpp \
                abstractdisplay.cpp display_intro.cpp display_two.cpp \
                cardsprite.cpp pixmapsprite.cpp rectanglesprite.cpp textsprite.cpp \
                abstractengine.cpp engine_two.cpp  \
                player.cpp abstractinput.cpp mouseinput.cpp aiinput.cpp


lskat_LDADD   = $(LIB_KFILE) $(LIB_KDEGAMES)
lskat_DEPENDENCIES = $(LIB_KDEGAMES_DEP)

INCLUDES = -I$(top_srcdir)/libkdegames -I$(top_srcdir)/libkdegames/kgame $(all_includes)
#INCLUDES = $(all_includes)

METASOURCES = AUTO

rcdir = $(kde_datadir)/lskat
rc_DATA = lskatui.rc

lskat_LDFLAGS = $(all_libraries) $(KDE_RPATH)

#kde_kcfg_DATA = lskat.kcfg

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/lskat.pot 


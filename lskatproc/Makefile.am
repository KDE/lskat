bin_PROGRAMS = lskatproc

lskatproc_SOURCES = lskatproc.cpp KChildConnect.cpp KInputChildProcess.cpp KEMessage.cpp KMessageEntry.cpp main.cpp 

lskatproc_LDADD   = $(LIB_KFILE)

# set the include path for X, qt and KDE
INCLUDES= $(all_includes)
# claim, which subdirectories you want to install
# you can add here more. This one gets installed 
bin_PROGRAMS = lskatproc 

METASOURCES = AUTO

# the library search path. 
lskatproc_LDFLAGS = $(all_libraries) $(KDE_RPATH)


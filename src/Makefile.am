AM_CPPFLAGS = -I$(top_builddir) -I$(top_srcdir)
SUBDIRS = socket thread file

bin_PROGRAMS = rfswd
rfswd_SOURCES = main.cpp
rfswd_LDADD = -lpthread $(top_builddir)/src/socket/libsocket.a $(top_builddir)/src/thread/libthread.a $(top_builddir)/src/file/libfile.a

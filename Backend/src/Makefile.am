AM_CPPFLAGS = -Wall -g -I$(top_srcdir)/include -I$(top_srcdir)/../Contrib/cajun $(BOOST_CPPFLAGS)
AM_LDFLAGS = $(BOOST_THREAD_LDFLAGS) $(BOOST_SYSTEM_LDFLAGS) $(BOOST_PROGRAM_OPTIONS_LDFLAGS) $(BOOST_ASIO_LDFLAGS) $(BOOST_FILESYSTEM_LDFLAGS)
LIBS = $(BOOST_THREAD_LIBS) $(BOOST_SYSTEM_LIBS) $(BOOST_PROGRAM_OPTIONS_LIBS) $(BOOST_ASIO_LIBS) $(BOOST_FILESYSTEM_LIBS) -lssl -lpion-net -lpion-common -ltheron
bin_PROGRAMS = brainslug_backend
brainslug_backend_SOURCES = main.cpp
brainslug_backend_LDADD = ../lib/libbrainslug.la

check_LTLIBRARIES = libgtest.la
libgtest_la_SOURCES = ../googletest/googletest/src/gtest-all.cc
libgtest_la_CPPFLAGS = -I$(top_srcdir)/googletest/googletest/include -I$(top_srcdir)/googletest/googletest
libgtest_la_LDFLAGS = -pthread

bin_PROGRAMS = summary
summary_SOURCES = summary.cpp ../src/tsdb.cpp ../googletest/googletest/src/gtest_main.cc
summary_LDADD = libgtest.la
summary_LDFLAGS = -lpthread
summary_CPPFLAGS = -I$(top_srcdir)/googletest/googletest/include -I$(top_srcdir)/googletest/googletest -pthread

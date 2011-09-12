noinst_PROGRAMS = \
    Pt-Net-test

AM_CPPFLAGS = -I$(top_srcdir)/include

Pt_Net_test_SOURCES = Pt-Net-test.cpp TcpSocketTest.cpp

Pt_Net_test_LDADD = $(top_builddir)/src/Pt/libPt.la \
                    $(top_builddir)/src/Pt-System/libPt-System.la \
                    $(top_builddir)/src/Pt-Net/libPt-Net.la \
                    $(top_builddir)/src/Pt-Unit/libPt-Unit.la

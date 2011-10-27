AM_CPPFLAGS = -I$(top_srcdir)/include

noinst_PROGRAMS = Pt-XmlRpc-test RpcBenchClient RpcBenchServer

Pt_XmlRpc_test_SOURCES = \
	Pt-XmlRpc-test.cpp

Pt_XmlRpc_test_LDADD = $(top_builddir)/src/Pt/libPt.la \
	$(top_builddir)/src/Pt-XmlRpc/libPt-XmlRpc.la \
	$(top_builddir)/src/Pt-Unit/libPt-Unit.la

RpcBenchClient_SOURCES = \
	RpcBenchClient.cpp

RpcBenchClient_LDADD = $(top_builddir)/src/Pt/libPt.la \
	$(top_builddir)/src/Pt-XmlRpc/libPt-XmlRpc.la

RpcBenchServer_SOURCES = \
	RpcBenchServer.cpp

RpcBenchServer_LDADD = $(top_builddir)/src/Pt/libPt.la \
	$(top_builddir)/src/Pt-XmlRpc/libPt-XmlRpc.la

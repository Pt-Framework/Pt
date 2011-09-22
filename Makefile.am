ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src include

EXTRA_dist = \
	m4/acx_pthread.m4

utest: all
	src/Pt/tests/Pt-test
	src/Pt-System/tests/Pt-System-test
	src/Pt-Net/tests/Pt-Net-test
	src/Pt-Http/tests/Pt-Http-test
	src/Pt-Xml/tests/Pt-Xml-test
	src/Pt-XmlRpc/tests/Pt-XmlRpc-test

SUBDIRS = plugin media baseline fonts extrafonts testmediadir

EXTRA_DIST = $(wildcard *.avg) $(wildcard *.xml) $(wildcard *.py) $(wildcard *.svg)

pkgpyexec_PYTHON = testcase.py testapp.py
testdir = $(prefix)/share/python-libavg/test
test_DATA = $(EXTRA_DIST)

TESTS = Test.py

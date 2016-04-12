## Makefile.am -- Process this file with automake to produce Makefile.in

SUBDIRS = src test doc man

# The following removes the autotools stuff generated locally
# You'll need to add all the stuff to this target
distclean-local:
	find . -name '*.in' -exec rm -v "{}" \;
	rm -v config/!(README)
	rm -v aclocal.m4
	rm -rv autom4te.cache
	rm configure

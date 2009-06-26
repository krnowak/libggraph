SUBDIRS = src docs

DIST_SUBDIRS = $(SUBDIRS) m4

ACLOCAL_AMFLAGS = -I m4

DISTCHECK_CONFIGURE_FLAGS = --enable-gtk-doc

dist-hook:
	@if test -d "$(srcdir)/.git"; \
	then \
	        echo Creating ChangeLog && \
	        ( cd "$(top_srcdir)" && \
	          $(top_srcdir)/missing --run git log --date=short --pretty=format:"@@@@%cd  %an  <%ae>%n%n%s%n%n%b" | \
	          @PERL@ -ne 'if ( m/^\s*$$/ ) { if (!$$blank) { print; $$blank=1; } } else {$$blank=0; if (!s/^@@@@// && !/^\t/) { print "\t"; } print; }') > ChangeLog.tmp \
	        && mv -f ChangeLog.tmp $(top_distdir)/ChangeLog \
	        || ( rm -f ChangeLog.tmp ; \
	             echo Failed to generate ChangeLog >&2 ); \
	else \
	        echo A git clone is required to generate a ChangeLog >&2; \
	fi

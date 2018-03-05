1 i\
MAKEFLAGS=-s --no-print-directory
:0
/^#/ {
	d
}
/^[ 	]*$/ {
	d
}
/=/ {
	/^[^=]*[^R]CFLAGS/ b 8
	/^[^=]*CPPFLAGS/ b 8
	/^[^=]*CXXFLAGS/ b 8
	b 9
	:8
	s/ -Wl,/ -%l,/g
	s/ -Wa,/ -%a,/g
	s/ -Wp,/ -%p,/g
	/ -W/ s/ -W[^ ]*//g
	s/ -%l,/ -Wl,/g
	s/ -%a,/ -Wa,/g
	s/ -%p,/ -Wp,/g
       /^MULEC\(XX\)\?FLAGS/! b 9
       / -w/! s/=/= -w/
	:9
}
/^[^ 	].*:/ {
	/=/ b
	h
	s/\([^:][^:]*\):.*/\1/
	x
	:1
	/\\$/ {
		n
		b 1
	}
	:2
	n
	:3
	/^#/ {
		s/.*//
		N
		s/\n//g
		b 3
	}
	/^$/ b 0
	/^[^	]/ b 0
	x
	/^\n/ b 5
	/^install-strip$/ {
		s/.*//
		x
		b 0
	}
	/\.o$/ {
		i\
	echo "Compiling $(<F)"
		b 4
	}
	/\.obj$/ {
		i\
	echo "Compiling $(<F)"
		b 4
	}
	/\.a$/ {
		i\
	echo "Building $(@F)"
		b 4
	}
	/$(EXEEXT)$/ {
		i\
	echo "Linking $(@F)"
		b 4
	}
	/^\.po\.g\?mo$/ {
		i\
	echo "Generating translations for $(<F:.po=)"
		b 4
	}
	/\.cpp$/ {
		i\
	echo "Generating $(@F)"
		b 4
	}
	/\.$(OBJEXT)$/ {
		i\
	echo "Compiling $(<F)"
		b 4
	}
	/^$(DOMAIN)\.pot-update$/ {
		i\
	echo "Updating translation catalog template"
		b 4
	}
	/^$(POFILES)$/ {
		i\
	echo "Updating translation catalog for $(@F:.po=)"
		b 4
	}
	/^\.nop\.po-update$/ {
		i\
	echo "Updating translation catalog for $(<F:.nop=)"
		b 4
	}
	:4
	s/^/\
/
	:5
	/Makefile$/ {
		x
		/echo/! {
			/config\.status[ 	 ][ 	]*[^ 	][^ 	]*/ s/\(config\.status[ 	 ][ 	]*[^ 	][^ 	]*[^;\&\|]*\)\(.*\)/\1 denoiser\2/
		}
		x
	}
	x
	/^[ 	]*@*echo/ {
		x
		/maintainer-clean-generic/ b 6a
		b 6b
		:6a
		x
		b 6
		:6b
		x
		G
		h
		s/\(echo[^;][^;]*\).*/\1/
		/>/ {
			g
			s/[^\n]*//
			x
			s/\n.*//
			b 6
		}
		g
		s/[^\n]*//
		x
		s/\n.*//
		s/\(.*echo[^;][^;]*\)\(.*\)/\1>\/dev\/null\2/
		:6
	}
	/$(mkinstalldirs)/ {
		G
		h
		s/\($(mkinstalldirs)[^;][^;]*\).*/\1/
		/>/ {
			g
			s/[^\n]*//
			x
			s/\n.*//
			b 7
		}
		g
		s/[^\n]*//
		x
		s/\n.*//
		s/\(.*$(mkinstalldirs)[^;][^;]*\)\(.*\)/\1>\/dev\/null\2/
		:7
	}
	/$(GMSGFMT)/ s/--statistics//
	s/$(MSGMERGE)/\0 --silent/
	s/$(MSGMERGE_UPDATE)/\0 --silent/
	/^[ 	]*rm -f[^;]*$(DESTDIR)/ {
		x
		/uninstall/ {
			H
			g
			s/.*rm -f[^;]*$(DESTDIR)\([^;" ][^;" ]*\).*/	echo "Uninstalling \1"; \\/
			P
			g
			s/[^\n]*//
			x
			s/\n.*//
			b 2
		}
		x
	}
	/^[ 	]*$([^)]*INSTALL/ {
		x
		/install/ {
			H
			g
			s/.*$(DESTDIR)\([^;" ][^;" ]*\).*/	echo "Installing \1"; \\/
			P
			g
			s/[^\n]*//
			x
			s/\n.*//
			b 2
		}
		x
	}
	b 2
}

VERSION=0.3.8
PACKAGE=upgpverify
CLASS=mail
prefix=/usr/local
EXTRA_DIST=$(AUTOMAN) install.list Makefile.pre
CLEANFILES=check.out
DEFS=-DDJBLIBS


CC=gcc
L=./auto-link.sh
C=./auto-compile.sh
W=./auto-compilew.sh
GZIP=gzip --force --best
CLEANFILES+=distcheck.tmp distcheck.tmp2
PACKAGE_VERSION=$(PACKAGE)-$(VERSION)
INCLUDES=-DPACKAGE_VERSION=\"$(PACKAGE)-$(VERSION)\" \
 -DPACKAGE=\"$(PACKAGE)\" -DVERSION=\"$(VERSION)\"
NEED_LFS=0


#
#
#


#
#
libtai_a_SOURCES=taia_sub.c taia_approx.c taia_frac.c taia_less.c \
taia_now.c taia_uint.c tai_uint.c tai_sub.c taia_add.c tai_add.c 
libtai_a_OBJECTS=$(libtai_a_SOURCES:.c=.o)
 
libcdb_a_SOURCES=cdb.c cdb_hash.c cdb_make.c 
libcdb_a_OBJECTS=$(libcdb_a_SOURCES:.c=.o)
 
libuomisc_a_SOURCES=uogetopt.c bailout.c nonblock.c uogetopt_join.c 
libuomisc_a_OBJECTS=$(libuomisc_a_SOURCES:.c=.o)
 
libbyte_a_SOURCES=str_len.c byte_copy.c byte_cr.c byte_chr.c \
byte_diff.c byte_zero.c str_chr.c fmt_ulong.c case_diffs.c \
case_lwrtab.c case_diffb.c uint16_unpack.c uint16_pack.c \
uint32_unpack.c scan_ulong.c str_start.c case_lowerb.c str_diff.c \
str_rchr.c case_starts.c scan_long.c scan_sign.c uint32_pack.c 
libbyte_a_OBJECTS=$(libbyte_a_SOURCES:.c=.o)
 
libr822_a_SOURCES=r822_headhash.c r822_canon.c r822_address.c \
r822_lex.c 
libr822_a_OBJECTS=$(libr822_a_SOURCES:.c=.o)
 
libunix_a_SOURCES=error_str.c error.c iopause.c seek.c open_read.c \
open_append.c open_excl.c open_readwri.c alloc.c alloc_re.c \
stralloc_opys.c stralloc_opyb.c stralloc_eady.c gen_alloc_ady.c \
stralloc_free.c stralloc_pend.c gen_alloc_end.c stralloc_catb.c \
stralloc_plus.c gen_alloc_lus.c stralloc_cats.c stralloc_copy.c \
stralloc_cat.c buffer.c buffer_put.c buffer_get.c buffer_0.c \
buffer_1.c buffer_copy.c getln.c coe.c pathexec_run.c pathexec_env.c \
env_get.c wait_pid.c open_trunc.c fd_move.c fd_copy.c buffer_2.c 
libunix_a_OBJECTS=$(libunix_a_SOURCES:.c=.o)
 
libstrhash_a_SOURCES=strhash_crea.c strhash_ente.c strhash_look.c \
strhash_get.c strhash_hash.c strhash_dest.c 
libstrhash_a_OBJECTS=$(libstrhash_a_SOURCES:.c=.o)
 
LIBRARIES += libunix.a libbyte.a libuomisc.a libcdb.a libr822.a \
libtai.a libstrhash.a
 
TARGETS += upgpverify upgprules upgpverify.1 upgprules.1
 
CLEANFILES += fsync.o fsync.lib gettimeofday.lib inet_aton.o \
inet_aton.lib auto-lfs.* socket.lib socklen_t.h socklen_t.stamp \
ssize_t.h ssize_t.stamp auto-typesize.h typesize.stamp have_futimes.h \
have_utimes.h iopause.h utimes.o utimes.lib vfork.o vfork.lib \
upgpverify.1.t upgprules.1.t upgpverify.html.t upgprules.html.t \
auto-version.tmac install.html.t INSTALL.t pod2html-itemcache \
pod2html-dircache pod2htmi.x~~ pod2htmd.x~~ *.stamp
 
MAINTAINERCLEAN_FILES += upgpverify.1 upgprules.1 upgpverify.html \
upgprules.html install.html INSTALL
 

all:  auto-systype $(TARGETS) $(EXTRA_TARGETS)
 
have_futimes.h: have_func.sh auto-compile.sh auto-link.sh
	sh have_func.sh futimes "" "futimes(0,0)" "" "" >$@.t && mv $@.t \
$@
have_utimes.h:have_func.sh auto-compile.sh auto-link.sh
	sh have_func.sh utimes "" "utimes(0,0)" "" "" >$@.t && mv $@.t \
$@
iopause.h: iopause.sh auto-compile.sh auto-link.sh
	sh iopause.sh >iopause.t && mv iopause.t $@
upgpverify:  upgpverify.o sub_pgp5.o sub_gpg.o sub_pgp2.o mail_in.o \
mime_ct.o mime_qp.o mime_b64.o read_2_fd.o libuomisc.a libr822.a \
libstrhash.a libcdb.a libunix.a libuomisc.a libtai.a libbyte.a \
gettimeofday.lib auto-link.sh
	$L upgpverify  upgpverify.o sub_pgp5.o sub_gpg.o sub_pgp2.o mail_in.o \
mime_ct.o mime_qp.o mime_b64.o read_2_fd.o libuomisc.a libr822.a \
libstrhash.a libcdb.a libunix.a libuomisc.a libtai.a libbyte.a `cat \
gettimeofday.lib` 
upgprules:  upgprules.o libuomisc.a libr822.a libstrhash.a libcdb.a \
libunix.a libuomisc.a libtai.a libbyte.a gettimeofday.lib \
auto-link.sh
	$L upgprules  upgprules.o libuomisc.a libr822.a libstrhash.a libcdb.a \
libunix.a libuomisc.a libtai.a libbyte.a `cat gettimeofday.lib` \

libtai.a: $(libtai_a_OBJECTS) auto-makelib.sh
	./auto-makelib.sh $@ $(libtai_a_OBJECTS)
libcdb.a: $(libcdb_a_OBJECTS) auto-makelib.sh
	./auto-makelib.sh $@ $(libcdb_a_OBJECTS)
libuomisc.a: $(libuomisc_a_OBJECTS) auto-makelib.sh
	./auto-makelib.sh $@ $(libuomisc_a_OBJECTS)
libbyte.a: $(libbyte_a_OBJECTS) auto-makelib.sh
	./auto-makelib.sh $@ $(libbyte_a_OBJECTS)
libr822.a: $(libr822_a_OBJECTS) auto-makelib.sh
	./auto-makelib.sh $@ $(libr822_a_OBJECTS)
libunix.a: $(libunix_a_OBJECTS) auto-makelib.sh
	./auto-makelib.sh $@ $(libunix_a_OBJECTS)
libstrhash.a: $(libstrhash_a_OBJECTS) \
auto-makelib.sh
	./auto-makelib.sh $@ $(libstrhash_a_OBJECTS)
upgpverify.1: upgpverify.pod
	pod2man --section 1 --center upgpverify --release 0.3.8 upgpverify.pod \
>upgpverify.1.t && \
	mv upgpverify.1.t upgpverify.1
upgprules.1: upgprules.pod
	pod2man --section 1 --center upgpverify --release 0.3.8 upgprules.pod \
>upgprules.1.t && \
	mv upgprules.1.t upgprules.1
upgpverify.html: upgpverify.pod
	pod2html --htmlroot=. --podpath=. --outfile upgpverify.html.t \
--noindex --title "" upgpverify.pod >upgpverify.html.t && mv \
upgpverify.html.t upgpverify.html
upgprules.html: upgprules.pod
	pod2html --htmlroot=. --podpath=. --outfile upgprules.html.t --noindex \
--title "" upgprules.pod >upgprules.html.t && mv upgprules.html.t \
upgprules.html
install.html: install.0.in groff.tmac auto-version.tmac
	groff -mwww -Thtml -P-l groff.tmac auto-version.tmac install.0.in \
>install.html.t && mv install.html.t install.html
INSTALL: install.0.in groff.tmac auto-version.tmac
	groff -man -Tascii groff.tmac auto-version.tmac install.0.in |col -b \
>INSTALL.t && mv INSTALL.t INSTALL
auto-version.tmac: version.tmac.stamp
version.tmac.stamp: Makefile
	(echo .ds PA $(PACKAGE) ;\
	 echo .ds VE $(VERSION) ;\
	 echo .ds CL $(CLASS) ;\
	) >auto-version.tmac.t
	cmp auto-version.tmac auto-version.tmac.t \
	 || cp auto-version.tmac.t auto-version.tmac
	touch $@ && rm auto-version.tmac.t
html: upgpverify.html upgprules.html install.html INSTALL \
$(EXTRA_HTML)

 
cdb_make.o: cdb_make.c readwrite.h seek.h error.h alloc.h cdb.h uint32.h \
  typesize.h auto-typesize.h cdb_make.h buffer.h auto-compile.sh
	$C  -c $<
buffer_put.o: buffer_put.c buffer.h str.h byte.h error.h auto-compile.sh
	$C  -c $<
stralloc_copy.o: stralloc_copy.c stralloc.h gen_alloc.h auto-compilew.sh
	$W  -c $<
bailout.o: bailout.c error.h buffer.h bailout.h fmt.h auto-compilew.sh
	$W  -c $<
r822_headhash.o: r822_headhash.c r822.h stralloc.h gen_alloc.h strhash.h \
  uint32.h typesize.h auto-typesize.h uotime.h alloc.h byte.h case.h \
  scan.h str.h auto-compilew.sh
	$W  -c $<
str_len.o: str_len.c str.h auto-compilew.sh
	$W  -c $<
strhash_crea.o: strhash_crea.c strhashi.h uint32.h typesize.h \
  auto-typesize.h strhash.h auto-compilew.sh
	$W  -c $<
taia_less.o: taia_less.c taia.h tai.h uint64.h typesize.h auto-typesize.h auto-compilew.sh
	$W  -c $<
mime_qp.o: mime_qp.c stralloc.h gen_alloc.h mime.h auto-compilew.sh
	$W  -c $<
alloc_re.o: alloc_re.c alloc.h byte.h auto-compilew.sh
	$W  -c $<
getln.o: getln.c byte.h getln.h buffer.h stralloc.h gen_alloc.h auto-compilew.sh
	$W  -c $<
open_excl.o: open_excl.c open.h auto-compilew.sh
	$W  -c $<
stralloc_opyb.o: stralloc_opyb.c stralloc.h gen_alloc.h byte.h auto-compilew.sh
	$W  -c $<
uint32_unpack.o: uint32_unpack.c uint32.h typesize.h auto-typesize.h auto-compilew.sh
	$W  -c $<
case_diffb.o: case_diffb.c case.h auto-compilew.sh
	$W  -c $<
fmt_ulong.o: fmt_ulong.c gen_fmt.h fmt.h auto-compilew.sh
	$W  -c $<
buffer.o: buffer.c buffer.h auto-compile.sh
	$C  -c $<
mail_in.o: mail_in.c stralloc.h gen_alloc.h buffer.h getln.h bailout.h \
  error.h open.h readwrite.h close.h fmt.h mail_in.h r822.h strhash.h \
  uint32.h typesize.h auto-typesize.h uotime.h str.h seek.h nowarn.h auto-compilew.sh
	$W  -c $<
tai_sub.o: tai_sub.c tai.h uint64.h typesize.h auto-typesize.h auto-compilew.sh
	$W  -c $<
fd_move.o: fd_move.c fd.h auto-compile.sh
	$C  -c $<
strhash_get.o: strhash_get.c strhashi.h uint32.h typesize.h \
  auto-typesize.h auto-compilew.sh
	$W  -c $<
nonblock.o: nonblock.c nonblock.h auto-compilew.sh
	$W  -c $<
byte_copy.o: byte_copy.c byte.h auto-compilew.sh
	$W  -c $<
case_lwrtab.o: case_lwrtab.c case.h auto-compilew.sh
	$W  -c $<
buffer_2.o: buffer_2.c readwrite.h buffer.h auto-compile.sh
	$C  -c $<
uogetopt.o: uogetopt.c readwrite.h str.h env.h scan.h uogetopt.h \
  attributes.h auto-compilew.sh
	$W  -c $<
stralloc_opys.o: stralloc_opys.c str.h stralloc.h gen_alloc.h auto-compilew.sh
	$W  -c $<
stralloc_cat.o: stralloc_cat.c stralloc.h gen_alloc.h auto-compilew.sh
	$W  -c $<
case_diffs.o: case_diffs.c case.h auto-compilew.sh
	$W  -c $<
sub_pgp2.o: sub_pgp2.c error.h stralloc.h gen_alloc.h open.h nowarn.h \
  close.h str.h byte.h buffer.h readwrite.h env.h seek.h getln.h case.h \
  fmt.h fd.h pathexec.h bailout.h upgpverify.h auto-compilew.sh
	$W  -c $<
pathexec_run.o: pathexec_run.c error.h stralloc.h gen_alloc.h str.h env.h \
  pathexec.h auto-compilew.sh
	$W  -c $<
wait_pid.o: wait_pid.c error.h wait.h auto-compilew.sh
	$W  -c $<
iopause.o: iopause.c taia.h tai.h uint64.h typesize.h auto-typesize.h \
  iopause.h auto-compilew.sh
	$W  -c $<
taia_now.o: taia_now.c taia.h tai.h uint64.h typesize.h auto-typesize.h auto-compilew.sh
	$W  -c $<
case_starts.o: case_starts.c case.h auto-compilew.sh
	$W  -c $<
read_2_fd.o: read_2_fd.c iopause.h taia.h tai.h uint64.h typesize.h \
  auto-typesize.h stralloc.h gen_alloc.h upgpverify.h readwrite.h error.h \
  nonblock.h auto-compilew.sh
	$W  -c $<
taia_frac.o: taia_frac.c taia.h tai.h uint64.h typesize.h auto-typesize.h auto-compilew.sh
	$W  -c $<
uogetopt_join.o: uogetopt_join.c uogetopt.h auto-compilew.sh
	$W  -c $<
error_str.o: error_str.c error.h auto-compilew.sh
	$W  -c $<
r822_lex.o: r822_lex.c r822.h stralloc.h gen_alloc.h strhash.h uint32.h \
  typesize.h auto-typesize.h uotime.h byte.h auto-compilew.sh
	$W  -c $<
buffer_1.o: buffer_1.c readwrite.h buffer.h auto-compile.sh
	$C  -c $<
mime_b64.o: mime_b64.c stralloc.h gen_alloc.h mime.h auto-compilew.sh
	$W  -c $<
error.o: error.c error.h auto-compilew.sh
	$W  -c $<
byte_zero.o: byte_zero.c byte.h auto-compilew.sh
	$W  -c $<
cdb.o: cdb.c readwrite.h error.h seek.h byte.h cdb.h uint32.h typesize.h \
  auto-typesize.h auto-compile.sh
	$C  -c $<
fd_copy.o: fd_copy.c fd.h auto-compile.sh
	$C  -c $<
r822_canon.o: r822_canon.c r822.h stralloc.h gen_alloc.h strhash.h \
  uint32.h typesize.h auto-typesize.h uotime.h auto-compilew.sh
	$W  -c $<
byte_chr.o: byte_chr.c byte.h auto-compilew.sh
	$W  -c $<
upgpverify.o: upgpverify.c attributes.h bailout.h error.h open.h nowarn.h \
  close.h str.h alloc.h byte.h buffer.h readwrite.h fmt.h mime.h \
  stralloc.h gen_alloc.h mail_in.h r822.h strhash.h uint32.h typesize.h \
  auto-typesize.h uotime.h env.h seek.h getln.h case.h fd.h cdb.h \
  uogetopt.h wait.h pathexec.h upgpverify.h auto-compilew.sh
	$W  -c $<
taia_uint.o: taia_uint.c taia.h tai.h uint64.h typesize.h auto-typesize.h auto-compilew.sh
	$W  -c $<
open_append.o: open_append.c open.h auto-compilew.sh
	$W  -c $<
buffer_copy.o: buffer_copy.c buffer.h auto-compile.sh
	$C  -c $<
strhash_ente.o: strhash_ente.c strhashi.h uint32.h typesize.h \
  auto-typesize.h strhash.h auto-compilew.sh
	$W  -c $<
byte_cr.o: byte_cr.c byte.h auto-compilew.sh
	$W  -c $<
gen_alloc_ady.o: gen_alloc_ady.c alloc.h gen_alloci.h auto-compilew.sh
	$W  -c $<
str_start.o: str_start.c str.h auto-compilew.sh
	$W  -c $<
r822_address.o: r822_address.c r822.h stralloc.h gen_alloc.h strhash.h \
  uint32.h typesize.h auto-typesize.h uotime.h auto-compilew.sh
	$W  -c $<
scan_ulong.o: scan_ulong.c gen_scan.h scan.h case.h auto-compilew.sh
	$W  -c $<
cdb_hash.o: cdb_hash.c cdb.h uint32.h typesize.h auto-typesize.h auto-compile.sh
	$C  -c $<
scan_long.o: scan_long.c gen_scan.h scan.h case.h auto-compilew.sh
	$W  -c $<
sub_pgp5.o: sub_pgp5.c error.h stralloc.h gen_alloc.h open.h nowarn.h \
  close.h str.h byte.h buffer.h readwrite.h env.h seek.h getln.h case.h \
  fd.h fmt.h pathexec.h bailout.h upgpverify.h auto-compilew.sh
	$W  -c $<
alloc.o: alloc.c alloc.h error.h auto-compilew.sh
	$W  -c $<
coe.o: coe.c coe.h auto-compilew.sh
	$W  -c $<
scan_sign.o: scan_sign.c scan.h auto-compilew.sh
	$W  -c $<
strhash_look.o: strhash_look.c strhashi.h uint32.h typesize.h \
  auto-typesize.h strhash.h auto-compilew.sh
	$W  -c $<
buffer_get.o: buffer_get.c buffer.h byte.h error.h auto-compile.sh
	$C  -c $<
str_diff.o: str_diff.c str.h auto-compilew.sh
	$W  -c $<
uint32_pack.o: uint32_pack.c uint32.h typesize.h auto-typesize.h auto-compilew.sh
	$W  -c $<
buffer_0.o: buffer_0.c readwrite.h buffer.h auto-compile.sh
	$C  -c $<
stralloc_pend.o: stralloc_pend.c stralloc.h gen_alloc.h gen_alloci.h auto-compilew.sh
	$W  -c $<
stralloc_eady.o: stralloc_eady.c stralloc.h gen_alloc.h gen_alloci.h auto-compilew.sh
	$W  -c $<
tai_uint.o: tai_uint.c tai.h uint64.h typesize.h auto-typesize.h auto-compilew.sh
	$W  -c $<
stralloc_free.o: stralloc_free.c stralloc.h gen_alloc.h alloc.h auto-compilew.sh
	$W  -c $<
case_lowerb.o: case_lowerb.c case.h auto-compilew.sh
	$W  -c $<
strhash_hash.o: strhash_hash.c strhashi.h uint32.h typesize.h \
  auto-typesize.h strhash.h auto-compilew.sh
	$W  -c $<
seek.o: seek.c seek.h auto-compilew.sh
	$W  -c $<
taia_approx.o: taia_approx.c taia.h tai.h uint64.h typesize.h \
  auto-typesize.h auto-compilew.sh
	$W  -c $<
mime_ct.o: mime_ct.c mime.h stralloc.h gen_alloc.h str.h r822.h strhash.h \
  uint32.h typesize.h auto-typesize.h uotime.h bailout.h auto-compilew.sh
	$W  -c $<
pathexec_env.o: pathexec_env.c stralloc.h gen_alloc.h alloc.h str.h \
  byte.h env.h pathexec.h auto-compilew.sh
	$W  -c $<
byte_diff.o: byte_diff.c byte.h auto-compilew.sh
	$W  -c $<
uint16_unpack.o: uint16_unpack.c uint16.h typesize.h auto-typesize.h auto-compilew.sh
	$W  -c $<
taia_add.o: taia_add.c taia.h tai.h uint64.h typesize.h auto-typesize.h auto-compilew.sh
	$W  -c $<
stralloc_plus.o: stralloc_plus.c stralloc.h gen_alloc.h gen_alloci.h auto-compilew.sh
	$W  -c $<
open_readwri.o: open_readwri.c open.h auto-compilew.sh
	$W  -c $<
uint16_pack.o: uint16_pack.c uint16.h typesize.h auto-typesize.h auto-compilew.sh
	$W  -c $<
gen_alloc_end.o: gen_alloc_end.c gen_alloci.h byte.h auto-compilew.sh
	$W  -c $<
stralloc_catb.o: stralloc_catb.c stralloc.h gen_alloc.h byte.h auto-compilew.sh
	$W  -c $<
env_get.o: env_get.c env.h auto-compilew.sh
	$W  -c $<
str_chr.o: str_chr.c str.h auto-compilew.sh
	$W  -c $<
gen_alloc_lus.o: gen_alloc_lus.c gen_alloci.h auto-compilew.sh
	$W  -c $<
str_rchr.o: str_rchr.c str.h auto-compilew.sh
	$W  -c $<
upgprules.o: upgprules.c attributes.h stralloc.h gen_alloc.h buffer.h \
  str.h getln.h cdb_make.h uint32.h typesize.h auto-typesize.h scan.h \
  fmt.h uogetopt.h bailout.h open.h error.h readwrite.h alloc.h byte.h \
  nowarn.h close.h auto-compilew.sh
	$W  -c $<
sub_gpg.o: sub_gpg.c attributes.h error.h stralloc.h gen_alloc.h open.h \
  nowarn.h close.h str.h byte.h buffer.h fmt.h readwrite.h env.h seek.h \
  getln.h case.h fd.h pathexec.h bailout.h upgpverify.h auto-compilew.sh
	$W  -c $<
stralloc_cats.o: stralloc_cats.c str.h stralloc.h gen_alloc.h auto-compilew.sh
	$W  -c $<
tai_add.o: tai_add.c tai.h uint64.h typesize.h auto-typesize.h auto-compilew.sh
	$W  -c $<
strhash_dest.o: strhash_dest.c strhashi.h uint32.h typesize.h \
  auto-typesize.h strhash.h auto-compilew.sh
	$W  -c $<
open_read.o: open_read.c open.h auto-compilew.sh
	$W  -c $<
taia_sub.o: taia_sub.c taia.h tai.h uint64.h typesize.h auto-typesize.h auto-compilew.sh
	$W  -c $<
open_trunc.o: open_trunc.c open.h auto-compilew.sh
	$W  -c $<
auto-typesize.h: typesize.stamp
typesize.stamp: typesize.sh auto-compile.sh auto-link.sh
	CC="$(CC)" sh typesize.sh >typesize.t
	@if cmp typesize.t auto-typesize.h 2>/dev/null >/dev/null ; then : ; else mv typesize.t auto-typesize.h ; fi
	rm -f typesize.t
	touch $@

gettimeofday.lib: gettimeofday.sh auto-compile.sh auto-link.sh
	sh gettimeofday.sh >gettimeofday.t && mv gettimeofday.t $@

fsync.lib: fsync.sh auto-compile.sh auto-link.sh
	sh fsync.sh >fsync.t && mv fsync.t $@

.PRECIOUS: Makefile
remake: clean
	rm -f auto-compile.sh auto-link.sh auto-compilew.sh auto-makelib.sh
packageinfo: 
	@echo version="$(VERSION)"
	@echo package="$(PACKAGE)"
	@echo class="$(CLASS)"
auto-compile.sh: 
	@echo remaking $@
	@( echo "#! /bin/sh" ; \
	echo '# automatically generated, do not edit' ; \
	echo 'CFLAGS="$${CFLAGS:-$$_CFLAGS_OG}"' ; \
	echo 'CFLAGS="$${CFLAGS:--O2}"' ; \
	echo 'DEFS="$${DEFS:--${DEFS}}"' ; \
	echo 'INCLUDES="$${INCLUDES:--${INCLUDES}}"' ; \
	echo 'CC="$${CC:-gcc}"' ; \
	if test $(NEED_LFS) -gt 0 ; then echo 'CFLAGS="$$CFLAGS `head -1 auto-lfs.cflags`"' ; fi ; \
	echo 'exec $$CC $(DEFS) $(INCLUDES) $$CFLAGS $${1+"$$@"}' ; \
	) > auto-compile.t && chmod 755 auto-compile.t
	@mv auto-compile.t $@
auto-compilew.sh: 
	@echo remaking $@
	@( echo "#! /bin/sh" ; \
	echo '# automatically generated, do not edit' ; \
	echo 'CFLAGS="$${CFLAGS:-$$_CFLAGS_OWG}"' ; \
	echo 'CFLAGS="$${CFLAGS:--O2 -Wall}"' ; \
	echo 'DEFS="$${DEFS:--${DEFS}}"' ; \
	echo 'INCLUDES="$${INCLUDES:--${INCLUDES}}"' ; \
	echo 'CC="$${CC:-gcc}"' ; \
	if test $(NEED_LFS) -gt 0 ; then echo 'CFLAGS="$$CFLAGS `head -1 auto-lfs.cflags`"' ; fi ; \
	echo 'exec $$CC $(DEFS) $(INCLUDES) $$CFLAGS $${1+"$$@"}' ; \
	) > auto-compilew.t && chmod 755 auto-compilew.t
	@mv auto-compilew.t auto-compilew.sh
auto-link.sh:
	@echo remaking $@
	@( echo "#! /bin/sh" ; \
	echo '# automatically generated, do not edit' ; \
	echo 'LDFLAGS="$${LDFLAGS:-$$_CFLAGS_G}"' ; \
	echo 'LDFLAGS="$${LDFLAGS:--g}"' ; \
	echo 'CC="$${CC:-gcc}"' ; \
	if test $(NEED_LFS) -gt 0 ; then echo 'LDFLAGS="$$LDFLAGS `head -1 auto-lfs.ldflags`"' ; fi ; \
	echo 'exec $$CC $$LDFLAGS -o $${1+"$$@"} ' ; \
	) > auto-link.t && chmod 755 auto-link.t
	@mv auto-link.t auto-link.sh
auto-makelib.sh:
	@echo remaking $@
	@( echo "#! /bin/sh" ; \
	echo '# automatically generated, do not edit' ; \
	echo 'RANLIB="$${RANLIB:-ranlib}"' ; \
	echo 'AR="$${AR:-ar}"' ; \
	echo '$$AR cru $${1+"$$@"} ' ; \
	echo 'exec $$RANLIB $$1' ; \
	) > auto-makelib.t && chmod 755 auto-makelib.t
	@mv auto-makelib.t auto-makelib.sh
maintainer-clean: distclean
	rm -f Makefile $(MAINTAINERCLEANFILES)
distclean: clean
	rm -f $(DISTCLEANFILES) dummy.tmp # to avoid "no arguments" error
clean spotless: mostlyclean
	rm -f $(LIBRARIES) $(TARGETS) auto-link.sh auto-compile.sh auto-compilew.sh makelib.sh
	rm -rf deps
mostlyclean:
	rm -f *.o $(TARGETS) $(CLEANFILES)

NEWS-check:
	@if sed 1q NEWS | grep -e "$(VERSION)" > /dev/null; then :; else \
		echo "NEWS not updated; not releasing" 1>&2; \
		exit 1;                                   \
	fi
	@if grep -e "$(VERSION)" ChangeLog > /dev/null; then :; else \
		echo "ChangeLog not updated; not releasing" 1>&2; \
		exit 1;                                   \
	fi
bk-dist: NEWS-check
	bk tag `echo "Release-$(VERSION)" | sed 's/\./-/g'`
	$(MAKE) dist
gpg-dist: $(DIST)
	gpg -b --armor $(DIST)
DIST=$(PACKAGE)-$(VERSION).tar.gz

dist: $(DIST)
$(DIST): $(DISTFILES)
	rm -f $@
	package

force:	

auto-systype: auto-typesize.h $(EXTRA_SYSTYPE) $(PREMAKE_EXTRA_SYSTYPE)
	rm -f $@.t
	@echo  >>$@.t
	@echo "PACKAGE: $(PACKAGE) $(VERSION)" >>$@.t
	@echo "REALNAME: $(REALNAME)" >>$@.t
	@echo "uname-a:" `uname -a` >>$@.t
	@echo "uname-s:" `uname -s` >>$@.t
	@echo "uname-r:" `uname -r` >>$@.t
	@echo "uname-v:" `uname -v` >>$@.t
	@echo "uname-m:" `uname -m` >>$@.t
	cat auto-typesize.h >>$@.t
	if test "$(PREMAKE_EXTRA_SYSTYPE)x$(EXTRA_SYSTYPE)" = x ; then true ; else \
	  for i in $(PREMAKE_EXTRA_SYSTYPE) $(EXTRA_SYSTYPE) dummy ; do \
	    if test "x$$i" = "xdummy" ; then true ; else \
	      grep systype-info $$i 2>/dev/null >> $@.t || exit 0; \
	    fi \
	  done \
	fi
	mv $@.t $@

# that's not the "right" dependency, but that way it works even if
# uolib is absent.
LICENSES: Makefile.pre LICENSES.HEAD LICENSES.LOCAL
	( cat LICENSES.HEAD ; \
	for i in *.[ch] ; do \
		grep -h -s "^$$i:" /home/uwe/work/uolib/LICENSES.REPO \
		|| grep -h -s "^$$i:" ./LICENSES.LOCAL \
		|| grep -h -s "^$$i:" ./LICENSES \
		|| echo "$$i: unknown" ; \
	done ; \
	) >>$@.t
	mv $@.t $@

check.init: docheck.init
	env - PGP5="$$PGP5" PGP2="$$PGP2" GPG="$$GPG" PATH="`pwd`:$$PATH" sh docheck.init
check.base: check.init docheck.base check.exp.base
	env - PGP5="$$PGP5" PGP2="$$PGP2" GPG="$$GPG" PATH="`pwd`:$$PATH" sh docheck.base 2>&1 | cat -v | \
		sed 's/ *$$//' | tee check.out | diff -u check.exp.base -

check.gpginit: docheck.gpginit docheck.vars
	env - PGP5="$$PGP5" PGP2="$$PGP2" GPG="$$GPG" PATH="`pwd`:$$PATH" sh docheck.gpginit
check.gpg: check.base check.gpginit docheck.gpg check.exp.gpg docheck.sub
	env - PGP5="$$PGP5" PGP2="$$PGP2" GPG="$$GPG" PATH="`pwd`:$$PATH" sh docheck.gpg 2>&1 | cat -v | \
		sed 's/ *$$//' | tee check.out | diff -u check.exp.gpg -

check.pgp5init: docheck.pgp5init docheck.vars
	env - PGP5="$$PGP5" PGP2="$$PGP2" GPG="$$GPG" PATH="`pwd`:$$PATH" sh docheck.pgp5init
check.pgp5: check.base check.pgp5init docheck.pgp5 check.exp.pgp5 docheck.sub
	env - PGP5="$$PGP5" PGP2="$$PGP2" GPG="$$GPG" PATH="`pwd`:$$PATH" sh docheck.pgp5 2>&1 | cat -v | \
		sed 's/ *$$//' | tee check.out | diff -u check.exp.pgp5 -

check.pgp2init: docheck.pgp2init docheck.vars
	env - PGP5="$$PGP5" PGP2="$$PGP2" GPG="$$GPG" PATH="`pwd`:$$PATH" sh docheck.pgp2init
check.pgp2: check.base check.pgp2init docheck.pgp2 check.exp.pgp2 docheck.sub
	env - PGP5="$$PGP5" PGP2="$$PGP2" GPG="$$GPG" PATH="`pwd`:$$PATH" sh docheck.pgp2 2>&1 | cat -v | \
		sed 's/ *$$//' | tee check.out | diff -u check.exp.pgp2 -

check.clean:
	rm -rf tmp.check
check: check.init check.base check.gpg check.pgp5 check.pgp2 check.clean

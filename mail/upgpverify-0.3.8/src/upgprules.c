/* somewhat based on cdbmake.c by djb at pobox.com, but the bugs are my own */

#include "attributes.h"
#include "stralloc.h"
#include "buffer.h"
#include "str.h"
#include "getln.h"
#include "cdb_make.h"
#include "scan.h"
#include "fmt.h"
#include "uogetopt.h"
#include "bailout.h"
#include "open.h"
#include "error.h"
#include "readwrite.h"
#include "alloc.h"
#include "byte.h"
#include "nowarn.h"
#include "close.h"


static struct cdb_make *cdbm;
static buffer b;
static char bspace[2048];
static void usage (void) attribute_noreturn;
static void die_bad (stralloc *) attribute_noreturn;

static void 
usage (void)
{
	xbailout(100, 0, "Give --help option for usage information",0,0,0);
}
static void 
die_bad (stralloc *s)
{
	if (!stralloc_0(s)) oom();
	xbailout(100,0, "bad input format: ", s->s,0,0);
}

static void
add_one(char *key, uint32 keylen, char *data, uint32 datalen)
{
	if (-1==cdb_make_add(cdbm,key,keylen,data,datalen))
		bailout(errno,"cdb_make_add",0,0,0);

}

static uogetopt_t myopts[]={    {0,0,0,0,0,0,0} };

int 
main (int argc, char **argv)
{
	char *fntemp;
	char *fn;
	int fd;
	static stralloc sa;

	unsigned int l;

	l=str_rchr(argv[0],'/');
	if (argv[0][l] && argv[0][l+1])
		flag_bailout_log_name=argv[0]+l+1;
	else
		flag_bailout_log_name=argv[0];

	uogo_posixmode=1;
	uogetopt(flag_bailout_log_name,PACKAGE,VERSION,
		&argc, argv, uogetopt_out,
		"usage: upgprules cdb tmp [configfile]",
		myopts,
		"  Input will be read from stdin unless a configuration file is given\n"
		"  on the command line.\n"
		"  The cdb- and tmp-files have to be located on the same filesystem.\n"
		"  Report bugs to uwe-upgpverify@bulkmail.ohse.de"
		);

	fn = argv[1];
	if (!fn)
		usage ();

	fntemp = argv[2];
	if (!fntemp)
		usage ();
	if (argc != 3 && argc != 4)
		usage ();

	if (argc==4) {
		fd=open_read(argv[3]);
		if (fd==-1) xbailout(111,errno,"failed to open input file",0,0,0);
		buffer_init(&b,(buffer_op_t) read, fd,bspace,sizeof(bspace));
	} else
		buffer_init(&b,(buffer_op_t) read, 0,bspace,sizeof(bspace));

	fd = open_trunc_mode (fntemp, 0644);
	if (fd == -1) 
		xbailout(111, errno, "failed to open_trunc ", fntemp,0,0);

	cdbm=(struct cdb_make *) alloc(sizeof(*cdbm));
	if (!cdbm) oom();
	byte_zero((char *)cdbm,sizeof(cdbm));
	cdb_make_start (cdbm,fd);

	while (1) {
		unsigned int colon;
		static stralloc id;
		static stralloc data;
		unsigned int off;
		int gotlf;
		sa.len=0;
		if (-1==getln(&b,&sa,&gotlf,'\n'))
			xbailout(111,errno,"failed to read",0,0,0);
		if (!sa.len) break;
		if (!gotlf)
			xbailout(111,0,"premature EOF",0,0,0);
		if (sa.s[0]=='#') continue;
		if (sa.s[0]=='\n') continue;
		while (sa.len) {
			unsigned char ch;
			ch = sa.s[sa.len - 1];
			if (ch != '\n' && ch != ' ' && ch != '\t') 
				break;
			sa.len--;
		}
		if (sa.len<8) 
			die_bad(&sa);
		if (sa.s[1]!='*')
			die_bad(&sa);
#define ALLOWED "ufsla"
		if (!ALLOWED[str_chr(ALLOWED,sa.s[0])])
			die_bad(&sa);
		colon = byte_chr(sa.s,sa.len,':');
		if (colon == sa.len) continue;
		if (!stralloc_copyb(&id,sa.s,colon)) oom();
		if (!stralloc_copys(&data,"")) oom();
		off=colon+1;
#define REST (sa.len-off)
#define STR (sa.s+off)
		if ((REST >= 4) && byte_equal(STR,4,"deny")) {
			if (!stralloc_catb(&data,"D",2)) oom();
			off+=4;
		} else if ((REST >= 5) && byte_equal(STR,5,"allow")) {
			off+=5;
		} else
			die_bad(&sa);
		while (REST)
			switch(*STR) {
				unsigned int i;
				unsigned int ch;
			case ',':
				i = byte_chr(STR,REST,'=');
				if (i == REST) die_bad(&sa);
				if (!stralloc_catb(&data,"+",1)) oom();
				if (!stralloc_catb(&data,STR + 1,i)) oom();
				off+=i+1;
				if (!REST) die_bad(&sa);
				ch = *STR;
				off++;
				i = byte_chr(STR,REST,ch);
				if (i == REST) die_bad(&sa);
				if (!stralloc_catb(&data,STR,i)) oom();
				if (!stralloc_0(&data)) oom();
				off+=i+1;
				break;
			default:
				die_bad(&sa);
			}
		add_one(id.s,id.len,data.s,data.len);
	}

	if (-1==cdb_make_finish(cdbm)) bailout(errno,"cdb_make_finish",0,0,0);
	if (fsync (fd) == -1) bailout (errno, "fsync",0,0,0);
	if (close (fd) == -1) bailout (errno, "close",0,0,0);
	if (rename (fntemp, fn))
		bailout (errno, "rename ", fntemp, " to ", fn);
	return (0);
}



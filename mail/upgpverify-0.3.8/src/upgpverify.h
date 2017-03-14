struct check_result {
	stralloc user;
	stralloc keyid;
	stralloc long_keyid;
	stralloc fingerprint;
	stralloc output;
	stralloc enc_to;
};
struct check_param {
	const char *xprog;
	const char *secring;
	const char *pubring;
	stralloc micalg;
	stralloc message;
	stralloc signature;
	int passphrase_fd;
	int need_decryption;
	int allow_key_retrieve;
};
typedef int (*check_func)(struct check_param *, struct check_result *);

int check_pgp5_sig(struct check_param *, struct check_result *);
int check_pgp2_sig(struct check_param *, struct check_result *);
int check_gpg_sig(struct check_param *, struct check_result *);

int read_2_fd(int fd1, stralloc *s1, unsigned int max1, 
	int fd2, stralloc *s2, unsigned int max2);

void av_add(const char *s);
void av_add_copy(const char *s);
char **av_get(void);

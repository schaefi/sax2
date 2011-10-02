/**************
FILE          : vncp.c
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : Program to create a password string used for
              : VNC connections. The code is based on the source
              : of tightvnc
              :
STATUS        : Status: Development
**************/
#include "vncp.h"

//=====================================
// Main
//-------------------------------------
int main(int argc, char *argv[]) {
	char passwd1[9];
	char passwd2[9];
	char *passwd2_ptr;
	char passwdFile[256];

	strcpy (passwdFile, "-");
	passwd2_ptr = NULL;

	/* Read passwords from stdin */
	if (!read_password(passwd1)) {
		fprintf(stderr, "Could not read password\n");
		exit(1);
	}
	if (read_password(passwd2)) {
		passwd2_ptr = passwd2;
	}

	/* Actually write the passwords. */
	if (!vncEncryptAndStorePasswd2(passwd1, passwd2_ptr, passwdFile)) {
		memset(passwd1, 0, strlen(passwd1));
		memset(passwd2, 0, strlen(passwd2));
		fprintf(stderr, "Cannot write password file %s\n", passwdFile);
		exit(1);
	}

	/* Zero the memory. */
	memset(passwd1, 0, strlen(passwd1));
	memset(passwd2, 0, strlen(passwd2));
	return 0;
}

//=====================================
// read_password
//-------------------------------------
static int read_password (char *result) {
	// .../
	// Read a password from stdin. The password is terminated either by an
	// end of line, or by the end of stdin data. Return 1 on success, 0 on
	// error. On success, the password will be stored in the specified
	// 9-byte buffer
	// ----
	char passwd[256];
	char *ptr;

	/* Try to read the password. */
	if (fgets(passwd, 256, stdin) == NULL) {
		return 0;
	}

	/* Remove the newline if present. */
	ptr = strchr(passwd, '\n');
	if (ptr != NULL) {
		*ptr = '\0';
	}

	/* Truncate if necessary. */
	if (strlen(passwd) > 8) {
		memset(passwd + 8, 0, strlen(passwd) - 8);
		fprintf(stderr, "Warning: password truncated to the length of 8.\n");
	}

	/* Save the password and zero our copies. */
	strcpy(result, passwd);
	memset(passwd, 0, strlen(passwd));

	return 1;
}

//=====================================
// vncEncryptAndStorePasswd2
//-------------------------------------
int vncEncryptAndStorePasswd2 (char *passwd, char *passwdViewOnly, char *fname) {
    FILE *fp;
    int bytesToWrite, bytesWrote;
    unsigned char encryptedPasswd[16] = {
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0
    };

    if (strcmp(fname, "-") != 0) {
      fp = fopen(fname, "w");
      if (fp == NULL) {
    return 0;
      }
      chmod(fname, S_IRUSR|S_IWUSR);
    } else {
      fp = stdout;
    }

    strncpy((char*)encryptedPasswd, passwd, 8);
    if (passwdViewOnly != NULL)
    strncpy((char*)encryptedPasswd + 8, passwdViewOnly, 8);

    /* Do encryption in-place - this way we overwrite our copies of
       plaintext passwords. */

    deskey(s_fixedkey, EN0);
    des(encryptedPasswd, encryptedPasswd);
    if (passwdViewOnly != NULL)
    des(encryptedPasswd + 8, encryptedPasswd + 8);

    bytesToWrite = (passwdViewOnly == NULL) ? 8 : 16;
    bytesWrote = fwrite(encryptedPasswd, 1, bytesToWrite, fp);

    if (fp != stdout) {
      fclose(fp);
    }
    return (bytesWrote == bytesToWrite);
}

//=====================================
// deskey
//-------------------------------------
void deskey(unsigned char* key, int edf) {
	// .../
	// Thanks to James Gillogly & Phil Karn!
	// ----
	register int i, j, l, m, n;
	unsigned char pc1m[56], pcr[56];
	unsigned long kn[32];
    
	for ( j = 0; j < 56; j++ ) {
		l = pc1[j];
		m = l & 07;
		pc1m[j] = (key[l >> 3] & bytebit[m]) ? 1 : 0;
	}
	for( i = 0; i < 16; i++ ) {
		if ( edf == DE1 ) {
			m = (15 - i) << 1;
		} else {
			m = i << 1; 
		}
		n = m + 1;
		kn[m] = kn[n] = 0L;
		for( j = 0; j < 28; j++ ) {
			l = j + totrot[i];
			if( l < 28 ) {
				pcr[j] = pc1m[l];
			} else {
				pcr[j] = pc1m[l - 28];
			}
		}
		for( j = 28; j < 56; j++ ) {
			l = j + totrot[i];
			if( l < 56 ) {
				pcr[j] = pc1m[l];
			} else {
				pcr[j] = pc1m[l - 28];
			}
		}
		for( j = 0; j < 24; j++ ) {
			if( pcr[pc2[j]] ) {
				kn[m] |= bigbyte[j];
			}
            if( pcr[pc2[j+24]] ) {
				kn[n] |= bigbyte[j];
			}
		}
	}
	cookey(kn);
}

//=====================================
// cookey
//-------------------------------------
static void cookey (register unsigned long* raw1) {
	register unsigned long *cook, *raw0;
	unsigned long dough[32];
	register int i;

	cook = dough;
	for( i = 0; i < 16; i++, raw1++ ) {
		raw0 = raw1++;
		*cook    = (*raw0 & 0x00fc0000L) << 6;
		*cook   |= (*raw0 & 0x00000fc0L) << 10;
		*cook   |= (*raw1 & 0x00fc0000L) >> 10;
		*cook++ |= (*raw1 & 0x00000fc0L) >> 6;
		*cook    = (*raw0 & 0x0003f000L) << 12;
		*cook   |= (*raw0 & 0x0000003fL) << 16;
		*cook   |= (*raw1 & 0x0003f000L) >> 4;
		*cook++ |= (*raw1 & 0x0000003fL);
	}
	usekey(dough);
}

//=====================================
// usekey
//-------------------------------------
void usekey(register unsigned long* from) {
	register unsigned long *to, *endp;

	to = KnL, endp = &KnL[32];
	while ( to < endp ) {
		*to++ = *from++;
	}
}

//=====================================
// des
//-------------------------------------
void des(unsigned char* inblock, unsigned char* outblock) {
    unsigned long work[2];

    scrunch(inblock, work);
    desfunc(work, KnL);
    unscrun(work, outblock);
}

//=====================================
// scrunch
//-------------------------------------
static void scrunch(register unsigned char* outof, register unsigned long* into) {
	*into    = (*outof++ & 0xffL) << 24;
	*into   |= (*outof++ & 0xffL) << 16;
	*into   |= (*outof++ & 0xffL) << 8;
	*into++ |= (*outof++ & 0xffL);
	*into    = (*outof++ & 0xffL) << 24;
	*into   |= (*outof++ & 0xffL) << 16;
	*into   |= (*outof++ & 0xffL) << 8;
	*into   |= (*outof   & 0xffL);
}

//=====================================
// unscrun
//-------------------------------------
static void unscrun(register unsigned long* outof, register unsigned char* into) {
	*into++ = (*outof >> 24) & 0xffL;
	*into++ = (*outof >> 16) & 0xffL;
	*into++ = (*outof >>  8) & 0xffL;
	*into++ =  *outof++  & 0xffL;
	*into++ = (*outof >> 24) & 0xffL;
	*into++ = (*outof >> 16) & 0xffL;
	*into++ = (*outof >>  8) & 0xffL;
	*into   =  *outof    & 0xffL;
}

//=====================================
// desfunc
//-------------------------------------
static void desfunc(register unsigned long* block, register unsigned long* keys) {
	register unsigned long fval, work, right, leftt;
	register int round;
    
	leftt = block[0];
	right = block[1];
	work = ((leftt >> 4) ^ right) & 0x0f0f0f0fL;
	right ^= work;
	leftt ^= (work << 4);
	work = ((leftt >> 16) ^ right) & 0x0000ffffL;
	right ^= work;
	leftt ^= (work << 16);
	work = ((right >> 2) ^ leftt) & 0x33333333L;
	leftt ^= work;
	right ^= (work << 2);
	work = ((right >> 8) ^ leftt) & 0x00ff00ffL;
	leftt ^= work;
	right ^= (work << 8);
	right = ((right << 1) | ((right >> 31) & 1L)) & 0xffffffffL;
	work = (leftt ^ right) & 0xaaaaaaaaL;
	leftt ^= work;
	right ^= work;
	leftt = ((leftt << 1) | ((leftt >> 31) & 1L)) & 0xffffffffL;

	for( round = 0; round < 8; round++ ) {
		work  = (right << 28) | (right >> 4);
		work ^= *keys++;
		fval  = SP7[ work        & 0x3fL];
		fval |= SP5[(work >>  8) & 0x3fL];
		fval |= SP3[(work >> 16) & 0x3fL];
		fval |= SP1[(work >> 24) & 0x3fL];
		work  = right ^ *keys++;
		fval |= SP8[ work        & 0x3fL];
		fval |= SP6[(work >>  8) & 0x3fL];
		fval |= SP4[(work >> 16) & 0x3fL];
		fval |= SP2[(work >> 24) & 0x3fL];
		leftt ^= fval;
		work  = (leftt << 28) | (leftt >> 4);
		work ^= *keys++;
		fval  = SP7[ work        & 0x3fL];
		fval |= SP5[(work >>  8) & 0x3fL];
		fval |= SP3[(work >> 16) & 0x3fL];
		fval |= SP1[(work >> 24) & 0x3fL];
		work  = leftt ^ *keys++;
		fval |= SP8[ work        & 0x3fL];
		fval |= SP6[(work >>  8) & 0x3fL];
		fval |= SP4[(work >> 16) & 0x3fL];
		fval |= SP2[(work >> 24) & 0x3fL];
		right ^= fval;
	}

	right = (right << 31) | (right >> 1);
	work = (leftt ^ right) & 0xaaaaaaaaL;
	leftt ^= work;
	right ^= work;
	leftt = (leftt << 31) | (leftt >> 1);
	work = ((leftt >> 8) ^ right) & 0x00ff00ffL;
	right ^= work;
	leftt ^= (work << 8);
	work = ((leftt >> 2) ^ right) & 0x33333333L;
	right ^= work;
	leftt ^= (work << 2);
	work = ((right >> 16) ^ leftt) & 0x0000ffffL;
	leftt ^= work;
	right ^= (work << 16);
	work = ((right >> 4) ^ leftt) & 0x0f0f0f0fL;
	leftt ^= work;
	right ^= (work << 4);
	*block++ = right;
	*block = leftt;
}

/**************
FILE          : remote.c
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHORS       : Thomas Fehr <fehr@suse.de>
              : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : remote will check the local adresses against
              : the remote adresses. If one local adress equals
              : one remote adress we are logged in locally
              : 
              :
STATUS        : Status: Up-to-date
**************/
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>

#include "ifaddrs.h"
#include "remote.h"

//=============================================
// checkRemote
//---------------------------------------------
int checkRemote( const char* addr ) {
	int remote = 0;
	struct addrinfo *res;
	struct ifaddrs *local_addr;
	if ( getifaddrs( &local_addr ) == 0 ) {
	struct ifaddrs *myad = local_addr;
	if ( getaddrinfo( addr, NULL, NULL, &res ) == 0 ) {
	    struct addrinfo * rmad = res;
		remote = 1;
		while ( rmad != NULL && remote==1 ) {
		if ( rmad->ai_family==2 || rmad->ai_family==10 ) {
			myad = local_addr;
			while ( myad ) {
			if ( myad->ifa_addr->sa_family==rmad->ai_family ) {
			if ( rmad->ai_family==2 ) {
				struct sockaddr_in *in1, *in2;
				in1 = (struct sockaddr_in *)myad->ifa_addr;
				in2 = (struct sockaddr_in *)rmad->ai_addr;
				if (in1->sin_addr.s_addr == in2->sin_addr.s_addr) {
					remote = 0;
				}
			} else if ( rmad->ai_family==10 ) {
				struct sockaddr_in6 *in1, *in2;
				in1 = (struct sockaddr_in6 *)myad->ifa_addr;
				in2 = (struct sockaddr_in6 *)rmad->ai_addr;
				if ( 
				in1->sin6_addr.s6_addr32[0] == in2->sin6_addr.s6_addr32[0] &&
				in1->sin6_addr.s6_addr32[1] == in2->sin6_addr.s6_addr32[1] &&
				in1->sin6_addr.s6_addr32[2] == in2->sin6_addr.s6_addr32[2] &&
				in1->sin6_addr.s6_addr32[3] == in2->sin6_addr.s6_addr32[3] 
				) {
					remote = 0;
				}
			}
			}
			myad = myad->ifa_next;
			}
		}
		rmad = rmad->ai_next;
		}
		freeaddrinfo( res );
	}
	freeifaddrs( local_addr );
	}
	return( remote );
}

//=====================================
// remoteDisplay()
//-------------------------------------
int remoteDisplay (char* display) {
	char* host = NULL;
	char* dpy  = NULL;
	dpy = (char*) malloc (
		sizeof(char) * 255
	);
	if (! display) {
		dpy = getenv ("DISPLAY");
	} else {
		sprintf (dpy,"%s",display);
	}
	if (! dpy) {
		return (0);
	}
	if (dpy[0] == ':') {
		return (0);
	}
	host = (char*)malloc (sizeof(char) * 255);
	host = strtok (dpy,":");
	return (
		checkRemote (host)
	);
}

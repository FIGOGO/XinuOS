#include <xinu.h>
#include <stdio.h>
#include <string.h>
#include <future.h>

shellcmd xsh_futureserver(int nargs, char *args[])
{
	int32	retval;			/* return value from sys calls	*/
	uint32	localip;		/* local IP address		*/
	uint32	remip;			/* remote sender's IP address	*/
	uint16	remport;		/* remote sender's UDP port	*/
	char	buff[1500];		/* buffer for incoming reply	*/
	int32	msglen;			/* length of outgoing message	*/
	int32	slot;			/* slot in UDP table 		*/
	uint16	echoserverport= 7;	/* port number for UDP echo	*/

	/* For argument '--help', emit a help message	*/

	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Use: %s\n\n", args[0]);
		printf("Description:\n");
		printf("\tBecome a UDP future server\n");
		printf("Options:\n");
		printf("\t--help\t display this help and exit\n");
		return 0;
	}

	/* Check for valid IP address argument */

	if (nargs != 1) {
		fprintf(stderr, "%s: no arguments expected\n", args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",
				args[0]);
		return 1;
	}

	localip = getlocalip();
	if (localip == SYSERR) {
		fprintf(stderr,
			"%s: could not obtain a local IP address\n",
			args[0]);
		return 1;
	}

	/* register local UDP port */

	slot = udp_register(0, 0, echoserverport);
	if (slot == SYSERR) {
		fprintf(stderr, "%s: could not reserve UDP port %d\n",
				args[0], echoserverport);
		return 1;
	}

  int32 value;
  future *f = future_alloc(FUTURE_EXCLUSIVE);
  resume( create(future_get, 1024, 20, "future_get", 2, f, &value) );
  printf ("I want a value for futre %d\n", f);
	/* Do forever: read an incoming datagram and send it back */
	while (TRUE) {
		retval = udp_recvaddr(slot, &remip, &remport, buff,
						sizeof(buff), 600000);

		if (retval == TIMEOUT) {
			continue;
		} else if (retval == SYSERR) {
			fprintf(stderr, "%s: error receiving UDP\n",
				args[0]);
			return 1;
		}
		msglen = retval;
    int val = atoi(buff);
    future_set(f, &val);
		printf("Future server received: %s",buff);
		retval = udp_sendto(slot, remip, remport, buff, msglen);
		if (retval == SYSERR) {
			fprintf(stderr, "%s: udp_sendto failed\n",
				args[0]);
			return 1;
		}
	}
	return 0;
}

#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

#include "famouso.h"
#include "famouso_binding.h"


static int cd = -1;  /* connection file descriptor */
static int is_connected = 0;


/*
 * Routines for flattening host byte order
 * 64 and 32 bit values to a network byte order stream
 * and vice versa.
 */

/* 64 bit value <- network byte order stream. */
static inline u_int64_t make64n(void *ptr)
{
	unsigned char *sp = ptr;

	return (((u_int64_t)sp[0]) << 56) | (((u_int64_t)sp[1]) << 48) |
		(((u_int64_t)sp[2]) << 40) | (((u_int64_t)sp[3]) << 32) |
		(((u_int64_t)sp[4]) << 24) | (((u_int64_t)sp[5]) << 16) |
		(((u_int64_t)sp[6]) << 8) | ((u_int64_t)sp[7]);
}


/* 32 bit value <- network byte order stream. */
static inline u_int32_t make32n(void *ptr)
{
	unsigned char *sp = ptr;

	return (((u_int32_t)sp[0])<<24) | (((u_int32_t)sp[1])<<16) |
		(((u_int32_t)sp[2])<<8) | ((u_int32_t)sp[3]);
}


/* Network byte order stream <- 64 bit value. */
static inline void maken64(void *ptr, u_int64_t val)
{
	unsigned char *sp = ptr;

	sp[0] = (unsigned char)((val >> 56) & 0xff);
	sp[1] = (unsigned char)((val >> 48) & 0xff);
	sp[2] = (unsigned char)((val >> 40) & 0xff);
	sp[3] = (unsigned char)((val >> 32) & 0xff);
	sp[4] = (unsigned char)((val >> 24) & 0xff);
	sp[5] = (unsigned char)((val >> 16) & 0xff);
	sp[6] = (unsigned char)((val >> 8) & 0xff);
	sp[7] = (unsigned char)(val & 0xff);
}


/* Network byte order stream <- 32 bit value. */
static inline void maken32(void *ptr, u_int32_t val)
{
	unsigned char *sp = ptr;

	sp[0] = (unsigned char)((val>>24)&0xff);
	sp[1] = (unsigned char)((val>>16)&0xff);
	sp[2] = (unsigned char)((val>>8)&0xff);
	sp[3] = (unsigned char)(val&0xff);
}


/* Report an error to stderr, but only once. */
static void error(char *txt)
{
	static int already_reported_an_error = 0;

	if(!already_reported_an_error) {
		already_reported_an_error = 1;
		perror(txt);
	}
}


/*
 * The following internal functions deal with the connection to the event channel handler.
 */
static int do_create_socket(void)
{
	if(cd>0)
		return 0;

	if((cd = socket(AF_INET, SOCK_STREAM, 0))<0) {
		error("[pubsub client] can't create a unix-domain socket");
		return 0;
	}

	return 1;
}

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

static int do_connect(void)
{

#define SERV_TCP_PORT   5005
	struct sockaddr_in serv_addr;
	if(is_connected)
		return 0;

	if(cd < 0)
		if(!do_create_socket())
			return 0;
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family   = AF_INET;
	serv_addr.sin_addr.s_addr  = inet_addr("127.0.0.1");
//	serv_addr.sin_addr.s_addr  = inet_addr("192.168.0.122");
	serv_addr.sin_port   = htons(SERV_TCP_PORT);


	if(connect(cd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))){
//connect(cd, (struct sockaddr *)&sau, sizeof(sau))) {
		error("[pubsub client] unable to connect to the ECH");
		return 0;
	}
	fcntl(cd, F_SETFL, fcntl(cd, F_GETFL) | O_NONBLOCK | O_ASYNC);

	is_connected = 1;
	return 1;
}


static int reset_connection(void)
{
	if(!is_connected)
		return 0;

	close(cd);
	is_connected = 0;
	cd = -1;

	return 0;
}


/*
 * Write a request of the type "opcode" and length len to the ECH.
 */
static int writereq(unsigned char opcode, void *data, int len)
{
	int rv;

	if(!is_connected)
		if(!do_connect())
			return 0;

	rv = write(cd, data, len);
	if(rv!=len) {
		reset_connection();
		return 0;
	}

	return 1;
}


int famouso_announce(famouso_subject_t chn)
{
	char d[1+sizeof(famouso_subject_t)];
	maken64(d+1, chn);
	d[0]=(unsigned char)ANNOUNCE;
	return writereq(ANNOUNCE, d, sizeof(d));
}

int famouso_subscribe(famouso_subject_t chn)
{
	char d[1+sizeof(famouso_subject_t)];
	maken64(d+1,chn);
	d[0]=(unsigned char)SUBSCRIBE;
	return writereq(SUBSCRIBE, d, sizeof(d));
}


int famouso_unsubscribe(famouso_subject_t chn)
{
	char d[1+sizeof(famouso_subject_t)];
	maken64(d+1,chn);
	d[0]=(unsigned char)UNSUBSCRIBE;
	return writereq(UNSUBSCRIBE, d, sizeof(d));
}


int famouso_publish(famouso_event_t event)
{
	char d[1+sizeof(famouso_subject_t)+4+MAX_EVENT_DATA_LENGTH];
	maken64(d+1, event.subject);
	maken32(&d[sizeof(famouso_subject_t)+1], event.len);
	memcpy(&d[sizeof(famouso_subject_t)+4+1], event.data, (event.len>MAX_EVENT_DATA_LENGTH) ? MAX_EVENT_DATA_LENGTH :event.len);
	d[0]=(unsigned char)PUBLISH;
	return writereq(PUBLISH, d, sizeof(d));
}


int famouso_poll(famouso_event_t *event) {
	char data[1+sizeof(famouso_subject_t)+4+MAX_EVENT_DATA_LENGTH+4096];
	int rv=0, c=13, i=0;

	if(!is_connected)
		if(!do_connect())
			return 0;

	rv = read(cd, &data[i], c);

	/* no message */
	if((rv < 0) && (errno == EAGAIN))
		return 0;

	/* error */
	if(rv < 0) {
		error("connection reseted\n");
		reset_connection();
		return 0;
	}
//	printf("0x%x FAMOUSO::PUBLISH: 0x%x ECH_MSG_PUBLISHED: 0x%x\n",data[0],FAMOUSO::PUBLISH, ECH_MSG_PUBLISHED);
	switch(data[0]) {
		case PUBLISH:
#warning "Preliminary Version. Something to do, but for the first strike it should do"
			// lesen der ersten 13 Zeichen, was bedeutet, wir haben
			// den Nachrichten Type gelesen sowie das Subject und
			// die Laenge
			c-=rv;
			i=rv;
			while (c) {
				rv = read(cd, &data[i], c);
				if (rv<0) {
					if (errno != EAGAIN){
						printf("connection reseted");
						abort();
					}
				} else {
					c-=rv;
					i+=rv;
				}
			}

			event->subject = make64n(data+1);
			event->len = ntohl(*(uint32_t*)&data[9]);
//			printf("Length: %4d \tSubject: 0x%16llx\n",event->len, event->subject);
			c=event->len;
			i=0;
			while (c) {
				rv = read(cd, &data[i], c);
				if (rv<0) {
					if (errno != EAGAIN){
						printf("connection reseted");
						abort();
					}
				} else {
					c-=rv;
					i+=rv;
				}
			}

			if( event->len > MAX_EVENT_DATA_LENGTH )
				event->len = MAX_EVENT_DATA_LENGTH;
			for(i=0; i<event->len; i++)
				event->data[i] = data[i];
//			printf("Count of Bytes Read: %d Length: %4d \tSubject: 0x%16llx\n",rv,event->len, event->subject);
			return 1;
	}

	error("[pubsub client] received unknown opcode from ECH");
	return 0;
}

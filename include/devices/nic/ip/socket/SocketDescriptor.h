#ifndef __SocketDescriptor_h__
#define __SocketDescriptor_h__

/**
 *   Base class representing basic communication endpoint
 */
class SocketDescriptor {
protected:
	int sockDesc;              // Socket descriptor
	SocketDescriptor() : sockDesc(-1) {}
};

#endif

#include "devices/nic/ip/socket/bsd/BSDSocket.h"

#include <stdio.h>
BSDSocket::~BSDSocket() {
  close(sockDesc);
}


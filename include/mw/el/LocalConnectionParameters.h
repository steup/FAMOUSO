#ifndef __LocalConnectionParameters_h__
#define __LocalConnectionParameters_h__

/*! \brief This struct or better its defined enum is used
 *         for tagging messages. That allows indicating
 *         the type of a message and therewith to do the
 *         correct steps while processing them.
 */
struct FAMOUSO {
  enum {
    SUBSCRIBE=0x50,
    UNSUBSCRIBE,
    PUBLISH=0x52,

    ANNOUNCE=0x56
  };
};

/*! \brief  definition of the local port, at which
 *          the ECH accepts new incomming connections
 */
#define ServPort 5005

/*! \brief  definition of the ip adress, at which
 *          the ECH accepts new incomming connections.
 *          Normally, it is the localhost adress.
 */
#define servAddress "127.0.0.1"

/*! \brief  BUFSIZE describes the maximal message size.
 *          At the moment it is fixed to 2^16, but it is
 *          an open question -- What big can a message be?
 */
#define BUFSIZE 65535

#endif

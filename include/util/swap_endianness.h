#ifndef __swap_endianness_h__
#define __swap_endianness_h__

#define __swap_endianness_16(x) \
     ((((x) >> 8) & 0xff) | (((x) & 0xff) << 8))

#define __swap_endianness_32(x) \
     ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >>  8) | \
      (((x) & 0x0000ff00) <<  8) | (((x) & 0x000000ff) << 24))

#define __swap_endianness_64(x) \
     ((((x) & 0xff00000000000000ull) >> 56)		\
      | (((x) & 0x00ff000000000000ull) >> 40)	\
      | (((x) & 0x0000ff0000000000ull) >> 24)	\
      | (((x) & 0x000000ff00000000ull) >> 8)	\
      | (((x) & 0x00000000ff000000ull) << 8)	\
      | (((x) & 0x0000000000ff0000ull) << 24)	\
      | (((x) & 0x000000000000ff00ull) << 40)	\
      | (((x) & 0x00000000000000ffull) << 56))

#endif

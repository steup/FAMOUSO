/*******************************************************************************
 *
 * Copyright (c) 2008-2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
 *               2009-2010 Michael Kriese <kriese@cs.uni-magdeburg.de>
 * All rights reserved.
 *
 *    Redistribution and use in source and binary forms, with or without
 *    modification, are permitted provided that the following conditions
 *    are met:
 *
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in
 *      the documentation and/or other materials provided with the
 *      distribution.
 *
 *    * Neither the name of the copyright holders nor the names of
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 *
 *    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 *    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 *    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 *    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *    OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * $Id$
 *
 ******************************************************************************/

#ifndef _Attributes_h_
#define _Attributes_h_

#include <cstring>
#include "mw/nl/awds/AWDS_Packet.h"

namespace famouso {
	namespace mw {
		namespace nl {
			namespace _awds {

				typedef uint16_t value_t;

				value_t cast_get(const void *data) {
					const value_t *p = static_cast<const value_t *> (data);
					return *p;
				}

				void cast_set(void *data, value_t value) {
					value_t *p = static_cast<value_t *> (data);
					*p = value;
				}

				class Attributes {
						struct constants {
								enum {
									payload = awds::AWDS_Packet::constants::packet_size::payload,
									attrib_header_size = 1,
									attrib_size = 5
								};

								struct offsets {
										enum {
											attrib_count = 0,
											attrib_data = attrib_count + attrib_header_size
										};
								};
						};

						enum AttributeType {
							TTL = 0x1,
							Latency = 0x2
						};

						void set(awds::AWDS_Packet &p) {
							// get number of attributes
							num = p.data[constants::offsets::attrib_count];

							uint16_t size = ntohl(p.header.size);

							if (size < (num * constants::attrib_size + constants::attrib_header_size)) {
								num = 0;
								return;
							}

							// copy attributes
							std::memcpy(data, &p.data[constants::offsets::attrib_data], num * constants::attrib_size);
						}

						void get(awds::AWDS_Packet &p) {
							// set number of attributes
							p.data[constants::offsets::attrib_count] = num;

							// size of attributes
							uint16_t size = num * constants::attrib_size;

							// copy attributes
							std::memcpy(&p.data[constants::offsets::attrib_data], data, size);

							// size of attributes and header
							size += constants::attrib_header_size;

							// set packet size
							p.header.size = htonl(size);
						}

						value_t get(AttributeType attrib) const {
							for (int p = 0; p < num * constants::attrib_size; p += constants::attrib_size) {
								if (data[p] == attrib)
									return ntohl(cast_get(&data[p + 1]));
							}
							return ~0;
						}

						void set(AttributeType attrib, uint32_t value) {
							int p = 0;
							for (; p < num * constants::attrib_size; p += constants::attrib_size) {
								if (data[p] == attrib) {
									// copy value to data
									cast_set(&data[p + 1], htonl(value));
									return;
								}
							}

							data[p] = (uint8_t) attrib;
							cast_set(&data[p + 1], htonl(value));
							num++;
						}

						/*! \brief Checks weather attributes are matching
						 *
						 *  \param o The other attributes.
						 *  \return true if all attributes of o are more to this, otherwise false.
						 */
						bool operator<(const Attributes & o) const {
							if (this->get(TTL) < o.get(TTL))
								return false;
							if (this->get(Latency) < o.get(Latency))
								return false;

							return true;
						}

						/*! \brief Checks weather attributes are matching
						 *
						 *  \param o The other attributes.
						 *  \return true if all attributes of o are less to this, otherwise false.
						 */
						bool operator>(const Attributes & o) const {
							if (this->get(TTL) > o.get(TTL))
								return false;
							if (this->get(Latency) > o.get(Latency))
								return false;

							return true;
						}

						/*! \brief Checks weather attributes are matching
						 *
						 *  \param o The other attributes.
						 *  \return true if all attributes of o are equal to this, otherwise false.
						 */
						bool operator==(const Attributes & o) const {
							if (this->get(TTL) != o.get(TTL))
								return false;
							if (this->get(Latency) != o.get(Latency))
								return false;

							return true;
						}

						/*! \brief Checks weather attributes are matching
						 *
						 *  \param o The other attributes.
						 *  \return true if all attributes of o are more or equal to this, otherwise false.
						 */
						bool operator<=(const Attributes & o) const {
							return !(*this > o);
						}

						/*! \brief Checks weather attributes are matching
						 *
						 *  \param o The other attributes.
						 *  \return true if all attributes of o are less or equal to this, otherwise false.
						 */
						bool operator>=(const Attributes & o) const {
							return !(*this < o);
						}

						/*! \brief Checks weather attributes are matching
						 *
						 *  \param o The other attributes.
						 *  \return true if one attributes of o are not equal to this, otherwise false.
						 */
						bool operator!=(const Attributes &o) const {
							return !(*this == o);
						}

					private:
						uint8_t num;
						uint8_t data[constants::payload];
				};

			} /* _awds */

			namespace awds {
				typedef _awds::Attributes Attributes;

				typedef _awds::value_t value_t;
			} /* awds */
		} /* nl */
	} /* mw */
} /* famouso */
#endif /* _Attributes_h_ */

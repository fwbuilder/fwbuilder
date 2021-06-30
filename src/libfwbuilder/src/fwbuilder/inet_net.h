/*
 * Copyright (c) 2004 by Internet Systems Consortium, Inc. ("ISC")
 * Copyright (c) 1996,1999 by Internet Software Consortium.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

#ifndef INET_NET_H
#define INET_NET_H


#ifdef _WIN32
#  include <winsock2.h>
#else

#  include <sys/types.h>
#  include <netinet/in.h>
//#  include <arpa/inet.h>
#  include <sys/types.h>
#  include <sys/socket.h>
#endif

/*
 * char *
 * inet_net_ntop(af, src, bits, dst, size)
 *  convert host/network address from network to presentation format.
 *  "src"'s size is determined from its "af".
 * return:
 *  pointer to dst, or nullptr if an error occurred (check errno).
 * note:
 *  192.5.5.1/28 has a nonzero host part, which means it isn't a network
 *  as called for by inet_net_pton() but it can be a host address with
 *  an included netmask.
 * author:
 *  Paul Vixie (ISC), October 1998
 */
extern char* inet_net_ntop(int af, const void *src, int bits, char *dst, size_t size);

/*
 * int
 * inet_net_pton(af, src, dst, size)
 *  convert network number from presentation to network format.
 *  accepts hex octets, hex strings, decimal octets, and /CIDR.
 *  "size" is in bytes and describes "dst".
 * return:
 *  number of bits, either imputed classfully or specified with /CIDR,
 *  or -1 if some failure occurred (check errno).  ENOENT means it was
 *  not a valid network specification.
 * author:
 *  Paul Vixie (ISC), June 1996
 *
 * Changes:
 *  I added the inet_cidr_pton function (also from Paul) and changed
 *  the names to reflect their current use.
 *
 */
extern int inet_net_pton(int af, const char *src, void *dst, size_t size);

#endif


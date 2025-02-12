/*
 * ggentropy v1.0
 *
 * Copyright (c) 2021 Michael Savage <mike@mikejsavage.co.uk>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#if defined( _WIN32 )
#  define PLATFORM_WINDOWS 1

#elif defined( __linux__ )
#  define PLATFORM_LINUX 1

#elif defined( __APPLE__ )
#  define PLATFORM_HAS_ARC4RANDOM 1

#elif defined( __FreeBSD__ ) || defined( __OpenBSD__ ) || defined( __NetBSD__ )
#  define PLATFORM_HAS_ARC4RANDOM 1
#endif

#include <stdbool.h>
#include <stddef.h>
#include <assert.h>

#if PLATFORM_WINDOWS

#pragma comment( lib, "bcrypt.lib" )

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <bcrypt.h>

bool ggentropy( void * buf, size_t n ) {
	assert( n <= 256 );
	return !FAILED( BCryptGenRandom( NULL, ( PUCHAR ) buf, n, BCRYPT_USE_SYSTEM_PREFERRED_RNG ) );
}

#elif PLATFORM_LINUX

#include <unistd.h>
#include <sys/syscall.h>

bool ggentropy( void * buf, size_t n ) {
	assert( n <= 256 );
	int ok = syscall( SYS_getrandom, buf, n, 0 );
	return ok >= 0 && ( size_t ) ok == n;
}

#elif PLATFORM_HAS_ARC4RANDOM

#include <stdlib.h>

bool ggentropy( void * buf, size_t n ) {
	assert( n <= 256 );
	arc4random_buf( buf, n );
	return true;
}

#else

#include <fcntl.h>
#include <unistd.h>

bool ggentropy( void * buf, size_t n ) {
	int fd = open("/dev/urandom", O_RDONLY);

	if (fd != -1) {
		int ret = read(fd, buf, n);
		close(fd);
		return ret == n;
	}
	return false;
}
#endif

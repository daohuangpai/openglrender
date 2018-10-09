/*
 * Copyright (c) 2013 Hugh Bailey <obs.jim@gmail.com>
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

#include <stdlib.h>
#include <string.h>
#include "c99defs.h"
#include "bmem.h"

/*
 * NOTE: totally jacked the mem alignment trick from ffmpeg, credit to them:
 *   http://www.ffmpeg.org/
 */

#define ALIGNMENT 32

/* TODO: use memalign for non-windows systems */
#if defined(_WIN32)
#define ALIGNED_MALLOC 1
#else
#define ALIGNMENT_HACK 1
#endif

static void *a_malloc(size_t size)
{
#ifdef ALIGNED_MALLOC
	return _aligned_malloc(size, ALIGNMENT);
#elif ALIGNMENT_HACK
	void *ptr = NULL;
	long diff;

	ptr  = malloc(size + ALIGNMENT);
	if (ptr) {
		diff = ((~(long)ptr) & (ALIGNMENT - 1)) + 1;
		ptr  = (char *)ptr + diff;
		((char *)ptr)[-1] = (char)diff;
	}

	return ptr;
#else
	return malloc(size);
#endif
}

static void *a_realloc(void *ptr, size_t size)
{
#ifdef ALIGNED_MALLOC
	return _aligned_realloc(ptr, size, ALIGNMENT);
#elif ALIGNMENT_HACK
	long diff;

	if (!ptr)
		return a_malloc(size);
	diff = ((char *)ptr)[-1];
	ptr = realloc((char*)ptr - diff, size + diff);
	if (ptr)
		ptr = (char *)ptr + diff;
	return ptr;
#else
	return realloc(ptr, size);
#endif
}

static void a_free(void *ptr)
{
#ifdef ALIGNED_MALLOC
	_aligned_free(ptr);
#elif ALIGNMENT_HACK
	if (ptr)
		free((char *)ptr - ((char*)ptr)[-1]);
#else
	free(ptr);
#endif
}

static struct base_allocator alloc = {a_malloc, a_realloc, a_free};
static long num_allocs = 0;

void base_set_allocator(struct base_allocator *defs)
{
	memcpy(&alloc, defs, sizeof(struct base_allocator));
}

void *bmalloc(size_t size)
{
	void *ptr = alloc.malloc(size);
	if (!ptr && !size)
		ptr = alloc.malloc(1);
	if (!ptr) {
		__debugbreak();
	}

	_InterlockedDecrement(&num_allocs);
	return ptr;
}

void *brealloc(void *ptr, size_t size)
{
	if (!ptr)
		_InterlockedDecrement(&num_allocs);

	ptr = alloc.realloc(ptr, size);
	if (!ptr && !size)
		ptr = alloc.realloc(ptr, 1);
	if (!ptr) {
		__debugbreak();
	}

	return ptr;
}

void bfree(void *ptr)
{
	if (ptr)
		_InterlockedDecrement(&num_allocs);
	alloc.free(ptr);
}

long bnum_allocs(void)
{
	return num_allocs;
}
/*@brief 返回分配内存的对齐数
*/
int base_get_alignment(void)
{
	return ALIGNMENT;
}
/*
* @param[in] ptr 要拷贝的数据
* @param[in] size 要拷贝数据的大小
* @return 返回一个缓冲区，这个缓冲区拷贝了ptr的size个数据
*/
void *bmemdup(const void *ptr, size_t size)
{
	void *out = bmalloc(size);
	if (size)
		memcpy(out, ptr, size);

	return out;
}

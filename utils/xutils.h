/*
*   Copyright (C) 2022, Xiaosan Zhai(tom.zhai@aliyun.com)
*   This file is part of the xalgos library.
*
*   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
*   without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*   See the MIT License for more details.
*
*   You should have received a copy of the MIT License along with this program.
*   If not, see <https://mit-license.org/>.
*/

#ifndef XUTILS_INCLUDED
#define XUTILS_INCLUDED

#include <stdbool.h>

/* TODO ? : put all below variables into one config file ?? */
static const int XUTILS_HASH_SLOTS_DEFAULT_HINT      = 6151;

static const int XUTILS_MEM_EXPAND_DEFAULT_LENGTH    = 4096;

static const int XUTILS_ARRAY_DEFAULT_LENGTH         = 4096;
static const int XUTILS_ARRAY_EXPAND_DEFAULT_LENGTH  = 4096;

static const int XUTILS_ARRAY2_LAYER1_DEFAULT_LENGTH = 256;
static const int XUTILS_ARRAY2_LAYER2_DEFAULT_LENGTH = 4096;

static const int XUTILS_SEQ_DEFAULT_LENGTH           = 4096;
static const int XUTILS_SEQ_EXPAND_DEFAULT_LENGTH    = 4096;

static const int XUTILS_DEQUE_LAYER1_DEFAULT_LENGTH  = 256;
static const int XUTILS_DEQUE_LAYER2_DEFAULT_LENGTH  = 4096;

static const int XUTILS_UNLIMITED_BASED_ON_POWER_2   = 64;

/* Used by xarena.c */
static const int XUTILS_ARENA_MIN_ALIGN_SIZE         = 8;
static const int XUTILS_ARENA_MAX_BYTES              = 512;

/* strategy used when add new element to sequence/queue/deque... */
static const int XUTILS_QUEUE_STRATEGY_DISCARD_NEW   = 0;
static const int XUTILS_QUEUE_STRATEGY_DISCARD_FRONT = 1;
static const int XUTILS_QUEUE_STRATEGY_DISCARD_BACK  = 2;
static const int XUTILS_QUEUE_STRATEGY_DISCARD_TOP   = 3;

extern int       xutils_hash_buckets_num (int hint);
extern int       xutils_max_hash_buckets_size(void);

extern int       xutils_pointer_equal    (void *x, void *y, void *cl);

extern bool      xutils_match            (void *x, void *y);

/* TODO ? : put all hash function into one file : xhash_func.c ? */

extern int       xutils_hash_atom        (void *x);

extern int       xutils_hash_int         (int m);
extern int       xutils_hash_float       (float f);
extern int       xutils_hash_string      (char* str);
extern int       xutils_hash_const_chars (const char *str, int len);
extern int       xutils_hash_generic     (void *key);

extern int       xutils_hash_pointer     (void *pointer);

extern bool      xutils_generic_swap     (void *x, void *y, int size);

extern void*     xutils_deep_copy        (void* source, int size);

#endif

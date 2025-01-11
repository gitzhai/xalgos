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

#ifndef XTS_DLIST_INCLUDED
#define XTS_DLIST_INCLUDED

#if defined(__linux__)

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct XTS_DList*       XTS_DList_PT;

/* O(1) */
extern XTS_DList_PT    xts_dlist_new                        (int capacity);

/* O(1) */
extern bool            xts_dlist_push_front_repeat          (XTS_DList_PT dlist, void *value);
extern void*           xts_dlist_pop_front                  (XTS_DList_PT dlist);
extern void*           xts_dlist_pop_front_timeout          (XTS_DList_PT dlist, long timeout_ms);

/* O(1) */
extern bool            xts_dlist_push_back_repeat           (XTS_DList_PT dlist, void *value);
extern void*           xts_dlist_pop_back                   (XTS_DList_PT dlist);
extern void*           xts_dlist_pop_back_timeout           (XTS_DList_PT dlist, long timeout_ms);

/* O(N) */
extern void            xts_dlist_free                       (XTS_DList_PT *pdlist);
extern void            xts_dlist_deep_free                  (XTS_DList_PT *pdlist);

/* O(N) */
extern void            xts_dlist_clear                      (XTS_DList_PT dlist);
extern void            xts_dlist_deep_clear                 (XTS_DList_PT dlist);

/* O(1) */
extern int             xts_dlist_size                       (XTS_DList_PT dlist);
extern bool            xts_dlist_is_empty                   (XTS_DList_PT dlist);
extern int             xts_dlist_capacity                   (XTS_DList_PT dlist);

#ifdef __cplusplus
}
#endif

#endif
#endif

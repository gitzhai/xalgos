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

#ifndef XTS_SLIST_INCLUDED
#define XTS_SLIST_INCLUDED

#if defined(__linux__)

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct XTS_SList*    XTS_SList_PT;

/* O(1) */
extern XTS_SList_PT      xts_slist_new                        (int capacity);   /* -1 means unlimited */

/* O(1) */
extern bool              xts_slist_push_front_repeat          (XTS_SList_PT slist, void *value);
extern bool              xts_slist_push_back_repeat           (XTS_SList_PT slist, void *value);

/* O(1) */
extern void*             xts_slist_pop_front                  (XTS_SList_PT slist);
extern void*             xts_slist_pop_front_timeout          (XTS_SList_PT slist, long timeout_ms);

/* O(N) */
extern void              xts_slist_free                       (XTS_SList_PT *pslist);
extern void              xts_slist_deep_free                  (XTS_SList_PT *pslist);

/* O(N) */
extern void              xts_slist_clear                      (XTS_SList_PT slist);
extern void              xts_slist_deep_clear                 (XTS_SList_PT slist);

/* O(1) */
extern int               xts_slist_size                       (XTS_SList_PT slist);
extern bool              xts_slist_is_empty                   (XTS_SList_PT slist);
extern int               xts_slist_capacity                   (XTS_SList_PT slist);

#ifdef __cplusplus
}
#endif

#endif
#endif

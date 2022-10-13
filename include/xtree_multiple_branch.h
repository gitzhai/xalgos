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

#ifndef XMTREE_INCLUDED
#define XMTREE_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

typedef struct XMTree*      XMTree_PT;
typedef struct XMTree_Node* XMTree_Node_PT;

extern XMTree_PT       xmtree_new            (int (*cmp)(void *key1, void *key2, void *cl), void *cl);
extern XMTree_Node_PT  xmtree_put_repeat     (XMTree_PT tree, XMTree_Node_PT parent, void *key, void *value);

extern int             xmtree_map_preorder   (XMTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern int             xmtree_map_postorder  (XMTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern int             xmtree_map_levelorder (XMTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);

extern bool            xmtree_map_preorder_break_if_true  (XMTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);
extern bool            xmtree_map_preorder_break_if_false (XMTree_PT tree, bool (*apply)(void *key, void **value, void *cl), void *cl);

extern void            xmtree_clear          (XMTree_PT tree);
extern void            xmtree_deep_clear     (XMTree_PT tree);

extern void            xmtree_free           (XMTree_PT *ptree);
extern void            xmtree_deep_free      (XMTree_PT *ptree);

extern bool            xmtree_to_string      (XMTree_PT tree);

#ifdef __cplusplus
}
#endif

#endif

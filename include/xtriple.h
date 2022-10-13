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

#ifndef XTRIPLE_INCLUDED
#define XTRIPLE_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/* define XTriple_PT here to make direct access to member "first", "second" and "third" */
typedef struct XTriple*  XTriple_PT;
struct XTriple {
    void *first;
    void *second;
    void *third;
};

extern XTriple_PT  xtriple_new         (void *first, void *second, void *third);

extern XTriple_PT  xtriple_copy        (XTriple_PT triple);
extern XTriple_PT  xtriple_deep_copy   (XTriple_PT triple, int first_size, int second_size, int third_size);

extern void*       xtriple_first       (XTriple_PT triple);
extern void*       xtriple_second      (XTriple_PT triple);
extern void*       xtriple_third       (XTriple_PT triple);

extern void        xtriple_set_first   (XTriple_PT triple, void* first);
extern void        xtriple_set_second  (XTriple_PT triple, void *second);
extern void        xtriple_set_third   (XTriple_PT triple, void *third);

extern void        xtriple_free        (XTriple_PT *triple);
extern void        xtriple_deep_free   (XTriple_PT *triple);

extern void        xtriple_free_first  (XTriple_PT triple);
extern void        xtriple_free_second (XTriple_PT triple);
extern void        xtriple_free_third  (XTriple_PT triple);

#ifdef __cplusplus
}
#endif

#endif

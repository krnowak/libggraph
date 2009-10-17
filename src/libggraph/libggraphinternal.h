#ifndef _LIB_G_GRAPH_INTERNAL_H_
#define _LIB_G_GRAPH_INTERNAL_H_

#include "ggraph.h"

G_BEGIN_DECLS

G_GNUC_INTERNAL gboolean
_libggraph_internal_are_separate(GGraph* graph,
                                 GGraph* other_graph);

G_END_DECLS

#endif /* _LIB_G_GRAPH_INTERNAL_H_ */

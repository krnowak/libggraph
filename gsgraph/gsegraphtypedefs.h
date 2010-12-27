/*
 * Copyright (C) 2009, 2010 Krzesimir Nowak
 *
 * This file is part of libggraph.
 *
 * libggraph is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libggraph is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libggraph.  If not, see <http://www.gnu.org/licenses/>.
 */

#if defined (G_DISABLE_SINGLE_INCLUDES) && !defined (_G_SEGRAPH_H_INSIDE_) && !defined (_LIB_G_GRAPH_COMPILATION_)
#error "Only <gsgraph/gsegraph.h> can be included directly."
#endif

/*
 * This header exist because there are circular dependencies:
 * .-> GSEGraphNode -> GPtrArray -> GSEGraphEdge -.
 * `----------------------------------------------'
 *
 * So here typedefs to these structs exist. It should be enough, because none of
 * them needs information about sizes of other structs.
 */

#include <glib.h>

#ifndef _G_SEGRAPH_TYPEDEFS_H_
#define _G_SEGRAPH_TYPEDEFS_H_

G_BEGIN_DECLS

typedef struct _GSEGraphEdge GSEGraphEdge;

typedef struct _GSEGraphNode GSEGraphNode;

G_END_DECLS

#endif /* _G_GRAPH_TYPEDEFS_H_ */

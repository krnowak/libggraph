/*
 * Copyright (C) 2009 Krzesimir Nowak <qdlacz@gmail.com>
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

#if defined(G_DISABLE_SINGLE_INCLUDES) && !defined (_G_SGRAPH_H_INSIDE_) && !defined (_LIB_G_GRAPH_COMPILATION_)
#error "Only <gsgraph/gsgraph.h> can be included directly."
#endif

#ifndef _G_SGRAPH_ENUMS_H_
#define _G_SGRAPH_ENUMS_H_

#include <glib.h>

G_BEGIN_DECLS

/**
 * SECTION: gsgraphenums
 * @title: Enums and flags
 * @short_description: GSGraph enumerations.
 * @include: gsgraph/gsgraph.h
 *
 * Enums used by GSGraph.
 */

/**
 * GSGraphTraverseType:
 * @G_SGRAPH_TRAVERSE_BFS: Breadth first search.
 * @G_SGRAPH_TRAVERSE_DFS: Depth first search.
 *
 * Used to specify traversing algorithm.
 */
typedef enum
{
  G_SGRAPH_TRAVERSE_BFS,
  G_SGRAPH_TRAVERSE_DFS
} GSGraphTraverseType;

G_END_DECLS

#endif /* _G_SGRAPH_ENUMS_H_ */

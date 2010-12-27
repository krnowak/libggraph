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

#if defined (G_DISABLE_SINGLE_INCLUDES) && !defined (_G_SGRAPH_H_INSIDE_) && !defined (_LIB_G_GRAPH_COMPILATION_)
#error "Only <gsgraph/gsgraph.h> can be included directly."
#endif

#ifndef _G_SGRAPH_DATA_PAIR_H_
#define _G_SGRAPH_DATA_PAIR_H_

#include <glib.h>

G_BEGIN_DECLS

/**
 * GSGraphDataPair:
 * @first: data for first node.
 * @second: data for second node.
 *
 * Convenience struct holding two #gpointer variables.
 */
typedef struct _GSGraphDataPair GSGraphDataPair;

struct _GSGraphDataPair
{
  gpointer first;
  gpointer second;
};

GSGraphDataPair*
g_sgraph_data_pair_new (gpointer first,
                        gpointer second) G_GNUC_WARN_UNUSED_RESULT;

GSGraphDataPair*
g_sgraph_data_pair_copy (GSGraphDataPair* data_pair) G_GNUC_WARN_UNUSED_RESULT;

void
g_sgraph_data_pair_free (GSGraphDataPair* data_pair);

void
g_sgraph_data_pair_free_v (GSGraphDataPair** data_pairs,
                           guint count,
                           gboolean free_array);

gboolean
g_sgraph_data_pair_is_valid (GSGraphDataPair* data_pair);

G_END_DECLS

#endif /* _G_SGRAPH_DATA_PAIR_H_ */

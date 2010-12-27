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

#ifndef _G_SEGRAPH_DATA_TRIPLET_H_
#define _G_SEGRAPH_DATA_TRIPLET_H_

#include <glib.h>

G_BEGIN_DECLS

/**
 * GSEGraphDataTriplet:
 * @first: data for first node.
 * @second: data for second node.
 * @edge: data for edge connecting both nodes.
 *
 * Convenience struct holding three #gpointer variables.
 */
typedef struct _GSEGraphDataTriplet GSEGraphDataTriplet;

struct _GSEGraphDataTriplet
{
  gpointer first;
  gpointer second;
  gpointer edge;
};

GSEGraphDataTriplet*
g_segraph_data_triplet_new (gpointer first,
                            gpointer second,
                            gpointer edge) G_GNUC_WARN_UNUSED_RESULT;

GSEGraphDataTriplet*
g_segraph_data_triplet_copy (GSEGraphDataTriplet* data_triplet) G_GNUC_WARN_UNUSED_RESULT;

void
g_segraph_data_triplet_free (GSEGraphDataTriplet* data_triplet);

void
g_segraph_data_triplet_free_v (GSEGraphDataTriplet** data_triplets,
                               guint count,
                               gboolean free_array);

gboolean
g_segraph_data_triplet_is_valid (GSEGraphDataTriplet* data_triplet);

G_END_DECLS

#endif /* _G_SEGRAPH_DATA_TRIPLET_H_ */

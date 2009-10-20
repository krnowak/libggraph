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

#include "gsgraphdatapair.h"

/**
 * SECTION: gsgraphdatapair
 * @title: Data pairs
 * @short_description: data pairs for constructing simple graphs
 * @include: gsgraph/gsgraph.h
 * @see_also: #GSGraphNode, g_sgraph_node_construct()
 *
 * Data pair are used primarly for constructing simple graphs by
 * g_sgraph_construct(). They contain all necessary data to create a pair of
 * nodes - nodes data.
 *
 * To create new data pair, use g_sgraph_data_pair_new().
 *
 * To copy it, use g_sgraph_data_pair_copy().
 *
 * To free it, use g_sgraph_data_pair_free().
 *
 * Convenient function for freeing an array of data pairs is also provided -
 * g_sgraph_data_pair_free_v().
 */

/* function definitions */

/**
 * g_sgraph_data_pair_new:
 * @first: first data.
 * @second: second data.
 *
 * Creates new data pair.
 *
 * Returns: newly created #GSGraphDataPair.
 */
GSGraphDataPair*
g_sgraph_data_pair_new(gpointer first,
                       gpointer second)
{
  GSGraphDataPair* data_pair = g_slice_new(GSGraphDataPair);
  data_pair->first = first;
  data_pair->second = second;
  return data_pair;
}

/**
 * g_sgraph_data_pair_copy:
 * @data_pair: first data.
 *
 * Creates a copy of @data_pair.
 *
 * Returns: newly created #GSGraphDataPair.
 */
GSGraphDataPair*
g_sgraph_data_pair_copy(GSGraphDataPair* data_pair)
{
  return g_sgraph_data_pair_new(data_pair->first, data_pair->second);
}

/**
 * g_sgraph_data_pair_free:
 * @data_pair: data pair to free.
 *
 * Frees memorty allocated to data pair.
 */
void
g_sgraph_data_pair_free(GSGraphDataPair* data_pair)
{
  g_slice_free(GSGraphDataPair, data_pair);
}

/**
 * g_sgraph_data_pair_free_v:
 * @data_pairs: #GSGraphDataPair array to free.
 * @count: length of @data_pairs.
 *
 * Frees all #GSGraphDataPair in array and then frees the array itself using
 * g_free(). If count is -1, it is assumed that @data_pairs is %NULL terminated.
 */
void
g_sgraph_data_pair_free_v(GSGraphDataPair** data_pairs,
                          gint count)
{
  gint iter;
  
  if ((data_pairs == NULL) || (count == 0))
  {
    return;
  }
  
  if (count == -1)
  {
    count = 0;
    while (data_pairs[count])
    {
      count++;
    }
  }
  
  for (iter = 0; iter < count; iter++)
  {
    g_sgraph_data_pair_free(data_pairs[iter]);
  }
  g_free(data_pairs);
}

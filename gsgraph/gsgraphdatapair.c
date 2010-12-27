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

#include "gsgraphdatapair.h"

/**
 * SECTION: gsgraphdatapair
 * @title: Simple graph data pairs
 * @short_description: data pairs for constructing simple graphs
 * @include: gsgraph/gsgraph.h
 * @see_also: #GSGraphSnapshot, g_sgraph_snapshot_new()
 *
 * Data pairs are used primarly for constructing simple graphs by
 * g_sgraph_snapshot_new(). They contain all necessary data to create a pair
 * of nodes - nodes data.
 *
 * To create new data pair, use g_sgraph_data_pair_new().
 *
 * To copy it, use g_sgraph_data_pair_copy().
 *
 * To free it, use g_sgraph_data_pair_free().
 *
 * Convenient function for freeing an array of data pairs is also provided -
 * g_sgraph_data_pair_free_v().
 *
 * To check whether data pair is valid, use g_sgraph_data_pair_is_valid().
 */

/* function definitions */

/**
 * g_sgraph_data_pair_new:
 * @first: first data.
 * @second: second data.
 *
 * Creates new data pair. Neither @first nor @second can be %NULL and both have
 * to be different.
 *
 * Returns: newly created #GSGraphDataPair.
 */
GSGraphDataPair*
g_sgraph_data_pair_new (gpointer first,
                        gpointer second)
{
  GSGraphDataPair* data_pair;

  g_return_val_if_fail (first != NULL, NULL);
  g_return_val_if_fail (second != NULL, NULL);
  g_return_val_if_fail (first != second, NULL);

  data_pair = g_slice_new (GSGraphDataPair);
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
g_sgraph_data_pair_copy (GSGraphDataPair* data_pair)
{
  g_return_val_if_fail (data_pair != NULL, NULL);

  return g_sgraph_data_pair_new (data_pair->first, data_pair->second);
}

/**
 * g_sgraph_data_pair_free:
 * @data_pair: data pair to free.
 *
 * Frees memorty allocated to data pair.
 */
void
g_sgraph_data_pair_free (GSGraphDataPair* data_pair)
{
  g_return_if_fail (data_pair != NULL);

  g_slice_free (GSGraphDataPair, data_pair);
}

/**
 * g_sgraph_data_pair_free_v:
 * @data_pairs: #GSGraphDataPair array to free.
 * @count: length of @data_pairs.
 * @free_array: whether to free array.
 *
 * Frees all #GSGraphDataPair in array and then, if @free_array is %TRUE, frees
 * the array itself using g_free(). If count is 0, it is assumed that
 * @data_pairs is %NULL terminated.
 */
void
g_sgraph_data_pair_free_v (GSGraphDataPair** data_pairs,
                           guint count,
                           gboolean free_array)
{
  guint iter;

  g_return_if_fail (data_pairs != NULL);

  if (!count)
  {
    while (data_pairs[count])
    {
      ++count;
    }
  }

  for (iter = 0; iter < count; ++iter)
  {
    g_sgraph_data_pair_free (data_pairs[iter]);
  }
  if (free_array)
  {
    g_free (data_pairs);
  }
}

/**
 * g_sgraph_data_pair_is_valid:
 * @data_pair: a pair.
 *
 * Checks if @data_pair is valid, that is - if both members of @data_pair are
 * not %NULL and are different.
 *
 * Returns: %TRUE if @data_pair is valid, otherwise %FALSE.
 */
gboolean
g_sgraph_data_pair_is_valid (GSGraphDataPair* data_pair)
{
  g_return_val_if_fail (data_pair != NULL, FALSE);

  return (data_pair->first && data_pair->second &&
          data_pair->first != data_pair->second);
}

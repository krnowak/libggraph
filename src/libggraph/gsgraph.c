#include "gsgraph.h"

// static declarations

static gboolean
_g_sgraph_no_indirect_connection(GSGraph* sgraph,
                                 GSGraph* other_sgraph);

static gboolean
_g_sgraph_recurrent_connection_check(GSGraph* sgraph,
                                     GSGraph* other_sgraph,
                                     GHashTable* visited_nodes);

static GSGraphArray*
_g_sgraph_array(GSGraph* sgraph);

static void
_g_sgraph_recurrent_array_append(GSGraph* sgraph,
                                 GSGraphArray* sgraph_array,
                                 GHashTable* visited_nodes);

// function definitions

GSGraph*
g_sgraph_new(gpointer data)
{
  GSGraph* sgraph = g_slice_new(GSGraph);
  sgraph->data = data;
  sgraph->neighbours = g_sgraph_array_new();
  return sgraph;
}

gboolean
g_sgraph_connect(GSGraph* sgraph,
                 GSGraph* other_sgraph)
{
  guint iter;
  GSGraphArray* s_n;
  
  g_return_val_if_fail(sgraph != NULL, FALSE);
  g_return_val_if_fail(other_sgraph != NULL, FALSE);
  
  s_n = sgraph->neighbours;
  for (iter = 0; iter < s_n->len; iter++)
  {
    GSGraph* node = g_sgraph_array_index(s_n, iter);
    if (node == other_sgraph)
    {
      return FALSE;
    }
  }
  g_sgraph_array_add(s_n, other_sgraph);
  g_sgraph_array_add(other_sgraph->neighbours, sgraph);
  return TRUE;
}

GSGraphArray*
g_sgraph_remove(GSGraph* sgraph)
{
  GSGraphArray* s_n;
  GSGraphArray* separate_sgraphs;
  guint iter;
  
  g_return_val_if_fail(sgraph != NULL);
  
  separate_sgraphs = g_sgraph_array_new();
  s_n = sgraph->neighbours;
  for (iter = 0; iter < s_n->len; iter++)
  {
    GSGraph* other_sgraph = g_sgraph_array_index(s_n, iter);
    if (g_sgraph_break_connection(sgraph, other_sgraph))
    {
      g_sgraph_array_add(separate_sgraphs, other_sgraph);
    }
  }
  g_sgraph_free(sgraph);
  return separate_sgraphs;
}

void
g_sgraph_free(GSGraph* sgraph)
{
  GSGraphArray* sgraph_array;
  guint iter;
  
  if (!sgraph)
  {
    return;
  }
  
  sgraph_array = _g_sgraph_array(sgraph);
  for (iter = 0; iter < sgraph_array->len; iter++)
  {
    GSGraph* node = g_sgraph_array_index(sgraph_array, iter);
    g_sgraph_array_free(node->neighbours, TRUE);
    g_slice_free(GSGraph, node);
  }
  g_sgraph_array_free(sgraph_array, TRUE);
}

guint
g_sgraph_count(GSGraph* sgraph)
{
  GSGraphArray* sgraph_array;
  guint count;
  
  g_return_val_if_fail(sgraph != NULL, 0);
  
  sgraph_array = _g_sgraph_array(sgraph);
  count = sgraph_array->len;
  g_sgraph_array_free(sgraph_array, TRUE);
  return count;
}

void
g_sgraph_foreach(GSGraph* sgraph,
                 GFunc func,
                 gpointer user_data)
{
  GSGraphArray* sgraph_array;
  guint iter;
  
  g_return_if_fail(sgraph != NULL);
  g_return_if_fail(func != NULL);
  
  sgraph_array = _g_sgraph_array(sgraph);
  for (iter = 0; iter < sgraph_array->len; iter++)
  {
    GSGraph* node = g_sgraph_array_index(sgraph_array, iter);
    (*func)(node->data, user_data);
  }
  g_sgraph_array_free(sgraph_array, TRUE);
}

gboolean
g_sgraph_break_connection(GSGraph* sgraph,
                          GSGraph* other_sgraph)
{
  GSGraphArray* s_n;
  GSGraphArray* o_n;
  guint iter;
  gboolean found = FALSE;
  
  g_return_val_if_fail(sgraph != NULL, FALSE);
  g_return_val_if_fail(other_sgraph != NULL, FALSE);
  
  s_n = sgraph->neighbours;
  for (iter = 0; iter < s_n->len; iter++)
  {
    GSGraph* node = g_sgraph_array_index(s_n, iter);
    if (node == other_sgraph)
    {
      g_sgraph_array_remove_index(s_n, iter);
      found = TRUE;
      break;
    }
  }
  if (!found)
  {
    return FALSE;
  }
  o_n = other_sgraph->neighbours;
  for (iter = 0; iter < o_n->len; iter++)
  {
    GSGraph* node = g_sgraph_array_index(o_n, iter);
    if (node == sgraph)
    {
      g_sgraph_array_remove_index(o_n, iter);
      break;
    }
  }
  
  return _g_sgraph_no_indirect_connection(sgraph, other_sgraph);
}

GSGraphArray*
g_sgraph_find(GSGraph* sgraph,
              gconstpointer data)
{
  return g_sgraph_find_custom(sgraph, data, g_direct_equal);
}

GSGraphArray*
g_sgraph_find_custom(GSGraph* sgraph,
                     gconstpointer data,
                     GEqualFunc func)
{
  GSGraphArray* matching_sgraphs;
  GSGraphArray* sgraph_array;
  guint iter;
  
  g_return_val_if_fail(sgraph != NULL, NULL);
  g_return_val_if_fail(func != NULL, NULL);
  
  matching_sgraphs = g_sgraph_array_new();
  sgraph_array = _g_sgraph_array(sgraph);
  for (iter = 0; iter < sgraph_array->len; iter++)
  {
    GSGraph* node = g_sgraph_array_index(sgraph_array, iter);
    if ((*func)(node->data, data))
    {
      g_sgraph_array_add(matching_sgraphs, node);
    }
  }
  g_sgraph_array_free(sgraph_array, TRUE);
  if (!matching_sgraphs->len)
  {
    g_sgraph_array_free(matching_sgraphs, TRUE);
    matching_sgraphs = NULL;
  }
  return matching_sgraphs;
}

gpointer
g_sgraph_data(GSGraph* sgraph)
{
  return sgraph->data;
}

GSGraphArray*
g_sgraph_neighbours(GSGraph* sgraph)
{
  return sgraph->neighbours;
}

// static function definitions

static gboolean
_g_sgraph_no_indirect_connection(GSGraph* sgraph,
                                 GSGraph* other_sgraph)
{
  GHashTable* visited_nodes = g_hash_table_new(NULL, NULL);
  gboolean not_connected = !_g_sgraph_recurrent_connection_check(sgraph,
                                                                 other_sgraph,
                                                                 visited_nodes);
  g_hash_table_unref(visited_nodes);
  return not_connected;
}

static gboolean
_g_sgraph_recurrent_connection_check(GSGraph* sgraph,
                                     GSGraph* other_sgraph,
                                     GHashTable* visited_nodes)
{
  guint iter;
  GSGraphArray* s_n;
  if (sgraph == other_sgraph)
  {
    return TRUE;
  }
  if (g_hash_table_lookup_extended(visited_nodes, sgraph, NULL, NULL))
  {
    return FALSE;
  }
  g_hash_table_insert(visited_nodes, sgraph, TRUE);
  s_n = sgraph->neighbours;
  for (iter = 0; iter < s_n->len; iter++)
  {
    GSGraph* node = g_sgraph_array_index(s_n, iter);
    if (_g_sgraph_recurrent_connection_check(node, other_sgraph, visited_nodes))
    {
      return TRUE;
    }
  }
  return FALSE;
}

static GSGraphArray*
_g_sgraph_array(GSGraph* sgraph)
{
  GHashTable* visited_nodes = g_hash_table_new(NULL, NULL);
  GSGraphArray* sgraph_array = g_sgraph_array_new();
  _g_sgraph_recurrent_array_append(sgraph, sgraph_array, visited_nodes);
  g_hash_table_unref(visited_nodes);
  return sgraph_array;
}

static void
_g_sgraph_recurrent_array_append(GSGraph* sgraph,
                                 GSGraphArray* sgraph_array,
                                 GHashTable* visited_nodes)
{
  guint iter;
  GSGraphArray* s_n;
  if (g_hash_table_lookup_extended(visited_nodes, sgraph, NULL, NULL))
  {
    return;
  }
  g_hash_table_insert(visited_nodes, sgraph, TRUE);
  g_sgraph_array_add(sgraph_array, sgraph);
  s_n = sgraph->neighbours;
  for (iter = 0; iter < s_n->len; iter++)
  {
    GSGraph* node = g_sgraph_array_index(s_n, iter);
    _g_sgraph_recurrent_array_append(node, sgraph_array, visited_nodes);
  }
}

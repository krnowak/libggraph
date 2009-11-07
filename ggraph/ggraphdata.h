#if defined(G_DISABLE_SINGLE_INCLUDES) && !defined (_LIB_G_GRAPH_H_INSIDE_) && !defined (_LIB_G_GRAPH_COMPILATION_)
#error "Only <libggraph.h> can be included directly."
#endif

#ifndef _G_GRAPH_DATA_H_
#define _G_GRAPH_DATA_H_

#include <glib.h>
#include <glib-object.h>

#include <libggraph/ggraphtypes.h>

G_BEGIN_DECLS

#define G_TYPE_GRAPH_DATA (g_graph_data_get_type())
#define G_GRAPH_DATA(object) (G_TYPE_CHECK_INSTANCE_CAST((object), G_TYPE_GRAPH_DATA, GGraphData))
#define G_GRAPH_DATA_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), G_TYPE_GRAPH_DATA, GGraphDataClass))
#define G_IS_GRAPH_DATA(object) (G_TYPE_CHECK_INSTANCE_TYPE((object), G_TYPE_GRAPH_DATA))
#define G_IS_GRAPH_DATA_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), G_TYPE_GRAPH_DATA))
#define G_GRAPH_DATA_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), G_TYPE_GRAPH_DATA, GGraphDataClass))

/**
 * GGraphData:
 * @first: first end of this edge.
 * @second: second end of this edge.
 * @connected: flag describing interconnection between @first and @second.
 * @data: data contained by this edge.
 *
 * An edge from the node containing it to the @graph.
 */

typedef struct _GGraphDataPrivate GGraphDataPrivate;

struct _GGraphData
{
  GObject parent;
  
  GGraphDataPrivate* priv;
};

struct _GGraphDataClass
{
  GObjectClass parent_class;
};

GType
g_graph_data_get_type(void) G_GNUC_CONST;

GGraphData*
g_graph_data_new(GValue* value,
                 GGraph* graph);

GType
g_graph_data_get_data_type(GGraphData* data);

const GValue*
g_graph_data_get_data(GGraphData* data);

void
g_graph_data_set_data(GGraphData* data,
                      const GValue* value);

GGraph*
g_graph_data_get_graph(GGraphData* data);

void
g_graph_data_set_graph(GGraphData* data,
                       GGraph* graph);

gboolean
g_graph_data_get_marked(GGraphData* data);

void
g_graph_data_set_marked(GGraphData* data,
                        gboolean marked);

GGraph*
g_graph_data_get_marked_graph(GGraphData* data);

void
g_graph_data_set_marked_graph(GGraphData* data,
                              GGraph* graph);

G_END_DECLS

#endif /* _G_GRAPH_EDGE_H_ */

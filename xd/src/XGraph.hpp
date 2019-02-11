#ifndef X_GRAPH_HPP
#define X_GRAPH_HPP

#include <cassert>
#include "XList.hpp"
#include "XPair.hpp"


template<typename V, typename E>
class XGraph
{
public:
	void addVertex(const V& vertex)
	{
		assert(!m_vertices.contains(vertex));
		m_vertices.append(vertex);
	}
	bool containsVertex(const V& vertex)const { return m_vertices.contains(vertex); }

	void addEdge(const V& vertex1, const V& vertex2, const E& edge)
	{
		if (!containsVertex(vertex1))
			addVertex(vertex1);
		if (!containsVertex(vertex2))
			addVertex(vertex2);

		Edge item;
		item.vertex = xMakePair(vertex1, vertex2);
		item.edge = edge;
		m_edges.append(item);
	}

private:
	struct Edge { XPair<V, V> vertex; E edge; };
	XList<V> m_vertices;
	XList<Edge> m_edges;
};

#endif // X_GRAPH_HPP

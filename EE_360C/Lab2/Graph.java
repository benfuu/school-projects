import java.util.Map;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.List;

/**
 * Graph class that uses an adjacency list.
 * @author Ben Fu
 */
public class Graph {
    /* Each entry is one vertex and a list of all adjacent vertexes with their weight. */
    private Map<Integer, Map<Integer, Double>> adjList;

    /* Constructors */
    /* Creates an empty adjacency list. */
    public Graph() {
        adjList = new HashMap<Integer, Map<Integer, Double>>();
    }
    /* Creates an empty list of connected nodes for each node */
    public Graph(List<Vertex> nodeList) {
        this();
        for (int i = 0; i < nodeList.size(); i++) {
            adjList.put(i, new LinkedHashMap<Integer, Double>());
        }
    }
    /* Sets the edges in the adjacency list given an edge list. */
    public Graph(List<Vertex> nodeList, List<Edge> edgeList) {
        this(nodeList);
        for (Edge e : edgeList) {
            setEdge(e.getU(), e.getV(), e.getW());
        }
    }
    /* Sets the edges in the adjacency list given an edge list and a transmission range. */
    public Graph(List<Vertex> nodeList, List<Edge> edgeList, double transmissionRange) {
        this(nodeList);
        for (Edge e: edgeList) {
            setEdgeWithRange(e.getU(), e.getV(), e.getW(), nodeList, transmissionRange);
        }
    }

    /* Getters and Setters */
    /* Returns a list of edges with (index of the connect node, weight) */
    public Map<Integer, Double> getEdge(int u) {
        return adjList.get(u);
    }
    /* Sets the edge in the adjacency list. */
    public void setEdge(int u, int v, double w) {
        adjList.get(u).put(v, w);
        adjList.get(v).put(u, w);
    }
    /* Sets the edge given a transmission range. Sets the weight to infinity if the distance > transmission range. */
    public void setEdgeWithRange(int u, int v, double w, List<Vertex> nodeList, double transmissionRange) {
        Vertex v1 = nodeList.get(u);
        Vertex v2 = nodeList.get(v);
        if (v1.distance(v2) <= transmissionRange) {
            adjList.get(u).put(v, w);
            adjList.get(v).put(u, w);
        }
        else {
            adjList.get(u).put(v, Double.POSITIVE_INFINITY);
            adjList.get(v).put(u, Double.POSITIVE_INFINITY);
        }
    }

    /* Useful Helper Functions */
    public boolean hasNode(int u) {
        return adjList.containsKey(u);
    }

    public boolean hasEdge(int u, int v) {
        return adjList.get(u).containsKey(v);
    }

    public int size() {
        return adjList.size();
    }

}

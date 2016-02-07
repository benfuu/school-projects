/*
 * Name: Ben Fu
 * EID: byf69
 */

import java.util.*;

/* Your solution goes in this file.
 *
 * Please do not modify the other files we have provided for you, as we will use
 * our own versions of those files when grading your project. You are
 * responsible for ensuring that your solution works with the original version
 * of all the other files we have provided for you.
 * 
 * That said, please feel free to add additional files and classes to your
 * solution, as you see fit. We will use ALL of your additional files when
 * grading your solution.
 */

public class Program2 extends VertexNetwork {
    /* DO NOT FORGET to add a graph representation and 
       any other fields and/or methods that you think 
       will be useful. 
       DO NOT FORGET to modify the constructors when you 
       add new fields to the Program2 class. */
    
    Program2() {
        super();
    }
    
    Program2(String locationFile) {
        super(locationFile);
    }
    
    Program2(String locationFile, double transmissionRange) {
        super(locationFile, transmissionRange);
    }
    
    Program2(double transmissionRange, String locationFile) {
        super(transmissionRange, locationFile);
    }

    /**
     * This method returns a path from a source at location sourceIndex
     * and a sink at location sinkIndex using the GPSR algorithm. An empty
     * path is returned if the GPSR algorithm fails to find a path.
     * @param sourceIndex The index of the source.
     * @param sinkIndex The index of the sink.
     * @return A path to the sink using the GPSR algorithm (includes all nodes but the source).
     */
    public ArrayList<Vertex> gpsrPath(int sourceIndex, int sinkIndex) {
        /*
        System.out.println("nodes:\t" + this.location);
        System.out.println("edges:\t" + this.edges);
        System.out.println("source:\t" + sourceIndex);
        System.out.println("sink:\t" + sinkIndex);
        */
        /* Create Graph */
        Graph G = new Graph(location, edges, transmissionRange);
        /* Initializations */
        ArrayList<Vertex> result = new ArrayList<Vertex>();
        result.add(location.get(sourceIndex));
        HashSet<Integer> visited = new HashSet<Integer>();
        int curNode = sourceIndex;
        while (curNode != sinkIndex) {  // keep trying until failure or sink is reached
            //System.out.println(curNode);
            visited.add(curNode);
            double curDist = location.get(curNode).distance(location.get(sinkIndex));
            double minDist = Double.POSITIVE_INFINITY;
            int minIndex = -1;
            Map<Integer, Double> neighbors = G.getEdge(curNode);
            if (neighbors.size() == 0) {    // if there are no neighbors in range, fail
                result.clear();
                break;
            }
            for (Map.Entry<Integer, Double> entry : neighbors.entrySet()) {  // iterate through all neighbors
                int neighborIndex = entry.getKey();
                double neighborDist = entry.getValue();
                double sinkDist = location.get(neighborIndex).distance(location.get(sinkIndex));  // distance between neighbor and sink
                if (sinkDist < minDist && sinkDist < curDist && neighborDist < Double.POSITIVE_INFINITY) {   // found a node with a smaller distance to the sink
                    minDist = sinkDist;
                    minIndex = neighborIndex;
                }
            }
            if (minIndex == -1) {  // could not find another node, so fail
                result.clear();
                break;
            }
            result.add(location.get(minIndex));
            curNode = minIndex;
        }
        /*
        for (Vertex v : result) {
            System.out.print(v.getNodeNumber() + ", ");
        }
        */
        return result;
    }

    /**
     * This method returns a path (shortest in terms of latency) from a source at
     * location sourceIndex and a sink at location sinkIndex using Dijkstra's algorithm.
     * An empty path is returned if Dijkstra's algorithm fails to find a path.
     * @param sourceIndex The index of the source.
     * @param sinkIndex The index of the sink.
     * @return The minimum path to the sink (includes all nodes but the source).
     */
    public ArrayList<Vertex> dijkstraPathLatency(int sourceIndex, int sinkIndex) {
        /* Create Graph */
        Graph G = new Graph(this.location, this.edges, this.transmissionRange);
        /*
        System.out.println("nodes:\t" + nodes);
        System.out.println("edges:\t" + edges);
        System.out.println("source:\t" + sourceIndex);
        System.out.println("sink:\t" + sinkIndex);
        */
        /* Initializations */
        ArrayList<Vertex> result = new ArrayList<Vertex>();
        HashSet<Integer> visited = new HashSet<Integer>();
        HashMap<Integer, Double> distance = new HashMap<Integer, Double>();
        HashMap<Integer, Integer> previous = new HashMap<Integer, Integer>();
        PriorityQueue<DVertex> unvisited = new PriorityQueue<DVertex>();
        for (int i = 0; i < G.size(); i++) {    // initialize all distances to infinity (-1) and all previous nodes to null
            if (i == sourceIndex) { // add the source to the univisited queue and set its distance to 0
                unvisited.add(new DVertex(i, 0.0));
            }
            else {  // set all other distances to infinity
                unvisited.add(new DVertex(i, Double.POSITIVE_INFINITY));
            }
            distance.put(i, Double.POSITIVE_INFINITY);
            previous.put(i, null);
        }
        distance.put(sourceIndex, 0.0); // the distance from source to source is 0
        while (!unvisited.isEmpty()) {  // while there are still nodes to visit
            int curNode = unvisited.poll().getIndex();  // choose node with the smallest distance
            visited.add(curNode);
            double curDistance = distance.get(curNode);
            if (curDistance == Double.POSITIVE_INFINITY) {    // if the smallest distance is infinity, dead end
                break;
            }
            if (curNode == sinkIndex) { // reached the sink, so break
                break;
            }
            /* Find the neighboring nodes */
            Map<Integer, Double> neighbors = G.getEdge(curNode);
            for (Map.Entry<Integer, Double> entry : neighbors.entrySet()) {
                int neighborIndex = entry.getKey();
                double neighborDist = entry.getValue();
                double nextDist = curDistance + neighborDist;
                if (nextDist < distance.get(neighborIndex)) {   // relax the current edge
                    distance.put(neighborIndex, nextDist);
                    previous.put(neighborIndex, curNode);
                    unvisited.remove(new DVertex(neighborIndex, 0.0));
                    unvisited.add(new DVertex(neighborIndex, nextDist));
                }
            }
        }
        if (distance.get(sinkIndex) == Double.POSITIVE_INFINITY) {
            result.clear();
            return result;
        }
        int reverseNode = sinkIndex;
        while (reverseNode != sourceIndex) {    // backtrack to find the full path
            result.add(location.get(reverseNode));
            if (previous.get(reverseNode) == null) {
                break;
            }
            reverseNode = previous.get(reverseNode);
        }
        result.add(location.get(sourceIndex));
        Collections.reverse(result);
        /*
        for (Vertex v : result) {
            System.out.print(v.getNodeNumber() + ", ");
        }
        System.out.println(distance);
        */
        return result;
    }

    /**
     * This method returns a path (shortest in terms of hops) from a source at
     * location sourceIndex and a sink at location sinkIndex using Dijkstra's algorithm.
     * An empty path is returned if Dijkstra's algorithm fails to find a path.
     * @param sourceIndex The index of the source.
     * @param sinkIndex The index of the sink.
     * @return The minimum path to the sink (includes all nodes but the source).
     */
    public ArrayList<Vertex> dijkstraPathHops(int sourceIndex, int sinkIndex) {
        /*
        System.out.println("nodes:\t" + this.location);
        System.out.println("edges:\t" + this.edges);
        System.out.println("source:\t" + sourceIndex);
        System.out.println("sink:\t" + sinkIndex);
        */
        /* Create Graph */
        Graph G = new Graph(this.location, this.edges, this.transmissionRange);
        /* Initializations */
        ArrayList<Vertex> result = new ArrayList<Vertex>();
        HashSet<Integer> visited = new HashSet<Integer>();
        HashMap<Integer, Double> distance = new HashMap<Integer, Double>();
        HashMap<Integer, Integer> previous = new HashMap<Integer, Integer>();
        PriorityQueue<DVertex> unvisited = new PriorityQueue<DVertex>();
        for (int i = 0; i < G.size(); i++) {    // initialize all distances to infinity (-1) and all previous nodes to null
            if (i == sourceIndex) { // add the source to the univisited queue and set its distance to 0
                unvisited.add(new DVertex(i, 0.0));
            }
            else {  // set all other distances to infinity
                unvisited.add(new DVertex(i, Double.POSITIVE_INFINITY));
            }
            distance.put(i, Double.POSITIVE_INFINITY);
            previous.put(i, null);
        }
        distance.put(sourceIndex, 0.0); // the distance from source to source is 0
        while (!unvisited.isEmpty()) {  // while there are still nodes to visit
            int curNode = unvisited.poll().getIndex();  // choose node with the smallest distance
            visited.add(curNode);
            double curDistance = distance.get(curNode);
            if (distance.get(curNode) == Double.POSITIVE_INFINITY) {    // if the smallest distance is infinity, dead end
                break;
            }
            if (curNode == sinkIndex) { // reached the sink, so break
                break;
            }
            /* Find the neighboring nodes */
            Map<Integer, Double> neighbors = G.getEdge(curNode);
            for (Map.Entry<Integer, Double> entry : neighbors.entrySet()) {
                int neighborIndex = entry.getKey();
                double neighborDist = entry.getValue();
                double nextDist = curDistance + 1;              // distance is always 1
                if (nextDist < distance.get(neighborIndex) && neighborDist < Double.POSITIVE_INFINITY) {   // relax the current edge
                    distance.put(neighborIndex, nextDist);
                    previous.put(neighborIndex, curNode);
                    unvisited.remove(new DVertex(neighborIndex, 0.0));
                    unvisited.add(new DVertex(neighborIndex, nextDist));
                }
            }
        }
        if (distance.get(sinkIndex) == Double.POSITIVE_INFINITY) {
            result.clear();
            return result;
        }
        int reverseNode = sinkIndex;
        while (reverseNode != sourceIndex) {    // backtrack to find the full path
            result.add(location.get(reverseNode));
            if (previous.get(reverseNode) == null) {
                break;
            }
            reverseNode = previous.get(reverseNode);
        }
        result.add(location.get(sourceIndex));
        Collections.reverse(result);
        return result;
    }

}


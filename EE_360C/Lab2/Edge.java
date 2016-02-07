public class Edge {
    
    private int u; // index of vertex u.
    private int v; // index of vertex v.
    private double w; // weight of undirected edge {u,v}
    
    Edge() {
        /* This constructor sets vertex u to 0, v to 1, and w to 0.0 */
        this(0, 1, 0.0);
    }
    
    Edge(int u, int v, double w) {
        /* This constructor sets the u-vertex to u, 
           v-vertex to v, and weight to w */
        this.u = u;
        this.v = v;
	this.w = w;
    }
    
    public void setU(int u) {
        /* This method sets the u-vertex to u. */
        this.u = u;
    }
    
    public void setV(int v) {
        /* This method sets the v-vertex to v. */
        this.v = v;
    }
    
    public void setW(double w) {
        /* This method sets the weight to w. */
        this.w = w;
    }
    
    public int getU() {
        /* This method returns the u-vertex. */
        return u;
    }
    
    public int getV() {
        /* This method returns the v-vertex. */
        return v;
    }
    
    public double getW() {
        /* This method returns the weight. */
        return w;
    }
    
    public String toString() {
        /* This method returns a string representation 
           of the edge. */
        return "(" + u + "," + v + "," + w + ")";
    }
    
}


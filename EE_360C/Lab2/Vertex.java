public class Vertex {
    /* FEEL FREE to add any fields and/or methods that you 
       think will be useful to the Vertex class. 
       DO NOT FORGET to modify the constructors when you add 
       new fields to the Vertex class. */
    
    private double x; // x-coordinate of the vertex.
    private double y; // y-coordinate of the vertex.
    private double tempDistance; // distance so far for dijkstra's
    private Vertex prevNode; // previous shortest path for dijkstra's
    private int nodeNumber; // node number for easy access to adjacency matrix
    
    Vertex() {
        /* This constructor sets both coordinates to 0.0. */
        this(0.0, 0.0, -1);
    }
    Vertex(double x, double y) {
        /* This constructor sets the x-coordinate to x 
           and the y-coordinate to y. */
        this.x = x;
        this.y = y;
    }
    
    Vertex(double x, double y, int num) {
        /* This constructor sets the x-coordinate to x 
           and the y-coordinate to y. */
        this.x = x;
        this.y = y;
        tempDistance = -1;
        nodeNumber = num;
    }
    
    public void setX(double x) {
        /* This method sets the x-coordinate to x. */
        this.x = x;
    }
    
    public void setY(double y) {
        /* This method sets the y-coordinate to y. */
        this.y = y;
    }
    
    public void setXY(double x, double y) {
        /* This method sets the x-coordinate to x 
           and the y-coordinate to y. */
        this.x = x;
        this.y = y;
    }
    
    public double getX() {
        /* This method returns the x-coordinate. */
        return x;
    }
    
    public double getY() {
        /* This method returns the y-coordinate. */
        return y;
    }
    
    public double distance(Vertex v) {
        /* This method returns the Euclidean distance 
           between the vertex and another vertex v. */
        return Math.sqrt(Math.pow(x - v.getX(), 2) + Math.pow(y - v.getY(), 2));
    }
    
    public String toString() {
        /* This method returns a string representation 
           of the vertex. */
        return "(" + x + "," + y + ")";
    }
    
    public double getTempDistance() {
        return tempDistance;
    }
    
    public void setTempDistance(double newDistance){
        if (newDistance < tempDistance || tempDistance < 0)
            tempDistance = newDistance;
    }
    
    public Vertex getPrevNode() {
        return prevNode;
    }
    
    public void setPrevNode(Vertex previous){
        prevNode = previous;
    }
    
    public int getNodeNumber() {
        return nodeNumber;
    }
    
    public void setNodeNumber(int num){
        nodeNumber = num;
    }
    public boolean equals(Vertex A){
	if((x== A.getX()) && (y == A.getY())){
		return true;
	}else{
		return false;
	}
    }
}


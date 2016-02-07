import java.util.Comparator;

/**
 * Vertex with Distance (DVertex) class. This class extends the Vertex class by adding the distance field. The distance
 * represents the distance to the current DVertex node from an arbitrary source node.
 *
 * Includes the necessary overrides to be implemented in primitive data structures.
 *
 * @author Ben Fu
 */
public class DVertex extends Vertex implements Comparator<DVertex>, Comparable<DVertex> {

    private int index;          // the index of the node (same as this.location.indexOf(DVertex))
    private double distance;    // distance to this node from the source

    /* Constructors */
    public DVertex(int i, double d) {
        this.index = i;
        this.distance = d;
    }

    /* Getters and Setters */
    public int getIndex() {
        return this.index;
    }
    public double getDistance() {
        return this.distance;
    }
    public void setIndex(int i) {
        this.index = i;
    }
    public void setDistance(double d) {
        this.distance = d;
    }

    /* Overrides for implementation with Java data structures such as Priority Queue and HashMap. */
    @Override
    /* Compare nodes by their distance. */
    public int compare(DVertex u, DVertex v) {
        if (u.getDistance() < v.getDistance()) {
            return -1;
        }
        if (u.getDistance() > u.getDistance()) {
            return 1;
        }
        return 0;
    }

    @Override
    /* Compare nodes by their distance. */
    public int compareTo(DVertex v) {
        if (this.getDistance() < v.getDistance()) {
            return -1;
        }
        if (this.getDistance() > v.getDistance()) {
            return 1;
        }
        return 0;
    }

    @Override
    public int hashCode() {
        int hash = 1;
        hash = 53 * hash + this.index;
        return hash;
    }

    @Override
    /* Two nodes are equal if their index is the same. */
    public boolean equals(Object obj) {
        if (!(obj instanceof DVertex)) {
            return false;
        }
        DVertex v = (DVertex) obj;
        if (this.index == v.getIndex()) {
            return true;
        }
        return false;
    }
}

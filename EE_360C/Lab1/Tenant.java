import java.util.ArrayList;
import java.util.Comparator;

/**
 * Tenant class for Program 1.
 */
public class Tenant {
    /** The tenant's id. */
    private Integer id;
    /** The apartment that the tenant is matched to. */
    private Apt apt;
    /** The history of attempted matches. */
    private ArrayList<Integer> history;

    /** Default constructor. */
    public Tenant(Integer id) {
        this.id = id;
        this.apt = null;
        this.history = new ArrayList<Integer>();
    }

    @Override
    public boolean equals(Object obj) {
        if (obj instanceof Tenant) {
            Tenant t = (Tenant) obj;
            return this.id.equals(t.getId());
        }
        return false;
    }

    @Override
    public int hashCode() {
        return id.hashCode();
    }

    // getters and setters
    public Integer getId() { return this.id; }
    public Apt getApt() { return this.apt; }
    public ArrayList<Integer> getHistory() { return this.history; }
    public void setApt(Apt apt) {
        this.apt = apt;
    }
    public void addHistory(Integer attempt) { this.history.add(attempt); }

    /** Checks if current attempt is already in the history. */
    public Boolean inHistory(Integer attempt) { return this.history.contains(attempt); }
}


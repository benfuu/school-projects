/**
 * Apt class for Program 1.
 */
public class Apt {
    /** The apartment's id. */
    private Integer id;
    /** The tenant that the apartment is matched to. */
    private Tenant tenant;

    /** Default constructor. */
    public Apt(Integer id) {
        this.id = id;
        this.tenant = null;
    }

    @Override
    public boolean equals(Object obj) {
        if (obj instanceof Apt) {
            Apt a = (Apt) obj;
            return this.id.equals(a.getId());
        }
        return false;
    }

    @Override
    public int hashCode() {
        return id.hashCode();
    }

    // getters and setters
    public Integer getId() { return this.id; }
    public Tenant getTenant() { return this.tenant; }
    public void setTenant(Tenant tenant) {
        this.tenant = tenant;
    }


}


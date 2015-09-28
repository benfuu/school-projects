public abstract class AbstractProgram1 {
    public abstract boolean isStableMatching(Matching given_matching);

    /**
     * Brute force solution to the Stable Marriage problem. Relies on the
     * function isStableMatching(Matching) to determine whether a candidate
     * Matching is stable.
     * 
     * @return A stable Matching.
     */
    public Matching stableMatchingBruteForce(Matching given_matching) {
        int l = given_matching.getLandlordCount();
        int n = given_matching.getTenantCount();

        Permutation p = new Permutation(n,n);

        Matching matching;
        while ((matching = p.getNextMatching(given_matching)) != null) {
            if (isStableMatching(matching) == true) {
                return matching;
            }
        }

        return new Matching(given_matching);
    }

    public abstract Matching stableMatchingGaleShapley(Matching given_matching);
}

/*
 * Name:    Ben Fu
 * EID:     byf69
 */

import java.lang.reflect.Array;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.LinkedList;

/**
 * Your solution goes in this class.
 * <p>
 * Please do not modify the other files we have provided for you, as we will use
 * our own versions of those files when grading your project. You are
 * responsible for ensuring that your solution works with the original version
 * of all the other files we have provided for you.
 * <p>
 * That said, please feel free to add additional files and classes to your
 * solution, as you see fit. We will use ALL of your additional files when
 * grading your solution.
 */
public class Program1 extends AbstractProgram1 {
    /**
     * Determines whether a candidate Matching represents a solution to the
     * Stable Matching problem. Study the description of a Matching in the
     * project documentation to help you with this.
     */
    public boolean isStableMatching(Matching given_matching) {
        // get all the values of Matching into local variables
        Integer l = given_matching.getLandlordCount();
        Integer n = given_matching.getTenantCount();
        ArrayList<ArrayList<Integer>> landlord_own = given_matching.getLandlordOwners();
        ArrayList<ArrayList<Integer>> landlord_pref = given_matching.getLandlordPref();
        ArrayList<ArrayList<Integer>> tenant_pref = given_matching.getTenantPref();
        ArrayList<Integer> tenant_matching = given_matching.getTenantMatching();
        // create a preference list of the apartments
        ArrayList<ArrayList<Integer>> apt_pref = new ArrayList<ArrayList<Integer>>();
        apt_pref = createAptPref(given_matching);
        /*
        System.out.println("Apt Pref:\t\t" + apt_pref);
        System.out.println("Tenant pref:\t" + tenant_pref);
        System.out.println("Tenant matching:\t" + tenant_matching);
        */

        // empty tenant matching check
        if (tenant_matching.size() == 0) {
            System.out.println("Sorry, tenant matching is empty!");
            return false;
        }
        // now check for unstable matches
        for (int i = 0; i < n; i++) {   // i is the current tenant being tested
            Integer tenant_index = i;
            Integer apt_index = tenant_matching.get(tenant_index);
            // find the preferences for current tenant
            ArrayList<Integer> cur_tenant_pref = tenant_pref.get(i);
            Integer cur_ten_match_rank = cur_tenant_pref.get(apt_index);
            // now find all the apts that the current tenant prefers to his match (if any)
            ArrayList<Integer> better_rank = indexOfBetterMatches(cur_tenant_pref, cur_ten_match_rank);
            //
            // if there were other apts that the current tenant prefers, now check through all of those apts
            for (int j = 0; j < better_rank.size(); j++) {
                Integer better_rank_index = better_rank.get(j); // get the rank of the current tenant for that apt
                ArrayList<Integer> cur_apt_pref = apt_pref.get(better_rank_index);
                Integer match_tenant_index = tenant_matching.indexOf(better_rank_index);
                // now see if the apt prefers the current tenant to its currently matched one
                Integer cur_apt_match_rank = cur_apt_pref.get(match_tenant_index);
                Integer cur_tenant_rank = cur_apt_pref.get(tenant_index);
                /*
                System.out.println("better_rank:\t" + better_rank);
                System.out.println("cur_apt_pref:\t" + cur_apt_pref);
                System.out.println("cur_apt_match_rank:\t" + cur_apt_match_rank);
                System.out.println("better ranks again:\t" + better_rank_again);
                */
                if (cur_tenant_rank < cur_apt_match_rank) { // if apt prefers the current tenant, match is unstable
                    // System.out.println("unstable!");
                    return false;
                }
            }

        }
        // iterated through all the matches and no unstable matches were found, so current match is stable/weakly stable
        // System.out.println("All matches are stable.");
        return true;
    }

    /**
     * Determines a solution to the Stable Matching problem from the given input
     * set. Study the project description to understand the variables which
     * represent the input to your solution.
     *
     * @return A stable Matching.
     */
    public Matching stableMatchingGaleShapley(Matching given_matching) {
        ArrayList<Integer> result = new ArrayList<Integer>();
        // get all the values of Matching into local variables
        Integer l = given_matching.getLandlordCount();
        Integer n = given_matching.getTenantCount();
        ArrayList<ArrayList<Integer>> landlord_own = given_matching.getLandlordOwners();
        ArrayList<ArrayList<Integer>> landlord_pref = given_matching.getLandlordPref();
        ArrayList<ArrayList<Integer>> tenant_pref = given_matching.getTenantPref();
        ArrayList<Integer> tenant_matching = given_matching.getTenantMatching();
        // create a preference list of the apartments
        ArrayList<ArrayList<Integer>> apt_pref = new ArrayList<ArrayList<Integer>>();
        apt_pref = createAptPref(given_matching);
        /*
        System.out.println("Apt Pref:\t\t" + apt_pref);
        System.out.println("Tenant pref:\t" + tenant_pref);
        */
        // Implement the Gale-Shapley algorithm. Let's pretend the tenants are the men and apts are the women.
        Integer top_pref_rank = 2;   // the top preference of each tenant starts at 2 since indexOfBetterMatches uses <
        // create arrays to hold the matched tenants and apts
        ArrayList<Tenant> matched_tenants = new ArrayList<Tenant>();
        ArrayList<Apt> matched_apts = new ArrayList<Apt>();
        // create and initialize an array of all the free tenants
        ArrayList<Tenant> free_tenants = new ArrayList<Tenant>();
        for (int i = 0; i < n; i++) {
            Tenant t = new Tenant(i);
            free_tenants.add(t);
        }
        Integer cur_tenant_id;
        while (free_tenants.size() > 0) {   // i is the current tenant being examined
            Tenant cur_tenant = free_tenants.get(0);
            cur_tenant_id = cur_tenant.getId();
            ArrayList<Integer> cur_tenant_pref = tenant_pref.get(cur_tenant_id);
            ArrayList<Integer> top_pref = indexOfBetterMatches(cur_tenant_pref, top_pref_rank);
            // System.out.println("top preferences:\t" + top_pref);
            // try a match for the tenant's top preferences
            int num_matches = 0;    // hold the number of matches that have been completed
            for (int i = 0; i < top_pref.size(); i++) {
                Integer cur_apt_id = top_pref.get(i);
                Apt cur_apt = new Apt(cur_apt_id);
                if (cur_tenant.inHistory(cur_apt_id)) { // already tried matching, so skip current apt
                    continue;
                }
                else {
                    cur_tenant.addHistory(cur_apt_id);  // save the attempted match
                }
                if (!matched_apts.contains(cur_apt)) {  // if not already matched, match the apt with tenant
                    cur_tenant.setApt(cur_apt);
                    cur_apt.setTenant(cur_tenant);
                    matched_tenants.add(cur_tenant);
                    matched_apts.add(cur_apt);
                    free_tenants.remove(cur_tenant);
                    // reset counters
                    top_pref_rank = 2;
                    num_matches++;
                    break;
                }
            }
            if (num_matches == 0) { // all of the tenant's top apts have been matched, so check for stability
                for (int i = 0; i < top_pref.size(); i++) {
                    Integer cur_apt_id = top_pref.get(i);
                    Tenant matched_tenant = null;
                    Integer matched_tenant_id = null;
                    Apt cur_apt = null;
                    for (Apt a : matched_apts) {
                        if (a.getId().equals(cur_apt_id)) {
                            matched_tenant = a.getTenant();
                            matched_tenant_id = matched_tenant.getId();
                            cur_apt = a;
                            break;
                        }
                    }
                    ArrayList<Integer> cur_apt_pref = apt_pref.get(cur_apt_id);
                    Integer cur_tenant_rank = cur_apt_pref.get(cur_tenant_id);
                    Integer matched_tenant_rank = cur_apt_pref.get(matched_tenant_id);
                    if (cur_tenant_rank < matched_tenant_rank) {    // apt prefers current tenant
                        cur_tenant.setApt(cur_apt);
                        cur_apt.setTenant(cur_tenant);
                        matched_tenants.add(cur_tenant);
                        free_tenants.remove(cur_tenant);
                        // free the original match
                        matched_tenant.setApt(null);
                        matched_tenants.remove(matched_tenant);
                        free_tenants.add(matched_tenant);
                        // reset counters
                        top_pref_rank = 2;
                        num_matches++;
                        break;
                    }
                }
                // if all of the tenant's top apts are stably matched, then look for next preference
                if (num_matches == 0) {
                    top_pref_rank++;
                }
            }
            /*
            System.out.println("Matched Tenants:");
            for (Tenant t : matched_tenants) {
                System.out.println(t.getId() + ", " + t.getApt().getId());
            }
            System.out.println("Matched Apartments:");
            for (Apt a : matched_apts) {
                System.out.println(a.getId() + ", " + a.getTenant().getId());
            }
            */

        }

        // create a comparator to sort the matched tenants
        Comparator<Tenant> tenant_comparator = new Comparator<Tenant>() {
            public int compare(Tenant t1, Tenant t2) {
                return t1.getId() - t2.getId();
            }
        };
        Collections.sort(matched_tenants, tenant_comparator);
        for (Tenant t : matched_tenants) {   // add all the matches to the result
            result.add(t.getApt().getId());
        }
        // System.out.println("The result is:\t" + result);
        given_matching.setTenantMatching(result);
        return given_matching;
    }

    /**
     * Creates a list of preferences of tenants for apartments.
     * @param given_matching The given Matching object. This method only requires the landlord preferences and the
     *                       landlord ownership list.
     * @return The resulting apartment preference list.
     */
    public static ArrayList<ArrayList<Integer>> createAptPref(Matching given_matching) {
        // get all the values of Matching into local variables
        Integer n = given_matching.getTenantCount();
        ArrayList<ArrayList<Integer>> landlord_own = given_matching.getLandlordOwners();
        ArrayList<ArrayList<Integer>> landlord_pref = given_matching.getLandlordPref();
        // create a preference list of the apartments
        ArrayList<ArrayList<Integer>> apt_pref = new ArrayList<ArrayList<Integer>>();
        for (int i = 0; i < n; i++) {
            ArrayList<Integer> apt = new ArrayList<Integer>();
            int landlord = 0;
            // find which landlord owns the current apt
            for (int j = 0; j < n; j++) {
                apt = landlord_own.get(j);
                if (apt.contains(i)) {
                    landlord = j;
                    break;
                }
            }
            // now copy the landlord's pref into the apt's pref
            apt = landlord_pref.get(landlord);
            apt_pref.add(apt);
        }
        return apt_pref;
    }

    /**
     * Finds an array with the indexes of all elements that have a better (lower) rank than the given rank.
     * @param list The list of elements to be iterated.
     * @param rank The given rank. Every element that has a lower rank than the given rank is added to the result.
     * @return The resulting list of indexes.
     */
    public static ArrayList<Integer> indexOfBetterMatches(ArrayList<Integer> list, Integer rank) {
        ArrayList<Integer> result = new ArrayList<Integer>();
        for (int i = 0; i < list.size(); i++) {
            if (list.get(i) < rank) {
                result.add(i);
            }
        }
        return result;
    }
}

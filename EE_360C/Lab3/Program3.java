import java.util.ArrayList;

/**
 * Solution class to the given problem in Lab 3. Takes the number of total hours allotted and calculates the optimal
 * distribution of hours for each class.
 *
 * The matrix opt[i][h] holds the optimal max grade that can be received from spending h hours on classes 0 through i.
 * The matrix optH[i][h] holds the optimal number of hours that should be spent on class i given the total hours.
 *
 * @author Ben Fu (byf69)
 */
public class Program3 implements IProgram3 {

    private int numClasses;
    private int maxGrade;
    GradeFunction gf;

    private int[][] opt;            // to hold the optimal values
    private int[][] optH;           // to hold the optimal hours

    public Program3() {
        this.numClasses = 0;
        this.maxGrade = 0;
        this.gf = null;
    }

    public void initialize(int n, int g, GradeFunction gf) {
        this.numClasses = n;
        this.maxGrade = g;
        this.gf = gf;
        this.opt = null;
        this.optH = null;
    }

    /**
     * Computes the distribution of hours.
     * @param totalHours The total number of hours allotted.
     * @return The distribution of hours. Each index of the array corresponds to the unique Class ID, and the value
     *         of that index represents the optimal number of hours that should be spent on the class.
     */
    public int[] computeHours(int totalHours) {
        /* Initialize the opt array. */
        int optClasses = numClasses;
        int optHours = totalHours + 1;  // we need to store values from 0 to totalHours, so we need totalHours+1 columns
        this.opt = new int[optClasses][optHours];
        this.optH = new int[optClasses][optHours];
        for (int h = 0; h < optHours; h++) {
            opt[0][h] = gf.grade(0,h);  // set opt[0,h] = grade(0,h) since you're only working on class 0
            optH[0][h] = h;             // set optH[0,h] = h since you have to spend h hours on class 0 to get the max grade
        }
        int curSum = 0;
        for (int i = 0; i < optClasses; i++) {
            curSum += gf.grade(i,0);
            opt[i][0] = curSum;         // set opt[i,0] = sum of all f(j,0) from 0<=j<=i
            optH[i][0] = 0;             // set optH[i,0] = 0 since you are putting in 0 hours for each class i
        }
        for (int i = 1; i < optClasses; i++) {
            for (int h = 1; h < optHours; h++) {
                /* Here are the recursive equations we need to implement.
                opt[i,h] = max(from 0<=k<=h){grade(i,k)+opt[i-1,h-k]}
                optH[i,h] = value of k after computing the above opt[i,h]
                */
                int max = 0;
                int maxHours = 0;
                for (int k = 0; k < h + 1; k++) {   // find the max grade for all values of k
                    int curGrade = gf.grade(i, k) + opt[i-1][h-k];
                    if (curGrade > max) {
                        max = curGrade;
                        maxHours = k;
                    }
                }
                /*
                System.out.println("maxHours: " + maxHours);
                */
                opt[i][h] = max;
                optH[i][h] = maxHours;
            }
        }
        /* Now, create the optimal solution based on optH. */

        int[] result = new int[numClasses];
        int i = numClasses - 1; // last index of result
        int h = totalHours;
        while (i >= 0) {    // fill in array from i to 0
            int spendHour = optH[i][h];
            result[i] = spendHour;
            i -= 1;
            h -= spendHour;
        }
        return result;
    }

    /**
     * Computes the distribution of grades based on the optimal distribution of hours.
     * @param totalHours The total number of hours allotted.
     * @return The distribution of grades. Each element of the array represents the grade received based on spending
     *         the optimal number of hours on the class.
     */
    public int[] computeGrades(int totalHours) {
        /* Initialize the distribution arrays. Use the optimal solution calculated from computeHours */
        int[] distHours = computeHours(totalHours);     // we call computeHours to solve opt and optDist
        int[] distGrades = new int[distHours.length];
        for (int i = 0; i < distHours.length; i++) {    // calculate the grade received based on the number of hours
            distGrades[i] = gf.grade(i, distHours[i]);
        }
        return distGrades;
    }


}

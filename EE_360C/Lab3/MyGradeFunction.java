/**
 * Simple grade function that returns the grade received for spending a given number of hours and Class ID.
 */
public class MyGradeFunction implements GradeFunction {
    private int numClasses;
    private int maxGrade;
    public static final int LINEAR_OPERAND = 2;

    public MyGradeFunction(int n, int g){
        this.numClasses = n;
        this.maxGrade = g;
    }
    /**
     * Linear function that returns
     * @param classID The unique Class ID of the class.
     * @param hours The number of hours to be spent on the class.
     * @return The resulting grade.
     */
    public int grade(int classID, int hours) {
        int lin = (classID % 3 + 1) + LINEAR_OPERAND*hours;
        return Math.min(lin, maxGrade);
    }
}

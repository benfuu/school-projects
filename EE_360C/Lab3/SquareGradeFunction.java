public class SquareGradeFunction implements GradeFunction{

    private int numClasses;
    private int maxGrade;

    public SquareGradeFunction(int n, int g){
	    this.numClasses = n;
	    this.maxGrade = g;
    }

    /*
     * This grade function gives diminishing returns as
     * you put in more hours
     */
    public int grade(int classID, int hours){
    	//int root = (int) Math.round(Math.sqrt(hours));
    	int result = (hours*hours)+ (classID % 3 + 1);

    	return Math.min(result,maxGrade);
    }

}

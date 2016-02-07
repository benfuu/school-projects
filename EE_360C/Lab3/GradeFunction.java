public interface GradeFunction{

    /*
     * Returns the grade achieved for the specified class when the specified
     * number of hours are spent on it.  For a specific run, this function
     * should be guaranteed to give the SAME return value for any given 
     * combination of class, hours.
     */
    public int grade(int classID, int hours);

}
public class Driver {

    public static void main(String[] args) {
        int n=3;
        int h=5;
        if(args.length < 2)System.out.println("usage: java Driver <n> <h>");
        else{
            n=Integer.parseInt(args[0]);
            h=Integer.parseInt(args[1]);
        }
        Program3 program = new Program3();
        int sum_grade, sum_hours;
        int maxgrade = 100;
        SquareGradeFunction gf = new SquareGradeFunction(n, maxgrade);
        SquareRootGradeFunction gfr = new SquareRootGradeFunction(n, maxgrade);
        CustomGradeFunction gfc = new CustomGradeFunction(n, maxgrade);

        program.initialize(n, maxgrade, gfc);
        float curTime = System.nanoTime();
        int[] hours_array = program.computeHours(h);
        int[] grades_array = program.computeGrades(h);
        float elapsedTime = System.nanoTime() - curTime;
        sum_grade = 0; sum_hours = 0;
        for (int i=0; i<hours_array.length; i++) {
            System.out.println("Class ID " + i + " Hours " + hours_array[i] + " Grade " + grades_array[i]);
            sum_grade += grades_array[i];
            sum_hours += hours_array[i];
        }
        System.out.println("Total Hours " + sum_hours);
        System.out.println("Total Grade " + sum_grade);
        System.out.println("Total Time " + elapsedTime/1000000000);
    }
}

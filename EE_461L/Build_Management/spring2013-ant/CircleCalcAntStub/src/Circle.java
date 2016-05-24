import java.util.Scanner;



/**
 * The Circle class defines a circle specified in double precision.
 *
 */
public class Circle {
	/**
	 * The circle's immutable radius.
	 */
	private final double radius;

	/**
	 * Constructs a new Circle with the specified radius
	 * 
	 * @param radius the radius of the circle
	 */
	public Circle(double radius) {
		this.radius = radius;
	}
	
	/**
	 * Returns the radius of this Circle in double precision.
	 * 
	 * @return the radius of this Circle.
	 */
	public double getRadius() {
		return radius;
	}
	
	/**
	 * Returns the diameter of this Circle in double precision.
	 * 
	 * @return the diameter of this Circle.
	 */
	public double getDiameter() {
		return MyMath.TWO_PI * radius;
	}
	
	/**
	 * Returns the area of this Circle in double precision.
	 * 
	 * @return the area of this Circle.
	 */
	public double getArea() {
		return Math.PI * MyMath.square(radius);
	}
	
	/**
	 * Returns the length of an arc for this Circle given an angle specified in radians.
	 * 
	 * @param theta the angle in radians.
	 * @return the arc length for the given angle.
	 */
	public double getRadiansArcLength(double theta) {
		return radius * theta;
	}
	
	/**
	 * Returns the length of an arc for this Circle given an angle specified in degrees.
	 * 
	 * @param alpha the angle in degrees.
	 * @return the arc length for the given angle.
	 */
	public double getDegreesArcLength(double alpha) {
		return getRadiansArcLength(MyMath.toRadians(alpha));
	}
	
	/**
	 * Returns the area of a sector for this Circle given an angle specified in radians.
	 * 
	 * @param theta the angle in radians.
	 * @return the sector area for the given angle.
	 */
	public double getRadiansSectorArea(double theta) {
		return MyMath.square(radius) * theta / 2;
	}
	
	/**
	 * Returns the area of a sector for this Circle given an angle specified in degrees.
	 * 
	 * @param alpha the angle in degrees.
	 * @return the sector area for the given angle.
	 */
	public double getDegreesSectorArea(double alpha) {
		return getRadiansSectorArea(MyMath.toRadians(alpha)); 
	}

	/**
	 * Main method that tests the calculation of Circle properties.
	 * 
	 * @param args the unused command line arguments.
	 */
	public static void main(String[] args) {
		System.out.println("------------------------------------------------------------");
		System.out.println("CircleCalc v1.0");
		System.out.println();
		System.out.println("Calculates and prints information for a user-supplied radius");
		System.out.println("------------------------------------------------------------");

		System.out.println("Enter the circle's radius: ");

		Scanner in = new Scanner(System.in);
		Circle c = new Circle(in.nextDouble());

		System.out.println();
		System.out.println("Circle properties:");
		System.out.println("\tRadius: " + c.getRadius());
		System.out.println("\tDiameter: " + c.getDiameter());
		System.out.println("\tArea: " + c.getDiameter());
		System.out.println("\tArc length at 1.57 radian: " + c.getRadiansArcLength(1.57));
		System.out.println("\tArc length at 90 degrees: " + c.getDegreesArcLength(90));
		System.out.println("\tSector area at 1.57 radian: " + c.getRadiansSectorArea(1.57));
		System.out.println("\tSector area at 90 degrees: " + c.getDegreesSectorArea(90));
	}
}

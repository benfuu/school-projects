import org.apache.commons.math3.util.MathUtils;

/**
 * The MyMath class provides a unified math library for the CircleCalc program.
 *
 */
public class MyMath {
	/**
	 * The value of PI.
	 */
	public static final double PI = Math.PI;
	/**
	 * The value of PI multiplied by 2.
	 */
	public static final double TWO_PI = MathUtils.TWO_PI;

	/**
	 * Calculates the value of a number squared.
	 * 
	 * @param x the number to be squared.
	 * @return the value squared.
	 */
	public static double square(double x) {
		return Math.pow(x, 2);
	}
	
	/**
	 * Convert an angle from degrees to radians.
	 * 
	 * @param degrees the angle in degrees
	 * @return the angle in radians
	 */
	public static double toRadians(double degrees) {
		return degrees * PI / 180;
	}
}
/**
 * Created by Ben on 4/8/2016.
 */
public abstract class Price {
    public abstract int getPriceCode();

    abstract double getCharge(int daysRented);

    abstract int getFrequentRenterPoints(int daysRented);
}

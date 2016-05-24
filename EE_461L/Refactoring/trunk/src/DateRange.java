import java.util.Date;

/**
 * Created by Ben on 4/8/2016.
 */
public class DateRange {
    private Date _start;
    private Date _end;
    public DateRange() {

    }

    public DateRange(Date start, Date end) {
        _start = start;
        _end = end;
    }
    public Date getStart() {
        return _start;
    }
    public Date getEnd() {
        return _end;
    }
}

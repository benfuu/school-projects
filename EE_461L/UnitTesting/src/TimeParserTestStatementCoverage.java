import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import java.sql.Time;

import static org.junit.Assert.*;

/**
 * Created by Ben on 4/22/2016.
 */
public class TimeParserTestStatementCoverage {

    @Test (expected = NumberFormatException.class)
    public void testStatementCoverage() throws Exception {
        TimeParser.parseTimeToSeconds("00");
    }

    @Test (expected = NumberFormatException.class)
    public void testOneColon() throws Exception {
        TimeParser.parseTimeToSeconds("1:00");
    }

    @Test (expected = IllegalArgumentException.class)
    public void testInvalidTime() throws Exception {
        TimeParser.parseTimeToSeconds("24:00:00");
    }

    @Test
    public void testAM() throws Exception {
        assertEquals(TimeParser.parseTimeToSeconds("12:00:00am"), 0);
    }

    @Test
    public void testPM() throws Exception {
        assertEquals(TimeParser.parseTimeToSeconds("1:00:00pm"), 46800);
    }

}
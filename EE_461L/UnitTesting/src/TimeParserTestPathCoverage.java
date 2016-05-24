import org.junit.Test;

import static org.junit.Assert.*;

/**
 * Created by Ben on 4/22/2016.
 */
public class TimeParserTestPathCoverage {

    @Test (expected = NumberFormatException.class)
    public void testStatementCoverage() throws Exception {
        TimeParser.parseTimeToSeconds("00");
    }

    @Test (expected = NumberFormatException.class)
    public void testOneColon() throws Exception {
        TimeParser.parseTimeToSeconds("1:00");
    }

    @Test (expected = IllegalArgumentException.class)
    public void testInvalidHours() throws Exception {
        TimeParser.parseTimeToSeconds("24:00:00");
    }

    @Test (expected = IllegalArgumentException.class)
    public void testInvalidMinutes() throws Exception {
        TimeParser.parseTimeToSeconds("00:61:00");
    }

    @Test (expected = IllegalArgumentException.class)
    public void testInvalidSeconds() throws Exception {
        TimeParser.parseTimeToSeconds("00:00:61");
    }

    @Test
    public void testAM() throws Exception {
        assertEquals(TimeParser.parseTimeToSeconds("12:00:00am"), 0);
    }

    @Test
    public void testPM() throws Exception {
        assertEquals(TimeParser.parseTimeToSeconds("1:00:00pm"), 46800);
    }

    @Test
    public void test24Hour() throws Exception {
        assertEquals(TimeParser.parseTimeToSeconds("13:00:00"), 46800);
    }

}
import org.junit.Test;

import static org.junit.Assert.*;

/**
 * Created by Ben on 4/22/2016.
 */
public class PhiladelphiaTest extends Philadelphia {

    @Test
    public void testIsItSunny() throws Exception {
        assertTrue(Philadelphia.isItSunny());
    }

    @Test
    public void testIsItNotSunny() throws Exception {
        assertFalse(Philadelphia.isItSunny());
    }

    @Test
    public void testAdditional() throws Exception {
        assertEquals(true, Philadelphia.isItSunny());
    }

    @Test
    public void testThrowException() throws Exception {
        assertEquals(false, Philadelphia.isItSunny());
    }
}
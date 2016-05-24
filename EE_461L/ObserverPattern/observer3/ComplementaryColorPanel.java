package observer3;

import javax.swing.*;
import javax.swing.event.ChangeEvent;
import java.awt.*;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;

/**
 * Created by Ben on 3/24/2016.
 */
public class ComplementaryColorPanel extends ColorPanel implements PropertyChangeListener{

    public ComplementaryColorPanel(OriginalColorPanel originalColorPanel) {
        originalColorPanel.addPropertyChangeListener(this);
        this.color = Color.getHSBColor((float) .5, (float) .5, (float) .5);
    }

    @Override
    public void setColor(float newHue, float newSaturation, float newBrightness) {
        float complementaryHue = newHue - (float) 0.5;
        if (complementaryHue < 0) {
            complementaryHue = complementaryHue + 1;
        }
        this.color = Color.getHSBColor(complementaryHue, newSaturation, newBrightness);
        repaint();
    }

    public void propertyChange(PropertyChangeEvent e) {
        String propertyName = e.getPropertyName();
        if (propertyName.equals("background")) {
            Color newColor = (Color) e.getNewValue();
            float[] hsb = Color.RGBtoHSB(newColor.getRed(),newColor.getGreen(), newColor.getBlue(), null);
            setColor(hsb[0], hsb[1], hsb[2]);
        }
    }
}

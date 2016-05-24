package observer2;

import javax.swing.*;
import javax.swing.event.ChangeEvent;
import java.awt.*;

/**
 * Created by Ben on 3/24/2016.
 */
public class ComplementaryColorPanel extends ColorPanel {
    private JSlider hueSlider;
    private JSlider saturationSlider;
    private JSlider brightnessSlider;

    public ComplementaryColorPanel(JSlider hueSlider, JSlider saturationSlider, JSlider brightnessSlider) {
        this.hueSlider = hueSlider;
        this.saturationSlider = saturationSlider;
        this.brightnessSlider = brightnessSlider;
        this.hueSlider.addChangeListener(this);
        this.saturationSlider.addChangeListener(this);
        this.brightnessSlider.addChangeListener(this);
        this.color = Color.getHSBColor((float) .5, (float) .5, (float) .5);
    }

    public void setColor(float newHue, float newSaturation, float newBrightness) {
        float complementaryHue = newHue - (float) 0.5;
        if (complementaryHue < 0) {
            complementaryHue = complementaryHue + 1;
        }
        this.color = Color.getHSBColor(complementaryHue, newSaturation, newBrightness);
        repaint();
    }
    public void stateChanged(ChangeEvent e) {
        if (hueSlider != null && saturationSlider != null && brightnessSlider != null) {
            float newHue = (float) hueSlider.getValue() / 100;
            float newSaturation = (float) saturationSlider.getValue() / 100;
            float newBrightness = (float) brightnessSlider.getValue() / 100;
            // WHAT GOES HERE?
            // You need to update the two color panels with the appropriate colors
            setColor(newHue, newSaturation, newBrightness);
        }
    }
}

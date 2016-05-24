package observer3;

import javax.swing.*;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import java.awt.*;

public abstract class ColorPanel extends JPanel {

    protected Color color;

    public ColorPanel() {
        this.setPreferredSize(new Dimension(300, 200));
    }

    public abstract void setColor(float newHue, float newSaturation, float newBrightness);

    protected void paintComponent(Graphics g){
        this.setBackground(color);
        super.paintComponent(g);
    }
}
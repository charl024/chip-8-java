import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class GUI extends JPanel implements ActionListener {
    private Chip8 CPU;
    private Timer timer;

    public GUI(int width, int height, int pixelSize, int delay) {
        this.setPreferredSize(new Dimension(width * pixelSize, height * pixelSize));

        this.CPU = new Chip8();
        this.timer = new Timer(1000, this);

        this.setFocusable(true);
        this.requestFocusInWindow();
    }

    @Override
    public void paintComponent(Graphics g) {
        super.paintComponent(g);
        for (int i = 0; i < ; i++) {
            for (int j = 0; j < CPU.graphics)
        }
    }

    @Override
    public void actionPerformed(ActionEvent e) {
        repaint();
        this.requestFocusInWindow();
    }
}

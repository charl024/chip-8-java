import javax.swing.*;
import java.awt.event.*;
import java.awt.*;

public class Main extends JFrame {
    private final int width = Values.WIDTH.getValue();
    private final int height = Values.HEIGHT.getValue();
    private final int pixelSize = Values.PIXEL_SIZE.getValue();
    private final int delay = 3;

    public Main() {
        super("Chip 8 Emulator");

        GUI panel = new GUI(width, height, pixelSize, delay);

        this.setLayout(new BorderLayout());

        this.getContentPane().add(panel);
        this.setResizable(false);
        this.setDefaultCloseOperation(EXIT_ON_CLOSE);
        this.pack();
        this.setLocationRelativeTo(null);
        this.setVisible(true);

    }

    public static void main(String[] args) {
        new Main();
    }
}

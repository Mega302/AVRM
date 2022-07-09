import java.awt.*;
import javax.swing.*;
import java.net.URL;
import java.util.*;
import java.io.*;
//http://www.java2s.com/Code/Java/2D-Graphics-GUI/Loadfontfromttffile.htm
class DisplayFont {
    
    public static void main(String[] args) throws Exception {
        
        //String fontPath = "/usr/local/share/fonts/Rupali/Rupali.ttf";
        String fontPath = "/SmartCard_ASCRM_20191111/ascrm-version-20/Roboto-Regular.ttf";
        Font font = null;
        try (InputStream is = DisplayFont.class.getResourceAsStream(fontPath)) {
			font = Font.createFont(Font.TRUETYPE_FONT, is);
		} catch (Exception e) {
			System.out.println("Failed load font from path: "+e.getMessage());
			return;
		}//catch end
	
        font = font.deriveFont(Font.PLAIN,20);
        GraphicsEnvironment ge = GraphicsEnvironment.getLocalGraphicsEnvironment();
        ge.registerFont(font);
        JLabel l = new JLabel("The quick brown fox jumped over the lazy dog. 0123456789");
        l.setFont(font);
        JOptionPane.showMessageDialog(null, l);
    
    }//public static void main(String[] args) ends
    
}//class DisplayFont end

// ============================================================================
// Main.java
// ============================================================================
//
//  Created by leuski on 9/1/19.
//  Copyright (c) 2019 Anton Leuski and ICT. All rights reserved.
// ============================================================================

package main;

import javax.swing.*;
import java.awt.*;
import java.io.File;

public class Main extends JFrame {
  public static void main(String[] inArgs) {
    System.out.println(System.getProperty("java.library.path"));
    System.out.println(new File(".").getAbsolutePath());
    new Main(inArgs).setVisible(true);
  }

  public Main(String[] inArgs) throws HeadlessException {
    setSize(400, 300);
    setContentPane(new MainWindow(inArgs).getContentPane());
    setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
  }
}

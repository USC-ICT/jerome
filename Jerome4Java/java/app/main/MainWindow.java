// ============================================================================
// MainWindow.java
// ============================================================================
//
//  Created by leuski on 9/1/19.
//  Copyright (c) 2019 Anton Leuski and ICT. All rights reserved.
// ============================================================================

package main;

import jerome.Jerome;

import javax.swing.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;

public class MainWindow {
  private JTextPane  mTranscriptPane;
  private JPanel     mContentPane;
  private JTextField mEntryField;
  private Jerome mJerome;

  public MainWindow(String[] inArgs) {
    mJerome = new Jerome();

    mJerome.loadModel(new File(inArgs[1]), new File(inArgs[1]), (e) -> {
      System.err.println(e);
      System.exit(1);
    });

    mEntryField.addActionListener(new ActionListener() {
      @Override
      public void actionPerformed(ActionEvent e) {
        System.out.println(e.getActionCommand());
      }
    });
  }

  public JPanel getContentPane() {
    return mContentPane;
  }
}

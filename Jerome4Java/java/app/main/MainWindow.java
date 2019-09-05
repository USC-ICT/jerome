// ============================================================================
// MainWindow.java
// ============================================================================
//
//  Created by leuski on 9/1/19.
//  Copyright (c) 2019 Anton Leuski and ICT. All rights reserved.
// ============================================================================

package main;

import jerome.Jerome;
import jerome.Utterance;

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

    mJerome.loadModel(new File(inArgs[0]), new File(inArgs[1]), inArgs[2],
        (e) -> {
      if (e != null) {
        System.err.println("error in loader: " + e);
        System.exit(1);
      }
    });

    mEntryField.addActionListener(new ActionListener() {
      @Override
      public void actionPerformed(ActionEvent e) {
        System.out.println(e.getActionCommand());
        mJerome.respondTo(e.getActionCommand(), new Jerome.ResponseCallback() {
          @Override
          public void didSelectUtterance(Utterance inUtterance,
              Jerome.Completion inCompletion)
          {
            System.out.println(inUtterance.valueForKey("text"));
            inCompletion.succeeded();
          }

          @Override
          public void didFinishProcessing() {
            System.out.println("NLU done");
          }
        });
      }
    });
  }

  public JPanel getContentPane() {
    return mContentPane;
  }
}

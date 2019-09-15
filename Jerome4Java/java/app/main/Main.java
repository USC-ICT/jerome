// ============================================================================
// Main.java
// ============================================================================
//
//  Created by leuski on 9/1/19.
//  Copyright (c) 2019 Anton Leuski and ICT. All rights reserved.
// ============================================================================

package main;

import jerome.Jerome;
import jerome.Utterance;

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStreamReader;

public class Main {

  private static Object LOCK = new Object();

  public static void main(String[] inArgs) {
    System.out.println(System.getProperty("java.library.path"));
    System.out.println(new File(".").getAbsolutePath());

    BufferedReader input = new BufferedReader(new InputStreamReader(System.in));

    Jerome mJerome = new Jerome();

    mJerome.loadModel(new File(inArgs[0]), new File(inArgs[1]), inArgs[2],
        (e) -> {
          if (e != null) {
            System.err.println("error in loader: " + e);
            System.exit(1);
          }
        });

    try {
      while (true) {

        System.out.print("Enter your question: ");
        String question = input.readLine().trim();
        if (question == "" || question.toLowerCase() == "q") {
          break;
        }

        mJerome.respondTo(question, new Jerome.ResponseCallback() {
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
            synchronized (LOCK) {
              LOCK.notify();
            }
          }
        });

        synchronized (LOCK) {
          LOCK.wait();
        }
      }

    } catch (Throwable inE) {
      inE.printStackTrace();
    }

  }


}

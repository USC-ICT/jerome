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

  private final Object LOCK = new Object();
  private boolean mInited = false;
  private final Jerome mJerome;

  Main() {
    mJerome = new Jerome();
  }
  
  private void handleQuestion(String inQuestion) throws Throwable {
    mJerome.respondTo(inQuestion, new Jerome.ResponseCallback() {
      @Override
      public void didSelectUtterance(Utterance inUtterance,
          Jerome.Completion inCompletion)
      {
        System.out.println(inUtterance.valueForKey("text"));
        inCompletion.succeeded();
      }

      @Override
      public void didFinishProcessing() {
        System.out.println("NLU is done");
        
        // Please note that this LOCK stuff is only needed to serialize the
        // code in this tool. In general, e.g., when using in an Android app
        // you do not need this serialization.
        // Again:
        // When copying the code into you app remove every synchronized(...) block
        // Just make you sure understand that the callback can be called either on 
        // the calling thread or on some arbitrary thread and you have to handle 
        // that yourself. 
        synchronized (LOCK) {
          LOCK.notify();
        }
      }
    });
    
    synchronized (LOCK) {
      LOCK.wait();
    }
  }

  private void run(String[] inArgs) throws Throwable {
    BufferedReader input = new BufferedReader(new InputStreamReader(System.in));

    mJerome.loadModel(new File(inArgs[0]), new File(inArgs[1]), inArgs[2],
      (e) -> {
        if (e != null) {
          System.err.println("error in loader: " + e);
          System.exit(1);
        }
        
        synchronized (LOCK) {
          mInited = true;
          LOCK.notify();
        }
      });
        
    synchronized (LOCK) {
      if (!mInited) {
        LOCK.wait();
      }
    }

    System.out.println();
    System.out.println("***> Loading caches by saying 'hello' ...");
    System.out.println("***> Wait for the prompt ...");
    handleQuestion("Hello");
    System.out.println("Ready");

    while (true) {
      System.out.println();
      System.out.print("Enter your question: ");
      String question = input.readLine().trim();
      handleQuestion(question);
    }
  }

  public static void main(String[] inArgs) {
    System.out.println("java.library.path: " + System.getProperty("java.library.path"));
    System.out.println("Working directory: " + new File(".").getAbsolutePath());
        
    try {
      new Main().run(inArgs);
    } catch (Throwable inE) {
      inE.printStackTrace();
    }

  }


}

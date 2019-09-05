// ============================================================================
// Jerome.java
// ============================================================================
//
//  Created by leuski on 8/31/19.
//  Copyright (c) 2019 Anton Leuski and ICT. All rights reserved.
// ============================================================================

package jerome;

import java.io.File;
import java.lang.annotation.Native;

public class Jerome extends NativePointer {

  static {
    System.loadLibrary("Jerome4Java");
  }

  @FunctionalInterface
  public interface ModelLoaderCallback {
    void didLoadModel(JeromeException inException);
  }

  public static class Completion extends NativePointer {
    public Completion(long inPointer) {
      super(inPointer);
    }
    public native void succeeded();
    public native void canceled();
    public native void interrupted();
    protected native void release();
  }

  public interface ResponseCallback {
    void didSelectUtterance(Utterance inUtterance, Completion inCompletion);
    void didFinishProcessing();
  }

  public void loadModel(
      File inCollectionFile,
      File inDialogueManagerFile,
      ModelLoaderCallback inCallback)
  {
    loadModel(
        inCollectionFile.getAbsolutePath(),
        inDialogueManagerFile.getAbsolutePath(),
        inCallback);
  }

  private native void loadModel(String inCollectionFile,
      String inDialogueManagerFile, ModelLoaderCallback inCallback);

  public native void collectionWasUpdated();
  public native void collectionWasUpdatedInState(String inStateName);
  public native Utterance utteranceWithID(String inID);

  public native void resetDialogue();
  public native void tossDialogueToStateWithName(String inStateName);
  public native void respondTo(String inText,
      ResponseCallback inResponseCallback);

  private static native long init();
  protected native void release();

  public Jerome() {
    super(init());
  }
}

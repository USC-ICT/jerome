// ============================================================================
// Utterance.java
// ============================================================================
//
//  Created by leuski on 8/31/19.
//  Copyright (c) 2019 Anton Leuski and ICT. All rights reserved.
// ============================================================================

package jerome;

public class Utterance extends NativePointer {
  protected Utterance(long inPointer) {
    super(inPointer);
  }

  public native boolean hasValueForKey(String inKey);
  public native String valueForKey(String inKey);
  public native void setValueForKey(String inValue, String inKey);

  private static native long init();

  public Utterance() {
    super(init());
  }
}

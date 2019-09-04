// ============================================================================
// NativePointer.java
// ============================================================================
//
//  Created by leuski on 8/31/19.
//  Copyright (c) 2019 Anton Leuski and ICT. All rights reserved.
// ============================================================================

package jerome;

class NativePointer {
  private long mPointer;
  private native void release();

  public NativePointer(long inPointer) {
    mPointer = inPointer;
  }

  @Override
  protected void finalize() throws Throwable {
    super.finalize();
    release();
  }
}

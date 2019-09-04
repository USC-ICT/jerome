// ============================================================================
// NativePointer.java
// ============================================================================
//
//  Created by leuski on 8/31/19.
//  Copyright (c) 2019 Anton Leuski and ICT. All rights reserved.
// ============================================================================

package jerome;

abstract class NativePointer {
  private long mPointer;
  protected abstract void release();

  public NativePointer(long inPointer) {
    mPointer = inPointer;
  }

  @Override
  protected void finalize() throws Throwable {
    super.finalize();
    release();
  }
}

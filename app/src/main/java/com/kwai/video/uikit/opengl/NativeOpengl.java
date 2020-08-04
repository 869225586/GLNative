package com.kwai.video.uikit.opengl;

import android.view.Surface;

public class NativeOpengl {


    static {
        System.loadLibrary("native-lib");
    }

    public native void surfaceCreate(Surface surface);

    public native  void surfaceChange(int i1, int i2);
}

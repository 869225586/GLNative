package com.sunyeyu.video.uikit.opengl;

import android.content.Context;
import android.graphics.SurfaceTexture;
import android.util.AttributeSet;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.TextureView;

public class MyTextureView extends TextureView {

    private NativeOpengl nativeOpengl;
    private WlSurfaceView.OnSurfaceListener onSurfaceListener;
    private Surface msurface;
    public MyTextureView(Context context) {
        super(context);
        init();
    }



    public MyTextureView(Context context, AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    public MyTextureView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init();
    }
    private void init() {
        setSurfaceTextureListener(new SurfaceTextureListener() {
            @Override
            public void onSurfaceTextureAvailable(SurfaceTexture surface, int width, int height) {
                msurface = new Surface(surface);
                if(nativeOpengl != null)
                {
                    nativeOpengl.setSurface(msurface);
                    if(onSurfaceListener!=null)
                        onSurfaceListener.init();
                    nativeOpengl.surfaceChange(width,height);
                }
            }

            @Override
            public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width, int height) {
                Log.i("hhh","surface change "+width+"=="+height);
                nativeOpengl.surfaceChange(width,height);
            }

            @Override
            public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
                return false;
            }

            @Override
            public void onSurfaceTextureUpdated(SurfaceTexture surface) {

            }
        });
    }

    public void setOnSurfaceListener(WlSurfaceView.OnSurfaceListener onSurfaceListener) {
        this.onSurfaceListener = onSurfaceListener;
    }

    public void setNativeOpengl(NativeOpengl nativeOpengl) {
        this.nativeOpengl = nativeOpengl;
    }


   public void startFullScreen(){
            nativeOpengl.changewindow(true);
   }
}

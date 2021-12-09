package com.sunyeyu.video.uikit.camera;

import android.graphics.ImageFormat;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;

import java.io.IOException;

/**
 * 摄像头工具类用于打开摄像头
 */
public class CameraUtils {
    private Camera camera;
    private SurfaceTexture surfaceTexture;

    public void initCamera(SurfaceTexture
                                   surfaceTexture, int cameraId) {
        this.surfaceTexture = surfaceTexture;
        setcameraParm(cameraId);
    }

    private void setcameraParm(int cameraId) {
        try {
            camera = Camera.open(cameraId);
            camera.setPreviewTexture(surfaceTexture);
            Camera.Parameters parameters = camera.getParameters();
            parameters.setFlashMode("off"); //闪光灯

            parameters.setPreviewFormat(ImageFormat.NV21);//摄像头预览 采集格式

            parameters.setPictureSize(parameters.getSupportedPictureSizes().get(0).width,
                    parameters.getSupportedPictureSizes().get(0).height);
            parameters.setPreviewSize(parameters.getSupportedPreviewSizes().get(0).width,
                    parameters.getSupportedPreviewSizes().get(0).height);

            camera.setParameters(parameters);
            camera.startPreview();

        } catch (IOException e) {

        }
    }

    public void stopPreview() {
        if (camera != null) {
            camera.stopPreview();
            camera = null;
        }
    }

    public void changeCamera(int cameraId){
        if(camera!=null){
            stopPreview();
        }
        setcameraParm(cameraId);
    }
}

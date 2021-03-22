package com.sunyeyu.video.uikit.opengl;

import android.media.MediaCodec;
import android.media.MediaFormat;
import android.util.Log;
import android.view.Surface;

import com.sunyeyu.video.uikit.util.LogUtils;

import java.nio.ByteBuffer;

public class NativeOpengl {
    Surface surface;
    OnCameraTextureCall onCameraTextureCall;
    onPreparedListner preparedListner;
    private double pos ;
    static {
        System.loadLibrary("native-lib");
    }

    public interface OnCameraTextureCall {
        void callTextTure(int textureId);
        void update();
    }

    private MediaFormat mediaFormat;
    private MediaCodec mediaCodec;
    private MediaCodec.BufferInfo info;


    public void initSurfaceTextture(int cameraTextureId) {
        if (onCameraTextureCall != null) {
            onCameraTextureCall.callTextTure(cameraTextureId);
        }
    }
    public void updateSurfaceTextture() {
        if (onCameraTextureCall != null) {
            onCameraTextureCall.update();
        }
    }

   public interface  onPreparedListner{
          void onPrepared();
   }

    public void setTextTureCall(OnCameraTextureCall onCameraTextureCall) {
        this.onCameraTextureCall = onCameraTextureCall;
    }


    /**
     * 初始化MediaCodec
     *
     * @param codecName
     * @param width
     * @param height
     * @param csd_0
     * @param csd_1
     */
    public void initMediaCodec(String codecName, int width, int height, byte[] csd_0, byte[] csd_1) {
        try {
            String mime = VideoSupportUitl.findVideoCodecName(codecName);
            LogUtils.Companion.I(mime);
            mediaFormat = MediaFormat.createVideoFormat(mime, width, height);
            mediaFormat.setInteger(MediaFormat.KEY_WIDTH, width);
            mediaFormat.setInteger(MediaFormat.KEY_HEIGHT, height);
            mediaFormat.setLong(MediaFormat.KEY_MAX_INPUT_SIZE, width * height);
            mediaFormat.setByteBuffer("csd-0", ByteBuffer.wrap(csd_0));
            mediaFormat.setByteBuffer("csd-1", ByteBuffer.wrap(csd_1));
            mediaFormat.setInteger(MediaFormat.KEY_FRAME_RATE,40);
            mediaCodec = MediaCodec.createDecoderByType(mime);

            info = new MediaCodec.BufferInfo();
            mediaCodec.configure(mediaFormat, surface, null, 0);
            mediaCodec.start();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public boolean onCallIsSupportMediaCodec(String ffcodecname) {
        Log.i("syy", " 是否支持硬解");
        return VideoSupportUitl.isSupportCodec(ffcodecname);
    }

    public void decodeAVPacket(int datasize, byte[] data) {
        if (surface != null && datasize > 0 && data != null && mediaCodec != null) {
            int intputBufferIndex = mediaCodec.dequeueInputBuffer(10);
            if (intputBufferIndex >= 0) {
                ByteBuffer byteBuffer = mediaCodec.getInputBuffers()[intputBufferIndex];
                byteBuffer.clear();
                byteBuffer.put(data);
                mediaCodec.queueInputBuffer(intputBufferIndex, 0, datasize, 0, 0);
            }
            int outputBufferIndex = mediaCodec.dequeueOutputBuffer(info, 10);
            while (outputBufferIndex >= 0) {
                mediaCodec.releaseOutputBuffer(outputBufferIndex, true);
                outputBufferIndex = mediaCodec.dequeueOutputBuffer(info, 10);
            }
        }
    }


    public void setSurface(Surface surface) {
        this.surface = surface;
        surfaceCreate(surface);
    }

    public void callPrepared(){
        if(preparedListner!=null){
            preparedListner.onPrepared();
        }
    }

    public void nativeCallTimeProgress(double currentPos,long duration ){
        this.pos = currentPos;
    }

    public void setPreparedListner(onPreparedListner preparedListner){
        this.preparedListner = preparedListner;
    }

    public double  getCurrentPos(){
        return pos;
    }
    public native void surfaceCreate(Surface surface);

    public native void surfaceChange(int i1, int i2);

    public native void surfaceDestroy();

    public native void changeFilter();

    public native void imgData(int w, int h, int length, byte[] data);

    public native void setYuvData(byte[] y, byte[] u, byte[] v, int w, int h);

    public native void playFromFFmpeg(String url);

    public native void pause();

    public native void resume();

    public native void changewindow(boolean fullscreen);

    public native long getDuration();

    public native void seek(long second);

    public native void setUrl(String url);

    public native void start();
}

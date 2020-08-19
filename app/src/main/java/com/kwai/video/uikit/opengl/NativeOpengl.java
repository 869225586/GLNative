package com.kwai.video.uikit.opengl;

import android.media.MediaCodec;
import android.media.MediaFormat;
import android.util.Log;
import android.view.Surface;

import java.nio.ByteBuffer;

public class NativeOpengl {
    Surface surface;

    static {
        System.loadLibrary("native-lib");
    }

    private MediaFormat mediaFormat;
    private MediaCodec mediaCodec;
    private MediaCodec.BufferInfo info;

    public void setSurface(Surface surface) {
        this.surface = surface;
        surfaceCreate(surface);
    }

    public native void surfaceCreate(Surface surface);

    public native void surfaceChange(int i1, int i2);

    public native void surfaceDestroy();

    public native void changeFilter();

    public native void imgData(int w, int h, int length, byte[] data);

    public native void setYuvData(byte[] y, byte[] u, byte[] v, int w, int h);

    public native void playFromFFmpeg(String url);

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
            mediaFormat = MediaFormat.createVideoFormat(mime, width, height);
            mediaFormat.setInteger(MediaFormat.KEY_MAX_INPUT_SIZE, width * height);
            mediaFormat.setByteBuffer("csd-0", ByteBuffer.wrap(csd_0));
            mediaFormat.setByteBuffer("csd-1", ByteBuffer.wrap(csd_1));
            mediaCodec = MediaCodec.createDecoderByType(mime);

            info = new MediaCodec.BufferInfo();
            mediaCodec.configure(mediaFormat, surface, null, 0);
            mediaCodec.start();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public boolean onCallIsSupportMediaCodec(String ffcodecname) {
        Log.i("syy"," 是否支持硬解");
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

}

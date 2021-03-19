package com.sunyeyu.video.uikit.widget;


import android.content.Context;
import android.graphics.Matrix;
import android.graphics.SurfaceTexture;
import android.util.AttributeSet;
import android.view.Gravity;
import android.view.TextureView;
import android.widget.FrameLayout;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import androidx.annotation.IntDef;

/**
 * 自适应  填充 缩放 裁剪
 */
public class MyTextureView extends FrameLayout implements TextureView.SurfaceTextureListener {
    @Retention(RetentionPolicy.SOURCE)
    @IntDef(value={VideoScalConst.MODE_SCALE_FIT,VideoScalConst.MODE_NOSCALE_TO_FIT,VideoScalConst.MODE_SCALE_TO_FIT_CROPPING})
    public @interface VideoScalConst{
        int MODE_SCALE_FIT = 1;//缩放填充
        int MODE_SCALE_TO_FIT_CROPPING = 2;//按照宽或者高进行缩放适应 会裁剪画面 裁剪其实是针对当前textureview 进行了matrix缩放 。起到了 填充裁剪的作用
        int MODE_NOSCALE_TO_FIT = 3; //不缩放填充
    }
    private TextureViewIml mTextureView;
    private int mVideoWidth;
    private int mVideoHeight;
    private int mDegree = 0;
    @VideoScalConst
    private int mScaleMode = VideoScalConst.MODE_SCALE_FIT;
    private TextureView.SurfaceTextureListener mSurfaceTextureListener;
    private SurfaceTexture mSurfaceTexture;


    public MyTextureView(Context context) {
        super(context);
        initVideoView(context);
    }


    public MyTextureView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }


    public MyTextureView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        initVideoView(context);
    }


    private void initVideoView(Context context) {
        LayoutParams layoutParams = new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT);
        layoutParams.gravity = Gravity.CENTER;


        mTextureView = new TextureViewIml(context);
        mTextureView.setLayoutParams(layoutParams);
        mTextureView.setSurfaceTextureListener(this);


        this.addView(mTextureView);


        mTextureView.setVerticalOrientation(true);
        mVideoWidth = mVideoHeight = 0;
        mTextureView.setScaleMode(mScaleMode);
    }


    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        if (mVideoWidth == 0 || mVideoHeight == 0) {
            super.onMeasure(widthMeasureSpec, heightMeasureSpec);
            return;
        }
        if (mTextureView == null) {
            super.onMeasure(widthMeasureSpec, heightMeasureSpec);
            return;
        }


        measureChildren(widthMeasureSpec, heightMeasureSpec);
        int width = 0;
        int height = 0;
        int widthSpecMode = MeasureSpec.getMode(widthMeasureSpec);
        int widthSpecSize = MeasureSpec.getSize(widthMeasureSpec);
        int heightSpecMode = MeasureSpec.getMode(heightMeasureSpec);
        int heightSpecSize = MeasureSpec.getSize(heightMeasureSpec);


        if (widthSpecMode == MeasureSpec.EXACTLY && heightSpecMode == MeasureSpec.EXACTLY) {
            width = widthSpecSize;
            height = heightSpecSize;
        } else if (widthSpecMode == MeasureSpec.EXACTLY) {
            width = widthSpecSize;
            height = mTextureView.getMeasureHeight();
            if ((mDegree / 90) % 2 != 0) {
                height = mTextureView.getMeasureWidth();
            }
            if (height > heightSpecSize) {
                height = heightSpecSize;
            }
        } else if (heightSpecMode == MeasureSpec.EXACTLY) {
            height = heightSpecSize;
            width = mTextureView.getMeasureWidth();
            if ((mDegree / 90) % 2 != 0) {
                width = mTextureView.getMeasureHeight();
            }
            if (width > widthSpecSize) {
                width = widthSpecSize;
            }


        } else {
            width = mTextureView.getMeasureWidth();
            height = mTextureView.getMeasureHeight();
            if ((mDegree / 90) % 2 != 0) {
                height = mTextureView.getMeasureWidth();
                width = mTextureView.getMeasureHeight();
            }
            if (width > widthSpecSize) {
                width = widthSpecSize;
            }
            if (height > heightSpecSize) {
                height = heightSpecSize;
            }
        }


        setMeasuredDimension(width, height);
    }


    @Override
    public void onSurfaceTextureAvailable(SurfaceTexture surfaceTexture, int i, int i1) {
        mSurfaceTexture = surfaceTexture;
        if (mSurfaceTextureListener != null) {
            mSurfaceTextureListener.onSurfaceTextureAvailable(surfaceTexture, i, i1);
        }
    }


    @Override
    public void onSurfaceTextureSizeChanged(SurfaceTexture surfaceTexture, int i, int i1) {
        if (mSurfaceTextureListener != null) {
            mSurfaceTextureListener.onSurfaceTextureSizeChanged(surfaceTexture, i, i1);
        }
        requestLayout();
    }


    @Override
    public boolean onSurfaceTextureDestroyed(SurfaceTexture surfaceTexture) {
        if (mSurfaceTextureListener != null) {
            return mSurfaceTextureListener.onSurfaceTextureDestroyed(surfaceTexture);
        }
        return false;
    }


    @Override
    public void onSurfaceTextureUpdated(SurfaceTexture surfaceTexture) {
        if (mSurfaceTextureListener != null) {
            mSurfaceTextureListener.onSurfaceTextureUpdated(surfaceTexture);
        }
    }


    public void setVideoScalingMode(@VideoScalConst int mode) {
        if (mTextureView != null) {
            mScaleMode = mode;
            mTextureView.setScaleMode(mode);
        }
    }


    public SurfaceTexture getSurfaceTexture() {
        return mSurfaceTexture;
    }


    /**
     * 获取但前视频的缩放比例值
     */
    public float getVideoScaleRatio() {
        if (mTextureView != null) {
            return mTextureView.getVideoScaleRatio();
        }
        return 1.0f;
    }


    public void setVideoSize(int width, int height) {
        mVideoWidth = width;
        mVideoHeight = height;
        mTextureView.setMeasureSize(width, height);
    }


    public void setSurfaceTextureListener(TextureView.SurfaceTextureListener listener) {
        this.mSurfaceTextureListener = listener;
    }


    class TextureViewIml extends TextureView implements TextureView.SurfaceTextureListener {
        public static final int STATUS_NORMAL = 1;
        boolean mUseSettingRatio = false;
        private SurfaceTextureListener mListener;
        private int mVideoWidth;
        private int mVideoHeight;
        private int mVideoSarNum;
        private int mVideoSarDen;
        private int mMeasureWidth;
        private int mMeasureHeight;
        private int mScaleMode = VideoScalConst.MODE_SCALE_FIT;
        private int mDegree;
        private boolean mMirror = false;
        private boolean mVerticalOrientation;
        private float mHOffset = 0.0f;
        private float mVOffset = 0.0f;
        private Matrix mMatrix = new Matrix(); //缩放矩阵
        private int mLayoutWidth;
        private int mLayoutHeight;
        private float mCurrentVideoWidth;
        private float mCurrentVideoHeight;
        private float mTotalTranslateX;
        private float mTotalTranslateY;
        private float mTotalRatio = 1.0f;
        private int mCurrentDispStatus = STATUS_NORMAL;


        public TextureViewIml(Context context) {
            super(context);
            super.setSurfaceTextureListener(this);
        }


        public TextureViewIml(Context context, AttributeSet attrs) {
            this(context, attrs, 0);


        }


        public TextureViewIml(Context context, AttributeSet attrs, int defStyleAttr) {
            super(context, attrs, defStyleAttr);
            super.setSurfaceTextureListener(this);
        }


        private void Normal(int widthSpecMode, int heightSpecMode) {
            float ratio = 1.0f;
            float hOffset = 0.0f;
            float vOffset = 0.0f;
            int videoWidth = mVideoWidth;
            int videoHeight = mVideoHeight;
            int width = mLayoutWidth;
            int height = mLayoutHeight;
            Matrix matrix = mMatrix;


            if (mVideoSarNum > 0 && mVideoSarDen > 0) {
                videoWidth = videoWidth * mVideoSarNum / mVideoSarDen;
            }


            float scaleX = (float) videoWidth / mLayoutWidth;
            float scaleY = (float) videoHeight / mLayoutHeight;


            if ((mDegree / 90) % 2 != 0) {
                videoHeight = mVideoWidth;
                videoWidth = mVideoHeight;


                if (mVideoSarNum > 0 && mVideoSarDen > 0) {
                    videoHeight = videoHeight * mVideoSarNum / mVideoSarDen;
                }
            }


            switch (mScaleMode) {
                case VideoScalConst.MODE_SCALE_TO_FIT_CROPPING:
                    ratio = Math.max((float) width / videoWidth, (float) height / videoHeight);
                    hOffset = vOffset = 0.0f;
                    mTotalRatio = ratio;
                    break;


                case VideoScalConst.MODE_SCALE_FIT:
                    ratio = Math.min((float) width / videoWidth, (float) height / videoHeight);
                    hOffset = mHOffset;
                    vOffset = mVOffset;
                    mTotalRatio = ratio;
                    break;


                case VideoScalConst.MODE_NOSCALE_TO_FIT:
                    if ((mDegree / 90) % 2 != 0) {
                        scaleX = (float) height / width;
                        scaleY = (float) width / height;
                    } else {
                        scaleX = 1.0f;
                        scaleY = 1.0f;
                    }
                    break;
            }




            if ((mDegree / 90) % 2 != 0) {
                mCurrentVideoWidth = height * scaleY * ratio;
                mCurrentVideoHeight = width * scaleX * ratio;
            } else {
                mCurrentVideoWidth = width * scaleX * ratio;
                mCurrentVideoHeight = height * scaleY * ratio;
            }


            matrix.reset();


            matrix.postScale(ratio * scaleX, ratio * scaleY);
            matrix.postRotate(mDegree);


            float translateX = 0.0f;
            float translateY = 0.0f;




            switch (mDegree) {
                case 0:
                    translateX = ((width - mCurrentVideoWidth) / 2f);
                    translateY = ((height - mCurrentVideoHeight) / 2f);
                    break;
                case -90:
                    translateX = (width - mCurrentVideoWidth) / 2;
                    translateY = (height + mCurrentVideoHeight) / 2;
                    break;
                case -180:
                    translateX = (width + mCurrentVideoWidth) / 2;
                    translateY = (height + mCurrentVideoHeight) / 2;
                    break;
                case -270:
                    translateX = ((width + mCurrentVideoWidth) / 2f);
                    translateY = ((height - mCurrentVideoHeight) / 2f);
                    break;
            }
            mTotalTranslateX = translateX + hOffset * width / 2;
            mTotalTranslateY = translateY - vOffset * height / 2;


            matrix.postTranslate(mTotalTranslateX, mTotalTranslateY);


            mMeasureWidth = (int) (width * ratio * scaleX);
            mMeasureHeight = (int) (height * ratio * scaleY);
        }


        private void Measure(int widthMeasureSpec, int heightMeasureSpec) {
            if (mVideoWidth == 0 || mVideoHeight == 0) return;


            int widthSpecMode = MeasureSpec.getMode(widthMeasureSpec);
            int widthSpecSize = MeasureSpec.getSize(widthMeasureSpec);
            int heightSpecMode = MeasureSpec.getMode(heightMeasureSpec);
            int heightSpecSize = MeasureSpec.getSize(heightMeasureSpec);


            mLayoutWidth = widthSpecSize;
            mLayoutHeight = heightSpecSize;


            if (mCurrentDispStatus == STATUS_NORMAL) {
                Normal(widthSpecMode, heightSpecMode);
            }
            setTransform(mMatrix);
        }


        @Override
        protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
            Measure(widthMeasureSpec, heightMeasureSpec);
            setMeasuredDimension(widthMeasureSpec, heightMeasureSpec);
        }


        @Override
        public void onSurfaceTextureAvailable(SurfaceTexture surfaceTexture, int i, int i1) {
            if (mListener != null) {
                mListener.onSurfaceTextureAvailable(surfaceTexture, i, i1);
            }
            requestLayout();
        }


        @Override
        public void onSurfaceTextureSizeChanged(SurfaceTexture surfaceTexture, int i, int i1) {
            if (mListener != null) {
                mListener.onSurfaceTextureSizeChanged(surfaceTexture, i, i1);
            }
            mTextureView.setScaleMode(mScaleMode);
            requestLayout();
        }


        @Override
        public boolean onSurfaceTextureDestroyed(SurfaceTexture surfaceTexture) {
            if (mListener != null) {
                return mListener.onSurfaceTextureDestroyed(surfaceTexture);
            }
            return false;
        }


        @Override
        public void onSurfaceTextureUpdated(SurfaceTexture surfaceTexture) {
            if (mListener != null) {
                mListener.onSurfaceTextureUpdated(surfaceTexture);
            }
        }


        public void setSurfaceTextureListener(SurfaceTextureListener listener) {
            mListener = listener;
        }


        public void setMeasureSize(int width, int height) {
            mVideoWidth = width;
            mVideoHeight = height;
            requestLayout();
        }


        public void setScaleMode(int mode) {
            mScaleMode = mode;
            mUseSettingRatio = false;
            mCurrentDispStatus = STATUS_NORMAL;


            requestLayout();
        }


        public float getVideoScaleRatio() {
            return mTotalRatio;
        }


        public void setVerticalOrientation(boolean vertical) {
            mVerticalOrientation = vertical;
            mCurrentDispStatus = STATUS_NORMAL;
            requestLayout();
        }


        public int getMeasureWidth() {
            return mMeasureWidth;
        }


        public int getMeasureHeight() {
            return mMeasureHeight;
        }
    }
}


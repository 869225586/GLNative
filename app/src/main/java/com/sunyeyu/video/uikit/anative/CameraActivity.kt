package com.sunyeyu.video.uikit.anative

import android.Manifest
import android.graphics.SurfaceTexture
import android.hardware.Camera
import android.os.Bundle
import android.util.Log
import android.view.SurfaceView
import android.view.View
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import com.sunyeyu.video.uikit.camera.CameraUtils
import com.sunyeyu.video.uikit.opengl.NativeOpengl
import com.sunyeyu.video.uikit.opengl.WlSurfaceView


class CameraActivity : AppCompatActivity() {
    var cameraUtils = CameraUtils()
    private lateinit var surfaceView:WlSurfaceView
    private lateinit var surfaceTexture: SurfaceTexture
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_camera)
        surfaceView=findViewById(R.id.surface_view)
        if (!ActivityCompat.shouldShowRequestPermissionRationale(
                this, Manifest.permission.CAMERA
            )
        ) {
            ActivityCompat.requestPermissions(
                this,
                arrayOf(Manifest.permission.CAMERA), 0
            );
        }
        var nativeOpengl=NativeOpengl()
        nativeOpengl.setTextTureCall ( callText() )

        surfaceView.setNativeOpengl(nativeOpengl)




    }
    inner class callText :NativeOpengl.OnCameraTextureCall{
        override fun callTextTure(textureId: Int) {
            Log.i("syy","创建surfactexture")
            surfaceTexture= SurfaceTexture(textureId)
        }

        override fun update() {

            surfaceTexture.updateTexImage()
        }

    }
    fun openCamera(view: View) {
        if(surfaceTexture!=null)
        cameraUtils.initCamera(surfaceTexture, Camera.CameraInfo.CAMERA_FACING_BACK)

    }
}
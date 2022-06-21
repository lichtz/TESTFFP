package com.licht.lipusher

import android.hardware.Camera
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import android.view.SurfaceView
import android.view.View
import androidx.camera.core.CameraSelector
import androidx.camera.core.Preview
import androidx.camera.lifecycle.ProcessCameraProvider
import androidx.camera.view.PreviewView
import androidx.core.content.ContextCompat
import androidx.lifecycle.LifecycleOwner
import com.google.common.util.concurrent.ListenableFuture

class LivePusherActivity : AppCompatActivity() {
    companion object{
         var   c =   NativeLib();
    }
//    private lateinit var previewView: PreviewView
    lateinit var livePusher: LivePusher
//    private lateinit var cameraProviderFuture : ListenableFuture<ProcessCameraProvider>
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_live_pusher)
    val surfaceView = findViewById<SurfaceView>(R.id.surface)
//        previewView = findViewById<PreviewView>(R.id.previewView)
//        cameraProviderFuture = ProcessCameraProvider.getInstance(this)
//        cameraProviderFuture.addListener(Runnable {
//            val cameraProvider  = cameraProviderFuture.get()
//            bindPreview(cameraProvider)
//        },ContextCompat.getMainExecutor(this))

    surfaceView.post { Log.e("BBBN", "onCreate: "+surfaceView.width+"____"+surfaceView.height ) }
//
        livePusher=   LivePusher(this,1080,1920,8000,10, Camera.CameraInfo.CAMERA_FACING_FRONT);
        livePusher.setPreviewDisplay(surfaceView.holder)
        val stringFromJNI = c.stringFromJNI()
        Log.e("AAAS", "onCreate: "+stringFromJNI )
    }

//    private fun bindPreview(cameraProvider: ProcessCameraProvider?) {
//        val preview = Preview.Builder().build()
//        var cameraSelector : CameraSelector = CameraSelector.Builder()
//            .requireLensFacing(CameraSelector.LENS_FACING_BACK)
//            .build()
//        preview.setSurfaceProvider(previewView.getSurfaceProvider())
//        var camera = cameraProvider?.bindToLifecycle(this as LifecycleOwner, cameraSelector, preview)
//    }

    fun changeCamera(view: View) {
        livePusher.switchCamera();
    }
    fun startPusher(view: View) {
        livePusher.startPerview();
    }
    fun stopPusher(view: View) {
        livePusher.stopPerview();
    }


}
package com.licht.livideoplayer

import android.view.Surface
import android.view.SurfaceHolder
import android.view.SurfaceView
import java.io.File

class LiPlayer : SurfaceHolder.Callback {
    companion object {
        init {
            System.loadLibrary("livideoplayer")
        }
    }

    private var dataSource: String? = null;


    private var surfaceHolder: SurfaceHolder? = null;
    fun setSurfaceView(surfaceView: SurfaceView) {
        if (surfaceHolder != null) {
            surfaceHolder!!.removeCallback(this);
        }
        this.surfaceHolder = surfaceView.holder;
        this.surfaceHolder!!.addCallback(this)
    }

    override fun surfaceCreated(holder: SurfaceHolder) {
    }

    override fun surfaceChanged(holder: SurfaceHolder, format: Int, width: Int, height: Int) {
        surfaceHolder = holder;
        setSufaceNative(holder.surface);
    }


    override fun surfaceDestroyed(holder: SurfaceHolder) {
    }

    fun start(absoluteString: String) {
        Thread {
            native_start(absoluteString, surfaceHolder!!.surface)

        }.start()
    }

    fun start(){
        startNative();
    }

    external fun native_start(absoluteString: String, surface: Surface);

    external fun startAudio(std: String);

    external fun switchMp3ToPcm(src: String, dest: String);

    external fun setSufaceNative(surface: Surface) ;


    fun prepare() {
        prepareNative(dataSource);

    }

    fun  onPrepare(){
        playerNativeStateCallback?.onPrepared()
    }

    external fun prepareNative(dataSource: String?);

    external fun startNative();


    fun setDataSource(absolutePath: String) {
        dataSource = absolutePath;
    }


    public fun onError(errorCode: Int, std: String) {
        liplayerListener?.onError(errorCode, std);
    }

    interface LiplayerListener {
        fun onError(errorCode: Int, std: String);
    }

    interface PlayerNativeStateCallback {
        fun onPrepared();
    }

    var liplayerListener: LiplayerListener? = null;
    var playerNativeStateCallback:PlayerNativeStateCallback? =null;
}
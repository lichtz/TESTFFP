package com.licht.livideoplayer

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.os.Environment
import android.util.Log
import android.view.SurfaceView
import android.widget.Button
import java.io.File

class TestActivity : AppCompatActivity() {
    var surfaceView:SurfaceView?= null;
    val liPlayer = LiPlayer();
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_test)
         surfaceView = findViewById<SurfaceView>(R.id.surfaceview)
        liPlayer.setSurfaceView(surfaceView!!)
        val file = File("/data/data/com.example.livideo/files/C0126.MP4")
        liPlayer.setDataSource(file.absolutePath)
        liPlayer.liplayerListener = object: LiPlayer.LiplayerListener{
            override fun onError(errorCode: Int, std: String) {
                Log.e("XAA","errorCode");
            }

        }
        liPlayer.playerNativeStateCallback = object :LiPlayer.PlayerNativeStateCallback{
            override fun onPrepared() {
                Log.e("XAA","onPrepared");
                liPlayer.start();
            }

        }
    }




    fun open(view: android.view.View) {
        val file = File(filesDir, "C0131.MP4")
        Log.e("DDDDD", "open: "+file.absoluteFile +file.exists() )
        liPlayer.start(file.absolutePath);

    }

    fun switchx(view: android.view.View) {
        val file = File(filesDir, "xx.mp3")
        val afile = File(filesDir, "aa.PCM")
        liPlayer.switchMp3ToPcm(file.absolutePath,afile.absolutePath);

    }

    override fun onResume() {

        super.onResume()

        liPlayer.prepare();
    }
}
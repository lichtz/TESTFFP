package com.example.livideo

import android.content.Intent
import android.os.Build
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.view.View
import android.widget.TextView
import androidx.annotation.RequiresApi
import androidx.core.content.ContextCompat.startActivity
import com.example.livideo.databinding.ActivityMainBinding
import com.example.opengles.EGLActivity
import com.example.opengles.ToolsActivity
import com.licht.livideoplayer.TestActivity

//import com.example.opengles.EGLActivity
//import com.example.opengles.ToolsActivity
////import com.licht.lipusher.LivePusherActivity
//import com.licht.livideoplayer.TestActivity

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)
    }

    fun jm(view: View) {
        startActivity(Intent(this, TestActivity::class.java))
    }

    fun pusher(view: View) {

//        startActivity(Intent(this,LivePusherActivity::class.java))

    }

    @RequiresApi(Build.VERSION_CODES.M)
    fun opengles(view: View) {
        startActivity(Intent(this, EGLActivity::class.java))
    }

    /**
     * A native method that is implemented by the 'livideo' native library,
     * which is packaged with this application.
     */


    fun TOOLSaV(view: View) {
        startActivity(Intent(this, ToolsActivity::class.java))
    }
}
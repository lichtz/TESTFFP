package com.example.opengles;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import android.media.MediaCodec;
import android.os.Bundle;
import android.view.View;

public class ToolsActivity extends AppCompatActivity {


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_tools);
    }

    public void EncoderAACLC(View view) {
AudioToolsHelper.INSTANCE.encodePCMToAAC("/sdcard/vocal.pcm",1,128 * 1024, 48000,"/sdcard/vocalx.aac");
    }

    public void HARDENCODED(View view) {
        MediaCodec mediaCodec = Audio.INSTANCE.initCoverPcmToAAC(44100, 2, 128 * 1024);

    }
}
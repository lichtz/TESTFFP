package com.licht.lipusher;

import android.app.Activity;
import android.view.SurfaceHolder;

import com.licht.lipusher.meida.AudioChannel;
import com.licht.lipusher.meida.VideoChannel;

public class LivePusher {
    private AudioChannel audioChannel;
    private VideoChannel videoChannel;

    static {
        System.loadLibrary("lipusher");
    }

    public LivePusher(Activity activity,int w,int h,int bitrate,int fps ,int cameraId){
        pusherInit();
        videoChannel = new VideoChannel(this,activity,cameraId,w,h,fps,bitrate);
        audioChannel = new AudioChannel(this);
    }
    public void setPreviewDisplay(SurfaceHolder surfaceHolder){
        videoChannel.setPreviewDisplay(surfaceHolder);
    }

    public  void  switchCamera(){
        videoChannel.switchCamera();
    }

    public void  startPerview(){
        videoChannel.startPerview();
    }

    public void stopPerview() {
        videoChannel.stopPerview();
    }

   public native void pusherInit();

    public native  void start_pusher(String path);

    public native void  native_setVideoEncInfo(int width,int height,int fps,int bitrate);
}

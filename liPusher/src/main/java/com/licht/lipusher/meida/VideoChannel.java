package com.licht.lipusher.meida;


import android.app.Activity;
import android.hardware.Camera;
import android.util.Log;
import android.view.SurfaceHolder;

import com.licht.lipusher.LivePusher;

public class VideoChannel implements Camera.PreviewCallback,CameraHelper.OnChangeSizeListener {
    private CameraHelper cameraHelper;
    private int mFps;
    private boolean isLiving;
    private int mBitrate;
    private LivePusher livePusher;

    public   VideoChannel(LivePusher livePusher, Activity activity, int cameraId, int width, int height , int mFps, int mBitrate) {
        this.cameraHelper = new CameraHelper(activity,cameraId,width,height);
        this.mFps = mFps;
        this.mBitrate = mBitrate;
        this.livePusher = livePusher;
        cameraHelper.setPreviewCallback(this);
        cameraHelper.setOnChangedSizeListener(this);
    }

    @Override
    public void onPreviewFrame(byte[] data, Camera camera) {
        Log.e("xiangjy", "onPreviewFrame: --" );
    }


    @Override
    public void onChanged(int w, int h) {
        livePusher .native_setVideoEncInfo(w,h,mFps,mBitrate);
    }
    public void switchCamera(){
        cameraHelper.switchCamera();
    }

    public void setPreviewDisplay(SurfaceHolder surfaceHolder){
        cameraHelper.setPreviewDisplay(surfaceHolder);
    }

    public void startPerview() {
        cameraHelper.startPreview();
    }

    public void stopPerview() {
        cameraHelper.stopPreview();

    }
}

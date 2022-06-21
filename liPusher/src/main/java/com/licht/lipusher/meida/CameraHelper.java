package com.licht.lipusher.meida;

import android.app.Activity;
import android.graphics.ImageFormat;
import android.hardware.Camera;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;

import androidx.annotation.NonNull;

import java.io.IOException;
import java.util.Iterator;
import java.util.List;

public class CameraHelper implements SurfaceHolder.Callback , Camera.PreviewCallback {
    private Activity mActivity;
    private int mHeight;
    private int mWidth;
    private int mCameraId;
    private Camera mCamera;
    private byte[] buffer;
    private SurfaceHolder mSurfaceHolder;
    private Camera.PreviewCallback mPreviewCallback;
    private int mRotation;
    private OnChangeSizeListener mOnChangeSizeListener;
    byte[] bytes;

    public CameraHelper(Activity activity, int cameraId, int width, int height) {
        mActivity = activity;
        mCameraId = cameraId;
        mWidth = width;
        mHeight = height;
    }

    public void setPreviewCallback(Camera.PreviewCallback callback) {
        mPreviewCallback = callback;
    }

    public void setOnChangedSizeListener(VideoChannel videoChannel) {
        mOnChangeSizeListener =videoChannel;
    }

    public void switchCamera() {
        if (mCameraId == Camera.CameraInfo.CAMERA_FACING_BACK) {
            mCameraId = Camera.CameraInfo.CAMERA_FACING_FRONT;
        } else {
            mCameraId = Camera.CameraInfo.CAMERA_FACING_BACK;
        }
        stopPreview();
        startPreview();

    }

    public void startPreview() {
        mCamera = Camera.open(mCameraId);
        Camera.Parameters parameters = mCamera.getParameters();
        parameters.setPreviewFormat(ImageFormat.NV21);
        setPreviewSize(parameters);
        setPreviewOrientation(parameters);
        int bitsPerPixel = ImageFormat.getBitsPerPixel(ImageFormat.NV21);

        buffer = new byte[mWidth * mHeight * bitsPerPixel/8];
        Log.e("BBBBB", "startPreview: bitsPerPixel"+bitsPerPixel );
        bytes = new byte[buffer.length];

        mCamera.addCallbackBuffer(buffer);
        mCamera.setPreviewCallbackWithBuffer(this);
        try {
            mCamera.setPreviewDisplay(mSurfaceHolder);
        } catch (IOException e) {
            e.printStackTrace();
        }
        mCamera.startPreview();
    }

    private void setPreviewOrientation(Camera.Parameters parameters) {
        Camera.CameraInfo info = new Camera.CameraInfo();
        Camera.getCameraInfo(mCameraId,info);
        mRotation = mActivity.getWindowManager().getDefaultDisplay().getRotation();
        int degress = 0;
        switch (mRotation){
            case Surface.ROTATION_0:
                degress =0;
                mOnChangeSizeListener.onChanged(mHeight,mWidth);
                break;
            case Surface.ROTATION_90:
                degress = 90;
                mOnChangeSizeListener.onChanged(mWidth,mHeight);
                break;
            case Surface.ROTATION_270:
                degress = 270;
                mOnChangeSizeListener.onChanged(mWidth,mHeight);
                break;

        }
        int result;
        if (info.facing == Camera.CameraInfo.CAMERA_FACING_FRONT){
            result = (info.orientation +degress)%360;
            result =(360 -result)%360;
        }else {
            result =( info.orientation -degress +360)%360;
        }

        mCamera.setDisplayOrientation(result);
    }

    private void setPreviewSize(Camera.Parameters parameters) {
        List<Camera.Size> supportedPreviewSizes = parameters.getSupportedPreviewSizes();
        Camera.Size size = supportedPreviewSizes.get(0);
        int m = Math.abs(size.height * size.width - mWidth * mHeight);
        supportedPreviewSizes.remove(0);
        Iterator<Camera.Size> iterator = supportedPreviewSizes.iterator();
        while (iterator.hasNext()){
            Camera.Size next = iterator.next();
            int n = Math.abs(next.height * next.width - mWidth * mHeight);
            if (n <m){
                m =n;
                size = next;
            }
            Log.e("BBBB", "xxx: "+next.width +"__"+next.height );
        }
        mWidth = size.width;
        mHeight = size.height;
        Log.e("BBBB", "setPreviewSize: "+size.width +"__"+size.height );
        parameters.setPreviewSize(mWidth,mHeight);
    }

    public void stopPreview() {
        if (mCamera != null) {
            mCamera.setPreviewCallback(null);
            mCamera.stopPreview();
            mCamera.release();
            mCamera = null;
        }


    }

    public void setPreviewDisplay(SurfaceHolder surfaceHolder) {
        mSurfaceHolder = surfaceHolder;
        surfaceHolder.addCallback(this);
    }

    @Override
    public void surfaceCreated(@NonNull SurfaceHolder holder) {

    }

    @Override
    public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {
        stopPreview();
        startPreview();
    }

    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder holder) {
        stopPreview();
    }

    @Override
    public void onPreviewFrame(byte[] data, Camera camera) {

        switch (mRotation){
            case Surface.ROTATION_0:
//                roat
                break;
        }

        if (mPreviewCallback != null){
            mPreviewCallback.onPreviewFrame(data,camera);
        }
        camera.addCallbackBuffer(buffer);
    }

    interface OnChangeSizeListener {

        void onChanged(int w, int h);
    }
}

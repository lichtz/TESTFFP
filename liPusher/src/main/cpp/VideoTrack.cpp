//
// Created by zhaoy on 2021/10/20.
//

#include <x264.h>
#include "VideoTrack.h"

void VideoTrack::setVideoEncInfo(int w, int h, int fps, int bitrate) {
    mW = w;
    mH = h;
    mFps = fps;
    mBitrate = bitrate;
    ySize = w * h;
    uvSize = ySize/4;
            //初始化x264的编码器

    x264_param_t param;
    x264_param_default_preset(&param,"ultrafast","zerolatency");
    param.i_level_idc = 32;
    param.i_csp=X264_CSP_I420;
    param.i_width = w;
    param.i_height = h;

    param.i_bframe =0;
    param.rc.i_rc_method = X264_RC_ABR;
    param.rc.i_bitrate = bitrate/1000;
    param.rc.i_vbv_max_bitrate = bitrate/1000*1.2;
    param.rc.i_vbv_buffer_size = bitrate/100;
    param.i_fps_num =fps;
    param.i_fps_den =1;
    param.i_timebase_den = param.i_fps_num;
    param.i_timebase_num = param.i_fps_den;
    param.b_vfr_input =0;
    param.i_keyint_max =fps*2;
    param.b_repeat_headers =1;
    param.i_threads =1;
    x264_param_apply_profile(&param,"baseline");
    x264_t*  videoCodec = x264_encoder_open(&param);
    pic_in =  new x264_picture_t ;
    x264_picture_alloc(pic_in,X264_CSP_I420,w,h);



}

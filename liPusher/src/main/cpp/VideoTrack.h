//
// Created by zhaoy on 2021/10/20.
//

#ifndef LIVIDEO_VIDEOTRACK_H
#define LIVIDEO_VIDEOTRACK_H
class VideoTrack{

public:
    void setVideoEncInfo(int w, int h, int fps, int bitrate);

private:
    int mW;
    int mH;
    int mFps;
    int mBitrate;
    int ySize;
    int uvSize;
    x264_picture_t *pic_in;
};


#endif //LIVIDEO_VIDEOTRACK_H

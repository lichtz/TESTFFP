////
//// Created by zhaoy on 2022/4/19.
////
//
//#include "AudioEncoder.h"
//AudioEncoder::AudioEncoder() {}
//
//AudioEncoder::~AudioEncoder() {}
//
//#ifndef LOG_TAG
//#define LOG_TAG "LICHT"
//#endif
//int AudioEncoder::init(int bitRate, int channels, int bitsPerSample, const char *aacFilePath,
//                       const char *codec_name) {
//    return init(bitRate, channels, 44100, bitsPerSample, aacFilePath, codec_name);
//
//}
//
//int AudioEncoder::init(int bitRate, int channels, int sampleRate, int bitsPerSample,
//                       const char *aacFilePath, const char *codec_name) {
//    avCodecContext = NULL;
//    avFormatContext = NULL;
//    input_frame = NULL;
//    samples = NULL;
//    samplesCursor = 0;
//    swrContext = NULL;
//    swrFrame = NULL;
//    swrBuffer = NULL;
//    convert_data = NULL;
//    isWriteHeaderSuccess = false;
//    totalEncodeTimeMills = 0;
//    totalSWRTimeMills = 0;
//    this->publishBitRate = bitRate;
//    this->audioChannels = channels;
//    this->audioSampleRate = sampleRate;
//    int ret;
////    avcodec_register_all();
////    av_register_all();
// avFormatContext =  avformat_alloc_context();
//    LOGEXX("aacFilePath is %s ", aacFilePath);
//    if ((ret = avformat_alloc_output_context2(&avFormatContext, NULL, NULL, aacFilePath)) != 0) {
//        LOGEXX("avFormatContext   alloc   failed : %s", av_err2str(ret));
//        return -1;
//    }
//    return 0;
//}
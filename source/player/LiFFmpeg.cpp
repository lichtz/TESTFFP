//
// Created by zhaoy on 2021/9/22.
//

#include <pthread.h>
#include "LiFFmpeg.h"
#include "cstring"
#include "macro.h"
#include "queen.h"

extern "C" {
#include "libavformat/avformat.h"
#include "libavutil/time.h"
}

LiFFmpeg::LiFFmpeg(JavaCallHelper *pHelper, char *dataSource) {
    this->javaCallHelper = pHelper;
    this->dataSource = new char[strlen(dataSource) + 1];
    strcpy(this->dataSource, dataSource);

}

void *task_prepare(void *args) {
    LiFFmpeg *fFmpeg = static_cast<LiFFmpeg *>(args);
    fFmpeg->_prepare();

    return 0;
}

void *task_start(void *args) {
    LiFFmpeg *fFmpeg = static_cast<LiFFmpeg *>(args);
    fFmpeg->_start();
    return 0;
}


LiFFmpeg::~LiFFmpeg() {
    if (dataSource) {
        delete dataSource;
        dataSource = 0;
    }
    if (javaCallHelper) {
        delete javaCallHelper;
        javaCallHelper = 0;
    }

    DELETE(dataSource);
    DELETE(javaCallHelper);
}

void LiFFmpeg::prepare() {
    pthread_create(&pid_prepare, 0, task_prepare, this);

}

AVFormatContext *avFormatContext;

void LiFFmpeg::_prepare() {

    avFormatContext = avformat_alloc_context();
    AVDictionary *dictionary = 0;
    av_dict_set(&dictionary, "timeout", "10000000", 0);
    int ret = avformat_open_input(&avFormatContext, dataSource, 0, &dictionary);
    av_dict_free(&dictionary);
    if (ret) {
        LOGE("open error %s ", av_err2str(ret));
        //calljavaerror
        if (javaCallHelper) {
            javaCallHelper->onError(THREAD_CHILD, FFMPEG_CAN_NOT_OPEN_URL);
        }
        return;
    }
    int findStreamRet = avformat_find_stream_info(avFormatContext, NULL);
    if (findStreamRet < 0) {
        LOGE("find_stream %s", av_err2str(findStreamRet));
        return;
    }

    for (int i = 0; i < avFormatContext->nb_streams; ++i) {
        AVStream *stream = avFormatContext->streams[i];
        AVCodecParameters *codecParameters = stream->codecpar;
        AVCodec *avCodec = avcodec_find_decoder(codecParameters->codec_id);
        if (!avCodec) {
            //todo
            return;
        }
        AVCodecContext *avCodecContext = avcodec_alloc_context3(avCodec);
        avcodec_parameters_to_context(avCodecContext, codecParameters);

        int avCodecOpenRet = avcodec_open2(avCodecContext, avCodec, NULL);
        if (avCodecOpenRet) {
            //todo
            return;
        }

        if (codecParameters->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioChannel = new AudioChannel(i, avCodecContext);
        } else if (codecParameters->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoChannel = new VideoChannel(i, avCodecContext);
            videoChannel->setRenderCallback(renderCallback);
        }

        if (!audioChannel && !videoChannel) {
            //todo
            return;
        }
        if (audioChannel && videoChannel) {
            if (frame_queue_init(&audioChannel->frameQueue, &audioChannel->packetQueue,
                                 SAMPLE_QUEUE_SIZE, 0) < 0) {
                LOGE("frame_queue_init audioChannel error");
                return;
            }
            if (frame_queue_init(&videoChannel->frameQueue, &videoChannel->packetQueue,
                                 VIDEO_PICTURE_QUEUE_SIZE, 1) < 0) {
                LOGE("frame_queue_init frameQueue error");
                return;
            }
            if (packet_queue_init(&audioChannel->packetQueue) < 0) {
                LOGE("packet_queue_init  audioChannel error");
                return;
            }
            if (packet_queue_init(&videoChannel->packetQueue) < 0) {
                LOGE("packet_queue_init  videoChannel error");
                return;
            }

            if (javaCallHelper) {
                javaCallHelper->onPrepared(THREAD_CHILD);
            }
        }


    }


}


void LiFFmpeg::start() {
    isPlaying = 1;
    pthread_create(&pid_start, 0, task_start, this);
    pthread_setname_np(pid_start, "readPacket");
    if (videoChannel) {
        videoChannel->start();
    }
    if (audioChannel) {
        audioChannel->start();
    }


}

void LiFFmpeg::_start() {
    packet_queue_start(&audioChannel->packetQueue);
    packet_queue_start(&videoChannel->packetQueue);
    while (isPlaying) {
        if (videoChannel && videoChannel->packetQueue.nb_packets > 50) {
            av_usleep(1000 * 10);
            continue;
        }
        if (audioChannel && audioChannel->packetQueue.nb_packets > 250) {
            av_usleep(1000 * 10);
            continue;
        }

        AVPacket *packet = av_packet_alloc();
        int ret = av_read_frame(avFormatContext, packet);
        if (!ret) {
            if (videoChannel && packet->stream_index == videoChannel->id) {
                packet_queue_put(&videoChannel->packetQueue, packet);

            } else if (audioChannel && packet->stream_index == audioChannel->id) {
                packet_queue_put(&audioChannel->packetQueue, packet);
            }

        } else if (ret == AVERROR_EOF) {
            if (packet->stream_index == videoChannel->id) {
//                packet_queue_put_nullpacket(&videoChannel->packetQueue,packet,packet->stream_index);
                LOGE("LiFFmpeg::_start()  videoChannel eof");
                videoChannel->eof = -1;
                videoChannel->packetQueue.endIndex = 1;
                audioChannel->eof = -1;
                audioChannel->packetQueue.endIndex = 1;
                break;
            }
            if (packet->stream_index == audioChannel->id) {
//                packet_queue_put_nullpacket(&audioChannel->packetQueue,packet,packet->stream_index);
//不走不知道为什么
                audioChannel->eof = -1;
                audioChannel->packetQueue.endIndex = 1;
                LOGE("LiFFmpeg::_start()  audioChannel eof");
            }
            av_packet_unref(packet);
            if (videoChannel->eof == -1 && audioChannel->eof == -1) {
                break;
            }


        } else {
            av_packet_unref(packet);
            if (javaCallHelper) {
                javaCallHelper->onError(THREAD_CHILD, FFMPEG_CAN_NOT_OPEN_URL);
            }
            break;
        }
    }
    isPlaying = 0;
    videoChannel->stop();
    audioChannel->stop();
}

void LiFFmpeg::setRenderCallback(RenderCallback r) {
    this->renderCallback = r;
}


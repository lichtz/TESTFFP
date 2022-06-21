//
// Created by zhaoy on 2021/10/7.
//

#ifndef LIVIDEO_QUEEN_H
#define LIVIDEO_QUEEN_H



extern "C" {
#include "libavutil/frame.h"
#include "libavcodec/avcodec.h"
#include "libavutil/fifo.h"
}

#include "pthread.h"
#define VIDEO_PICTURE_QUEUE_SIZE 3
#define SUBPICTURE_QUEUE_SIZE 16
#define SAMPLE_QUEUE_SIZE 9
#define FRAME_QUEUE_SIZE FFMAX(SAMPLE_QUEUE_SIZE, FFMAX(VIDEO_PICTURE_QUEUE_SIZE, SUBPICTURE_QUEUE_SIZE))

typedef struct AVPacketNode {
    AVPacket *pkt;
    int serial;

} AVPacketNode;

typedef struct PacketQueue {
    AVFifoBuffer *pkt_list;
    int nb_packets;
    int size;
    int64_t duration;
    int abort_request;
    int serial;
    pthread_cond_t cond;
    pthread_mutex_t mutex;
    int endIndex = 0;

} PacketQueue;

typedef struct Frame {
    AVFrame *frame;
    AVSubtitle sub;
    int serial;
    double pts;           /* presentation timestamp for the frame */
    double duration;      /* estimated duration of the frame */
    int64_t pos;          /* byte position of the frame in the input file */
    int width;
    int height;
    int format;
    AVRational sar;
    int uploaded;
    int flip_v;
} Frame;


typedef struct FrameQueue {
    Frame queue[FRAME_QUEUE_SIZE];
    int rindex;
    int windex;
    int size;
    int max_size;
    int keep_last;
    int rindex_shown;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    PacketQueue *pktq;
    int endIndex = 0;
} FrameQueue;


static void packet_queue_destroy(PacketQueue *q);
static void frame_queue_destory(FrameQueue *f);
static int frame_queue_init(FrameQueue *f, PacketQueue *pktq, int max_size, int keep_last);
static Frame *frame_queue_peek_writable(FrameQueue *f);
static void frame_queue_unref_item(Frame *vp);
static void frame_queue_push(FrameQueue *f);
static Frame *frame_queue_peek_readable(FrameQueue *f);
static void frame_queue_next(FrameQueue *f);
static int frame_queue_nb_remaining(FrameQueue *f);
static Frame *frame_queue_peek(FrameQueue *f);
static Frame *frame_queue_peek_last(FrameQueue *f);


static int packet_queue_init(PacketQueue *q);
static void packet_queue_start(PacketQueue *q);
static void packet_queue_abort(PacketQueue *q);
static int packet_queue_put(PacketQueue *q, AVPacket *pkt);
static int packet_queue_get(PacketQueue *q, AVPacket *pkt, int block, int *serial);
static int packet_queue_put_nullpacket(PacketQueue *q, AVPacket *pkt, int stream_index);
static void packet_queue_flush(PacketQueue *q);







static int frame_queue_init(FrameQueue *f, PacketQueue *pktq, int max_size, int keep_last)
{
    int i;
    memset(f, 0, sizeof(FrameQueue));
    pthread_mutex_init(&f->mutex,0);
    pthread_cond_init(&f->cond,0);

    f->pktq = pktq;
    f->max_size = FFMIN(max_size, FRAME_QUEUE_SIZE);
    f->keep_last = !!keep_last;
    for (i = 0; i < f->max_size; i++)
        if (!(f->queue[i].frame = av_frame_alloc()))
            return AVERROR(ENOMEM);
    return 0;
}

static void frame_queue_unref_item(Frame *vp)
{
    av_frame_unref(vp->frame);
    avsubtitle_free(&vp->sub);
}

static void frame_queue_destory(FrameQueue *f)
{
    int i;
    for (i = 0; i < f->max_size; i++) {
        Frame *vp = &f->queue[i];
        frame_queue_unref_item(vp);
        av_frame_free(&vp->frame);
    }
    pthread_mutex_destroy(&f->mutex);
    pthread_cond_destroy(&f->cond);
}



static Frame *frame_queue_peek_writable(FrameQueue *f)
{
    /* wait until we have space to put a new frame */
    pthread_mutex_lock(&f->mutex);
    while (f->size >= f->max_size &&
           !f->pktq->abort_request) {
        pthread_cond_wait(&f->cond,&f->mutex);
    }
    pthread_mutex_unlock(&f->mutex);

    if (f->pktq->abort_request)
        return NULL;

    return &f->queue[f->windex];
}

static void frame_queue_push(FrameQueue *f)
{
    if (++f->windex == f->max_size)
        f->windex = 0;
    pthread_mutex_lock(&f->mutex);
    f->size++;
    pthread_cond_signal(&f->cond);
    pthread_mutex_unlock(&f->mutex);
}



static Frame *frame_queue_peek_readable(FrameQueue *f)
{
    /* wait until we have a readable a new frame */
    pthread_mutex_lock(&f->mutex);
    while (f->size - f->rindex_shown <= 0 &&
           !f->pktq->abort_request) {
        pthread_cond_wait(&f->cond,&f->mutex);
    }
    pthread_mutex_unlock(&f->mutex);

    if (f->pktq->abort_request)
        return NULL;

    return &f->queue[(f->rindex + f->rindex_shown) % f->max_size];
}

static void frame_queue_next(FrameQueue *f)
{
    if (f->keep_last && !f->rindex_shown) {
        f->rindex_shown = 1;
        return;
    }
    frame_queue_unref_item(&f->queue[f->rindex]);
    if (++f->rindex == f->max_size)
        f->rindex = 0;
    pthread_mutex_lock(&f->mutex);
    f->size--;
    pthread_cond_signal(&f->cond);
    pthread_mutex_unlock(&f->mutex);
}

static int frame_queue_nb_remaining(FrameQueue *f)
{
    return f->size - f->rindex_shown;
}
//peek 出当前的。因为f->rindex + f->rindex_shown可能会超过max_size,所以用了取余
static Frame *frame_queue_peek(FrameQueue *f)
{
    return &f->queue[(f->rindex + f->rindex_shown) % f->max_size];
}
//取出最后一个。这个是在keep_last的时候才能用吗？这个表示的是真正的最后一个。因为通常我们会在视频和音频的队列中默认保留一帧的数据
static Frame *frame_queue_peek_last(FrameQueue *f)
{
    return &f->queue[f->rindex];
}

//seek 会用
/* return last shown position */
static int64_t frame_queue_last_pos(FrameQueue *f)
{
    Frame *fp = &f->queue[f->rindex];
    if (f->rindex_shown && fp->serial == f->pktq->serial)
        return fp->pos;
    else
        return -1;
}


static void frame_queue_signal(FrameQueue *f)
{
    pthread_mutex_lock(&f->mutex);
    pthread_cond_signal(&f->cond);
    pthread_mutex_unlock(&f->mutex);
}






/* packet queue handling */
static int packet_queue_init(PacketQueue *q)
{
    memset(q, 0, sizeof(PacketQueue));
    q->pkt_list = av_fifo_alloc(sizeof(AVPacketNode));
    if (!q->pkt_list)
        return AVERROR(ENOMEM);
    pthread_mutex_init(&q->mutex, 0);
    pthread_cond_init(&q->cond, 0);
    q->abort_request = 1;
    return 0;
}


static void packet_queue_start(PacketQueue *q) {
    pthread_mutex_lock(&q->mutex);
    q->abort_request = 0;
    q->serial++;
    pthread_mutex_unlock(&q->mutex);
}


static void packet_queue_abort(PacketQueue *q) {
    pthread_mutex_lock(&q->mutex);

    q->abort_request = 1;

    pthread_cond_signal(&q->cond);

    pthread_mutex_unlock(&q->mutex);
}

static int packet_queue_put_private(PacketQueue *q, AVPacket *pkt)
{
    AVPacketNode pkt1;

    if (q->abort_request)
        return -1;

    if (av_fifo_space(q->pkt_list) < sizeof(pkt1)) {
        if (av_fifo_grow(q->pkt_list, sizeof(pkt1)) < 0)
            return -1;
    }

    pkt1.pkt = pkt;
    pkt1.serial = q->serial;

    av_fifo_generic_write(q->pkt_list, &pkt1, sizeof(pkt1), NULL);
    q->nb_packets++;
    q->size += pkt1.pkt->size + sizeof(pkt1);
    q->duration += pkt1.pkt->duration;
    /* XXX: should duplicate packet data in DV case */
    pthread_cond_signal(&q->cond);
    return 0;
}
static int packet_queue_put(PacketQueue *q, AVPacket *pkt)
{
    AVPacket *pkt1;
    int ret;

    pkt1 = av_packet_alloc();
    if (!pkt1) {
        av_packet_unref(pkt);
        return -1;
    }
    av_packet_move_ref(pkt1, pkt);

    pthread_mutex_lock(&q->mutex);
    ret = packet_queue_put_private(q, pkt1);
    pthread_mutex_unlock(&q->mutex);

    if (ret < 0)
        av_packet_free(&pkt1);

    return ret;
}

static int packet_queue_get(PacketQueue *q, AVPacket *pkt, int block, int *serial)
{
    AVPacketNode pkt1;
    int ret;

    pthread_mutex_lock(&q->mutex);

    for (;;) {
        if (q->abort_request) {
            ret = -1;
            break;
        }
        if (q->endIndex){
            ret = -2;
            break;
        }

        if (av_fifo_size(q->pkt_list) >= sizeof(pkt1)) {
            av_fifo_generic_read(q->pkt_list, &pkt1, sizeof(pkt1), NULL);
            q->nb_packets--;
            q->size -= pkt1.pkt->size + sizeof(pkt1);
            q->duration -= pkt1.pkt->duration;
            av_packet_move_ref(pkt, pkt1.pkt);
            if (serial)
                *serial = pkt1.serial;
            av_packet_free(&pkt1.pkt);
            ret = 1;
            break;
        } else if (!block) {
            ret = 0;
            break;
        } else {
            pthread_cond_wait(&q->cond, &q->mutex);
        }
    }
    pthread_mutex_unlock(&q->mutex);
    return ret;
}



static int packet_queue_put_nullpacket(PacketQueue *q, AVPacket *pkt, int stream_index){
    pkt->stream_index = stream_index;
    return packet_queue_put(q, pkt);
}


static void packet_queue_flush(PacketQueue *q)
{
    AVPacketNode pkt1;

    pthread_mutex_lock(&q->mutex);
    while (av_fifo_size(q->pkt_list) >= sizeof(pkt1)) {
        av_fifo_generic_read(q->pkt_list, &pkt1, sizeof(pkt1), NULL);
        av_packet_free(&pkt1.pkt);
    }
    q->nb_packets = 0;
    q->size = 0;
    q->duration = 0;
    q->serial++;
    pthread_mutex_unlock(&q->mutex);
}


static void packet_queue_destroy(PacketQueue *q) {
    packet_queue_flush(q);
    av_fifo_freep(&q->pkt_list);
    pthread_mutex_destroy(&q->mutex);
    pthread_cond_destroy(&q->cond);

}




#endif //LIVIDEO_QUEEN_H

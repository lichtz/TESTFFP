package com.example.opengles

import android.media.MediaCodec
import android.media.MediaCodecInfo
import android.media.MediaFormat
import android.util.Log

object Audio {
    fun initCoverPcmToAAC(sampleRate:Int, channels:Int, bitRate:Int):MediaCodec?{
        val encodeAudioFormat = MediaFormat.createAudioFormat(MediaFormat.MIMETYPE_AUDIO_AAC, sampleRate, channels)
            encodeAudioFormat.setInteger(MediaFormat.KEY_BIT_RATE,bitRate);
        encodeAudioFormat.setInteger(MediaFormat.KEY_AAC_PROFILE,MediaCodecInfo.CodecProfileLevel.AACObjectLC);
        encodeAudioFormat.setInteger(MediaFormat.KEY_MAX_INPUT_SIZE,10*1024);
        val mediaCodec = MediaCodec.createByCodecName(MediaFormat.MIMETYPE_AUDIO_AAC)
        mediaCodec.configure(encodeAudioFormat,null,null,MediaCodec.CONFIGURE_FLAG_ENCODE)
        if (mediaCodec == null){
            Log.e("problem", "create mediaEncode failed");
            return null
        }
       mediaCodec.start()
        return  mediaCodec;
    }


    fun  encodePCMtoAAc(  mediaCodec:MediaCodec,byte:ByteArray,length:Int,outputAACDelegate: OutputAACDelegate){
        val dequeueInputBufferIndex = mediaCodec.dequeueInputBuffer(-1)
        if (dequeueInputBufferIndex > 0){
            val inputBuffer = mediaCodec.getInputBuffer(dequeueInputBufferIndex)
            inputBuffer?.apply {
                clear()
                put(byte)
                mediaCodec.queueInputBuffer(dequeueInputBufferIndex,0,length,System.nanoTime(),0);
            }
        }
        val bufferInfo = MediaCodec.BufferInfo();
        var dequeueOutputBufferIndex = mediaCodec.dequeueOutputBuffer(bufferInfo,0);
        while (dequeueOutputBufferIndex >=0 ){
            val outputBuffer = mediaCodec.getOutputBuffer(dequeueOutputBufferIndex)
                outputBuffer?.apply {
                    val outPacketSize = bufferInfo.size + 7 // 7为ADTS头部的大小
                    position(bufferInfo.offset)
                    limit(bufferInfo.offset + bufferInfo.size)
                    val outData = ByteArray(outPacketSize)
                    addADTStoPacket(outData, outPacketSize) //添加ADTS 代码后面会贴上
                    outputBuffer[outData, 7, bufferInfo.size] //将编码得到的AAC数据 取出到byte[]中 偏移量offset=7
                    position(bufferInfo.offset)
                    outputAACDelegate.outputAACPacket(outData)
                    mediaCodec.releaseOutputBuffer(dequeueOutputBufferIndex,false)
                }

            dequeueOutputBufferIndex = mediaCodec.dequeueOutputBuffer(bufferInfo,0);
        }

    }

    private fun addADTStoPacket(packet: ByteArray, packetLen: Int) {
        val profile = 2 // AAC LC
        val freqIdx = 4 // 44.1KHz
        val chanCfg = 2 // CPE
        // fill in ADTS data 70
        packet[0] = 0xFF.toByte()
        packet[1] = 0xF9.toByte()
        packet[2] = ((profile - 1 shl 6) + (freqIdx shl 2) + (chanCfg shr 2)).toByte()
        packet[3] = ((chanCfg and 3 shl 6) + (packetLen shr 11)).toByte()
        packet[4] = (packetLen and 0x7FF shr 3).toByte()
        packet[5] = ((packetLen and 7 shl 5) + 0x1F).toByte()
        packet[6] = 0xFC.toByte()
    }


    fun stop(mediaCodec:MediaCodec?) {
            mediaCodec?.stop()
            mediaCodec?.release()
    }
}

 interface OutputAACDelegate {
    fun outputAACPacket(data: ByteArray?)
}
package com.example.opengles

object AudioToolsHelper {
    external fun encodePCMToAAC (pcmPath:String, audioChannels:Int,bitRate:Int,sampleRate:Int,aacPath:String);
}
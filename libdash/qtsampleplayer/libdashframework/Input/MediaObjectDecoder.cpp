/*
 * MediaObjectDecoder.cpp
 *****************************************************************************
 * Copyright (C) 2013, bitmovin Softwareentwicklung OG, All Rights Reserved
 *
 * Email: libdash-dev@vicky.bitmovin.net
 *
 * This source code and its use and distribution, is subject to the terms
 * and conditions of the applicable license agreement.
 *****************************************************************************/

#include "MediaObjectDecoder.h"

using namespace sampleplayer::decoder;
using namespace libdash::framework::input;
using namespace libdash::framework::buffer;
using namespace dash::mpd;

MediaObjectDecoder::MediaObjectDecoder  (MediaObject* initSegment, MediaObject* mediaSegment, IMediaObjectDecoderObserver *observer) :
                    observer            (observer),
                    initSegment         (initSegment),
                    mediaSegment        (mediaSegment),
                    decoderInitialized  (false),
                    initSegmentOffset   (0)
{
    this->decoder = new LibavDecoder(this);
    this->decoder->AttachVideoObserver(this);
    this->decoder->AttachAudioObserver(this);
}
MediaObjectDecoder::~MediaObjectDecoder()
{
    //this->Stop();
    delete(this->decoder);
}

bool    MediaObjectDecoder::Start                   ()
{
    if(!decoder->Init())
        return false;

    this->run = true;
    this->decoderInitialized = true;
    this->threadHandle = CreateThreadPortable (Decode, this);

    if(this->threadHandle == NULL)
        return false;

    return true;
}
void    MediaObjectDecoder::Stop                    ()
{
    this->run = false;
    WaitForSingleObject(this->threadHandle, INFINITE);
}
void    MediaObjectDecoder::OnVideoDataAvailable    (const uint8_t **data, videoFrameProperties* props)
{
    this->observer->OnVideoFrameDecoded(data, props);
}
void    MediaObjectDecoder::OnAudioDataAvailable    (const uint8_t **data, audioFrameProperties* props)
{
    this->observer->OnAudioSampleDecoded(data, props);
}

void*   MediaObjectDecoder::Decode                  (void *data)
{
    MediaObjectDecoder *mediaDecodingThread = (MediaObjectDecoder *) data;

    while(mediaDecodingThread->decoder->Decode() && mediaDecodingThread->run);

    mediaDecodingThread->Notify();

    mediaDecodingThread->decoder->Stop();

    delete mediaDecodingThread;
    return NULL;
}
int     MediaObjectDecoder::Read                    (uint8_t *buf, int buf_size)
{
    int ret = 0;
    if (!this->decoderInitialized) 
    {
        ret = this->initSegment->Peek(buf, buf_size, this->initSegmentOffset);
        this->initSegmentOffset += (size_t) ret;
    }

    if (ret == 0) 
    {
        ret = this->mediaSegment->Read(buf, buf_size);
    }

    return ret;
}
void    MediaObjectDecoder::Notify                  ()
{
    this->observer->OnDecodingFinished();
}
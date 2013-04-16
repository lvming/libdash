/*
 * MultimediaStream.cpp
 *****************************************************************************
 * Copyright (C) 2013, bitmovin Softwareentwicklung OG, All Rights Reserved
 *
 * Email: libdash-dev@vicky.bitmovin.net
 *
 * This source code and its use and distribution, is subject to the terms
 * and conditions of the applicable license agreement.
 *****************************************************************************/

#include "MultimediaStream.h"

using namespace sampleplayer::managers;
using namespace sampleplayer::decoder;
using namespace libdash::framework::adaptation;
using namespace libdash::framework::input;
using namespace libdash::framework::buffer;
using namespace dash::mpd;

MultimediaStream::MultimediaStream  (IMPD *mpd, uint32_t bufferSize, uint32_t frameBufferSize, uint32_t sampleBufferSize) :
                  segmentBufferSize (bufferSize),
                  frameBufferSize   (frameBufferSize),
                  sampleBufferSize  (sampleBufferSize),
                  dashManager       (NULL),
                  mpd               (mpd)
{
    this->Init();
}
MultimediaStream::~MultimediaStream ()
{
    this->Stop();
    delete this->dashManager;

    this->frameBuffer->Clear();
    delete this->frameBuffer;

    // clear audio buffer
    // delete audio buffer
}

uint32_t    MultimediaStream::GetPosition               ()
{
    return this->dashManager->GetPosition();
}
void        MultimediaStream::SetPosition               (uint32_t segmentNumber)
{
    this->dashManager->SetPosition(segmentNumber);
}
void        MultimediaStream::SetPositionInMsec         (uint32_t milliSecs)
{
    this->dashManager->SetPositionInMsec(milliSecs);
}
void        MultimediaStream::Init                      ()
{
    this->dashManager   = new DASHManager(this->segmentBufferSize, this, this->mpd);
    this->frameBuffer   = new QImageBuffer(this->frameBufferSize);
    // audioSampleBuffer ...
}
bool        MultimediaStream::Start                     ()
{
    if(!this->StartDownload())
        return false;

    return true;
}
bool        MultimediaStream::StartDownload             ()
{
    if(!dashManager->Start())
        return false;

    return true;
}
void        MultimediaStream::Stop                      ()
{
    this->StopDownload();
}
void        MultimediaStream::StopDownload              ()
{
    this->dashManager->Stop();
}
void        MultimediaStream::Clear                     ()
{
    this->dashManager->Clear();
}
void        MultimediaStream::AddFrame                  (QImage *frame)
{
    this->frameBuffer->PushBack(frame);
}
QImage*     MultimediaStream::GetFrame                  ()
{
    return this->frameBuffer->GetFront();
}
void        MultimediaStream::AttachStreamObserver      (IStreamObserver *observer)
{
    this->observers.push_back(observer);
}
void        MultimediaStream::AttachBufferObserver      (IBufferObserver *observer)
{
}
void        MultimediaStream::SetRepresentation         (IPeriod *period, IAdaptationSet *adaptationSet, IRepresentation *representation)
{
    this->dashManager->SetRepresentation(period, adaptationSet, representation);
}
void        MultimediaStream::EnqueueRepresentation     (IPeriod *period, IAdaptationSet *adaptationSet, IRepresentation *representation)
{
    this->dashManager->EnqueueRepresentation(period, adaptationSet, representation);
}
void        MultimediaStream::SetAdaptationLogic        (libdash::framework::adaptation::IAdaptationLogic *logic)
{
    this->logic = logic;
}

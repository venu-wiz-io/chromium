/*
 * Copyright (C) 2021 Vizio Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY VIZIO INC. AND ITS CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "third_party/blink/public/platform/modules/mediastream/web_media_stream_helper.h"

#include <memory>
#include "third_party/blink/public/platform/web_string.h"
#include "third_party/blink/public/web/web_local_frame.h"
#include "third_party/blink/renderer/platform/mediastream/media_stream_audio_track.h"
#include "third_party/blink/renderer/platform/mediastream/media_stream_component.h"
#include "third_party/blink/renderer/platform/mediastream/media_stream_descriptor.h"
#include "third_party/blink/renderer/platform/mediastream/media_stream_source.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/modules/mediastream/media_stream_local_frame_wrapper.h"

namespace blink {

bool IsPlayableTrack(MediaStreamComponent* component) {
  return component && component->Source() &&
         component->Source()->GetReadyState() !=
             MediaStreamSource::kReadyStateEnded;
}

WebVector<WebMediaStreamTrack> WebMediaStreamHelper::AudioTracks(
    const WebMediaStream& stream) {
  MediaStreamDescriptor& descriptor = *stream;
  size_t number_of_tracks = descriptor.NumberOfAudioComponents();
  WebVector<WebMediaStreamTrack> result(number_of_tracks);
  for (size_t i = 0; i < number_of_tracks; ++i) {
    result[i] = descriptor.AudioComponent(i);
  }
  return result;
}

WebVector<WebMediaStreamTrack> WebMediaStreamHelper::VideoTracks(
    const WebMediaStream& stream) {
  MediaStreamDescriptor& descriptor = *stream;
  size_t number_of_tracks = descriptor.NumberOfVideoComponents();
  WebVector<WebMediaStreamTrack> result(number_of_tracks);
  for (size_t i = 0; i < number_of_tracks; ++i) {
    result[i] = descriptor.VideoComponent(i);
  }
  return result;
}

WebMediaStreamTrack WebMediaStreamHelper::GetAudioTrack(
    const WebMediaStream& stream, const WebString& track_id) {
  MediaStreamDescriptor& descriptor = *stream;
  size_t number_of_tracks = descriptor.NumberOfAudioComponents();
  String id = String(track_id);
  for (size_t i = 0; i < number_of_tracks; ++i) {
    MediaStreamComponent* audio_component = descriptor.AudioComponent(i);
    DCHECK(audio_component);
    if (audio_component->Id() == id)
      return WebMediaStreamTrack(descriptor.AudioComponent(i));
  }
  return WebMediaStreamTrack();
}

WebMediaStreamTrack WebMediaStreamHelper::GetVideoTrack(
    const WebMediaStream& stream, const WebString& track_id) {
  MediaStreamDescriptor& descriptor = *stream;
  size_t number_of_tracks = descriptor.NumberOfVideoComponents();
  String id = track_id;
  for (size_t i = 0; i < number_of_tracks; ++i) {
    MediaStreamComponent* video_component = descriptor.VideoComponent(i);
    DCHECK(video_component);
    if (video_component->Id() == id)
      return WebMediaStreamTrack(descriptor.VideoComponent(i));
  }
  return WebMediaStreamTrack();
}

RendererReloadAction WebMediaStreamHelper::GetRenderActionAndId(
    const WebMediaStream& web_stream, WebString &track_id,
    bool audio_render_exist) {
  RendererReloadAction renderer_action = RendererReloadAction::KEEP_RENDERER;
  MediaStreamDescriptor& descriptor = *web_stream;
  auto audio_components = descriptor.AudioComponents();

  if (audio_components.IsEmpty()) {
    if (audio_render_exist)
      renderer_action = RendererReloadAction::REMOVE_RENDERER;
    track_id = WebString();
  } else if (WebString(audio_components[0]->Id()) != track_id &&
             IsPlayableTrack(audio_components[0])) {
    renderer_action = RendererReloadAction::NEW_RENDERER;
    track_id = audio_components[0]->Id();
  }

  return renderer_action;
}

bool WebMediaStreamHelper::IsLocal(const WebMediaStream& web_stream)
{
  MediaStreamDescriptor& descriptor = *web_stream;
  size_t number_of_video_tracks = descriptor.NumberOfVideoComponents();
  if (number_of_video_tracks > 0) {
    MediaStreamTrackPlatform* track =
        descriptor.VideoComponent(0)->GetPlatformTrack();
    return track && track->is_local_track();
  } else {
    size_t number_of_audio_tracks = descriptor.NumberOfVideoComponents();
    if (number_of_audio_tracks > 0) {
      MediaStreamComponent* track = descriptor.AudioComponent(0);
      return MediaStreamAudioTrack::From(track)->is_local_track();
    }
  }
  return false;
}

MediaStreamToExternalFrameWrapper::MediaStreamToExternalFrameWrapper(
                                                          WebLocalFrame* frame)
     : internal_frame_(std::make_unique<blink::MediaStreamInternalFrameWrapper>
                                                                     (frame)) {
}

MediaStreamToExternalFrameWrapper::~MediaStreamToExternalFrameWrapper() {
}

WebLocalFrame* MediaStreamToExternalFrameWrapper::web_frame() {
    return internal_frame_->web_frame();
}

}  // namespace blink

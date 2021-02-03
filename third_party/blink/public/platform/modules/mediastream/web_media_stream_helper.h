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

#ifndef THIRD_PARTY_BLINK_PUBLIC_PLATFORM_MODULES_MEDIASTREAM_WEB_MEDIA_STREAM_HELPER_H_
#define THIRD_PARTY_BLINK_PUBLIC_PLATFORM_MODULES_MEDIASTREAM_WEB_MEDIA_STREAM_HELPER_H_

#include "third_party/blink/public/platform/modules/mediastream/web_media_stream.h"
#include "third_party/blink/public/platform/modules/mediastream/web_media_stream_track.h"

namespace blink {

class WebMediaStreamTrack;
class MediaStreamInternalFrameWrapper;
class WebLocalFrame;

enum class RendererReloadAction {
  KEEP_RENDERER,
  REMOVE_RENDERER,
  NEW_RENDERER
};

class WebMediaStreamHelper {
 public:
  BLINK_PLATFORM_EXPORT static WebVector<WebMediaStreamTrack>
  AudioTracks(const WebMediaStream& stream);

  BLINK_PLATFORM_EXPORT static WebVector<WebMediaStreamTrack>
  VideoTracks(const WebMediaStream& stream);

  static BLINK_PLATFORM_EXPORT WebMediaStreamTrack
  GetAudioTrack(const WebMediaStream& stream, const WebString& track_id);

  BLINK_PLATFORM_EXPORT static WebMediaStreamTrack
  GetVideoTrack(const WebMediaStream& stream, const WebString& track_id);

  BLINK_PLATFORM_EXPORT static RendererReloadAction
  GetRenderActionAndId(const WebMediaStream &web_stream, WebString &track_id,
                                               bool audio_render_exist);
};

class MediaStreamToExternalFrameWrapper {

 public:
  MediaStreamToExternalFrameWrapper(WebLocalFrame* web_frame);
  ~MediaStreamToExternalFrameWrapper();
  WebLocalFrame* web_frame();

 private:
  std::unique_ptr<blink::MediaStreamInternalFrameWrapper> internal_frame_;

  DISALLOW_COPY_AND_ASSIGN(MediaStreamToExternalFrameWrapper);
};

}  // namespace blink

#endif // THIRD_PARTY_BLINK_PUBLIC_PLATFORM_MODULES_MEDIASTREAM_WEB_MEDIA_STREAM_HELPER_H_

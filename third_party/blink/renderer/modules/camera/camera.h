// Copyright 2019 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_CAMERA_CAMERA_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_CAMERA_CAMERA_H_

#include <utility>

#include "base/macros.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/bindings/core/v8/script_promise.h"
#include "third_party/blink/renderer/core/frame/local_dom_window.h"
#include "third_party/blink/renderer/core/dom/events/event_target.h"
#include "third_party/blink/renderer/core/execution_context/execution_context_lifecycle_observer.h"
#include "third_party/blink/renderer/core/fileapi/blob.h"

namespace blink {

class LocalFrame;
class LocalDOMWindow;
class ScriptState;
class CameraVizioConfig;
class CameraSettingJson;

class Camera final : public ScriptWrappable,
	                    public ExecutionContextClient{
  DEFINE_WRAPPERTYPEINFO();

 public:
  static Camera* Create(ExecutionContext*);

  Camera(ExecutionContext*);
  ~Camera() override;

  void setConfig(CameraSettingJson* cameraSetting_json);
  ScriptPromise getConfig(ScriptState* script_state,Vector<String> config);
  void getConfigResponse(ScriptPromiseResolver* resolver,Vector<String> config);

  const AtomicString& InterfaceName() const ;
  ExecutionContext* GetExecutionContext() const;
  void Trace(Visitor*) const override;  

};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_CAMERA_CAMERA_H_


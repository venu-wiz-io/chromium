// Copyright 2019 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "third_party/blink/renderer/modules/camera/camera.h"

#include <utility>
#include <iostream>
#include "base/time/time.h"
#include "third_party/blink/renderer/bindings/modules/v8/v8_sanitizer_config.h"
#include "third_party/blink/renderer/modules/camera/camera_setting_json.h"
#include "third_party/blink/renderer/modules/camera/camera_vizio_config.h"
#include "third_party/blink/renderer/bindings/core/v8/script_promise_resolver.h"
#include "third_party/blink/renderer/core/execution_context/execution_context.h"
#include "third_party/blink/renderer/bindings/core/v8/v8_object_builder.h"

namespace blink {
Camera* Camera::Create(ExecutionContext* context) {
  return MakeGarbageCollected<Camera>(context);
}

Camera::Camera(ExecutionContext* context)
  : ExecutionContextClient(context){
}

Camera::~Camera() = default;

void Camera::setConfig(CameraSettingJson* cameraSetting_json)
{
  Vector<String> ii = cameraSetting_json->config();

  for(Vector<String>::iterator iter = ii.begin(); iter != ii.end();++iter)
  {
    if( iter->Contains("PAN") != 0)
                  std::cout << "Setting  " << std::endl;
    if( iter->Contains("ZOOM") != 0)
                  std::cout << "Call Platform Set function " << std::endl;
  }
}

ScriptPromise Camera::getConfig(ScriptState* script_state,
                                                         Vector<String> config)
{
  auto *resolver = MakeGarbageCollected<ScriptPromiseResolver>(script_state);
  ScriptPromise promise = resolver->Promise();
  LocalDOMWindow* window = To<LocalDOMWindow>(GetExecutionContext());

  HeapVector<ScriptValue> response_values;

  for(Vector<String>::iterator iter = config.begin();
                                                   iter != config.end();++iter)
  {
    std::cout << *iter << std::endl;
    if( iter->Contains("DO_NOT_DISTURB") != 0)
      response_values.push_back(V8ObjectBuilder(script_state)
                        .AddString("DO_NOT_DISTURB", "true").GetScriptValue());
    else if( iter->Contains("ENABLE_CAMERA") != 0)
      response_values.push_back(V8ObjectBuilder(script_state)
                        .AddString("ENABLE_CAMERA", "true").GetScriptValue());
    else if( iter->Contains("EXPOSURE") != 0)
      response_values.push_back(V8ObjectBuilder(script_state)
                                .AddString("EXPOSURE", "50").GetScriptValue());
    else if( iter->Contains("WIDTH") != 0)
      response_values.push_back(V8ObjectBuilder(script_state)
                                 .AddString("WIDTH", "1280").GetScriptValue());
    else if( iter->Contains("HEIGHT") != 0)
      response_values.push_back(V8ObjectBuilder(script_state)
                                 .AddString("HEIGHT", "720").GetScriptValue());
    else if( iter->Contains("ECHO_AND_NOISE_CANCELLATION") != 0)
      response_values.push_back(V8ObjectBuilder(script_state)
           .AddString("ECHO_AND_NOISE_CANCELLATION", "Auto").GetScriptValue());
    else if( iter->Contains("INPUT_MIC_VOLUME") != 0)
      response_values.push_back(V8ObjectBuilder(script_state)
                        .AddString("INPUT_MIC_VOLUME", "10").GetScriptValue());
    else if( iter->Contains("INCOMING_CALL_RING") != 0)
      response_values.push_back(V8ObjectBuilder(script_state)
                    .AddString("INCOMING_CALL_RING", "true").GetScriptValue());
    else if( iter->Contains("INCOMING_CALL_RING_VOLUME") != 0)
      response_values.push_back(V8ObjectBuilder(script_state)
               .AddString("INCOMING_CALL_RING_VOLUME", "10").GetScriptValue());
    else if( iter->Contains("VOICE_CONTROL_NOTIFICATION") != 0)
      response_values.push_back(V8ObjectBuilder(script_state)
            .AddString("VOICE_CONTROL_NOTIFICATION", "true").GetScriptValue());
    else if( iter->Contains("PROFILE_NAME_FIRST") != 0)
      response_values.push_back(V8ObjectBuilder(script_state)
                   .AddString("PROFILE_NAME_FIRST", "VIZIO").GetScriptValue());
    else if( iter->Contains("PROFILE_NAME_LAST") != 0)
      response_values.push_back(V8ObjectBuilder(script_state)
                   .AddString("PROFILE_NAME_LAST", "CAMERA").GetScriptValue());
  }
  response_values_.swap(response_values);

  window->GetTaskRunner(TaskType::kUserInteraction)->PostDelayedTask(
        FROM_HERE,
        base::BindOnce(&Camera::getConfigResponse,
                     WrapPersistent(this),WrapPersistent(resolver)),
        base::TimeDelta::FromMilliseconds(100));
  return promise;
}

void Camera::getConfigResponse(ScriptPromiseResolver* resolver){
  resolver->Resolve(response_values_);
}

const AtomicString& Camera::InterfaceName() const {
  return event_target_names::kClipboard;
}

ExecutionContext* Camera::GetExecutionContext() const {
  return ExecutionContextClient::GetExecutionContext();
}

void Camera::Trace(Visitor* visitor) const {
  visitor->Trace(response_values_);
  ScriptWrappable::Trace(visitor);
  ExecutionContextClient::Trace(visitor);
}

}  // namespace blink


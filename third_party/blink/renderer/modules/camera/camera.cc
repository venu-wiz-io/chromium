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
//#include "third_party/blink/renderer/bindings/modules/v8/v8_clipboard_item_options.h"

namespace blink {
Camera* Camera::Create(ExecutionContext* context) {
	std::cout<<" Camera::Create" << std::endl;
	return MakeGarbageCollected<Camera>(context);
}


Camera::Camera(ExecutionContext* context)
	: ExecutionContextClient(context){
}

Camera::~Camera() = default;


 void Camera::SetConfig(CameraSettingJson* cameraSetting_json)
 {
        std::cout << "Kiran Camera::setConfig do_not_disturb "<< cameraSetting_json->doNotDisturb() << std::endl;
        std::cout << "Camera::setConfig enable_camera "<< cameraSetting_json->enableCamera() <<std::endl;
        std::cout << "Camera::setConfig profile name last "<< cameraSetting_json->profileNameLast() <<std::endl;
 }


ScriptPromise Camera::getConfig(ScriptState* script_state,CameraVizioConfig* Camera_Vizio_Config)
{

	auto *resolver = MakeGarbageCollected<ScriptPromiseResolver>(script_state);

        ScriptPromise promise = resolver->Promise();

	LocalDOMWindow* window = To<LocalDOMWindow>(GetExecutionContext());

	window->GetTaskRunner(TaskType::kUserInteraction)->PostDelayedTask(
      	FROM_HERE,
      	base::BindOnce(&Camera::setconfig_callback,
                     WrapPersistent(this),WrapPersistent(resolver)),
      	base::TimeDelta::FromMilliseconds(50));
        return promise;
}
const AtomicString& Camera::InterfaceName() const {
  return event_target_names::kClipboard;
}


ExecutionContext* Camera::GetExecutionContext() const {
  return ExecutionContextClient::GetExecutionContext();
}

void Camera::Trace(Visitor* visitor) const {
  ExecutionContextClient::Trace(visitor);
ScriptWrappable::Trace(visitor); 
}

String Camera::saneStringFrom(const String& input) {
  String CameraString = input;
  return CameraString;
}

void Camera::setconfig_callback(ScriptPromiseResolver* resolver)
{
	CameraVizioConfig *ptr = CameraVizioConfig::Create();
        std::cout << "Camera:: getConfig DO_NOT_DISTURB "<< ptr->doNotDisturbCamera() <<std::endl;
        std::cout << "Camera:: getConfig last profile name "<< ptr->profileNameLast() <<std::endl;
        std::cout << "Camera:: setconfig_callback" <<std::endl;
	resolver->Resolve(ptr);
}

}  // namespace blink


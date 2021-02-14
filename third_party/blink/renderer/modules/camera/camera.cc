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
        std::cout << "Camera::setConfig do_not_disturb "<< cameraSetting_json->doNotDisturb() << std::endl;
	bool ss = cameraSetting_json->hasDoNotDisturb();
	std::cout << ss << std::endl;
}

ScriptPromise Camera::getConfig(ScriptState* script_state,Vector<String> config)
{
	auto *resolver = MakeGarbageCollected<ScriptPromiseResolver>(script_state);
        ScriptPromise promise = resolver->Promise();
	LocalDOMWindow* window = To<LocalDOMWindow>(GetExecutionContext());
	window->GetTaskRunner(TaskType::kUserInteraction)->PostDelayedTask(
      	FROM_HERE,
      	base::BindOnce(&Camera::getConfigResponse,
                     WrapPersistent(this),WrapPersistent(resolver),config),
      	base::TimeDelta::FromMilliseconds(50));
	return promise;
}

void Camera::getConfigResponse(ScriptPromiseResolver* resolver,Vector<String> config){
	Vector<String> populate_response;
	
	for(Vector<String>::iterator iter = config.begin(); iter != config.end();++iter)
	{
		std::cout << *iter << std::endl;
		if( iter->Contains("DO_NOT_DISTURB") != 0)
			populate_response.push_back("DO_NOT_DISTURB:ON");
		else if( iter->Contains("ENABLE_CAMERA") != 0)
                        populate_response.push_back("ENABLE_CAMERA:ON");
		else if( iter->Contains("EXPOSURE") != 0)
                        populate_response.push_back("EXPOSURE:50");
		else if( iter->Contains("EXPOSURE") != 0)
                        populate_response.push_back("EXPOSURE:50");
		else if( iter->Contains("WIDTH") != 0)
			populate_response.push_back("WIDTH:1280");
		else if( iter->Contains("HEIGHT") != 0)
                        populate_response.push_back("HEIGHT:720");
		else if( iter->Contains("ECHO_AND_NOISE_CANCELLATION") != 0)
                        populate_response.push_back("ECHO_AND_NOISE_CANCELLATION:1");
		else if( iter->Contains("INPUT_MIC_VOLUME") != 0)
                        populate_response.push_back("INPUT_MIC_VOLUME:10");
		else if( iter->Contains("INCOMING_CALL_RING") != 0)
			populate_response.push_back("INCOMING_CALL_RING:true");
		else if( iter->Contains("INCOMING_CALL_RING_VOLUME") != 0)
                        populate_response.push_back("INCOMING_CALL_RING:10");
		else if( iter->Contains("VOICE_CONTROL_NOTIFICATION") != 0)
                        populate_response.push_back("VOICE_CONTROL_NOTIFICATION:true");
		else if( iter->Contains("PROFILE_NAME_FIRST") != 0)
                        populate_response.push_back("PROFILE_NAME_FIRST:VIZIO");
		else if( iter->Contains("PROFILE_LAST_FIRST") != 0)
                        populate_response.push_back("PROFILE_LAST_FIRST:CAMERA");
	}

	auto *ptr = CameraVizioConfig::Create();
	ptr->setConfiguration(populate_response);
	
	resolver->Resolve(ptr);
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

}  // namespace blink


//@COPYRIGHT@//
//
// Copyright (c) 2012, Tomoto S. Washio
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//   * Redistributions of source code must retain the above copyright
//     notice, this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above copyright
//     notice, this list of conditions and the following disclaimer in the
//     documentation and/or other materials provided with the distribution.
//   * Neither the name of the Tomoto S. Washio nor the names of his
//     contributors may be used to endorse or promote products derived from
//     this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL TOMOTO S. WASHIO BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//@COPYRIGHT@//

#include "EyeSluggerRendererEx.h"

EyeSluggerRendererEx::EyeSluggerRendererEx(RenderingContext* rctx, HenshinDetector* henshinDetector) : EyeSluggerRenderer(rctx, henshinDetector)
{
	m_holdMode = HOLD_NORMAL;
}

EyeSluggerRendererEx::~EyeSluggerRendererEx()
{
}

void EyeSluggerRendererEx::setHoldMode(HoldMode holdMode)
{
	m_holdMode = holdMode;
}

void EyeSluggerRendererEx::shoot(const XV3& v, float rotation, int traceDencity)
{
	m_holdMode = HOLD_NORMAL;
	EyeSluggerRenderer::shoot(v, rotation, traceDencity);
}

bool EyeSluggerRendererEx::updateObjectFrame()
{
	if (m_holdMode == HOLD_NORMAL) {
		return EyeSluggerRenderer::updateObjectFrame();
	} else if (m_holdMode == HOLD_IN_HAND) {

		const float OFFSET = 200;

		UserDetector* ud = m_henshinDetector->getUserDetector();
		m_origin = ud->getSkeletonJointPosition(XU_SKEL_RIGHT_HAND);
		XV3 fv = -(m_origin - ud->getSkeletonJointPosition(XU_SKEL_NECK));
		XV3 uv = fv.cross(ud->getSkeletonJointPosition(XU_SKEL_LEFT_SHOULDER) - ud->getSkeletonJointPosition(XU_SKEL_RIGHT_SHOULDER));
		uv.normalizeM();
		m_origin -= uv * OFFSET;

		m_objectFrame.SetOrigin(m_origin.X, m_origin.Y, m_origin.Z);
		m_objectFrame.SetForwardVector(fv.X, fv.Y, fv.Z);
		m_objectFrame.SetUpVector(uv.X, uv.Y, uv.Z);
		m_objectFrame.Normalize();

		return true;
	} else {
		return false;
	}
}

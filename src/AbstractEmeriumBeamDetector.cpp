//@COPYRIGHT@//
//
// Copyright (c) 2011, Tomoto S. Washio
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

#include "AbstractEmeriumBeamDetector.h"
#include "util.h"

AbstractEmeriumBeamDetector::AbstractEmeriumBeamDetector(DepthProvider* depthProvider, UserDetector* userDetector, AbstractSimpleBeamRenderer* beamRenderer)
: AbstractPoseDetector(userDetector)
{
	m_beamRenderer = beamRenderer;
	setRequiredPosingStability(0.25f);
}

AbstractEmeriumBeamDetector::~AbstractEmeriumBeamDetector()
{
}

void AbstractEmeriumBeamDetector::shootBeam(const XV3& p, const XV3& v)
{
	const float VELOCITY = 8000.0f;
	const float FORWARD_OFFSET = 20.0f;
	const float WIDTH = 30.0f;

	XV3 d0(v.Z, 0.0f, -v.X);
	d0.normalizeM();

	GLFrame frame;

	frame.SetForwardVector(XV3toM3D(v));
	frame.SetUpVector(XV3toM3D(d0));

	for (int i = 0; i < 4; i++) {
		M3DVector3f tmp;
		frame.GetUpVector(tmp);
		XV3 d(tmp);

		m_beamRenderer->addElement(p + v * FORWARD_OFFSET - d * WIDTH / 2.0f, d * WIDTH, v * VELOCITY);

		frame.RotateLocal(float(M_PI_4), 0.0f, 0.0f, 1.0f);
	}
}

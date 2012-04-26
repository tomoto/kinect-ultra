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

#include "VoxelObjectMapper.h"
#include "vec.h"
#include "config.h"

const int PIXEL_STEP = 4;

const int VOXEL_SCALE_X = 100;
const int VOXEL_SCALE_Y = 100;
const int VOXEL_SCALE_Z = 100;
const int VOXEL_X_MIN = -40;
const int VOXEL_Y_MIN = -40;
const int VOXEL_Z_MIN = 0;
const int VOXEL_MAX_X = 40;
const int VOXEL_Y_MAX = 40;
const int VOXEL_Z_MAX = 100;

const int VOXEL_WIDTH  = VOXEL_MAX_X - VOXEL_X_MIN;
const int VOXEL_HEIGHT = VOXEL_Y_MAX - VOXEL_Y_MIN;
const int VOXEL_DEPTH  = VOXEL_Z_MAX - VOXEL_Z_MIN;
const int NUM_VOXELS = VOXEL_WIDTH * VOXEL_HEIGHT * VOXEL_DEPTH;

inline void convertRealWorldToVoxelCoords(const XV3& rp, int* pX, int* pY, int* pZ)
{
	int x = *pX = (int)(rp.X / VOXEL_SCALE_X + 0.5f);
	int y = *pY = (int)(rp.Y / VOXEL_SCALE_Y + 0.5f);
	int z = *pZ = (int)(rp.Z / VOXEL_SCALE_Z + 0.5f);
}

inline bool isInVoxelRange(int x, int y, int z)
{
	return x >= VOXEL_X_MIN && x < VOXEL_MAX_X && y >= VOXEL_Y_MIN && y < VOXEL_Y_MAX && z >= VOXEL_Z_MIN && z < VOXEL_Z_MAX;
}
inline int getVoxelIndex(int x, int y, int z)
{
	return (x - VOXEL_X_MIN) + (y - VOXEL_Y_MIN) * VOXEL_WIDTH + (z - VOXEL_Z_MIN) * (VOXEL_WIDTH * VOXEL_HEIGHT);
}

VoxelObjectMapper::VoxelObjectMapper(DepthProvider* depthProvider)
{
	m_depthProvider = depthProvider;
	m_voxels = new bool[NUM_VOXELS];
}

VoxelObjectMapper::~VoxelObjectMapper()
{
	delete[] m_voxels;
}

void VoxelObjectMapper::update()
{
	memset(m_voxels, FALSE, NUM_VOXELS * sizeof(*m_voxels));

	XV3 realWorldPoint;

	for (int y = 0; y < Y_RES; y += PIXEL_STEP) {
		const XuRawDepthPixel* dp = m_depthProvider->getData() + y * X_RES;
		for (int x = 0; x < X_RES; x += PIXEL_STEP, dp += PIXEL_STEP) {
			if (*dp) {
				m_depthProvider->transformDepthImageToSkeleton(x, y, *dp, &realWorldPoint);
				int vx, vy, vz;
				convertRealWorldToVoxelCoords(realWorldPoint, &vx, &vy, &vz);
				if (isInVoxelRange(vx, vy, vz)) {
					int i = getVoxelIndex(vx, vy, vz);
					if (!m_voxels[i]) {
						m_voxels[i] = true;
					}
				}
			}
		}
	}
}

bool VoxelObjectMapper::test(const XV3& p, const XV3& v)
{
	float k = std::max(std::max(abs(v.X / VOXEL_SCALE_X), abs(v.Y / VOXEL_SCALE_Y)), abs(v.Z / VOXEL_SCALE_Z));
	XV3 v0(v.X / k, v.Y / k, v.Z / k);
	XV3 p0(p);
	for (int i = 0; i < k; i++) {
		p0 += v0;
		int vx, vy, vz;
		convertRealWorldToVoxelCoords(p0, &vx, &vy, &vz);
		if (isInVoxelRange(vx, vy, vz)) {
			if (m_voxels[getVoxelIndex(vx, vy, vz)]) {
				return true;
			}
		}
	}
	return false;
}

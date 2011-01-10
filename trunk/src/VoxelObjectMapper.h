#ifndef _VOXEL_OBJECT_MAPPER_H_
#define _VOXEL_OBJECT_MAPPER_H_

#include "common.h"

class VoxelObjectMapper
{
private:
	DepthGenerator* m_depthGen;

	XnUInt32 m_maxNumPoints;
	XnPoint3D* m_projectivePoints;
	XnPoint3D* m_realWorldPoints;

	bool* m_voxels;

public:
	VoxelObjectMapper(DepthGenerator* depthGen);
	virtual ~VoxelObjectMapper();

	void update();
	bool test(const XnPoint3D& p, const XnPoint3D& v);
};

#endif

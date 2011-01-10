#ifndef _WIDESHOT_RENDERER_H_
#define _WIDESHOT_RENDERER_H_

#include "AbstractSimpleBeamRenderer.h"

class WideshotRenderer : public AbstractSimpleBeamRenderer
{
public:
	WideshotRenderer(RenderingContext* rctx, VoxelObjectMapper* voxelObjectMapper, SparkRenderer* sparkRenderer);
	virtual ~WideshotRenderer();
};

#endif

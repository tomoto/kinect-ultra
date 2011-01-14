#ifndef _EMERIUM_BEAM_RENDERER2_H_
#define _EMERIUM_BEAM_RENDERER2_H_

#include "common.h"
#include "AbstractSimpleBeamRenderer.h"

class EmeriumBeamRenderer2 : public AbstractSimpleBeamRenderer
{
public:
	EmeriumBeamRenderer2(RenderingContext* rctx, VoxelObjectMapper* voxelObjectMapper, SparkRenderer* sparkRenderer);
	virtual ~EmeriumBeamRenderer2();
};

#endif

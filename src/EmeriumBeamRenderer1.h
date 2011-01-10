#ifndef _EMERIUM_BEAM_RENDERER1_H_
#define _EMERIUM_BEAM_RENDERER1_H_

#include "AbstractSimpleBeamRenderer.h"

class EmeriumBeamRenderer1 : public AbstractSimpleBeamRenderer
{
public:
	EmeriumBeamRenderer1(RenderingContext* rctx, VoxelObjectMapper* voxelObjectMapper, SparkRenderer* sparkRenderer);
	virtual ~EmeriumBeamRenderer1();
};

#endif

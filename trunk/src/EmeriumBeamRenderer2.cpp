#include "EmeriumBeamRenderer2.h"

const M3DVector4f BEAM_COLOR = { 0.2f, 1.0f, 0.7f, 0.7f };

EmeriumBeamRenderer2::EmeriumBeamRenderer2(RenderingContext* rctx, VoxelObjectMapper* voxelObjectMapper, SparkRenderer* sparkRenderer)
: AbstractSimpleBeamRenderer(rctx, voxelObjectMapper, sparkRenderer, "emeriumBeam2Alpha.png", 1.0f, BEAM_COLOR, 10)
{
}

EmeriumBeamRenderer2::~EmeriumBeamRenderer2()
{
}

#include "EmeriumBeamRenderer1.h"

const M3DVector4f BEAM_COLOR = { 1.0f, 0.9f, 0.4f, 0.8f };

EmeriumBeamRenderer1::EmeriumBeamRenderer1(RenderingContext* rctx, VoxelObjectMapper* voxelObjectMapper, SparkRenderer* sparkRenderer)
: AbstractSimpleBeamRenderer(rctx, voxelObjectMapper, sparkRenderer, "emeriumBeam1Alpha.png", 1.0f, BEAM_COLOR, 5)
{
}

EmeriumBeamRenderer1::~EmeriumBeamRenderer1()
{
}

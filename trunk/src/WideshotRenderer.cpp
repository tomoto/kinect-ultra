//@copyright

#include "WideshotRenderer.h"

const M3DVector4f BEAM_COLOR = { 1.0f, 1.0f, 0.9f, 1.0f };

WideshotRenderer::WideshotRenderer(RenderingContext* rctx, VoxelObjectMapper* voxelObjectMapper, SparkRenderer* sparkRenderer)
: AbstractSimpleBeamRenderer(rctx, voxelObjectMapper, sparkRenderer, "wideshotAlpha.png", 1.0f, BEAM_COLOR, 1)
{
}

WideshotRenderer::~WideshotRenderer()
{
}

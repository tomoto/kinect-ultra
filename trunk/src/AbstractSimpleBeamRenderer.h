#ifndef _ABSTRACT_SIMPLE_BEAM_RENDERER_H_
#define _ABSTRACT_SIMPLE_BEAM_RENDERER_H_

#include "common.h"
#include "AbstractElementRenderer.h"
#include "VoxelObjectMapper.h"
#include "SparkRenderer.h"

struct AbstractSimpleBeamElement : public AbstractElementRendererElement
{
	XV3 d; // up vector
};

class AbstractSimpleBeamRenderer : public AbstractElementRenderer<AbstractSimpleBeamElement>
{
private:
	VoxelObjectMapper* m_voxelObjectMapper;
	SparkRenderer* m_sparkRenderer;
	float m_lifeTime;
	M3DVector4f m_color;
	int m_sparkDensity;
	cv::RNG m_random;

public:
	AbstractSimpleBeamRenderer(RenderingContext* rctx, VoxelObjectMapper* voxelObjectMapper, SparkRenderer* sparkRenderer,
		const char* alphaTextureFile, float lifeTime, const M3DVector4f color, int sparkDensity);
	virtual ~AbstractSimpleBeamRenderer();
	virtual void addElement(const XV3& p, const XV3& d, const XV3& v);

private:
	virtual void executeDraw();
	virtual void onProgress(AbstractSimpleBeamElement* element, float dt);
	void fireSpark(const AbstractSimpleBeamElement& element);
};

#endif

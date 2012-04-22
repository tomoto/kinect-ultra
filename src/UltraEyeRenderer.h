//@COPYRIGHT@//

#ifndef _ULTRA_EYE_RENDERER_H_
#define _ULTRA_EYE_RENDERER_H_

#include "common.h"
#include "AbstractOpenGLRenderer.h"
#include "HenshinDetector.h"

class UltraEyeRenderer : public AbstractOpenGLRenderer
{
private:
	HenshinDetector* m_henshinDetector;
	float m_phase;
	bool m_isNewUser;
	TimeTicker m_ticker;

public:
	UltraEyeRenderer(RenderingContext* rctx, HenshinDetector* henshinDetector);
	virtual ~UltraEyeRenderer();
	
	void draw();

private:
	void drawSearchingInstruction();
	void drawHenshinInstruction();
};

#endif

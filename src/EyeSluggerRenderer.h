#ifndef _EYE_SLUGGER_RENDERER_H_
#define _EYE_SLUGGER_RENDERER_H_

#include "common.h"
#include "AbstractOpenGLRenderer.h"
#include "HenshinDetector.h"
#include <vector>

// TODO: renderer part and motion controller part should be separated
class EyeSluggerRenderer : public AbstractOpenGLRenderer
{
public:
	enum HoldMode {
		HOLD_ON_HEAD,
		HOLD_IN_HAND,
		HOLD_IN_AIR
	};

private:
	HenshinDetector* m_henshinDetector;
	std::vector<GLBatch> m_batches;
	GLFrame m_objectFrame;
	XV3 m_origin;

	HoldMode m_holdMode;

	TimeTicker m_ticker;
	float m_shotLifeTime;
	float m_shotProgress;
	XV3 m_shotVector;
	float m_shotRotation;
	int m_shotTraceDencity;

	float m_brightness;

private:
	void setupObjectModel();
	
	void drawSlugger(float velocity, float rotation);
	void drawHeld(float dt);
	void drawShot(float dt);

public:
	EyeSluggerRenderer(RenderingContext* rctx, HenshinDetector* henshinDetector);
	virtual ~EyeSluggerRenderer();

	virtual void draw();

	XV3 getOrigin(); // TODO weird dependency direction; should this be give from outside?
	XV3 getForwardVector(); // TODO weird dependency direction; should this be give from outside?

	void setEnergy(float energy);
	void setHoldMode(HoldMode holdMode);

	bool isShot();

	void shoot(const XV3& v, float rotation, int traceDencity);

};

#endif

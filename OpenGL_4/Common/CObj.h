#ifndef CObj_H
#define CObj_H
#include "../header/Angel.h"
#include "CShape.h"

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

#define SOLIDCUBE_NUM 36  // 6 faces, 2 triangles/face , 3 vertices/triangle

class CObj : public CShape
{
private:
	vec4 m_vertices[8];
	int  m_iIndex;

	void Quad( int a, int b, int c, int d );
public:
	CObj(char *);
	~CObj(){};

	void Update(float dt, point4 vLightPos, color4 vLightI);
	void Update(float dt, const LightSource &Lights);
	void Update(float dt, const LightSource &Lights, const LightSource &Lights2);
	void Update(float dt, const LightSource &Lights, const LightSource &Lights2, const LightSource &Lights3, const LightSource &Lights4);

	void Update(float dt); // 不計算光源的照明

	void RenderWithFlatShading(point4 vLightPos, color4 vLightI);
	void RenderWithGouraudShading(point4 vLightPos, color4 vLightI);
	void RenderWithFlatShading(const LightSource &Lights);
	void RenderWithGouraudShading(const LightSource &Lights);
	void draw();
	void drawW();

	//	讀檔
	char lineHeader[1000];//一行讀檔暫存空間
	int indices[10];
	float vertex[3];
	FILE* objfile;
	char* adrFile ;
};

#endif
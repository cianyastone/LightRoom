#ifndef CSOLIDSPHERE_H
#define CSOLIDSPHERE_H
#include "../header/Angel.h"
#include "CShape.h"

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

#define SOLIDCUBE_NUM 36  // 6 faces, 2 triangles/face , 3 vertices/triangle

class CSolidSphere : public CShape
{
private:
	GLfloat m_fRadius;
	GLint m_iSlices, m_iStacks;

	vec4 m_Points[SOLIDCUBE_NUM];
	vec3 m_Normals[SOLIDCUBE_NUM];
	vec4 m_vertices[8];
	int  m_iIndex;

public:
	CSolidSphere(const GLfloat fRadius=1.0f, const int iSlices=12,const  int iStacks = 6);
	~CSolidSphere();

	void Update(float dt, point4 vLightPos, color4 vLightI);
	void Update(float dt, const LightSource &Lights, const LightSource &Lights2);

	void Update(float dt, const LightSource &Lights);
	void Update(float dt); // 不計算光源的照明

	// Sphere 的繪製方始使用多組的 GL_TRIANGLE_STRIP 來繪製, 因此沒有辦法提供 Flat Shading，
	// 只有以 vertex 為基礎的計算顏色的 Ground Shading
	void RenderWithFlatShading(point4 vLightPos, color4 vLightI);
	void RenderWithGouraudShading(point4 vLightPos, color4 vLightI);
	void RenderWithFlatShading(const LightSource &Lights);
	void RenderWithGouraudShading(const LightSource &Lights);
	void draw();
	void drawW(); // 呼叫不再次設定 Shader 的描繪方式

};

#endif
#ifndef CCHECKER_H
#define CCHECKER_H

#include "TypeDefine.h"
#include "CQuad.h"


// 以 (0,0) 為中心 劃出一個 nXn 的棋盤方格
// n 在宣告時給定
class CChecker {

private:
	CQuad *m_pSquares;
	int    m_iGridSize;
	float  *m_pfSquaresT;
	GLuint m_uiShaderHandle;
	float  m_fYPos;

public:
	CChecker(int iSize = 6, float fYPos = 0.0f); // 預設為 6 X 6 方格, 每一個方格邊長都是 1
	~CChecker();

	void setShader();
	void setProjectionMatrix(mat4 &mat);
	void setViewMatrix(mat4 &mat);
	void setTRSMatrix(mat4 &mat);
	void setShadingMode(int iMode);
	void Update(float dt, point4 vLightPos, color4 vLightI);
	void Update(float dt,const LightSource &Lights);
	void Update(float dt, const LightSource &Lights, const LightSource &Lights2);
	void Update(float dt, const LightSource &Lights, const LightSource &Lights2, const LightSource &Lights3, const LightSource &Lights4);
	void draw();

	// For setting materials 
	void setMaterials(color4 ambient, color4 diffuse, color4 specular);
	void setKaKdKsShini(float ka, float kd, float ks, float shininess); // ka kd ks shininess
};

#endif
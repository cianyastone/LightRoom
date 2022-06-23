#ifndef CLINSEGMENT_H
#define CLINSEGMENT_H
#include "../header/Angel.h"

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

#define VERTEX_NUM 2  // one line segment with two vertices

class CLineSegment
{
private:
	vec4 m_Points[VERTEX_NUM];
	vec4 m_Colors[VERTEX_NUM];

	//  for shader
	mat4 m_mxView, m_mxProjection;
	mat4 m_mxMVFinal, m_mxTRS;

	GLfloat m_fColor[4];
	GLuint m_uiModelView, m_uiProjection, m_uiColor;
	GLuint m_uiProgram ;
	GLuint m_uiBuffer;
	bool   m_bUpdateView;
	bool   m_bUpdateProj;

	// For VAO
	GLuint m_uiVao;
	void CreateBufferObject();

public:
	CLineSegment(const vec4 SPoint = vec4(-1,0,0,0), const vec4 EPoint = vec4(1,0,0,0), const vec4 vColor = vec4(1,0,0,0) );

	void setShader(mat4 &mxModelView, mat4 &mxProjection, GLuint uiShaderHandle=MAX_UNSIGNED_INT);
	void setShader(GLuint uiShaderHandle = MAX_UNSIGNED_INT);
	GLuint GetShaderHandle() { return m_uiProgram;}
	void setViewMatrix(mat4 &mat);
	void setProjectionMatrix(mat4 &mat);
	void setTRSMatrix(mat4 &mat);
	void setColor(vec4 vColor); // Single color
	void setVtxColors(vec4 vSPColor, vec4 vEPColor); // Vertices' Color
	void UpdatePosition(vec4 SPoint, vec4 EPoint);

	void draw();
	void drawW();
};




#endif
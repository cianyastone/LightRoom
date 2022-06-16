#ifndef C2DSHAPE_H
#define C2DSHAPE_H
#include "../Header/Angel.h"

class C2DShape
{
protected:
	// ���V�x�s�X���ƪ�����
	vec4 *m_pPoints;
	vec3 *m_pNormals;
	vec2 *m_pTex;
	int  m_iNumVtx;

	// �C�@�� Shape �ۤv���첾��m�A�Y��P���ਤ��

	GLfloat m_fColor[4]; // Object's color
						 // For shaders' name
	char *m_pVXshader, *m_pFSshader;

	// For VAO
	GLuint m_uiVao;

	// For Shader
	GLuint  m_uiModelView, m_uiProjection, m_uiColor;
	GLuint  m_uiProgram;
	GLuint  m_uiBuffer;

	// For Matrices
	mat4    m_mxView, m_mxProjection, m_mxTRS;
	mat4    m_mxMVFinal;
	bool    m_bProjUpdated, m_bViewUpdated, m_bTRSUpdated;
	bool    m_bColorUpdated;

	void		CreateBufferObject();
	void		DrawingSetShader();
	void		DrawingWithoutSetShader();

public:
	C2DShape();
	virtual ~C2DShape();
	virtual void Draw() = 0;
	virtual void DrawW() = 0; // Drawing without setting shaders
	virtual void Update(float dt) = 0;	// �������C�@�� frame ���i�H��X��ҫ����ܤ��e

	void SetShaderName(const char vxShader[], const char fsShader[]);
	void SetShader(mat4 &mxModelView, mat4 &mxProjection, GLuint uiShaderHandle = MAX_UNSIGNED_INT);
	void SetShader(GLuint uiShaderHandle = MAX_UNSIGNED_INT);
	void SetColor(vec4 vColor);
	void SetViewMatrix(mat4 &mat);
	void SetProjectionMatrix(mat4 &mat);
	void SetTRSMatrix(mat4 &mat);
};

#endif

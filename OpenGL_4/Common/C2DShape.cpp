#include "C2DShape.h"
#include "CCamera.h"

C2DShape::C2DShape()
{
	m_pPoints = NULL;
	m_pNormals = NULL;
	m_pTex = NULL;
	m_bProjUpdated = false;
	m_bViewUpdated = false;
	m_bTRSUpdated = false;
	m_bColorUpdated = false;
}

C2DShape::~C2DShape()
{
	if (m_pPoints != NULL) delete[] m_pPoints;
	if (m_pNormals != NULL) delete[] m_pNormals;
	if (m_pTex != NULL) delete	m_pTex;

	if (m_pVXshader != NULL) delete[] m_pVXshader;
	if (m_pFSshader != NULL) delete[] m_pFSshader;
}

void C2DShape::CreateBufferObject()
{
	glGenVertexArrays(1, &m_uiVao);
	glBindVertexArray(m_uiVao);

	// Create and initialize a buffer object
	glGenBuffers(1, &m_uiBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_uiBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec4)*m_iNumVtx + sizeof(vec3)*m_iNumVtx, NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec4)*m_iNumVtx, m_pPoints);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec4)*m_iNumVtx, sizeof(vec3)*m_iNumVtx, m_pNormals);
}

void C2DShape::SetShader(mat4 &mxModelView, mat4 &mxProjection, GLuint uiShaderHandle)
{
	// Load shaders and use the resulting shader program
	if (uiShaderHandle == MAX_UNSIGNED_INT) m_uiProgram = InitShader(m_pVXshader, m_pFSshader);
	else m_uiProgram = uiShaderHandle;

	// set up vertex arrays
	GLuint vPosition = glGetAttribLocation(m_uiProgram, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint vNormal = glGetAttribLocation(m_uiProgram, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vec4)*m_iNumVtx));

	m_uiModelView = glGetUniformLocation(m_uiProgram, "ModelView");
	m_mxMVFinal = m_mxView = mxModelView;
	glUniformMatrix4fv(m_uiModelView, 1, GL_TRUE, m_mxView);

	m_uiProjection = glGetUniformLocation(m_uiProgram, "Projection");
	m_mxProjection = mxProjection;
	glUniformMatrix4fv(m_uiProjection, 1, GL_TRUE, m_mxProjection);

	m_uiColor = glGetUniformLocation(m_uiProgram, "vColor");
	glUniform4fv(m_uiColor, 1, m_fColor);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void C2DShape::SetShader(GLuint uiShaderHandle)
{
	// Load shaders and use the resulting shader program
	if (uiShaderHandle == MAX_UNSIGNED_INT) m_uiProgram = InitShader(m_pVXshader, m_pFSshader);
	else m_uiProgram = uiShaderHandle;

	// set up vertex arrays
	GLuint vPosition = glGetAttribLocation(m_uiProgram, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint vNormal = glGetAttribLocation(m_uiProgram, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vec4)*m_iNumVtx));

	m_uiModelView = glGetUniformLocation(m_uiProgram, "ModelView");
	//	m_mxMVFinal , m_mxView �ŧi�ɴN�O���x�}
	glUniformMatrix4fv(m_uiModelView, 1, GL_TRUE, m_mxView);

	m_uiProjection = glGetUniformLocation(m_uiProgram, "Projection");
	// m_mxProjection �ŧi�ɴN�O���x�}
	glUniformMatrix4fv(m_uiProjection, 1, GL_TRUE, m_mxProjection);

	m_uiColor = glGetUniformLocation(m_uiProgram, "vColor");
	glUniform4fv(m_uiColor, 1, m_fColor);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void C2DShape::DrawingSetShader()
{
	glUseProgram(m_uiProgram);
	glBindVertexArray(m_uiVao);

	if (m_bViewUpdated || m_bTRSUpdated) { // Model View �������x�}���e�����
		m_mxMVFinal = m_mxView * m_mxTRS;
		m_bViewUpdated = m_bTRSUpdated = false;
	}
	glUniformMatrix4fv(m_uiModelView, 1, GL_TRUE, m_mxMVFinal);

	if (m_bProjUpdated) {
		glUniformMatrix4fv(m_uiProjection, 1, GL_TRUE, m_mxProjection);
		m_bProjUpdated = false;
	}
	if (m_bColorUpdated) {
		glUniform4fv(m_uiColor, 1, m_fColor);
		m_bColorUpdated = false;
	}
}

// ���B�w�]�ϥΫe�@�Ӵyø����ҨϥΪ� Shader
void C2DShape::DrawingWithoutSetShader()
{
	glBindVertexArray(m_uiVao);
	if (m_bViewUpdated || m_bTRSUpdated) { // Model View �������x�}���e�����
		m_mxMVFinal = m_mxView * m_mxTRS;
		m_bViewUpdated = m_bTRSUpdated = false;
	}
	glUniformMatrix4fv(m_uiModelView, 1, GL_TRUE, m_mxMVFinal);
	if (m_bProjUpdated) {
		glUniformMatrix4fv(m_uiProjection, 1, GL_TRUE, m_mxProjection);
		m_bProjUpdated = false;
	}
	glUniform4fv(m_uiColor, 1, m_fColor);
}

void C2DShape::SetShaderName(const char vxShader[], const char fsShader[])
{
	int len;
	len = strlen(vxShader);
	m_pVXshader = new char[len + 1];
	memcpy(m_pVXshader, vxShader, len + 1);

	len = strlen(fsShader);
	m_pFSshader = new char[len + 1];
	memcpy(m_pFSshader, fsShader, len + 1);
}

void C2DShape::SetViewMatrix(mat4 &mat)
{
	m_mxView = mat;
	m_bViewUpdated = true;
}

void C2DShape::SetProjectionMatrix(mat4 &mat)
{
	m_mxProjection = mat;
	m_bProjUpdated = true;
}
void C2DShape::SetTRSMatrix(mat4 &mat)
{
	m_mxTRS = mat;
	m_bTRSUpdated = true;
}

void C2DShape::SetColor(vec4 vColor)
{
	m_fColor[0] = vColor.x;
	m_fColor[1] = vColor.y;
	m_fColor[2] = vColor.z;
	m_fColor[3] = vColor.w;
	m_bColorUpdated = true;
}
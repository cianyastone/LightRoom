#include "CObj.h"

CObj::CObj(char * adrFile)
{
	m_pPoints = NULL; m_pNormals = NULL; m_pTex = NULL;

	//�޼ƼȦs�ܼ�
	int vindex = 0;
	int nindex = 0;
	int tindex = 0;
	int mindex = 0;

	objfile = fopen(adrFile, "r");
	if (objfile == NULL) {
		printf("fail to open the file !\n");
	}
	else {
		while (fscanf(objfile, "%s", lineHeader) != EOF) {
			if (strcmp(lineHeader, "f") == 0) { vindex++; } //�p���I�`��
		}
		fclose(objfile);
		m_iNumVtx = vindex * 3;
		m_pPoints = new vec4[m_iNumVtx + 1];
		vindex = 0; mindex = 0;//�t������I���Ȧs

							   //�̷ӹ�ڳ��I�ƨM�w�}�C�j�p
		m_pPoints = new vec4[m_iNumVtx];
		m_pNormals = new vec3[m_iNumVtx];
		m_pColors = new vec4[m_iNumVtx];
		m_pTex = new vec2[m_iNumVtx];
		vec4 *temp_vertices = new vec4[m_iNumVtx];
		vec3 *temp_pNormals = new vec3[m_iNumVtx];
		vec4 *temp_pColors = new vec4[m_iNumVtx];
		vec2 *temp_pTex = new vec2[m_iNumVtx];

		objfile = fopen(adrFile, "r");

		while (fscanf(objfile, "%s", lineHeader) != EOF) {
			if (strcmp(lineHeader, "v") == 0) {
				fscanf(objfile, "%f %f %f\n", &vertex[0], &vertex[1], &vertex[2]);
				temp_vertices[vindex] = vec4(vertex[0], vertex[1], vertex[2], 1.0f);
				vindex++;
			}
			else if (strcmp(lineHeader, "vn") == 0) {
				fscanf(objfile, "%f %f %f\n", &vertex[0], &vertex[1], &vertex[2]);
				temp_pNormals[nindex] = vec3(vertex[0], vertex[1], vertex[2]);
				nindex++;
			}

			else if (strcmp(lineHeader, "vt") == 0) {
				fscanf(objfile, "%f %f %f\n", &vertex[0], &vertex[1], &vertex[2]);
				temp_pTex[tindex] = vec2(vertex[0], vertex[1]);
				tindex++;
			}
			else if (strcmp(lineHeader, "f") == 0) {
				//�Ш̷Ӧۤv��OBJ�ɨM�wŪ���覡
				//"f ���I����/uv�I����/�k�u����"
				fscanf(objfile, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &indices[0], &indices[1], &indices[2], &indices[3], &indices[4], &indices[5], &indices[6], &indices[7], &indices[8]);
				m_pPoints[mindex] = temp_vertices[indices[0] - 1];
				m_pPoints[mindex + 1] = temp_vertices[indices[3] - 1];
				m_pPoints[mindex + 2] = temp_vertices[indices[6] - 1];
				m_pNormals[mindex] = temp_pNormals[indices[2] - 1];
				m_pNormals[mindex + 1] = temp_pNormals[indices[5] - 1];
				m_pNormals[mindex + 2] = temp_pNormals[indices[8] - 1];
				mindex += 3;
			}
		}
	}
	fclose(objfile);
	// �w�]�N�Ҧ��������]�w���Ǧ�
	for( int i = 0 ; i < m_iNumVtx ; i++ ) m_pColors[i] = vec4(-1.0f,-1.0f,-1.0f,1.0f);

#ifdef LIGHTING_WITHCPU
	// Default Set shader's name
	SetShaderName("vsLighting_CPU.glsl", "fsLighting_CPU.glsl");
#else // lighting with GPU
#ifdef PERVERTEX_LIGHTING
	SetShaderName("vsLighting_GPU.glsl", "fsLighting_GPU.glsl");
#else
	SetShaderName("vsPerPixelLighting.glsl", "fsPerPixelLighting.glsl");
#endif
#endif  

	// Create and initialize a buffer object �A�N���������]�w���J SetShader ��
//	CreateBufferObject();

	// �]�w����
	SetMaterials(vec4(0), vec4(0.5f, 0.5f, 0.5f, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	SetKaKdKsShini(0, 0.8f, 0.2f, 1);
}

void CObj::Quad( int a, int b, int c, int d )
{
    // Initialize temporary vectors along the quad's edge to
    //   compute its face normal 
    vec4 u = m_vertices[b] - m_vertices[a];
    vec4 v = m_vertices[c] - m_vertices[b];
    vec3 normal = normalize( cross(u, v) );

    m_pNormals[m_iIndex] = normal; m_pPoints[m_iIndex] = m_vertices[a]; m_iIndex++;
    m_pNormals[m_iIndex] = normal; m_pPoints[m_iIndex] = m_vertices[b]; m_iIndex++;
    m_pNormals[m_iIndex] = normal; m_pPoints[m_iIndex] = m_vertices[c]; m_iIndex++;
    m_pNormals[m_iIndex] = normal; m_pPoints[m_iIndex] = m_vertices[a]; m_iIndex++;
    m_pNormals[m_iIndex] = normal; m_pPoints[m_iIndex] = m_vertices[c]; m_iIndex++;
    m_pNormals[m_iIndex] = normal; m_pPoints[m_iIndex] = m_vertices[d]; m_iIndex++;
}

void CObj::Draw()
{
//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // Change to wireframe mode
	DrawingSetShader();
	glDrawArrays( GL_TRIANGLES, 0, m_iNumVtx);
//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Return to solid mode
}


void CObj::DrawW()
{
	DrawingWithoutSetShader();
	glDrawArrays( GL_TRIANGLES, 0, m_iNumVtx);
}


void CObj::RenderWithFlatShading(vec4 vLightPos, color4 vLightI)
{
	// �H�C�@�ӭ����T�ӳ��I�p��䭫�ߡA�H�ӭ��ߧ@���C��p�⪺�I��
	// �ھ� Phong lighting model �p��۹������C��A�ñN�C���x�s�즹�T���I��
	// �]���C�@�ӥ��������I�� Normal ���ۦP�A�ҥH���B�èS���p�⦹�T�ӳ��I������ Normal

	vec4 vCentroidP;
	for( int i = 0 ; i < m_iNumVtx ; i += 3 ) {
		// �p��T���Ϊ�����
		vCentroidP = (m_pPoints[i] + m_pPoints[i+1] + m_pPoints[i+2])/3.0f;
		m_pColors[i] = m_pColors[i + 1] = m_pColors[i + 2] = PhongReflectionModel(vCentroidP, m_pNormals[i], vLightPos, vLightI);
	}
	glBindBuffer( GL_ARRAY_BUFFER, m_uiBuffer );
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(vec4)*m_iNumVtx+sizeof(vec3)*m_iNumVtx, sizeof(vec4)*m_iNumVtx, m_pColors ); // vertcies' Color
}

void CObj::RenderWithFlatShading(const LightSource &Lights)
{
	// �H�C�@�ӭ����T�ӳ��I�p��䭫�ߡA�H�ӭ��ߧ@���C��p�⪺�I��
	// �ھ� Phong lighting model �p��۹������C��A�ñN�C���x�s�즹�T���I��
	// �]���C�@�ӥ��������I�� Normal ���ۦP�A�ҥH���B�èS���p�⦹�T�ӳ��I������ Normal

	vec4 vCentroidP;
	for (int i = 0; i < m_iNumVtx; i += 3) {
		// �p��T���Ϊ�����
		vCentroidP = (m_pPoints[i] + m_pPoints[i + 1] + m_pPoints[i + 2]) / 3.0f;
		m_pColors[i] = m_pColors[i + 1] = m_pColors[i + 2] = PhongReflectionModel(vCentroidP, m_pNormals[i], Lights.position, Lights.diffuse);
	}
	glBindBuffer(GL_ARRAY_BUFFER, m_uiBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec4)*m_iNumVtx + sizeof(vec3)*m_iNumVtx, sizeof(vec4)*m_iNumVtx, m_pColors); // vertcies' Color
}

void CObj::RenderWithGouraudShading(vec4 vLightPos, color4 vLightI)
{
	vec4 vCentroidP;
	for( int i = 0 ; i < m_iNumVtx ; i += 6 ) {
		m_pColors[i] = m_pColors[i + 3] = PhongReflectionModel(m_pPoints[i], m_pNormals[i], vLightPos, vLightI);
		m_pColors[i + 2] = m_pColors[i + 4] = PhongReflectionModel(m_pPoints[i + 2], m_pNormals[i + 2], vLightPos, vLightI);
		m_pColors[i + 1] = PhongReflectionModel(m_pPoints[i + 1], m_pNormals[i + 1], vLightPos, vLightI);
		m_pColors[i + 5] = PhongReflectionModel(m_pPoints[i + 5], m_pNormals[i + 5], vLightPos, vLightI);
	}
	glBindBuffer( GL_ARRAY_BUFFER, m_uiBuffer );
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(vec4)*m_iNumVtx+sizeof(vec3)*m_iNumVtx, sizeof(vec4)*m_iNumVtx, m_pColors ); // vertcies' Color
}

void CObj::RenderWithGouraudShading(const LightSource &Lights)
{
	vec4 vCentroidP;
	for (int i = 0; i < m_iNumVtx; i += 6) {
		m_pColors[i] = m_pColors[i + 3] = PhongReflectionModel(m_pPoints[i], m_pNormals[i], Lights);
		m_pColors[i + 2] = m_pColors[i + 4] = PhongReflectionModel(m_pPoints[i + 2], m_pNormals[i + 2], Lights);
		m_pColors[i + 1] = PhongReflectionModel(m_pPoints[i + 1], m_pNormals[i + 1], Lights);
		m_pColors[i + 5] = PhongReflectionModel(m_pPoints[i + 5], m_pNormals[i + 5], Lights);
	}
	glBindBuffer(GL_ARRAY_BUFFER, m_uiBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec4)*m_iNumVtx + sizeof(vec3)*m_iNumVtx, sizeof(vec4)*m_iNumVtx, m_pColors); // vertcies' Color
}


// ���B�ҵ��� vLightPos �����O�@�ɮy�Ъ��T�w�����m
void CObj::Update(float dt, point4 vLightPos, color4 vLightI)
{
#ifdef LIGHTING_WITHCPU
	if (m_bViewUpdated || m_bTRSUpdated) { // Model View �������x�}���e�����
		m_mxMVFinal = m_mxView * m_mxTRS;
		m_mxMV3X3Final = mat3(
			m_mxMVFinal._m[0].x,  m_mxMVFinal._m[1].x, m_mxMVFinal._m[2].x,
			m_mxMVFinal._m[0].y,  m_mxMVFinal._m[1].y, m_mxMVFinal._m[2].y,
			m_mxMVFinal._m[0].z,  m_mxMVFinal._m[1].z, m_mxMVFinal._m[2].z);

#ifdef GENERAL_CASE
		m_mxITMV = InverseTransposeMatrix(m_mxMVFinal); 
#endif

		m_bViewUpdated = m_bTRSUpdated = false;
	}
	if( m_iMode == FLAT_SHADING ) RenderWithFlatShading(vLightPos, vLightI);
	else RenderWithGouraudShading(vLightPos, vLightI);

#else // Lighting With GPU
	if (m_bViewUpdated || m_bTRSUpdated) {
		m_mxMVFinal = m_mxView * m_mxTRS;
		m_bViewUpdated = m_bTRSUpdated = false;
	}
	m_vLightInView = m_mxView * vLightPos;		// �N Light �ഫ�����Y�y�ЦA�ǤJ
	// ��X AmbientProduct DiffuseProduct �P SpecularProduct �����e
	m_AmbientProduct  = m_Material.ka * m_Material.ambient  * vLightI;
	m_DiffuseProduct  = m_Material.kd * m_Material.diffuse  * vLightI;
	m_SpecularProduct = m_Material.ks * m_Material.specular * vLightI;
#endif
}


void CObj::Update(float dt, const LightSource &Lights)
{
#ifdef LIGHTING_WITHCPU
	if (m_bViewUpdated || m_bTRSUpdated) { // Model View �������x�}���e�����
		m_mxMVFinal = m_mxView * m_mxTRS;
		m_mxMV3X3Final = mat3(
			m_mxMVFinal._m[0].x,  m_mxMVFinal._m[1].x, m_mxMVFinal._m[2].x,
			m_mxMVFinal._m[0].y,  m_mxMVFinal._m[1].y, m_mxMVFinal._m[2].y,
			m_mxMVFinal._m[0].z,  m_mxMVFinal._m[1].z, m_mxMVFinal._m[2].z);

#ifdef GENERAL_CASE
		m_mxITMV = InverseTransposeMatrix(m_mxMVFinal); 
#endif

		m_bViewUpdated = m_bTRSUpdated = false;
	}
	if (m_iMode == FLAT_SHADING) RenderWithFlatShading(Lights);
	else RenderWithGouraudShading(Lights);

#else // Lighting With GPU
	if( m_bViewUpdated || m_bTRSUpdated) {
		m_mxMVFinal = m_mxView * m_mxTRS;
		m_bViewUpdated = m_bTRSUpdated = false;
	}
	m_vLightInView = m_mxView * Lights.position;		// �N Light �ഫ�����Y�y�ЦA�ǤJ
	// ��X AmbientProduct DiffuseProduct �P SpecularProduct �����e
	m_AmbientProduct = m_Material.ka * m_Material.ambient  *  Lights.ambient;
	m_DiffuseProduct = m_Material.kd * m_Material.diffuse  *  Lights.diffuse;
	m_SpecularProduct = m_Material.ks * m_Material.specular * Lights.specular;
#endif

}
void CObj::Update(float dt, const LightSource &Lights, const LightSource &Lights2)
{
#ifdef LIGHTING_WITHCPU
	if (m_bViewUpdated || m_bTRSUpdated) { // Model View �������x�}���e�����
		m_mxMVFinal = m_mxView * m_mxTRS;
		m_mxMV3X3Final = mat3(
			m_mxMVFinal._m[0].x, m_mxMVFinal._m[1].x, m_mxMVFinal._m[2].x,
			m_mxMVFinal._m[0].y, m_mxMVFinal._m[1].y, m_mxMVFinal._m[2].y,
			m_mxMVFinal._m[0].z, m_mxMVFinal._m[1].z, m_mxMVFinal._m[2].z);

#ifdef GENERAL_CASE
		m_mxITMV = InverseTransposeMatrix(m_mxMVFinal);
#endif

		m_bViewUpdated = m_bTRSUpdated = false;
	}
	if (m_iMode == FLAT_SHADING) RenderWithFlatShading(Lights);
	else RenderWithGouraudShading(Lights);

#else // Lighting With GPU
	if (m_bViewUpdated || m_bTRSUpdated) {
		m_mxMVFinal = m_mxView * m_mxTRS;
		m_bViewUpdated = m_bTRSUpdated = false;
	}
	m_vLightInView = m_mxView * Lights.position;		// �N Light �ഫ�����Y�y�ЦA�ǤJ
														// ��X AmbientProduct DiffuseProduct �P SpecularProduct �����e
	m_AmbientProduct = m_Material.ka * m_Material.ambient  *  Lights.ambient;
	m_DiffuseProduct = m_Material.kd * m_Material.diffuse  *  Lights.diffuse;
	m_SpecularProduct = m_Material.ks * m_Material.specular * Lights.specular;

	m_vLight2InView = m_mxView * Lights2.position;		// �N Light �ഫ�����Y�y�ЦA�ǤJ
														// ��X AmbientProduct DiffuseProduct �P SpecularProduct �����e

	m_AmbientProduct2 = m_Material.ka * m_Material.ambient  * Lights2.ambient;
	m_DiffuseProduct2 = m_Material.kd * m_Material.diffuse  * Lights2.diffuse;
	m_SpecularProduct2 = m_Material.ks * m_Material.specular * Lights2.specular;
#endif

}

void CObj::Update(float dt, const LightSource &Lights, const LightSource &Lights2, const LightSource &Lights3, const LightSource &Lights4)
{
#ifdef LIGHTING_WITHCPU
	if (m_bViewUpdated || m_bTRSUpdated) { // Model View �������x�}���e�����
		m_mxMVFinal = m_mxView * m_mxTRS;
		m_mxMV3X3Final = mat3(
			m_mxMVFinal._m[0].x, m_mxMVFinal._m[1].x, m_mxMVFinal._m[2].x,
			m_mxMVFinal._m[0].y, m_mxMVFinal._m[1].y, m_mxMVFinal._m[2].y,
			m_mxMVFinal._m[0].z, m_mxMVFinal._m[1].z, m_mxMVFinal._m[2].z);

#ifdef GENERAL_CASE
		m_mxITMV = InverseTransposeMatrix(m_mxMVFinal);
#endif

		m_bViewUpdated = m_bTRSUpdated = false;
	}
	if (m_iMode == FLAT_SHADING) RenderWithFlatShading(Lights);
	else RenderWithGouraudShading(Lights);

#else // Lighting With GPU
	if (m_bViewUpdated || m_bTRSUpdated) {
		m_mxMVFinal = m_mxView * m_mxTRS;
		m_bViewUpdated = m_bTRSUpdated = false;
	}
	m_vLightInView = m_mxView * Lights.position;		// �N Light �ഫ�����Y�y�ЦA�ǤJ
														// ��X AmbientProduct DiffuseProduct �P SpecularProduct �����e
	m_AmbientProduct = m_Material.ka * m_Material.ambient  *  Lights.ambient;
	m_DiffuseProduct = m_Material.kd * m_Material.diffuse  *  Lights.diffuse;
	m_SpecularProduct = m_Material.ks * m_Material.specular * Lights.specular;

	m_vLight2InView = m_mxView * Lights2.position;		// �N Light �ഫ�����Y�y�ЦA�ǤJ
	m_AmbientProduct2 = m_Material.ka * m_Material.ambient  * Lights2.ambient;
	m_DiffuseProduct2 = m_Material.kd * m_Material.diffuse  * Lights2.diffuse;
	m_SpecularProduct2 = m_Material.ks * m_Material.specular * Lights2.specular;

	m_vLight3InView = m_mxView * Lights3.position;		// �N Light �ഫ�����Y�y�ЦA�ǤJ
	m_AmbientProduct3 = m_Material.ka * m_Material.ambient  * Lights3.ambient;
	m_DiffuseProduct3 = m_Material.kd * m_Material.diffuse  * Lights3.diffuse;
	m_SpecularProduct3 = m_Material.ks * m_Material.specular * Lights3.specular;

	m_vLight4InView = m_mxView * Lights4.position;		// �N Light �ഫ�����Y�y�ЦA�ǤJ
	m_AmbientProduct4 = m_Material.ka * m_Material.ambient  * Lights4.ambient;
	m_DiffuseProduct4 = m_Material.kd * m_Material.diffuse  * Lights4.diffuse;
	m_SpecularProduct4 = m_Material.ks * m_Material.specular * Lights4.specular;
#endif

}
void CObj::Update(float dt)
{
	if (m_bViewUpdated || m_bTRSUpdated) { // Model View �������x�}���e�����
		m_mxMVFinal = m_mxView * m_mxTRS;
		m_mxITView = InverseTransposeMatrix(m_mxMVFinal);
		m_bViewUpdated = m_bTRSUpdated = false;
	}
}
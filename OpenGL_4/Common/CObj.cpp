#include "CObj.h"

CObj::CObj(char * adrFile)
{
	m_pPoints = NULL; m_pNormals = NULL; m_pTex = NULL;

	//引數暫存變數
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
			if (strcmp(lineHeader, "f") == 0) { vindex++; } //計算點總數
		}
		fclose(objfile);
		m_iNumVtx = vindex * 3;
		m_pPoints = new vec4[m_iNumVtx + 1];
		vindex = 0; mindex = 0;//另當後續點的暫存

							   //依照實際頂點數決定陣列大小
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
				//請依照自己的OBJ檔決定讀取方式
				//"f 頂點索引/uv點索引/法線索引"
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
	// 預設將所有的面都設定成灰色
	for( int i = 0 ; i < m_iNumVtx ; i++ ) m_pColors[i] = vec4(-1.0f,-1.0f,-1.0f,1.0f);

#ifdef LIGHTING_WITHCPU
	// Default set shader's name
	setShaderName("vsLighting_CPU.glsl", "fsLighting_CPU.glsl");
#else // lighting with GPU
#ifdef PERVERTEX_LIGHTING
	setShaderName("vsLighting_GPU.glsl", "fsLighting_GPU.glsl");
#else
	setShaderName("vsPerPixelLighting.glsl", "fsPerPixelLighting.glsl");
#endif
#endif  

	// Create and initialize a buffer object ，將此部分的設定移入 setShader 中
//	CreateBufferObject();

	// 設定材質
	setMaterials(vec4(0), vec4(0.5f, 0.5f, 0.5f, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	setKaKdKsShini(0, 0.8f, 0.2f, 1);
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

void CObj::draw()
{
//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // Change to wireframe mode
	drawingsetShader();
	glDrawArrays( GL_TRIANGLES, 0, m_iNumVtx);
//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Return to solid mode
}


void CObj::drawW()
{
	drawingWithoutsetShader();
	glDrawArrays( GL_TRIANGLES, 0, m_iNumVtx);
}


void CObj::RenderWithFlatShading(vec4 vLightPos, color4 vLightI)
{
	// 以每一個面的三個頂點計算其重心，以該重心作為顏色計算的點頂
	// 根據 Phong lighting model 計算相對應的顏色，並將顏色儲存到此三個點頂
	// 因為每一個平面的頂點的 Normal 都相同，所以此處並沒有計算此三個頂點的平均 Normal

	vec4 vCentroidP;
	for( int i = 0 ; i < m_iNumVtx ; i += 3 ) {
		// 計算三角形的重心
		vCentroidP = (m_pPoints[i] + m_pPoints[i+1] + m_pPoints[i+2])/3.0f;
		m_pColors[i] = m_pColors[i + 1] = m_pColors[i + 2] = PhongReflectionModel(vCentroidP, m_pNormals[i], vLightPos, vLightI);
	}
	glBindBuffer( GL_ARRAY_BUFFER, m_uiBuffer );
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(vec4)*m_iNumVtx+sizeof(vec3)*m_iNumVtx, sizeof(vec4)*m_iNumVtx, m_pColors ); // vertcies' Color
}

void CObj::RenderWithFlatShading(const LightSource &Lights)
{
	// 以每一個面的三個頂點計算其重心，以該重心作為顏色計算的點頂
	// 根據 Phong lighting model 計算相對應的顏色，並將顏色儲存到此三個點頂
	// 因為每一個平面的頂點的 Normal 都相同，所以此處並沒有計算此三個頂點的平均 Normal

	vec4 vCentroidP;
	for (int i = 0; i < m_iNumVtx; i += 3) {
		// 計算三角形的重心
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


// 此處所給的 vLightPos 必須是世界座標的確定絕對位置
void CObj::Update(float dt, point4 vLightPos, color4 vLightI)
{
#ifdef LIGHTING_WITHCPU
	if (m_bViewUpdated || m_bTRSUpdated) { // Model View 的相關矩陣內容有更動
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
	m_vLightInView = m_mxView * vLightPos;		// 將 Light 轉換到鏡頭座標再傳入
	// 算出 AmbientProduct DiffuseProduct 與 SpecularProduct 的內容
	m_AmbientProduct  = m_Material.ka * m_Material.ambient  * vLightI;
	m_DiffuseProduct  = m_Material.kd * m_Material.diffuse  * vLightI;
	m_SpecularProduct = m_Material.ks * m_Material.specular * vLightI;
#endif
}


void CObj::Update(float dt, const LightSource &Lights)
{
#ifdef LIGHTING_WITHCPU
	if (m_bViewUpdated || m_bTRSUpdated) { // Model View 的相關矩陣內容有更動
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
	m_vLightInView = m_mxView * Lights.position;		// 將 Light 轉換到鏡頭座標再傳入
	// 算出 AmbientProduct DiffuseProduct 與 SpecularProduct 的內容
	m_AmbientProduct = m_Material.ka * m_Material.ambient  *  Lights.ambient;
	m_DiffuseProduct = m_Material.kd * m_Material.diffuse  *  Lights.diffuse;
	m_SpecularProduct = m_Material.ks * m_Material.specular * Lights.specular;
#endif

}
void CObj::Update(float dt, const LightSource &Lights, const LightSource &Lights2)
{
#ifdef LIGHTING_WITHCPU
	if (m_bViewUpdated || m_bTRSUpdated) { // Model View 的相關矩陣內容有更動
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
	m_vLightInView = m_mxView * Lights.position;		// 將 Light 轉換到鏡頭座標再傳入
														// 算出 AmbientProduct DiffuseProduct 與 SpecularProduct 的內容
	m_AmbientProduct = m_Material.ka * m_Material.ambient  *  Lights.ambient;
	m_DiffuseProduct = m_Material.kd * m_Material.diffuse  *  Lights.diffuse;
	m_SpecularProduct = m_Material.ks * m_Material.specular * Lights.specular;

	m_vLight2InView = m_mxView * Lights2.position;		// 將 Light 轉換到鏡頭座標再傳入
														// 算出 AmbientProduct DiffuseProduct 與 SpecularProduct 的內容

	m_AmbientProduct2 = m_Material.ka * m_Material.ambient  * Lights2.ambient;
	m_DiffuseProduct2 = m_Material.kd * m_Material.diffuse  * Lights2.diffuse;
	m_SpecularProduct2 = m_Material.ks * m_Material.specular * Lights2.specular;
#endif

}

void CObj::Update(float dt, const LightSource &Lights, const LightSource &Lights2, const LightSource &Lights3, const LightSource &Lights4)
{
#ifdef LIGHTING_WITHCPU
	if (m_bViewUpdated || m_bTRSUpdated) { // Model View 的相關矩陣內容有更動
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
	m_vLightInView = m_mxView * Lights.position;		// 將 Light 轉換到鏡頭座標再傳入
														// 算出 AmbientProduct DiffuseProduct 與 SpecularProduct 的內容
	m_AmbientProduct = m_Material.ka * m_Material.ambient  *  Lights.ambient;
	m_DiffuseProduct = m_Material.kd * m_Material.diffuse  *  Lights.diffuse;
	m_SpecularProduct = m_Material.ks * m_Material.specular * Lights.specular;

	m_vLight2InView = m_mxView * Lights2.position;		// 將 Light 轉換到鏡頭座標再傳入
	m_AmbientProduct2 = m_Material.ka * m_Material.ambient  * Lights2.ambient;
	m_DiffuseProduct2 = m_Material.kd * m_Material.diffuse  * Lights2.diffuse;
	m_SpecularProduct2 = m_Material.ks * m_Material.specular * Lights2.specular;

	m_vLight3InView = m_mxView * Lights3.position;		// 將 Light 轉換到鏡頭座標再傳入
	m_AmbientProduct3 = m_Material.ka * m_Material.ambient  * Lights3.ambient;
	m_DiffuseProduct3 = m_Material.kd * m_Material.diffuse  * Lights3.diffuse;
	m_SpecularProduct3 = m_Material.ks * m_Material.specular * Lights3.specular;

	m_vLight4InView = m_mxView * Lights4.position;		// 將 Light 轉換到鏡頭座標再傳入
	m_AmbientProduct4 = m_Material.ka * m_Material.ambient  * Lights4.ambient;
	m_DiffuseProduct4 = m_Material.kd * m_Material.diffuse  * Lights4.diffuse;
	m_SpecularProduct4 = m_Material.ks * m_Material.specular * Lights4.specular;
#endif

}
void CObj::Update(float dt)
{
	if (m_bViewUpdated || m_bTRSUpdated) { // Model View 的相關矩陣內容有更動
		m_mxMVFinal = m_mxView * m_mxTRS;
		m_mxITView = InverseTransposeMatrix(m_mxMVFinal);
		m_bViewUpdated = m_bTRSUpdated = false;
	}
}
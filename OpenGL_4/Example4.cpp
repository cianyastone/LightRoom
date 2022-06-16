// Gouraud shading with a single light source at (4, 4, 0);
// �P Exmaple4 �ۦP, Light ���ѼƧ令�H���c���覡�ǻ�
// �P�ɹ�{ Per Pixel Lighting�BNonphotorealistic Shading  �P Silhouette
//
// ����e���ǳƤu�@
// ���� CShape.h ���� #define LIGHTING_WITHCPU 
// �}�� CShape.h ���� #define LIGHTING_WITHGPU 
// ���� CShape.h ���� #define PERVERTEX_LIGHTING 
//

#include "header/Angel.h"
#include "Common/TypeDefine.h"
#include "Common/CQuad.h"
#include "Common/CSolidCube.h"
#include "Common/CSolidSphere.h"
#include "Common/CWireSphere.h"
#include "Common/CWireCube.h"
#include "Common/CChecker.h"
#include "Common/CCamera.h"
#include"Common\CObj.h"
#include "Common\CLineSegment.h"
#include"Common\C2DShape.h"
#include"Common\C2DSprite.h"

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 800
#define WIDTH_HALF (SCREEN_WIDTH/2) 
#define HEIGHT_HALF (SCREEN_HEIGHT/2)
#define SPACE_KEY 32
#define SCREEN_SIZE 600
#define HALF_SIZE SCREEN_SIZE /2 
#define VP_HALFWIDTH  20.0f
#define VP_HALFHEIGHT 20.0f
#define GRID_SIZE 20 // must be an even number

#define SETTING_MATERIALS 

// For Model View and Projection Matrix
mat4 g_mxModelView(1.0f);
mat4 g_mxProjection;
point4 eye;
point4 at;
// For Objects
CChecker      *g_pChecker;
CSolidCube    *g_pCube;
CSolidSphere  *g_pSphere;

CQuad         *g_LeftWall, *g_RightWall;
CQuad         *g_FrontWall, *g_BackWall;
//objs
CObj *g_pObj1;
CObj *g_pObj2;
CObj *g_pObj3;
CObj *g_pObj4;

// For View Point
GLfloat g_fRadius = 10.0;
GLfloat g_fTheta = 60*DegreesToRadians;
GLfloat g_fPhi = 45*DegreesToRadians;

vec4    g_vUp( 0.0, 1.0, 0.0, 0.0 );

//----------------------------------------------------------------------------
// Part 2 : for single light source
bool g_bAutoRotating = false;
float g_fElapsedTime = 0;
float g_fLightRadius = 6;
float g_fLightTheta = 0;

float g_fLightR = 0.95f;
float g_fLightG = 0.95f;
float g_fLightB = 0.95f;

LightSource g_Light1 = {
    color4(g_fLightR, g_fLightG, g_fLightB, 1.0f), // ambient 
	color4(g_fLightR, g_fLightG, g_fLightB, 1.0f), // diffuse
	color4(g_fLightR, g_fLightG, g_fLightB, 1.0f), // specular
	point4(g_fLightRadius, g_fLightRadius, 0.0f, 1.0f),   // position
    point4(0.0f, 0.0f, 0.0f, 1.0f),   // halfVector
	vec3(0.0f, 0.0f, 0.0f),			  //spotTarget
    vec3(0.0f, 0.0f, 0.0f),			  //spotDirection
	1.0f	,	// spotExponent(parameter e); cos^(e)(phi) 
	45.0f,	// spotCutoff;	// (range: [0.0, 90.0], 180.0)  spot ���ө��d��
	1.0f	,	// spotCosCutoff; // (range: [1.0,0.0],-1.0), �ө���V�P�Q�ө��I���������ר� cos ��, cut off ����
	1	,	// constantAttenuation	(a + bd + cd^2)^-1 ���� a, d ��������Q�ө��I���Z��
	0	,	// linearAttenuation	    (a + bd + cd^2)^-1 ���� b
	0		// quadraticAttenuation (a + bd + cd^2)^-1 ���� c
};
//spotlight1

float g_fSpotLightR = 0.95f;
float g_fSpotLightG = 0.15f;
float g_fSpotLightB = 0.15f;
LightSource g_spotLight1 = {
	color4(g_fSpotLightR, g_fSpotLightG, g_fSpotLightB, 1.0f), // ambient 
	color4(g_fSpotLightR, g_fSpotLightG, g_fSpotLightB, 1.0f), // diffuse
	color4(g_fSpotLightR, g_fSpotLightG, g_fSpotLightB, 1.0f), // specular
	point4(-5.5f, 5.0f, 1.0f, 1.0f),   // position
	point4(0.0f, 0.0f, 0.0f, 1.0f),   // halfVector
	vec3(-5.0f, 0.0f, 1.0f),		  // spotTarget
	vec3(0.0f, 0.0f, 0.0f),		  // spotDirection�A�ݭ��s�p��
	1.0f,	// spotExponent(parameter e); cos^(e)(phi) 
	1.0f,	// spotCutoff;	// (range: [0.0, 90.0], 180.0)  spot ���ө��d��
	0.5f,	// spotCosCutoff = cos(spotCutoff) ; spot ���ө��d��� cos
	0.001f,	// constantAttenuation	(a + bd + cd^2)^-1 ���� a, d ��������Q�ө��I���Z��
	0.001f,	// linearAttenuation	    (a + bd + cd^2)^-1 ���� b
	0	// quadraticAttenuation (a + bd + cd^2)^-1 ���� c
};
vec4 g_fSpotLightI(g_fSpotLightR, g_fSpotLightG, g_fSpotLightB, 1.0f);
CLineSegment *g_LightLine;
//spotlight2
float g_fSpotLightR2 = 0.15f;
float g_fSpotLightG2 = 0.15f;
float g_fSpotLightB2 = 0.85f;
LightSource g_spotLight2 = {
	color4(g_fSpotLightR2, g_fSpotLightG2, g_fSpotLightB2, 1.0f), // ambient 
	color4(g_fSpotLightR2, g_fSpotLightG2, g_fSpotLightB2, 1.0f), // diffuse
	color4(g_fSpotLightR2, g_fSpotLightG2, g_fSpotLightB2, 1.0f), // specular
	point4(-0.5f, 5.0f, -5.5f, 1.0f),   // position
	point4(0.0f, 0.0f, 0.0f, 1.0f),   // halfVector
	vec3(-0.5f, 0.0f, -10.0f),		  // spotTarget
	vec3(0.0f, 0.0f, 0.0f),		  // spotDirection�A�ݭ��s�p��
	1.0f,	// spotExponent(parameter e); cos^(e)(phi) 
	10.0f,	// spotCutoff;	// (range: [0.0, 90.0], 180.0)  spot ���ө��d��
	0.5f,	// spotCosCutoff = cos(spotCutoff) ; spot ���ө��d��� cos
	0.5f,	// constantAttenuation	(a + bd + cd^2)^-1 ���� a, d ��������Q�ө��I���Z��
	0,	// linearAttenuation	    (a + bd + cd^2)^-1 ���� b
	0	// quadraticAttenuation (a + bd + cd^2)^-1 ���� c
};
vec4 g_fSpotLightI2(g_fSpotLightR2, g_fSpotLightG2, g_fSpotLightB2, 1.0f);
CLineSegment *g_LightLine2;
//spotlight3
float g_fSpotLightR3 = 0.85f;
float g_fSpotLightG3 = 0.25f;
float g_fSpotLightB3 = 1.0f;
LightSource g_spotLight3 = {
	color4(g_fSpotLightR3, g_fSpotLightG3, g_fSpotLightB3, 1.0f), // ambient 
	color4(g_fSpotLightR3, g_fSpotLightG3, g_fSpotLightB3, 1.0f), // diffuse
	color4(g_fSpotLightR3, g_fSpotLightG3, g_fSpotLightB3, 1.0f), // specular
	point4(5.0f, 3.0f, 6.5f, 1.0f),   // position
	point4(0.0f, 0.0f, 0.0f, 1.0f),   // halfVector
	vec3(5.0f, 0.0f, 6.0f),		  // spotTarget
	vec3(0.0f, 0.0f, 0.0f),		  // spotDirection�A�ݭ��s�p��
	1.0f,	// spotExponent(parameter e); cos^(e)(phi) 
	10.0f,	// spotCutoff;	// (range: [0.0, 90.0], 180.0)  spot ���ө��d��
	0.5f,	// spotCosCutoff = cos(spotCutoff) ; spot ���ө��d��� cos
	0.5f,	// constantAttenuation	(a + bd + cd^2)^-1 ���� a, d ��������Q�ө��I���Z��
	0,	// linearAttenuation	    (a + bd + cd^2)^-1 ���� b
	0	// quadraticAttenuation (a + bd + cd^2)^-1 ���� c
};
vec4 g_fSpotLightI3(g_fSpotLightR3, g_fSpotLightG3, g_fSpotLightB3, 1.0f);
CLineSegment *g_LightLine3;

CWireSphere *g_pLight,*g_pSpotLight1, *g_pSpotLight2,*g_pSpotLight3;
//2d UI
C2DSprite *g_p2DBtn[4];
mat4  g_2DView;
mat4  g_2DProj;


//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// �禡���쫬�ŧi
extern void IdleProcess();

void init( void )
{
	mat4 mxT,mxS;
	
	vec4 vT, vColor;
	vec3 vS;
	// ���ͩһݤ� Model View �P Projection Matrix

	at= point4(sin(g_fTheta)*sin(g_fPhi), cos(g_fTheta), sin(g_fTheta)*cos(g_fPhi), 1.0f);
	eye=point4(0.0f, 5.0f, 0.0f, 1.0f);

	auto camera = CCamera::create();
	camera->updateViewLookAt(eye, eye+at);
	camera->updatePerspective(60.0, (GLfloat)SCREEN_SIZE / (GLfloat)SCREEN_SIZE, 1.0, 1000.0);

	// ���ͪ��󪺹���
	g_pChecker = new CChecker(GRID_SIZE);
// Part 3 : materials
#ifdef SETTING_MATERIALS
	g_pChecker->SetMaterials(vec4(0.15f,0.15f,0.15f,1.0f), vec4(0, 0.85f, 0, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	g_pChecker->	SetKaKdKsShini(0, 0.8f, 0.5f, 1);
#endif
	g_pChecker->SetShadingMode(GOURAUD_SHADING);
	g_pChecker->SetShader();

	vT.x = -10.0f; vT.y = 10.0f; vT.z = 0;
	mxT = Translate(vT);
	g_LeftWall = new CQuad;
	g_LeftWall->SetMaterials(vec4(0.15f, 0.15f, 0.15f, 1.0f), vec4(0, 0.85f, 0, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	g_LeftWall->SetShadingMode(GOURAUD_SHADING);
	g_LeftWall->SetShader();
	g_LeftWall->SetColor(vec4(0.6f));
	g_LeftWall->SetTRSMatrix(mxT*RotateZ(-90.0f)*Scale(20.0f, 1, 20.0f));
	g_LeftWall->SetKaKdKsShini(0, 0.8f, 0.5f, 1);

	vT.x = 10.0f; vT.y = 10.0f; vT.z = 0;
	mxT = Translate(vT);
	g_RightWall = new CQuad;
	g_RightWall->SetMaterials(vec4(0.15f, 0.15f, 0.15f, 1.0f), vec4(0, 0.85f, 0, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	g_RightWall->SetShadingMode(GOURAUD_SHADING);
	g_RightWall->SetShader();
	g_RightWall->SetColor(vec4(0.6f));
	g_RightWall->SetTRSMatrix(mxT*RotateZ(90.0f)*Scale(20.0f, 1, 20.0f));
	g_RightWall->SetKaKdKsShini(0, 0.8f, 0.5f, 1);

	vT.x = 0.0f; vT.y = 10.0f; vT.z = 10.0f;
	mxT = Translate(vT);
	g_FrontWall = new CQuad;
	g_FrontWall->SetMaterials(vec4(0.15f, 0.15f, 0.15f, 1.0f), vec4(0, 0.85f, 0, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	g_FrontWall->SetShadingMode(GOURAUD_SHADING);
	g_FrontWall->SetShader();
	g_FrontWall->SetColor(vec4(0.6f));
	g_FrontWall->SetTRSMatrix(mxT*RotateX(-90.0f)*Scale(20.0f, 1, 20.0f));
	g_FrontWall->SetKaKdKsShini(0, 0.8f, 0.5f, 1);

	vT.x = 0.0f; vT.y = 10.0f; vT.z = -10.0f;
	
	mxT = Translate(vT);
	g_BackWall = new CQuad;
	g_BackWall->SetMaterials(vec4(0.15f, 0.15f, 0.15f, 1.0f), vec4(0, 0.85f, 0, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	g_BackWall->SetShadingMode(GOURAUD_SHADING);
	g_BackWall->SetShader();
	g_BackWall->SetColor(vec4(0.6f));
	g_BackWall->SetTRSMatrix(mxT*RotateX(90.0f)*Scale(20.0f, 1, 20.0f));
	g_BackWall->SetKaKdKsShini(0, 0.8f, 0.5f, 1);

//OBJ1
	g_pObj1 = new CObj("./Common/Well.obj");
	g_pObj1->SetMaterials(vec4(0.15f, 0.15f, 0.15f, 1.0f), vec4(0.85f, 0.8, 0, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	g_pObj1->SetKaKdKsShini(0.15f, 0.8f, 0.2f, 2);

	g_pObj1->SetShader();
	vT.x = -5.5; vT.y = 2.5; vT.z = 1;
	vS.x = 1;	vS.y =1;	vS.z = 1;
	mxS = Scale(vS);
	mxT = Translate(vT);
	g_pObj1->SetTRSMatrix(mxT*mxS);
	g_pObj1->SetShadingMode(GOURAUD_SHADING);

	g_pObj2 = new CObj("./Common/Well.obj");
	g_pObj2->SetMaterials(vec4(0.15f, 0.15f, 0.15f, 1.0f), vec4(0.55f, 0.3, 0.2, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	g_pObj2->SetKaKdKsShini(0.15f, 0.8f, 0.2f, 2);
	g_pObj2->SetShader();
	vT.x = 0; vT.y = 0.5; vT.z =0;
	vS.x = 1;	vS.y = 1;	vS.z = 1;
	mxS = Scale(vS);
	mxT = Translate(vT);
	g_pObj2->SetTRSMatrix(mxT*mxS);
	g_pObj2->SetShadingMode(GOURAUD_SHADING);

	g_pObj3 = new CObj("./Common/Well.obj");
	g_pObj3->SetMaterials(vec4(0.15f, 0.15f, 0.15f, 1.0f), vec4(0.95f, 0.3, 0.5f, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	g_pObj3->SetKaKdKsShini(0.15f, 0.8f, 0.2f, 2);

	g_pObj3->SetShader();
	vT.x = -0.5; vT.y = 0.5; vT.z = -5.5;
	vS.x = 1;	vS.y = 1;	vS.z = 1;
	mxS = Scale(vS);
	mxT = Translate(vT);
	g_pObj3->SetTRSMatrix(mxT*mxS);
	g_pObj3->SetShadingMode(GOURAUD_SHADING);
	//wolf
	//lowpolytree
	//low-poly-mill
	//wood_sphere_low
	g_pObj4 = new CObj("./Common/Well.obj");
	g_pObj4->SetMaterials(vec4(0.15f, 0.15f, 0.15f, 1.0f), vec4(0.85f, 0.8f, 0.5f, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	g_pObj4->SetKaKdKsShini(0.15f, 0.8f, 0.2f, 2);

	g_pObj4->SetShader();
	vT.x = 5.5; vT.y = 1.0; vT.z = 3.5;
	vS.x = 1;	vS.y = 1;	vS.z = 1;
	mxS = Scale(vS);
	mxT = Translate(vT);
	g_pObj4->SetTRSMatrix(mxT*mxS);
	g_pObj4->SetShadingMode(GOURAUD_SHADING);

	g_pCube = new CSolidCube;
// Part 3 : materials
#ifdef SETTING_MATERIALS
	g_pCube->SetMaterials(vec4(0.15f,0.15f,0.15f,1.0f), vec4(0.85f, 0, 0, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	g_pCube->SetKaKdKsShini(0.15f, 0.8f, 0.2f, 2);
#endif
	g_pCube->SetShader();
	// �]�w Cube
	vT.x = -0.5; vT.y = 0.5; vT.z = -5.5;
	mxT = Translate(vT);
	g_pCube->SetTRSMatrix(mxT);
	g_pCube->SetShadingMode(GOURAUD_SHADING);

	g_pSphere = new CSolidSphere(1, 16, 16);
// Part 3 : materials
#ifdef SETTING_MATERIALS
	g_pSphere->SetMaterials(vec4(0.15f,0.15f,0.15f,1.0f), vec4(0, 0, 0.85f, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	g_pSphere->SetKaKdKsShini(0.15f, 0.45f, 0.55f, 5);
#endif
	g_pSphere->SetShader();
	// �]�w Sphere
	vT.x = 5.5; vT.y = 1.0; vT.z = 3.5;
	mxT = Translate(vT);
	g_pSphere->SetTRSMatrix(mxT);
	g_pSphere->SetShadingMode(GOURAUD_SHADING);

	// �]�w �N�� Light �� WireSphere
	g_pLight = new CWireSphere(0.25f, 6, 3);
	g_pLight->SetShader();
	mxT = Translate(g_Light1.position);
	g_pLight->SetTRSMatrix(mxT);
	g_pLight->SetColor(g_Light1.diffuse);
#ifdef LIGHTING_WITHGPU
	g_pLight->SetLightingDisable();
#endif
	//spotlight
	g_pSpotLight1 = new CWireSphere(0,0,0);
	g_pSpotLight1->SetShader();
	mxT = Translate(g_spotLight1.position);
	g_pSpotLight1->SetTRSMatrix(mxT);
	g_pSpotLight1->SetColor(g_fSpotLightI);
	g_spotLight1.UpdateDirection();

	g_LightLine = new CLineSegment(g_spotLight1.position, g_spotLight1.spotTarget, vec4(1, 0, 0, 1));
	g_LightLine->SetShader();
	//spotlight2
	g_pSpotLight2 = new CWireSphere(0, 0, 0);
	g_pSpotLight2->SetShader();
	mxT = Translate(g_spotLight2.position);
	g_pSpotLight2->SetTRSMatrix(mxT);
	g_pSpotLight2->SetColor(g_fSpotLightI2);
	g_spotLight2.UpdateDirection();

	g_LightLine2 = new CLineSegment(g_spotLight2.position, g_spotLight2.spotTarget, vec4(1, 0, 0, 1));
	g_LightLine2->SetShader();
	//spotlight3
	g_pSpotLight3 = new CWireSphere(0, 0, 0);
	g_pSpotLight3->SetShader();
	mxT = Translate(g_spotLight3.position);
	g_pSpotLight3->SetTRSMatrix(mxT);
	g_pSpotLight3->SetColor(g_fSpotLightI3);
	g_spotLight3.UpdateDirection();

	g_LightLine3 = new CLineSegment(g_spotLight3.position, g_spotLight3.spotTarget, vec4(1, 0, 0, 1));
	g_LightLine3->SetShader();
	//2D ui
	// �H�U���Q�Υ����v���� 2D ������
	// �d��b X/Y ������  -1 �� 1 �����A��������b Z = 0 

	g_p2DBtn[0] = new C2DSprite; g_p2DBtn[0]->SetShader();
	vColor.x = 1; vColor.y = 0; vColor.z = 0; g_p2DBtn[0]->SetDefaultColor(vColor);
	mxS = Scale(0.1f, 0.1f, 1.0f);
	mxT = Translate(-0.90f, -0.90f, 0);
	g_p2DBtn[0]->SetTRSMatrix(mxT*mxS);
	g_p2DBtn[0]->SetViewMatrix(g_2DView);
	g_p2DBtn[0]->SetViewMatrix(g_2DProj);

	g_p2DBtn[1] = new C2DSprite; g_p2DBtn[1]->SetShader();
	vColor.x = 0; vColor.y = 1; vColor.z = 0; g_p2DBtn[1]->SetDefaultColor(vColor);
	mxT = Translate(0.90f, -0.90f, 0);
	g_p2DBtn[1]->SetTRSMatrix(mxT*mxS);
	g_p2DBtn[1]->SetViewMatrix(g_2DView);
	g_p2DBtn[1]->SetViewMatrix(g_2DProj);

	g_p2DBtn[2] = new C2DSprite; g_p2DBtn[2]->SetShader();
	vColor.x = 0; vColor.y = 0; vColor.z = 1; g_p2DBtn[2]->SetDefaultColor(vColor);
	mxT = Translate(0.90f, 0.90f, 0);
	g_p2DBtn[2]->SetTRSMatrix(mxT*mxS);
	g_p2DBtn[2]->SetViewMatrix(g_2DView);
	g_p2DBtn[2]->SetViewMatrix(g_2DProj);

	g_p2DBtn[3] = new C2DSprite; g_p2DBtn[3]->SetShader();
	vColor.x = 1; vColor.y = 1; vColor.z = 1; g_p2DBtn[3]->SetDefaultColor(vColor);
	mxT = Translate(-0.90f, 0.90f, 0);
	g_p2DBtn[3]->SetTRSMatrix(mxT*mxS);
	g_p2DBtn[3]->SetViewMatrix(g_2DView);
	g_p2DBtn[3]->SetViewMatrix(g_2DProj);

//
	// �]�����d�Ҥ��|�ʨ� Projection Matrix �ҥH�b�o�̳]�w�@���Y�i
	// �N���g�b OnFrameMove ���C���� Check
	bool bPDirty;
	mat4 mpx = camera->getProjectionMatrix(bPDirty);
	g_pChecker->SetProjectionMatrix(mpx);
	g_pCube->SetProjectionMatrix(mpx);
	g_pSphere->SetProjectionMatrix(mpx);
	g_pLight->SetProjectionMatrix(mpx);
	g_LeftWall->SetProjectionMatrix(mpx);
	g_RightWall->SetProjectionMatrix(mpx);
	g_FrontWall->SetProjectionMatrix(mpx);
	g_BackWall->SetProjectionMatrix(mpx);
	g_pObj1->SetProjectionMatrix(mpx);
	g_pObj2->SetProjectionMatrix(mpx);
	g_pObj3->SetProjectionMatrix(mpx);
	g_pObj4->SetProjectionMatrix(mpx);

	g_pSpotLight1->SetProjectionMatrix(mpx);
	g_LightLine->SetProjectionMatrix(mpx);
	g_pSpotLight2->SetProjectionMatrix(mpx);
	g_LightLine2->SetProjectionMatrix(mpx);
	g_pSpotLight3->SetProjectionMatrix(mpx);
	g_LightLine3->SetProjectionMatrix(mpx);
}

//----------------------------------------------------------------------------
void GL_Display( void )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); // clear the window

	g_pChecker->Draw();
	//g_pSphere->Draw();
	//g_pCube->Draw();
	g_pLight->Draw();
	g_LeftWall->Draw();
	g_RightWall->Draw();
	g_FrontWall->Draw();
	g_BackWall->Draw();
	g_pObj1->Draw();
	g_pObj2->Draw();
	g_pObj3->Draw();
	g_pObj4->Draw();

	g_pSpotLight1->Draw();
	//g_LightLine->Draw();
	g_pSpotLight2->Draw();
	//g_LightLine2->Draw();
	g_pSpotLight3->Draw();
	//g_LightLine3->Draw();
	for (int i = 0; i < 4; i++) g_p2DBtn[i]->Draw();
	glutSwapBuffers();	// �洫 Frame Buffer
}

//----------------------------------------------------------------------------
// Part 2 : for single light source
void UpdateLightPosition(float dt)
{
	mat4 mxT;
	// �C��¶ Y �b�� 90 ��
	g_fElapsedTime += dt;
	g_fLightTheta = g_fElapsedTime*(float)M_PI_2;
	if( g_fLightTheta >= (float)M_PI*2.0f ) {
		g_fLightTheta -= (float)M_PI*2.0f;
		g_fElapsedTime -= 4.0f;
	}
	g_Light1.position.x = g_fLightRadius * cosf(g_fLightTheta);
	g_Light1.position.z = g_fLightRadius * sinf(g_fLightTheta);
	mxT = Translate(g_Light1.position);
	g_pLight->SetTRSMatrix(mxT);

	
}
//----------------------------------------------------------------------------

void onFrameMove(float delta)
{
	mat4 mvx;	// view matrix & projection matrix
	bool bVDirty;	// view �P projection matrix �O�_�ݭn��s������
	auto camera = CCamera::getInstance();
	mvx = camera->getViewMatrix(bVDirty);
	if (bVDirty) {
		g_pChecker->SetViewMatrix(mvx);
		g_pCube->SetViewMatrix(mvx);
		g_pSphere->SetViewMatrix(mvx);
		g_LeftWall->SetViewMatrix(mvx);
		g_RightWall->SetViewMatrix(mvx);
		g_FrontWall->SetViewMatrix(mvx);
		g_BackWall->SetViewMatrix(mvx);
		g_pLight->SetViewMatrix(mvx);
		g_pObj1->SetViewMatrix(mvx);
		g_pObj2->SetViewMatrix(mvx);
		g_pObj3->SetViewMatrix(mvx);
		g_pObj4->SetViewMatrix(mvx);

		g_pSpotLight1->SetViewMatrix(mvx);
		g_LightLine->SetViewMatrix(mvx);
		g_pSpotLight2->SetViewMatrix(mvx);
		g_LightLine2->SetViewMatrix(mvx);
		g_pSpotLight3->SetViewMatrix(mvx);
		g_LightLine3->SetViewMatrix(mvx);
	}

	if( g_bAutoRotating ) { // Part 2 : ���s�p�� Light ����m
		UpdateLightPosition(delta);
	}


	// �p�G�ݭn���s�p��ɡA�b�o��p��C�@�Ӫ����C��
	g_pChecker->Update(delta, g_Light1,  g_spotLight1, g_spotLight2,  g_spotLight3);
	//g_pCube->Update(delta, g_Light1, g_spotLight1);

	//g_pSphere->Update(delta, g_Light1, g_spotLight1);
	g_LeftWall->Update(delta, g_Light1);

	g_RightWall->Update(delta, g_Light1);
	g_FrontWall->Update(delta, g_Light1);
	g_BackWall->Update(delta, g_Light1);
	g_pObj2->Update(delta, g_Light1, g_spotLight1,  g_spotLight2, g_spotLight3);
	g_pObj1->Update(delta, g_Light1,  g_spotLight1);
	g_pObj3->Update(delta, g_Light1, g_spotLight2);
	g_pObj4->Update(delta, g_Light1, g_spotLight3);

	g_pLight->Update(delta);

	g_pSpotLight1->Update(delta);
	g_pSpotLight2->Update(delta);
	g_pSpotLight3->Update(delta);
	GL_Display();
}

//----------------------------------------------------------------------------

void Win_Keyboard( unsigned char key, int x, int y )
{
    switch ( key ) {
	case  SPACE_KEY:

		break;
//----------------------------------------------------------------------------
// Part 2 : for single light source
	case 99: // c key
		g_spotLight1.diffuse=0;
		g_spotLight1.specular = 0;
		//g_pSpotLight1->SetColor(0);
		break;
	case 100: // d key
		g_spotLight1.diffuse = g_fSpotLightI;
		g_spotLight1.specular = g_fSpotLightI;
		//g_pSpotLight1->SetColor(g_fSpotLightI);
		break;
	case 65: // A key
	case 97: // a key
		g_bAutoRotating = !g_bAutoRotating;
		break;
	case 82: // R key
		if( g_fLightR <= 0.95f ) g_fLightR += 0.05f;
		g_Light1.diffuse.x = g_fLightR;
		g_pLight->SetColor(g_Light1.diffuse);
		break;
	case 114: // r key
		if( g_fLightR >= 0.05f ) g_fLightR -= 0.05f;
		g_Light1.diffuse.x = g_fLightR;
		g_pLight->SetColor(g_Light1.diffuse);

		break;
	case 71: // G key
		if( g_fLightG <= 0.95f ) g_fLightG += 0.05f;
		g_Light1.diffuse.y = g_fLightG;
		g_pLight->SetColor(g_Light1.diffuse);

		break;
	case 103: // g key
		if( g_fLightG >= 0.05f ) g_fLightG -= 0.05f;
		g_Light1.diffuse.y = g_fLightG;
		g_pLight->SetColor(g_Light1.diffuse);
		g_pSpotLight1->SetColor(g_fSpotLightI);

		break;
	case 66: // B key
		if( g_fLightB <= 0.95f ) g_fLightB += 0.05f;
		g_Light1.diffuse.z = g_fLightB;
		g_pLight->SetColor(g_Light1.diffuse);

		break;
	case 98: // b key
		if( g_fLightB >= 0.05f ) g_fLightB -= 0.05f;
		g_Light1.diffuse.z = g_fLightB;
		g_pLight->SetColor(g_Light1.diffuse);

		break;
//----------------------------------------------------------------------------
    case 033:
		glutIdleFunc( NULL );
		delete g_pCube;
		delete g_pSphere;
		delete g_pChecker;
		delete g_pLight;
		delete g_pSpotLight1;
		delete g_pSpotLight2;
		delete g_pSpotLight3;

		delete g_LeftWall;
		CCamera::getInstance()->destroyInstance();
		for (int i = 0; i < 4; i++) delete g_p2DBtn[i];

        exit( EXIT_SUCCESS );
        break;
    }
}
inline void ScreenToUICoordinate(int x, int y, vec2 &pt)
{
	pt.x = 2.0f*(float)x / SCREEN_WIDTH - 1.0f;
	pt.y = 2.0f*(float)(SCREEN_HEIGHT - y) / SCREEN_HEIGHT - 1.0f;
}
//----------------------------------------------------------------------------
void Win_Mouse(int button, int state, int x, int y) {
	vec2 pt;
	switch(button) {
		case GLUT_LEFT_BUTTON:   // �ثe���U���O�ƹ�����
			if (state == GLUT_DOWN) {
				ScreenToUICoordinate(x, y, pt);
				if (g_p2DBtn[0]->OnTouches(pt)) {
					if (g_p2DBtn[0]->getButtonStatus()) {
						g_spotLight1.diffuse = 0;
						g_spotLight1.specular = 0;
					} 
					else {
						g_spotLight1.diffuse = g_fSpotLightI;
						g_spotLight1.specular = g_fSpotLightI;
					}
				}
				if (g_p2DBtn[1]->OnTouches(pt)) {
					if (g_p2DBtn[1]->getButtonStatus()) {
						g_spotLight3.diffuse = 0;
						g_spotLight3.specular = 0;
					}
					else {
						g_spotLight3.diffuse = g_fSpotLightI3;
						g_spotLight3.specular = g_fSpotLightI3;
					}
				}
				if (g_p2DBtn[2]->OnTouches(pt)) {
					if (g_p2DBtn[2]->getButtonStatus()) {
						g_spotLight2.diffuse = 0;
						g_spotLight2.specular = 0;
					}
					else {
						g_spotLight2.diffuse = g_fSpotLightI2;
						g_spotLight2.specular = g_fSpotLightI2;
					}
				}
				if (g_p2DBtn[3]->OnTouches(pt)) {
					if (g_p2DBtn[3]->getButtonStatus()) {
						g_bAutoRotating = !g_bAutoRotating;
					}
					else {
						g_bAutoRotating = !g_bAutoRotating;
					}
				}
			}
			break;
		case GLUT_MIDDLE_BUTTON:  // �ثe���U���O�ƹ����� �A���� Y �b
			//if ( state == GLUT_DOWN ) ; 
			break;
		case GLUT_RIGHT_BUTTON:   // �ثe���U���O�ƹ��k��
			//if ( state == GLUT_DOWN ) ;
			break;
		default:
			break;
	} 
}
//----------------------------------------------------------------------------
void Win_SpecialKeyboard(int key, int x, int y) {
	auto camera = CCamera::getInstance();
	vec4 camPos = camera->getViewPos();
	float cameraSpeed = 0.05f;
	vec4 localAt= camera->getAt();
	
	vec4 up = camera->getUp();
	switch(key) {
		case GLUT_KEY_UP:		// �ثe���U���O�V����V��
			printf("eye:%f\n", eye.x);
			if (eye.x >= -8.0f && eye.x <= 8.0f && eye.z>=-8.0f && eye.z<=8.0f) {
				eye = eye + cameraSpeed*normalize(vec4(at.x, 0, at.z, 0));
			}
			else {
				eye = eye - 2 * cameraSpeed*normalize(vec4(at.x, 0, at.z, 0));
			}
			camera->updateViewLookAt(eye, eye +at);

			break;
		case GLUT_KEY_DOWN:	// �ثe���U���O�V�k��V��
			if (eye.x >= -8.0f && eye.x <= 8.0f && eye.z >= -8.0f && eye.z <= 8.0f) {

				eye = eye - cameraSpeed*normalize(vec4(at.x, 0, at.z, 0));
			}
			else {
				eye = eye + 2 * cameraSpeed*normalize(vec4(at.x, 0, at.z, 0));
			}
			camera->updateViewLookAt(eye, eye + at);
			break;
		case GLUT_KEY_LEFT:		// �ثe���U���O�V����V��
			if (eye.x >= -8.0f && eye.x <= 8.0f && eye.z >= -8.0f && eye.z <= 8.0f) {

				eye = eye + cameraSpeed*normalize(vec4(sin(g_fTheta)*sin(g_fPhi + M_PI / 2), 0, sin(g_fTheta)*cos(g_fPhi + M_PI / 2), 0));
			}
			else {
				eye = eye - 2 * cameraSpeed*normalize(vec4(sin(g_fTheta)*sin(g_fPhi + M_PI / 2), 0, sin(g_fTheta)*cos(g_fPhi + M_PI / 2), 0));

			}
			camera->updateViewLookAt(eye, eye + at);

			break;
		case GLUT_KEY_RIGHT:		// �ثe���U���O�V����V��
			if (eye.x >= -8.0f && eye.x <= 8.0f && eye.z >= -8.0f && eye.z <= 8.0f) {

				eye = eye - cameraSpeed*normalize(vec4(sin(g_fTheta)*sin(g_fPhi + M_PI / 2), 0, sin(g_fTheta)*cos(g_fPhi + M_PI / 2), 0));
			}
			else {
				eye = eye + 2 * cameraSpeed*normalize(vec4(sin(g_fTheta)*sin(g_fPhi + M_PI / 2), 0, sin(g_fTheta)*cos(g_fPhi + M_PI / 2), 0));

			}
			camera->updateViewLookAt(eye, eye + at);

			break;
		default:
			break;
	}
}

//----------------------------------------------------------------------------
// The passive motion callback for a window is called when the mouse moves within the window while no mouse buttons are pressed.
void Win_PassiveMotion(int x, int y) {

	g_fPhi = (float)-M_PI*(x - HALF_SIZE)/(HALF_SIZE); // �ഫ�� g_fPhi ���� -PI �� PI ���� (-180 ~ 180 ����)
	g_fTheta = (float)M_PI*(float)y/SCREEN_SIZE;
	auto camera = CCamera::getInstance();
	point4 _at;
	_at= point4(g_fRadius*sin(g_fTheta)*sin(g_fPhi), g_fRadius*cos(g_fTheta), g_fRadius*sin(g_fTheta)*cos(g_fPhi), 1.0f);
	at = _at;	
	camera->updateViewLookAt(eye, eye+ _at);
}

// The motion callback for a window is called when the mouse moves within the window while one or more mouse buttons are pressed.
void Win_MouseMotion(int x, int y) {
	g_fPhi = (float)-M_PI*(x - HALF_SIZE)/(HALF_SIZE);  // �ഫ�� g_fPhi ���� -PI �� PI ���� (-180 ~ 180 ����)
	g_fTheta = (float)M_PI*(float)y/SCREEN_SIZE;;
	//point4  
	//	eye= point4(g_fRadius*sin(g_fTheta)*sin(g_fPhi), g_fRadius*cos(g_fTheta), g_fRadius*sin(g_fTheta)*cos(g_fPhi), 1.0f);
	//point4 
		//at = normalize(eye);
	at = point4(g_fRadius*sin(g_fTheta)*sin(g_fPhi), g_fRadius*cos(g_fTheta), g_fRadius*sin(g_fTheta)*cos(g_fPhi), 1.0f);

	auto camera = CCamera::getInstance();
	camera->updateViewLookAt(eye, eye + at);
}
//----------------------------------------------------------------------------
void GL_Reshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h);
	glClearColor( 0.0, 0.0, 0.0, 1.0 ); // black background
	glEnable(GL_DEPTH_TEST);
}

//----------------------------------------------------------------------------

int main( int argc, char **argv )
{
    
	glutInit(&argc, argv);
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowSize( SCREEN_SIZE, SCREEN_SIZE );

	// If you use freeglut the two lines of code can be added to your application 
	glutInitContextVersion( 3, 2 );
	glutInitContextProfile( GLUT_CORE_PROFILE );

    glutCreateWindow("Shading Example 3");

	// The glewExperimental global switch can be turned on by setting it to GL_TRUE before calling glewInit(), 
	// which ensures that all extensions with valid entry points will be exposed.
	glewExperimental = GL_TRUE; 
    glewInit();  

    init();

	glutMouseFunc(Win_Mouse);
	glutMotionFunc(Win_MouseMotion);
	glutPassiveMotionFunc(Win_PassiveMotion);  
    glutKeyboardFunc( Win_Keyboard );	// �B�z ASCI ����p A�Ba�BESC ��...����
	glutSpecialFunc( Win_SpecialKeyboard);	// �B�z NON-ASCI ����p F1�BHome�B��V��...����
    glutDisplayFunc( GL_Display );
	glutReshapeFunc( GL_Reshape );
	glutIdleFunc( IdleProcess );
	
    glutMainLoop();
    return 0;
}
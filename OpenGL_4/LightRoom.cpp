// Gouraud shading with a single light source at (4, 4, 0);
// 與 Exmaple4 相同, Light 的參數改成以結構的方式傳遞
// 同時實現 Per Pixel Lighting、Nonphotorealistic Shading  與 Silhouette
//
// 執行前的準備工作
// 關閉 CShape.h 中的 #define LIGHTING_WITHCPU 
// 開啟 CShape.h 中的 #define LIGHTING_WITHGPU 
// 關閉 CShape.h 中的 #define PERVERTEX_LIGHTING 
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
#define SCREEN_SIZE 700
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
	45.0f,	// spotCutoff;	// (range: [0.0, 90.0], 180.0)  spot 的照明範圍
	1.0f	,	// spotCosCutoff; // (range: [1.0,0.0],-1.0), 照明方向與被照明點之間的角度取 cos 後, cut off 的值
	1	,	// constantAttenuation	(a + bd + cd^2)^-1 中的 a, d 為光源到被照明點的距離
	0	,	// linearAttenuation	    (a + bd + cd^2)^-1 中的 b
	0		// quadraticAttenuation (a + bd + cd^2)^-1 中的 c
};
//spotlight1 red light

bool g_bLight1On = true;
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
	vec3(0.0f, 0.0f, 0.0f),		  // spotDirection，需重新計算
	1.0f,	// spotExponent(parameter e); cos^(e)(phi) 
	1.0f,	// spotCutoff;	// (range: [0.0, 90.0], 180.0)  spot 的照明範圍
	0.5f,	// spotCosCutoff = cos(spotCutoff) ; spot 的照明範圍取 cos
	0.001f,	// constantAttenuation	(a + bd + cd^2)^-1 中的 a, d 為光源到被照明點的距離
	0.001f,	// linearAttenuation	    (a + bd + cd^2)^-1 中的 b
	0	// quadraticAttenuation (a + bd + cd^2)^-1 中的 c
};
vec4 g_fSpotLightI(g_fSpotLightR, g_fSpotLightG, g_fSpotLightB, 1.0f);
CLineSegment *g_LightLine;
//spotlight2 Blue Light

bool g_bLight2On = true;
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
	vec3(0.0f, 0.0f, 0.0f),		  // spotDirection，需重新計算
	1.0f,	// spotExponent(parameter e); cos^(e)(phi) 
	10.0f,	// spotCutoff;	// (range: [0.0, 90.0], 180.0)  spot 的照明範圍
	0.5f,	// spotCosCutoff = cos(spotCutoff) ; spot 的照明範圍取 cos
	0.5f,	// constantAttenuation	(a + bd + cd^2)^-1 中的 a, d 為光源到被照明點的距離
	0,	// linearAttenuation	    (a + bd + cd^2)^-1 中的 b
	0	// quadraticAttenuation (a + bd + cd^2)^-1 中的 c
};
vec4 g_fSpotLightI2(g_fSpotLightR2, g_fSpotLightG2, g_fSpotLightB2, 1.0f);
CLineSegment *g_LightLine2;
//spotlight3 

bool g_bLight3On = true;
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
	vec3(0.0f, 0.0f, 0.0f),		  // spotDirection，需重新計算
	1.0f,	// spotExponent(parameter e); cos^(e)(phi) 
	10.0f,	// spotCutoff;	// (range: [0.0, 90.0], 180.0)  spot 的照明範圍
	0.5f,	// spotCosCutoff = cos(spotCutoff) ; spot 的照明範圍取 cos
	0.5f,	// constantAttenuation	(a + bd + cd^2)^-1 中的 a, d 為光源到被照明點的距離
	0,	// linearAttenuation	    (a + bd + cd^2)^-1 中的 b
	0	// quadraticAttenuation (a + bd + cd^2)^-1 中的 c
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
// 函式的原型宣告
extern void IdleProcess();

void init( void )
{
	mat4 mxT,mxS;
	
	vec4 vT, vColor;
	vec3 vS;
	// 產生所需之 Model View 與 Projection Matrix

	at= point4(sin(g_fTheta)*sin(g_fPhi), cos(g_fTheta), sin(g_fTheta)*cos(g_fPhi), 1.0f);
	eye=point4(0.0f, 5.0f, 0.0f, 1.0f);

	auto camera = CCamera::create();
	camera->updateViewLookAt(eye, eye+at);
	camera->updatePerspective(60.0, (GLfloat)SCREEN_SIZE / (GLfloat)SCREEN_SIZE, 1.0, 1000.0);

	// 產生物件的實體
	g_pChecker = new CChecker(GRID_SIZE);
// Part 3 : materials
#ifdef SETTING_MATERIALS
	g_pChecker->setMaterials(vec4(0.15f,0.15f,0.15f,1.0f), vec4(0, 0.85f, 0, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	g_pChecker->	setKaKdKsShini(0, 0.8f, 0.5f, 1);
#endif
	g_pChecker->setShadingMode(GOURAUD_SHADING);
	g_pChecker->setShader();

	vT.x = -10.0f; vT.y = 10.0f; vT.z = 0;
	mxT = Translate(vT);
	g_LeftWall = new CQuad;
	g_LeftWall->setMaterials(vec4(0.15f, 0.15f, 0.15f, 1.0f), vec4(0, 0.85f, 0, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	g_LeftWall->setShadingMode(GOURAUD_SHADING);
	g_LeftWall->setShader();
	g_LeftWall->setColor(vec4(0.6f));
	g_LeftWall->setTRSMatrix(mxT*RotateZ(-90.0f)*Scale(20.0f, 1, 20.0f));
	g_LeftWall->setKaKdKsShini(0, 0.8f, 0.5f, 1);

	vT.x = 10.0f; vT.y = 10.0f; vT.z = 0;
	mxT = Translate(vT);
	g_RightWall = new CQuad;
	g_RightWall->setMaterials(vec4(0.15f, 0.15f, 0.15f, 1.0f), vec4(0, 0.85f, 0, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	g_RightWall->setShadingMode(GOURAUD_SHADING);
	g_RightWall->setShader();
	g_RightWall->setColor(vec4(0.6f));
	g_RightWall->setTRSMatrix(mxT*RotateZ(90.0f)*Scale(20.0f, 1, 20.0f));
	g_RightWall->setKaKdKsShini(0, 0.8f, 0.5f, 1);

	vT.x = 0.0f; vT.y = 10.0f; vT.z = 10.0f;
	mxT = Translate(vT);
	g_FrontWall = new CQuad;
	g_FrontWall->setMaterials(vec4(0.15f, 0.15f, 0.15f, 1.0f), vec4(0, 0.85f, 0, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	g_FrontWall->setShadingMode(GOURAUD_SHADING);
	g_FrontWall->setShader();
	g_FrontWall->setColor(vec4(0.6f));
	g_FrontWall->setTRSMatrix(mxT*RotateX(-90.0f)*Scale(20.0f, 1, 20.0f));
	g_FrontWall->setKaKdKsShini(0, 0.8f, 0.5f, 1);

	vT.x = 0.0f; vT.y = 10.0f; vT.z = -10.0f;
	
	mxT = Translate(vT);
	g_BackWall = new CQuad;
	g_BackWall->setMaterials(vec4(0.15f, 0.15f, 0.15f, 1.0f), vec4(0, 0.85f, 0, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	g_BackWall->setShadingMode(GOURAUD_SHADING);
	g_BackWall->setShader();
	g_BackWall->setColor(vec4(0.6f));
	g_BackWall->setTRSMatrix(mxT*RotateX(90.0f)*Scale(20.0f, 1, 20.0f));
	g_BackWall->setKaKdKsShini(0, 0.8f, 0.5f, 1);

//OBJ1
	g_pObj1 = new CObj("./Common/Well.obj");
	g_pObj1->setMaterials(vec4(0.15f, 0.15f, 0.15f, 1.0f), vec4(0.85f, 0.8, 0, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	g_pObj1->setKaKdKsShini(0.15f, 0.8f, 0.2f, 2);

	g_pObj1->setShader();
	vT.x = -5.5; vT.y = 2.5; vT.z = 1;
	vS.x = 1;	vS.y =1;	vS.z = 1;
	mxS = Scale(vS);
	mxT = Translate(vT);
	g_pObj1->setTRSMatrix(mxT*mxS);
	g_pObj1->setShadingMode(GOURAUD_SHADING);

	g_pObj2 = new CObj("./Common/Donut.obj");
	g_pObj2->setMaterials(vec4(0.15f, 0.15f, 0.15f, 1.0f), vec4(0.55f, 0.3, 0.2, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	g_pObj2->setKaKdKsShini(0.15f, 0.8f, 0.2f, 2);
	g_pObj2->setShader();
	vT.x = 0; vT.y = 0.5; vT.z =0;
	vS.x = 20;	vS.y = 20;	vS.z = 20;
	mxS = Scale(vS);
	mxT = Translate(vT);
	g_pObj2->setTRSMatrix(mxT*mxS);
	g_pObj2->setShadingMode(GOURAUD_SHADING);

	g_pObj3 = new CObj("./Common/Well.obj");
	g_pObj3->setMaterials(vec4(0.15f, 0.15f, 0.15f, 1.0f), vec4(0.95f, 0.3, 0.5f, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	g_pObj3->setKaKdKsShini(0.15f, 0.8f, 0.2f, 2);
	g_pObj3->setShader();
	vT.x = -0.5; vT.y = 0.5; vT.z = -5.5;
	vS.x = 1;	vS.y = 1;	vS.z = 1;
	mxS = Scale(vS);
	mxT = Translate(vT);
	g_pObj3->setTRSMatrix(mxT*mxS);
	g_pObj3->setShadingMode(GOURAUD_SHADING);

	g_pObj4 = new CObj("./Common/Well.obj");
	g_pObj4->setMaterials(vec4(0.15f, 0.15f, 0.15f, 1.0f), vec4(0.85f, 0.8f, 0.5f, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	g_pObj4->setKaKdKsShini(0.15f, 0.8f, 0.2f, 2);
	g_pObj4->setShader();
	vT.x = 5.5; vT.y = 1.0; vT.z = 3.5;
	vS.x = 1;	vS.y = 1;	vS.z = 1;
	mxS = Scale(vS);
	mxT = Translate(vT);
	g_pObj4->setTRSMatrix(mxT*mxS);
	g_pObj4->setShadingMode(GOURAUD_SHADING);

	g_pCube = new CSolidCube;
// Part 3 : materials
#ifdef SETTING_MATERIALS
	g_pCube->setMaterials(vec4(0.15f,0.15f,0.15f,1.0f), vec4(0.85f, 0, 0, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	g_pCube->setKaKdKsShini(0.15f, 0.8f, 0.2f, 2);
#endif
	g_pCube->setShader();
	// 設定 Cube
	vT.x = -0.5; vT.y = 0.5; vT.z = -5.5;
	mxT = Translate(vT);
	g_pCube->setTRSMatrix(mxT);
	g_pCube->setShadingMode(GOURAUD_SHADING);

	g_pSphere = new CSolidSphere(1, 16, 16);
// Part 3 : materials
#ifdef SETTING_MATERIALS
	g_pSphere->setMaterials(vec4(0.15f,0.15f,0.15f,1.0f), vec4(0, 0, 0.85f, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	g_pSphere->setKaKdKsShini(0.15f, 0.45f, 0.55f, 5);
#endif
	g_pSphere->setShader();
	// 設定 Sphere
	vT.x = 5.5; vT.y = 1.0; vT.z = 3.5;
	mxT = Translate(vT);
	g_pSphere->setTRSMatrix(mxT);
	g_pSphere->setShadingMode(GOURAUD_SHADING);

	// 設定 代表 Light 的 WireSphere
	g_pLight = new CWireSphere(0.25f, 6, 3);
	g_pLight->setShader();
	mxT = Translate(g_Light1.position);
	g_pLight->setTRSMatrix(mxT);
	g_pLight->setColor(g_Light1.diffuse);
#ifdef LIGHTING_WITHGPU
	g_pLight->setLightingDisable();
#endif
	//spotlight
	g_pSpotLight1 = new CWireSphere(0,0,0);
	g_pSpotLight1->setShader();
	mxT = Translate(g_spotLight1.position);
	g_pSpotLight1->setTRSMatrix(mxT);
	g_pSpotLight1->setColor(g_fSpotLightI);
	g_spotLight1.UpdateDirection();

	g_LightLine = new CLineSegment(g_spotLight1.position, g_spotLight1.spotTarget, vec4(1, 0, 0, 1));
	g_LightLine->setShader();
	//spotlight2
	g_pSpotLight2 = new CWireSphere(0, 0, 0);
	g_pSpotLight2->setShader();
	mxT = Translate(g_spotLight2.position);
	g_pSpotLight2->setTRSMatrix(mxT);
	g_pSpotLight2->setColor(g_fSpotLightI2);
	g_spotLight2.UpdateDirection();

	g_LightLine2 = new CLineSegment(g_spotLight2.position, g_spotLight2.spotTarget, vec4(1, 0, 0, 1));
	g_LightLine2->setShader();
	//spotlight3
	g_pSpotLight3 = new CWireSphere(0, 0, 0);
	g_pSpotLight3->setShader();
	mxT = Translate(g_spotLight3.position);
	g_pSpotLight3->setTRSMatrix(mxT);
	g_pSpotLight3->setColor(g_fSpotLightI3);
	g_spotLight3.UpdateDirection();

	g_LightLine3 = new CLineSegment(g_spotLight3.position, g_spotLight3.spotTarget, vec4(1, 0, 0, 1));
	g_LightLine3->setShader();
	//2D ui
	// 以下為利用平行投影產生 2D 的介面
	// 範圍在 X/Y 平面的  -1 到 1 之間，介面都放在 Z = 0 

	g_p2DBtn[0] = new C2DSprite; g_p2DBtn[0]->setShader();
	vColor.x = 1; vColor.y = 0; vColor.z = 0; g_p2DBtn[0]->setDefaultColor(vColor);
	mxS = Scale(0.1f, 0.1f, 1.0f);
	mxT = Translate(0.60f, -0.90f, 0);
	g_p2DBtn[0]->setTRSMatrix(mxT*mxS);
	g_p2DBtn[0]->setViewMatrix(g_2DView);
	g_p2DBtn[0]->setViewMatrix(g_2DProj);

	g_p2DBtn[1] = new C2DSprite; g_p2DBtn[1]->setShader();
	vColor.x = 0; vColor.y = 1; vColor.z = 0; g_p2DBtn[1]->setDefaultColor(vColor);
	mxT = Translate(0.90f, -0.90f, 0);
	g_p2DBtn[1]->setTRSMatrix(mxT*mxS);
	g_p2DBtn[1]->setViewMatrix(g_2DView);
	g_p2DBtn[1]->setViewMatrix(g_2DProj);

	g_p2DBtn[2] = new C2DSprite; g_p2DBtn[2]->setShader();
	vColor.x = 0; vColor.y = 0; vColor.z = 1; g_p2DBtn[2]->setDefaultColor(vColor);
	mxT = Translate(0.75f, -0.90f, 0);
	g_p2DBtn[2]->setTRSMatrix(mxT*mxS);
	g_p2DBtn[2]->setViewMatrix(g_2DView);
	g_p2DBtn[2]->setViewMatrix(g_2DProj);

	g_p2DBtn[3] = new C2DSprite; g_p2DBtn[3]->setShader();
	vColor.x = 1; vColor.y = 1; vColor.z = 1; g_p2DBtn[3]->setDefaultColor(vColor);
	mxT = Translate(0.45f, -0.90f, 0);
	g_p2DBtn[3]->setTRSMatrix(mxT*mxS);
	g_p2DBtn[3]->setViewMatrix(g_2DView);
	g_p2DBtn[3]->setViewMatrix(g_2DProj);

	//-------------------------------------------------------------
	
//
	// 因為本範例不會動到 Projection Matrix 所以在這裡設定一次即可
	// 就不寫在 OnFrameMove 中每次都 Check
	bool bPDirty;
	mat4 mpx = camera->getProjectionMatrix(bPDirty);
	g_pChecker->setProjectionMatrix(mpx);
	g_pCube->setProjectionMatrix(mpx);
	g_pSphere->setProjectionMatrix(mpx);
	g_pLight->setProjectionMatrix(mpx);
	g_LeftWall->setProjectionMatrix(mpx);
	g_RightWall->setProjectionMatrix(mpx);
	g_FrontWall->setProjectionMatrix(mpx);
	g_BackWall->setProjectionMatrix(mpx);
	g_pObj1->setProjectionMatrix(mpx);
	g_pObj2->setProjectionMatrix(mpx);
	g_pObj3->setProjectionMatrix(mpx);
	g_pObj4->setProjectionMatrix(mpx);

	g_pSpotLight1->setProjectionMatrix(mpx);
	g_LightLine->setProjectionMatrix(mpx);
	g_pSpotLight2->setProjectionMatrix(mpx);
	g_LightLine2->setProjectionMatrix(mpx);
	g_pSpotLight3->setProjectionMatrix(mpx);
	g_LightLine3->setProjectionMatrix(mpx);
}

//----------------------------------------------------------------------------
void GL_Display( void )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); // clear the window

	g_pChecker->draw();
	g_pLight->draw();
	g_LeftWall->draw();
	g_RightWall->draw();
	g_FrontWall->draw();
	g_BackWall->draw();
	g_pObj1->draw();
	g_pObj2->draw();
	g_pCube->draw();
	g_pSphere->draw();

	g_pSpotLight1->draw();
	g_pSpotLight2->draw();
	g_pSpotLight3->draw();
	for (int i = 0; i < 4; i++) g_p2DBtn[i]->draw();
	glutSwapBuffers();	// 交換 Frame Buffer
}

//----------------------------------------------------------------------------
// Part 2 : for single light source
void UpdateLightPosition(float dt)
{
	mat4 mxT;
	// 每秒繞 Y 軸轉 90 度
	g_fElapsedTime += dt;
	g_fLightTheta = g_fElapsedTime*(float)M_PI_2;
	if( g_fLightTheta >= (float)M_PI*2.0f ) {
		g_fLightTheta -= (float)M_PI*2.0f;
		g_fElapsedTime -= 4.0f;
	}
	g_Light1.position.x = g_fLightRadius * cosf(g_fLightTheta);
	g_Light1.position.z = g_fLightRadius * sinf(g_fLightTheta);
	mxT = Translate(g_Light1.position);
	g_pLight->setTRSMatrix(mxT);

	
}
//----------------------------------------------------------------------------

void onFrameMove(float delta)
{
	mat4 mvx;	// view matrix & projection matrix
	bool bVDirty;	// view 與 projection matrix 是否需要更新給物件
	auto camera = CCamera::getInstance();
	mvx = camera->getViewMatrix(bVDirty);
	if (bVDirty) {
		g_pChecker->setViewMatrix(mvx);
		g_pCube->setViewMatrix(mvx);
		g_pSphere->setViewMatrix(mvx);
		g_LeftWall->setViewMatrix(mvx);
		g_RightWall->setViewMatrix(mvx);
		g_FrontWall->setViewMatrix(mvx);
		g_BackWall->setViewMatrix(mvx);
		g_pLight->setViewMatrix(mvx);
		g_pObj1->setViewMatrix(mvx);
		g_pObj2->setViewMatrix(mvx);
		g_pObj3->setViewMatrix(mvx);
		g_pObj4->setViewMatrix(mvx);

		g_pSpotLight1->setViewMatrix(mvx);
		g_LightLine->setViewMatrix(mvx);
		g_pSpotLight2->setViewMatrix(mvx);
		g_LightLine2->setViewMatrix(mvx);
		g_pSpotLight3->setViewMatrix(mvx);
		g_LightLine3->setViewMatrix(mvx);
	}

	if( g_bAutoRotating ) { // Part 2 : 重新計算 Light 的位置
		UpdateLightPosition(delta);
	}


	// 如果需要重新計算時，在這邊計算每一個物件的顏色
	g_pChecker->Update(delta, g_Light1,  g_spotLight1, g_spotLight2,  g_spotLight3);
	//g_pCube->Update(delta, g_Light1, g_spotLight1);

	//g_pSphere->Update(delta, g_Light1, g_spotLight1);
	g_LeftWall->Update(delta, g_Light1);

	g_RightWall->Update(delta, g_Light1);
	g_FrontWall->Update(delta, g_Light1);
	g_BackWall->Update(delta, g_Light1);
	g_pObj2->Update(delta, g_Light1, g_spotLight1,  g_spotLight2, g_spotLight3);
	g_pObj1->Update(delta, g_Light1,  g_spotLight1);
	g_pCube->Update(delta, g_Light1, g_spotLight2);
	g_pSphere->Update(delta, g_Light1, g_spotLight3);

	g_pLight->Update(delta);

	g_pSpotLight1->Update(delta);
	g_pSpotLight2->Update(delta);
	g_pSpotLight3->Update(delta);
	GL_Display();

	//--------------------------------------------------------
	if (g_bLight1On) {
		g_spotLight1.diffuse = g_fSpotLightI;
		g_spotLight1.specular = g_fSpotLightI;
	}
	else {
		g_spotLight1.diffuse = 0;
		g_spotLight1.specular = 0;
	}
	if (g_bLight2On) {
		g_spotLight2.diffuse = g_fSpotLightI2;
		g_spotLight2.specular = g_fSpotLightI2;
	}
	else {
		g_spotLight2.diffuse = 0;
		g_spotLight2.specular = 0;
	}
	if (g_bLight3On) {
		g_spotLight3.diffuse = g_fSpotLightI3;
		g_spotLight3.specular = g_fSpotLightI3;
	}
	else {
		g_spotLight3.diffuse = 0;
		g_spotLight3.specular = 0;
	}
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
		g_bLight2On = !g_bLight2On;
		//g_pSpotLight1->setColor(0);
		break;
	case 100: // d key
		g_bLight1On = !g_bLight1On;
		//g_pSpotLight1->setColor(g_fSpotLightI);
		break;
	case 65: // A key
	case 97: // a key
		g_bAutoRotating = !g_bAutoRotating;
		break;
	case 82: // R key
		if( g_fLightR <= 0.95f ) g_fLightR += 0.05f;
		g_Light1.diffuse.x = g_fLightR;
		g_pLight->setColor(g_Light1.diffuse);
		break;
	case 114: // r key
		if( g_fLightR >= 0.05f ) g_fLightR -= 0.05f;
		g_Light1.diffuse.x = g_fLightR;
		g_pLight->setColor(g_Light1.diffuse);

		break;
	case 71: // G key
		if( g_fLightG <= 0.95f ) g_fLightG += 0.05f;
		g_Light1.diffuse.y = g_fLightG;
		g_pLight->setColor(g_Light1.diffuse);

		break;
	case 103: // g key
		if( g_fLightG >= 0.05f ) g_fLightG -= 0.05f;
		g_Light1.diffuse.y = g_fLightG;
		g_pLight->setColor(g_Light1.diffuse);
		g_pSpotLight1->setColor(g_fSpotLightI);

		break;
	case 66: // B key
		if( g_fLightB <= 0.95f ) g_fLightB += 0.05f;
		g_Light1.diffuse.z = g_fLightB;
		g_pLight->setColor(g_Light1.diffuse);

		break;
	case 98: // b key
		if( g_fLightB >= 0.05f ) g_fLightB -= 0.05f;
		g_Light1.diffuse.z = g_fLightB;
		g_pLight->setColor(g_Light1.diffuse);

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

inline void ScreenToUICoordinate(int x, int y, vec2& pt)
{
	pt.x = 2.0f * (float)x / SCREEN_SIZE - 1.0f;
	pt.y = 2.0f * (float)(SCREEN_SIZE - y) / SCREEN_SIZE - 1.0f;
}
//----------------------------------------------------------------------------
void Win_Mouse(int button, int state, int x, int y) {
	vec2 pt;
	switch(button) {
		case GLUT_LEFT_BUTTON:   // 目前按下的是滑鼠左鍵
			if (state == GLUT_DOWN) {
				ScreenToUICoordinate(x, y, pt);
				if (g_p2DBtn[0]->OnTouches(pt)) {
					g_bLight1On = !g_bLight1On;
				}
				if (g_p2DBtn[1]->OnTouches(pt)) {
					g_bLight3On = !g_bLight3On;
				}
				if (g_p2DBtn[2]->OnTouches(pt)) {
					g_bLight2On = !g_bLight2On;
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
		case GLUT_MIDDLE_BUTTON:  // 目前按下的是滑鼠中鍵 ，換成 Y 軸
			//if ( state == GLUT_DOWN ) ; 
			break;
		case GLUT_RIGHT_BUTTON:   // 目前按下的是滑鼠右鍵
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
	float cameraSpeed = 0.3f;
	vec4 localAt= camera->getAt();
	
	vec4 up = camera->getUp();
	switch(key) {
		case GLUT_KEY_UP:		// 目前按下的是向上方向鍵
			printf("eye:%f\n", eye.x);
			if (eye.x >= -8.0f && eye.x <= 8.0f && eye.z>=-8.0f && eye.z<=8.0f) {
				eye = eye + cameraSpeed*normalize(vec4(at.x, 0, at.z, 0));
			}
			else {	// 修正卡牆
				if (eye.x > 8.0f) eye.x = 8.0f;
				else if (eye.x < -8.0f) eye.x = -8.0f;
				if (eye.z > 8.0f) eye.z = 8.0f;
				else if (eye.z < -8.0f) eye.z = -8.0f;
			}
			camera->updateViewLookAt(eye, eye +at);

			break;
		case GLUT_KEY_DOWN:	// 目前按下的是向下方向鍵
			if (eye.x >= -8.0f && eye.x <= 8.0f && eye.z >= -8.0f && eye.z <= 8.0f) {

				eye = eye - cameraSpeed*normalize(vec4(at.x, 0, at.z, 0));
			}
			else {	// 修正卡牆
				if (eye.x > 8.0f) eye.x = 8.0f;
				else if (eye.x < -8.0f) eye.x = -8.0f;
				if (eye.z > 8.0f) eye.z = 8.0f;
				else if (eye.z < -8.0f) eye.z = -8.0f;
			}
			camera->updateViewLookAt(eye, eye + at);
			break;
		case GLUT_KEY_LEFT:		// 目前按下的是向左方向鍵
			if (eye.x >= -8.0f && eye.x <= 8.0f && eye.z >= -8.0f && eye.z <= 8.0f) {

				eye = eye + cameraSpeed*normalize(vec4(sin(g_fTheta)*sin(g_fPhi + M_PI / 2), 0, sin(g_fTheta)*cos(g_fPhi + M_PI / 2), 0));
			}
			else {	// 修正卡牆
				if (eye.x > 8.0f) eye.x = 8.0f;
				else if (eye.x < -8.0f) eye.x = -8.0f;
				if (eye.z > 8.0f) eye.z = 8.0f;
				else if (eye.z < -8.0f) eye.z = -8.0f;
			}
			camera->updateViewLookAt(eye, eye + at);

			break;
		case GLUT_KEY_RIGHT:		// 目前按下的是向右方向鍵
			if (eye.x >= -8.0f && eye.x <= 8.0f && eye.z >= -8.0f && eye.z <= 8.0f) {

				eye = eye - cameraSpeed*normalize(vec4(sin(g_fTheta)*sin(g_fPhi + M_PI / 2), 0, sin(g_fTheta)*cos(g_fPhi + M_PI / 2), 0));
			}
			else {	// 修正卡牆
				if (eye.x > 8.0f) eye.x = 8.0f;
				else if (eye.x < -8.0f) eye.x = -8.0f;
				if (eye.z > 8.0f) eye.z = 8.0f;
				else if (eye.z < -8.0f) eye.z = -8.0f;
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

	g_fPhi = (float)-M_PI*(x - HALF_SIZE)/(HALF_SIZE); // 轉換成 g_fPhi 介於 -PI 到 PI 之間 (-180 ~ 180 之間)
	g_fTheta = (float)M_PI*(float)y/SCREEN_SIZE;
	auto camera = CCamera::getInstance();
	point4 _at;
	_at= point4(g_fRadius*sin(g_fTheta)*sin(g_fPhi), g_fRadius*cos(g_fTheta), g_fRadius*sin(g_fTheta)*cos(g_fPhi), 1.0f);
	at = _at;	
	camera->updateViewLookAt(eye, eye+ _at);
}

// The motion callback for a window is called when the mouse moves within the window while one or more mouse buttons are pressed.
void Win_MouseMotion(int x, int y) {
	g_fPhi = (float)-M_PI*(x - HALF_SIZE)/(HALF_SIZE);  // 轉換成 g_fPhi 介於 -PI 到 PI 之間 (-180 ~ 180 之間)
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
    glutKeyboardFunc( Win_Keyboard );	// 處理 ASCI 按鍵如 A、a、ESC 鍵...等等
	glutSpecialFunc( Win_SpecialKeyboard);	// 處理 NON-ASCI 按鍵如 F1、Home、方向鍵...等等
    glutDisplayFunc( GL_Display );
	glutReshapeFunc( GL_Reshape );
	glutIdleFunc( IdleProcess );
	
    glutMainLoop();
    return 0;
}
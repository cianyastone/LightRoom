// Phong reflection model
// #define BLINN_PHONG
#version 130
in  vec4  vPosition;	// Vertex Position
in  vec3  vNormal;    // Vertex Normal
in  vec4  vVtxColor;  // Vertex Color
out vec4  vColor;     // ��X���C��

uniform mat4  ModelView;   // Model View Matrix
uniform mat4  Projection;  // Projection Matrix
uniform vec4  vObjectColor;    // �N���󪺳�@�C��

// �W�������P  vsLighting_CPU.glsl �ۦP
// �H�U���s�W������
uniform vec4  LightInView;        // Light's position in View Space
uniform vec4  AmbientProduct;  // light's ambient  �P Object's ambient  �P ka �����n
uniform vec4  DiffuseProduct;  // light's diffuse  �P Object's diffuse  �P kd �����n
uniform vec4  SpecularProduct; // light's specular �P Object's specular �P ks �����n
uniform float fShininess;
//2

uniform vec4  LightInView2;        // Light's position in View Space
uniform vec4  AmbientProduct2;  // light's ambient  �P Object's ambient  �P ka �����n
uniform vec4  DiffuseProduct2;  // light's diffuse  �P Object's diffuse  �P kd �����n
uniform vec4  SpecularProduct2; // light's specular �P Object's specular �P ks �����n
//3
uniform vec4  LightInView3;        // Light's position in View Space
uniform vec4  AmbientProduct3;  // light's ambient  �P Object's ambient  �P ka �����n
uniform vec4  DiffuseProduct3;  // light's diffuse  �P Object's diffuse  �P kd �����n
uniform vec4  SpecularProduct3; // light's specular �P Object's specular �P ks �����n
//4
uniform vec4  LightInView4;        // Light's position in View Space
uniform vec4  AmbientProduct4;  // light's ambient  �P Object's ambient  �P ka �����n
uniform vec4  DiffuseProduct4;  // light's diffuse  �P Object's diffuse  �P kd �����n
uniform vec4  SpecularProduct4; // light's specular �P Object's specular �P ks �����n

uniform int   iLighting;

void main()
{
	// ���ŧi diffuse �P specular
    vec4 diffuse = vec4(0.0,0.0,0.0,1.0);
	vec4 specular = vec4(0.0,0.0,0.0,1.0);
	vec4 diffuse2 = vec4(0.0,0.0,0.0,1.0);
	vec4 specular2 = vec4(0.0,0.0,0.0,1.0);
	vec4 diffuse3 = vec4(0.0,0.0,0.0,1.0);
	vec4 specular3 = vec4(0.0,0.0,0.0,1.0);
	vec4 diffuse4 = vec4(0.0,0.0,0.0,1.0);
	vec4 specular4 = vec4(0.0,0.0,0.0,1.0);
	if( iLighting != 1 ) {
		gl_Position = Projection * ModelView * vPosition;	// �p�� vPosition ��v�᪺��m
		vColor = vObjectColor ;
	}
	else {	
		// 1. �N�I��������Y�y�Шt�ΡA�����H m_mxMVFinal �p��b�@�ɮy�Ъ������m
		vec4 vPosInView = ModelView * vPosition;  

		// 2. �N���� Normal ������Y�y�Шt�ΡA���ন���V�q
		// �̫�@�ӸɤW 0 �A���ݭn�첾�q�A�_�h�X�Ӫ� Normal �|����
		// �P�ɧQ�� normalize �ন���V�q

// �H�U���Ω�p��磌��i�� non-uniform scale �ɡA���� normal ���ץ��p��
//		mat3 ITModelView = transpose(inverse(mat3(ModelView)); 
//		vec3 vN = normalize(ITModelView * vNormal); 

		vec3 vN = normalize(ModelView * vec4(vNormal, 0.0)).xyz; 

		// 3. �p�� Ambient color : Ia = AmbientProduct = Ka * Material.ambient * La = 
		vec4 ambient = AmbientProduct ; // m_sMaterial.ka * m_sMaterial.ambient * vLightI;

		// 4. �ǤJ�� Light ����m�w�g�b���Y�y��
		vec3 vL = normalize(vec3(LightInView.xyz - vPosInView.xyz)); // normalize light vector
		//test2
		vec3 vL2 = normalize(vec3(LightInView2.xyz - vPosInView.xyz)); // normalize light vector
		vec3 vL3 = normalize(vec3(LightInView3.xyz - vPosInView.xyz)); // normalize light vector
		vec3 vL4 = normalize(vec3(LightInView4.xyz - vPosInView.xyz)); // normalize light vector

		// 5. �p�� L dot N
		float fLdotN = vL.x*vN.x + vL.y*vN.y + vL.z*vN.z;
		
		if( fLdotN >= 0 ) { // ���I�Q�����Ө�~�ݭn�p��
			// Diffuse Color : Id = Kd * Material.diffuse * Ld * (L dot N)
			diffuse = fLdotN * DiffuseProduct; 

			// Specular color
			// Method 1: Phone Model
			// �p�� View Vector
			// �ثe�w�g�H���Y�y�Ь���¦, �ҥH View ����m�N�O (0,0,0), �ҥH�I��m���t�V�N�O View Dir
			vec3 vV = -normalize(vPosInView.xyz);

#ifndef BLINN_PHONG
			//�p�� Light �� �Ϯg�� vRefL = 2 * fLdotN * vN - L
			// �P�ɧQ�� normalize �ন���V�q
			vec3 vRefL = normalize(2.0f * (fLdotN) * vN - vL);

			//   �p��  vReflectedL dot View
			float RdotV = vRefL.x*vV.x + vRefL.y*vV.y + vRefL.z*vV.z;

			// Specular Color : Is = Ks * Material.specular * Ls * (R dot V)^Shininess;
			if( RdotV > 0 ) specular = SpecularProduct * pow(RdotV, fShininess); 
#else

			// Blinn-Phong Reflection Model
			vec3 vH = normalize( vL + vV ); 
			float HdotN = vH.x*vN.x + vH.y*vN.y + vH.z*vN.z; //   �p��  H dot N
			if( HdotN > 0 ) specular = SpecularProduct * pow(HdotN, fShininess); 
#endif
		}
		//test2
		float fLdotN2 = vL2.x*vN.x + vL2.y*vN.y + vL2.z*vN.z;
		if( fLdotN2 >= 0 ) { // ���I�Q�����Ө�~�ݭn�p��
			// Diffuse Color : Id = Kd * Material.diffuse * Ld * (L dot N)
			diffuse2 = fLdotN2 * DiffuseProduct2; 

			// Specular color
			// Method 1: Phone Model
			// �p�� View Vector
			// �ثe�w�g�H���Y�y�Ь���¦, �ҥH View ����m�N�O (0,0,0), �ҥH�I��m���t�V�N�O View Dir
			vec3 vV2 = -normalize(vPosInView.xyz);

#ifndef BLINN_PHONG
			//�p�� Light �� �Ϯg�� vRefL = 2 * fLdotN * vN - L
			// �P�ɧQ�� normalize �ন���V�q
			vec3 vRefL2 = normalize(2.0f * (fLdotN2) * vN - vL2);

			//   �p��  vReflectedL dot View
			float RdotV2 = vRefL2.x*vV2.x + vRefL2.y*vV2.y + vRefL2.z*vV2.z;

			// Specular Color : Is = Ks * Material.specular * Ls * (R dot V)^Shininess;
			if( RdotV2 > 0 ) specular2 = SpecularProduct2 * pow(RdotV2, fShininess); 
#else

			// Blinn-Phong Reflection Model
			vec3 vH2 = normalize( vL2 + vV2 ); 
			float HdotN2 = vH2.x*vN.x + vH2.y*vN.y + vH2.z*vN.z; //   �p��  H dot N
			if( HdotN2 > 0 ) specular2 = SpecularProduct2 * pow(HdotN2, fShininess); 
#endif
		}
		//test3
		float fLdotN3 = vL3.x*vN.x + vL3.y*vN.y + vL3.z*vN.z;
		if( fLdotN3 >= 0 ) { // ���I�Q�����Ө�~�ݭn�p��
			// Diffuse Color : Id = Kd * Material.diffuse * Ld * (L dot N)
			diffuse3 = fLdotN3 * DiffuseProduct3; 

			// Specular color
			// Method 1: Phone Model
			// �p�� View Vector
			// �ثe�w�g�H���Y�y�Ь���¦, �ҥH View ����m�N�O (0,0,0), �ҥH�I��m���t�V�N�O View Dir
			vec3 vV3 = -normalize(vPosInView.xyz);

#ifndef BLINN_PHONG
			//�p�� Light �� �Ϯg�� vRefL = 2 * fLdotN * vN - L
			// �P�ɧQ�� normalize �ন���V�q
			vec3 vRefL3 = normalize(2.0f * (fLdotN3) * vN - vL3);

			//   �p��  vReflectedL dot View
			float RdotV3 = vRefL3.x*vV3.x + vRefL3.y*vV3.y + vRefL3.z*vV3.z;

			// Specular Color : Is = Ks * Material.specular * Ls * (R dot V)^Shininess;
			if( RdotV3 > 0 ) specular3 = SpecularProduct3 * pow(RdotV3, fShininess); 
#else

			// Blinn-Phong Reflection Model
			vec3 vH3 = normalize( vL3 + vV3 ); 
			float HdotN3 = vH3.x*vN.x + vH3.y*vN.y + vH3.z*vN.z; //   �p��  H dot N
			if( HdotN3 > 0 ) specular3 = SpecularProduct3 * pow(HdotN3, fShininess); 
#endif
		}
		//test4
		float fLdotN4 = vL4.x*vN.x + vL4.y*vN.y + vL4.z*vN.z;
		if( fLdotN4 >= 0 ) { // ���I�Q�����Ө�~�ݭn�p��
			// Diffuse Color : Id = Kd * Material.diffuse * Ld * (L dot N)
			diffuse4 = fLdotN4 * DiffuseProduct4; 

			// Specular color
			// Method 1: Phone Model
			// �p�� View Vector
			// �ثe�w�g�H���Y�y�Ь���¦, �ҥH View ����m�N�O (0,0,0), �ҥH�I��m���t�V�N�O View Dir
			vec3 vV4 = -normalize(vPosInView.xyz);

#ifndef BLINN_PHONG
			//�p�� Light �� �Ϯg�� vRefL = 2 * fLdotN * vN - L
			// �P�ɧQ�� normalize �ন���V�q
			vec3 vRefL4 = normalize(2.0f * (fLdotN4) * vN - vL4);

			//   �p��  vReflectedL dot View
			float RdotV4 = vRefL4.x*vV4.x + vRefL4.y*vV4.y + vRefL4.z*vV4.z;

			// Specular Color : Is = Ks * Material.specular * Ls * (R dot V)^Shininess;
			if( RdotV4 > 0 ) specular4 = SpecularProduct4 * pow(RdotV4, fShininess); 
#else

			// Blinn-Phong Reflection Model
			vec3 vH4 = normalize( vL4 + vV4 ); 
			float HdotN4 = vH4.x*vN.x + vH4.y*vN.y + vH4.z*vN.z; //   �p��  H dot N
			if( HdotN4 > 0 ) specular4 = SpecularProduct4 * pow(HdotN4, fShininess); 
#endif
		}
		gl_Position = Projection * vPosInView;	// �p�� vPosition ��v�᪺��m
		vColor = ambient + diffuse  + diffuse2 + specular2+ diffuse3 + specular3+ diffuse4 + specular4;  // �p���C�� ambient + diffuse + specular;
		vColor.w = 1.0;	// �]�w alpha �� 1.0
		// vColor = vec4((ambient + diffuse + specular).xyz, 1.0);
	}	
}

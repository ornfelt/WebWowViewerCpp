//
// Created by deamon on 09.07.18.
//

#ifndef AWEBWOWVIEWERCPP_UNIFORMBUFFERSTRUCTURES_H
#def  ine AWEBWOWVIEWERCPP_UNIFORMBUFFERSTRUCTURES_H

//Uniform buffer structures
#include <mathfu/glsl_mappings.h>
#include "mathfu/glsl_mappings.h"

#define MAX_MATRIX_NUM 220

struct sceneWideBlockVSPS {
    mathfu::mat4 uLookAtMat;
    mathfu::mat4 uPMatrix;
    mathfu::vec4_packed uViewUp;
    mathfu::vec4_packed uInteriorSunDir;
};


struct exteriorLightParams {
    mathfu::vec4_packed uExteriorAmbientColor;
    mathfu::vec4_packed uExteriorDirectColor;
    mathfu::vec4_packed uExteriorDirectColorDir;
};

struct interiorLightParams {
    mathfu::vec4_packed uInteriorAmbientColor;
};

//M2 Vertex buffer formats
struct modelWideBlockVS {
    mathfu::mat4 uPlacementMat;
    mathfu::mat4 uBoneMatrixes[MAX_MATRIX_NUM];
};

struct meshWideBlockVS {
    int VertexShader;
    int IsAffectedByLight;
    int isSkyBox;
    int padding;
    mathfu::vec4_packed Color_Transparency;

    mathfu::mat4 uTextMat[2];
};

//M2 Pixel buffer formats
struct modelWideBlockPS {
    mathfu::vec4_packed uFogStartAndFogEnd;
};

struct LocalLight
{
    mathfu::vec4_packed color;
    mathfu::vec4_packed position;
    mathfu::vec4_packed attenuation;
};

struct meshWideBlockPS {
    int PixelShader;
    int UnFogged;
    int IsAffectedByLight;
    int LightCount;
    mathfu::vec4_packed uFogColorAndAlphaTest;
    LocalLight pc_lights[4];
    mathfu::vec4_packed uPcColor;
};

//WMO VertexBuffer format
struct wmoModelWideBlockVS {
    mathfu::mat4 uPlacementMat;
};

struct wmoMeshWideBlockVS {
    int VertexShader;
    int UseLitColor;
    int padding[2];
};


struct wmoMeshWideBlockPS {
//    PACK({struct
    mathfu::vec4_packed uFogStartAndFogEndAndIsBatchA;
    int UseLitColor;
    int EnableAlpha;
    int PixelShader;
    int padding;
    mathfu::vec4_packed FogColor_AlphaTest;
//    )}
};

//ADT

struct adtMeshWideBlockVS {
    mathfu::vec4 uPos;
};

struct adtModelWideBlockPS {
    mathfu::vec4_packed uFogStartAndFogEnd;
    mathfu::vec4_packed uFogColor;
};

struct adtMeshWideBlockPS {
    float uHeightScale[4];
    float uHeightOffset[4];
};


struct bbModelWideBlockVS {
    mathfu::mat4 uPlacementMat;

    mathfu::vec4_packed uBBScale;
    mathfu::vec4_packed uBBCenter;
    mathfu::vec4_packed uColor;
};


#endif //AWEBWOWVIEWERCPP_UNIFORMBUFFERSTRUCTURES_H

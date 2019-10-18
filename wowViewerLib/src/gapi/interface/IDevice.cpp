//
// Created by Deamon on 8/25/2018.
//

#include <iostream>
#include <algorithm>
#include "IDevice.h"


bool IDevice::sortMeshes(const HGMesh &a, const HGMesh &b) {
	auto* pA = a.get();
	auto* pB = b.get();

    if (pA->getIsTransparent() > pB->getIsTransparent()) {
        return false;
    }
    if (pA->getIsTransparent() < pB->getIsTransparent()) {
        return true;
    }

    if (pA->getMeshType() > pB->getMeshType()) {
        return false;
    }
    if (pA->getMeshType() < pB->getMeshType()) {
        return true;
    }

    if (pA->m_renderOrder != pB->m_renderOrder ) {
        if (!pA->getIsTransparent()) {
            return pA->m_renderOrder < pB->m_renderOrder;
        } else {
            return pA->m_renderOrder > pB->m_renderOrder;
        }
    }

    if (pA->m_isSkyBox > pB->m_isSkyBox) {
        return true;
    }
    if (pA->m_isSkyBox < pB->m_isSkyBox) {
        return false;
    }

    if (pA->getMeshType() == MeshType::eM2Mesh && pA->getIsTransparent() && pB->getIsTransparent()) {
        if (pA->m_priorityPlane != pB->m_priorityPlane) {
            return pB->m_priorityPlane > pA->m_priorityPlane;
        }

        if (pA->m_sortDistance > pB->m_sortDistance) {
            return true;
        }
        if (pA->m_sortDistance < pB->m_sortDistance) {
            return false;
        }

        if (pA->m_m2Object > pB->m_m2Object) {
            return true;
        }
        if (pA->m_m2Object < pB->m_m2Object) {
            return false;
        }

        if (pB->m_layer != pA->m_layer) {
            return pB->m_layer < pA->m_layer;
        }
    }

    if (pA->getMeshType() == MeshType::eParticleMesh && pB->getMeshType() == MeshType::eParticleMesh) {
        if (pA->m_priorityPlane != pB->m_priorityPlane) {
            return pB->m_priorityPlane > pA->m_priorityPlane;
        }

        if (pA->m_sortDistance > pB->m_sortDistance) {
            return true;
        }
        if (pA->m_sortDistance < pB->m_sortDistance) {
            return false;
        }
    }

    if (pA->m_bindings != pB->m_bindings) {
        return pA->m_bindings > pB->m_bindings;
    }

    if (pA->getGxBlendMode() != pB->getGxBlendMode()) {
        return pA->getGxBlendMode() < pB->getGxBlendMode();
    }

    int minTextureCount = std::min(pA->m_textureCount, pB->m_textureCount);
    for (int i = 0; i < minTextureCount; i++) {
        if (pA->m_texture[i] != pB->m_texture[i]) {
            return pA->m_texture[i] < pB->m_texture[i];
        }
    }

    if (pA->m_textureCount != pB->m_textureCount) {
        return pA->m_textureCount < pB->m_textureCount;
    }

    if (pA->m_start != pB->m_start) {
        return pA->m_start < pB->m_start;
    }
    if (pA->m_end != pB->m_end) {
        return pA->m_end < pB->m_end;
    }


    return a > b;
}

int compressedTexturesSupported = -1;
int anisFiltrationSupported = -1;

#ifdef __EMSCRIPTEN__
#include <emscripten/html5.h>
#endif

bool IDevice::getIsCompressedTexturesSupported() {
#ifdef __EMSCRIPTEN__
    if (compressedTexturesSupported == -1){
        if (emscripten_webgl_enable_extension(emscripten_webgl_get_current_context(), "WEBGL_compressed_texture_s3tc") == EM_TRUE) {
            compressedTexturesSupported = 1;
        } else {
            compressedTexturesSupported = 0;
        }
    }
      return compressedTexturesSupported == 1;

#else
    return true;
#endif
}
bool IDevice::getIsAnisFiltrationSupported() {
#ifdef __EMSCRIPTEN__
    if (anisFiltrationSupported == -1){
        if (emscripten_webgl_enable_extension(emscripten_webgl_get_current_context(), "EXT_texture_filter_anisotropic ") == EM_TRUE) {
            anisFiltrationSupported = 1;
        } else {
            anisFiltrationSupported = 0;
        }
    }
      return anisFiltrationSupported == 1;

#else
    return true;
#endif
}



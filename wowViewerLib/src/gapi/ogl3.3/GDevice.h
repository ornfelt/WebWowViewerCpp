//
// Created by deamon on 05.06.18.
//

#ifndef WEBWOWVIEWERCPP_GDEVICE_H
#define WEBWOWVIEWERCPP_GDEVICE_H

#include <memory>

class GVertexBuffer;
class GVertexBufferBindings;
class GIndexBuffer;
class GUniformBuffer;
class GBlpTexture;
class GTexture;
class GShaderPermutation;
class GMeshGL33;
class GM2MeshGL33;
class GOcclusionQueryGL33;
class GParticleMesh;


class gMeshTemplate;

typedef std::shared_ptr<GUniformBuffer> HGLUniformBuffer;
typedef std::shared_ptr<GMeshGL33> HGLMesh;

#include <unordered_set>
#include <list>
#include "GVertexBufferBindings.h"
#include "buffers/GIndexBuffer.h"
#include "buffers/GVertexBuffer.h"
#include "textures/GTexture.h"
#include "textures/GBlpTexture.h"
#include "buffers/GUniformBuffer.h"
#include "GShaderPermutation.h"
#include "meshes/GMeshGL33.h"
#include "../interface/IDevice.h"



class GDeviceGL33 : public IDevice {
public:
    GDeviceGL33();

    void reset() override;

    bool getIsEvenFrame() override;

    void toogleEvenFrame() override;

    void bindProgram(IShaderPermutation *program) override;

    void bindIndexBuffer(IIndexBuffer *buffer) override;
    void bindVertexBuffer(IVertexBuffer *buffer) override;
    void bindVertexUniformBuffer(IUniformBuffer *buffer, int slot) override;
    void bindFragmentUniformBuffer(IUniformBuffer *buffer, int slot) override;
    void bindVertexBufferBindings(IVertexBufferBindings *buffer) override;

    void bindTexture(ITexture *texture, int slot) override;

    void updateBuffers(std::vector<HGMesh> &meshes) override;
    void drawMeshes(std::vector<HGMesh> &meshes) override;
    //    void drawM2Meshes(std::vector<HGM2Mesh> &meshes);
public:
    std::shared_ptr<IShaderPermutation> getShader(std::string shaderName) override;

    HGUniformBuffer createUniformBuffer(size_t size) override;
    HGVertexBuffer createVertexBuffer() override;
    HGIndexBuffer createIndexBuffer() override;
    HGVertexBufferBindings createVertexBufferBindings() override;

    HGTexture createBlpTexture(HBlpTexture &texture, bool xWrapTex, bool yWrapTex) override;
    HGTexture createTexture() override;
    HGMesh createMesh(gMeshTemplate &meshTemplate) override;
    HGM2Mesh createM2Mesh(gMeshTemplate &meshTemplate) override;
    HGParticleMesh createParticleMesh(gMeshTemplate &meshTemplate) override;

    HGOcclusionQuery createQuery(HGMesh boundingBoxMesh) override;

    HGVertexBufferBindings getBBVertexBinding() override;
    HGVertexBufferBindings getBBLinearBinding() override;

private:
    void drawMesh(HGMesh &hmesh);

private:
    struct BlpCacheRecord {
        HBlpTexture texture;
        bool wrapX;
        bool wrapY;

        bool operator==(const BlpCacheRecord &other) const {
          return
              (texture == other.texture) &&
              (wrapX == other.wrapX) &&
              (wrapY == other.wrapY);

        };
    };
    struct BlpCacheRecordHasher {
        std::size_t operator()(const BlpCacheRecord& k) const {
            using std::hash;
            return hash<void*>{}(k.texture.get()) ^ (hash<bool>{}(k.wrapX) << 8) ^ (hash<bool>{}(k.wrapY) << 16);
        };
    };
    std::unordered_map<BlpCacheRecord, std::weak_ptr<GTexture>, BlpCacheRecordHasher> loadedTextureCache;

    bool m_isEvenFrame = false;

    uint8_t m_lastColorMask = 0xFF;
    int8_t m_lastDepthWrite = -1;
    int8_t m_lastDepthCulling = -1;
    int8_t m_backFaceCulling = -1;
    int8_t m_triCCW = -1;
    int maxUniformBufferSize = -1;
    int uniformBufferOffsetAlign = -1;
    EGxBlendEnum m_lastBlendMode = EGxBlendEnum::GxBlend_UNDEFINED;
    GIndexBuffer *m_lastBindIndexBuffer = nullptr;
    GVertexBuffer *m_lastBindVertexBuffer = nullptr;
    GVertexBufferBindings *m_vertexBufferBindings = nullptr;
    GShaderPermutation * m_shaderPermutation = nullptr;

    HGVertexBufferBindings m_vertexBBBindings;
    HGVertexBufferBindings m_lineBBBindings;
    HGVertexBufferBindings m_defaultVao;

    GTexture *m_lastTexture[10] = {
        nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
        nullptr};

    GUniformBuffer * m_vertexUniformBuffer[3] = {nullptr};
    GUniformBuffer * m_fragmentUniformBuffer[3] = {nullptr};

    HGTexture m_blackPixelTexture;

private:
    //Caches
    std::unordered_map<size_t, HGShaderPermutation> m_shaderPermutCache;
    std::list<std::weak_ptr<GUniformBuffer>> m_unfiormBufferCache;
    struct FrameUniformBuffers {
        std::vector<HGUniformBuffer> m_unfiormBuffersForUpload;
    };
    FrameUniformBuffers m_firstUBOFrame;
    FrameUniformBuffers m_secondUBOFrame;

    std::vector<char> aggregationBufferForUpload;

    bool m_m2ShaderCreated = false;
    int uniformBuffersCreated = 0;
};




#endif //WEBWOWVIEWERCPP_GDEVICE_H

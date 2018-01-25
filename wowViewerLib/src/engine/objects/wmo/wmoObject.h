//
// Created by deamon on 03.07.17.
//

#ifndef WEBWOWVIEWERCPP_WMOOBJECT_H
#define WEBWOWVIEWERCPP_WMOOBJECT_H

struct WmoGroupResult;
class WmoGroupObject;
#include <string>
#include <unordered_set>
#include "../../persistance/header/adtFileHeader.h"
#include "mathfu/glsl_mappings.h"
#include "wmoGroupObject.h"
#include "../m2/m2Object.h"
#include "../../geometry/wmoMainGeom.h"
#include "../iWmoApi.h"


struct WmoGroupResult {
    M2Range topBottom;
    int groupId;
    std::vector<int> bspLeafList;
    int nodeId;
};

struct PortalResults {
    int groupId;
    int portalIndex;
#ifndef CULLED_NO_PORTAL_DRAWING
    std::vector<mathfu::vec3> portalVertices;
#endif
    std::vector<mathfu::vec4> frustumPlanes;
    int level;
};

class WmoObject : public IWmoApi {

public:
    WmoObject(IWoWInnerApi *api) : m_api(api) {
        m_getTextureFunc = std::bind(&WmoObject::getTexture, this, std::placeholders::_1, std::placeholders::_2);
    }
private:
    IWoWInnerApi *m_api;

    WmoMainGeom *mainGeom = nullptr;
    bool m_loading = false;
    bool m_loaded = false;
    CAaBox m_bbox;

    int m_nameSet;
    int m_doodadSet;

    std::vector<PortalInfo_t> geometryPerPortal;

    std::function <BlpTexture *(int materialId, bool isSpec)> m_getTextureFunc;

    mathfu::mat4 m_placementMatrix;
    mathfu::mat4 m_placementInvertMatrix;

    std::string m_modelName;
    bool useFileId = false;
    int m_modelFileId;

    std::vector<WmoGroupObject*> groupObjects = std::vector<WmoGroupObject*>(0);
    std::vector<WmoGroupObject*> groupObjectsLod1 = std::vector<WmoGroupObject*>(0);
    std::vector<BlpTexture> blpTextures;

    std::vector<bool> drawGroupWMO;
    std::vector<M2Object*> m_doodadsArray;

    std::unordered_map<int, BlpTexture*> diffuseTextures;
    std::unordered_map<int, BlpTexture*> specularTextures;

    void createPlacementMatrix(SMMapObjDef &mapObjDef);
    void createPlacementMatrix(SMMapObjDefObj1 &mapObjDef);
    void createBB(CAaBox bbox);
    void postWmoGroupObjectLoad(int groupId, int lod) override;
public:
    M2Object *getDoodad(int index);
    BlpTexture * getTexture(int materialId, bool isSpec);
    void setLoadingParam( SMMapObjDef &mapObjDef);
    void setLoadingParam( SMMapObjDefObj1 &mapObjDef);

    void setModelFileName(std::string modelName);
    void setModelFileId(int fileId);

    void startLoading();
    bool isLoaded(){ return m_loaded;}
    bool hasPortals() {
        return mainGeom->header->nPortals != 0;
    }
    virtual SMOHeader *getWmoHeader() override {
        return mainGeom->header;
    }
    virtual SMOLight *getLightArray() override {
        return mainGeom->lights;
    }
    virtual std::vector<PortalInfo_t> &getPortalInfos() override {
        return geometryPerPortal;
    };

    void draw();

    void createGroupObjects();


    bool checkFrustumCulling(mathfu::vec4 &cameraPos, std::vector<mathfu::vec4> &frustumPlanes,
                             std::vector<mathfu::vec3> &frustumPoints, std::unordered_set<M2Object *> &m2RenderedThisFrame);

    void update();

    void createM2Array();
    void updateBB();

public:
    std::vector<PortalResults> interiorPortals;
    std::vector<PortalResults> exteriorPortals;
    std::vector<PortalResults> antiPortals;

public:
    //Portal culling
    bool startTraversingFromInteriorWMO (
        std::vector<WmoGroupResult> &wmoGroupsResult,
        mathfu::vec4 &cameraVec4,
        mathfu::mat4 &viewPerspectiveMat,
        std::unordered_set<M2Object*> &m2RenderedThisFrame);

    bool startTraversingFromExterior (
        mathfu::vec4 &cameraVec4,
        mathfu::mat4 &viewPerspectiveMat,
        std::unordered_set<M2Object*> &m2RenderedThisFrame);

    void checkGroupDoodads(
        int groupId,
        mathfu::vec4 &cameraVec4,
        std::vector<mathfu::vec4> &frustumPlanes,
        int level,
        std::unordered_set<M2Object*> &m2RenderedThisFrame);

    void transverseGroupWMO (
        int groupId,
        bool fromInterior,
        mathfu::vec4 &cameraVec4,
        mathfu::vec4 &cameraLocal,
        mathfu::mat4 &inverseTransposeModelMat,
        std::vector<bool> &transverseVisitedGroups,
        std::vector<bool> &transverseVisitedPortals,
        std::vector<mathfu::vec4> &localFrustumPlanes,
        int level,
        std::unordered_set<M2Object*> &m2ObjectSet);

    bool getGroupWmoThatCameraIsInside(mathfu::vec4 cameraVec4, WmoGroupResult &result);

    bool isGroupWmoInterior(int groupId);

    void drawTransformedPortalPoints();

    void drawDebugLights();

    void createWorldPortals();

    void drawTransformedAntiPortalPoints();
};


#endif //WEBWOWVIEWERCPP_WMOOBJECT_H
/*****************************************************************************************
 *                                                                                       *
 * OpenSpace                                                                             *
 *                                                                                       *
 * Copyright (c) 2014-2018                                                               *
 *                                                                                       *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this  *
 * software and associated documentation files (the "Software"), to deal in the Software *
 * without restriction, including without limitation the rights to use, copy, modify,    *
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to    *
 * permit persons to whom the Software is furnished to do so, subject to the following   *
 * conditions:                                                                           *
 *                                                                                       *
 * The above copyright notice and this permission notice shall be included in all copies *
 * or substantial portions of the Software.                                              *
 *                                                                                       *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,   *
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A         *
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT    *
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF  *
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE  *
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                         *
 ****************************************************************************************/

#ifndef __OPENSPACE_MODULE_GAIAMISSION___OCTREEMANAGER___H__
#define __OPENSPACE_MODULE_GAIAMISSION___OCTREEMANAGER___H__

#include <vector>
#include <stack>
#include <map>
#include <ghoul/glm.h>
#include <ghoul/opengl/ghoul_gl.h>
#include <modules/gaiamission/rendering/renderoption.h>

namespace openspace {

class OctreeCuller;

class OctreeManager {
public:
    struct OctreeNode {
        std::shared_ptr<OctreeNode> Children[8];
        std::vector<float> posData;
        std::vector<float> colData;
        std::vector<float> velData;
        float originX;
        float originY;
        float originZ;
        float halfDimension;
        size_t numStars; // TODO: Use in LOD checks, else remove?
        bool isLeaf;
        bool isLoaded;
        int bufferIndex;
        size_t lodInUse;
    };

    OctreeManager();
    ~OctreeManager();

    void initOctree();
    void initBufferIndexStack(int maxIndex);
    void insert(std::vector<float> starValues);
    void printStarsPerNode() const;
    std::map<int, std::vector<float>> traverseData(const glm::mat4 mvp, const glm::vec2 screenSize, 
        int& deltaStars, gaiamission::RenderOption option, bool useVBO);
    std::vector<float> getAllData(gaiamission::RenderOption option);
    void clearAllData(int branchIndex = -1);

    void writeToFile(std::ofstream& outFileStream, bool writeData);
    void readFromFile(std::ifstream& inFileStream, bool readData);

    void writeToMultipleFiles(std::string outFolderPath, size_t branchIndex);

    size_t numLeafNodes() const;
    size_t numInnerNodes() const;
    size_t totalNodes() const;
    size_t totalDepth() const;
    size_t maxStarsPerNode() const;
    size_t biggestChunkIndexInUse() const;

private:
    const size_t POS_SIZE = 3;
    const size_t COL_SIZE = 2;
    const size_t VEL_SIZE = 3;

    size_t MAX_DIST = 10; // [kPc] Radius of Gaia DR1 is ~100 kParsec.
    // Stars/node depend on max_dist because it needs to be big enough to hold all stars
    // that falls outside of the biggest nodes, otherwise it causes a stack overflow.
    size_t MAX_STARS_PER_NODE = 10000; 
    const int DEFAULT_INDEX = -1;
    const float MIN_TOTAL_PIXELS_LOD = 0.0; // Will be multiplied by depth.
    const int FIRST_LOD_DEPTH = 3; // No layer beneath this will not store any LOD cache.
    const std::string BINARY_SUFFIX = ".bin"; 

    size_t getChildIndex(float posX, float posY, float posZ,
        float origX = 0.0, float origY = 0.0, float origZ = 0.0);
    bool insertInNode(std::shared_ptr<OctreeNode> node,
        std::vector<float> starValues, int depth = 1);
    void constructLodCache(std::shared_ptr<OctreeNode> node);
    void insertStarInLodCache(std::shared_ptr<OctreeNode> node, std::vector<float> starValues);
    std::string printStarsPerNode(std::shared_ptr<OctreeNode> node, std::string prefix) const;
    std::map<int, std::vector<float>> checkNodeIntersection(std::shared_ptr<OctreeNode> node, 
        const glm::mat4 mvp, const glm::vec2 screenSize, int& deltaStars, 
        gaiamission::RenderOption option);
    std::map<int, std::vector<float>> removeNodeFromCache(std::shared_ptr<OctreeNode> node, 
        int& deltaStars, bool recursive = true);
    std::vector<float> getNodeData(std::shared_ptr<OctreeNode> node, gaiamission::RenderOption option);
    void clearNodeData(std::shared_ptr<OctreeNode> node);
    void createNodeChildren(std::shared_ptr<OctreeNode> node);

    void writeNodeToFile(std::ofstream& outFileStream, 
        std::shared_ptr<OctreeNode> node, bool writeData);
    void readNodeFromFile(std::ifstream& inFileStream, 
        std::shared_ptr<OctreeNode> node, bool readData);

    void writeNodeToMultipleFiles(std::string outFilePrefix, std::shared_ptr<OctreeNode> node);
    void fetchNodeDataFromFile(std::string inFilePrefix, std::shared_ptr<OctreeNode> node);

    std::vector<float> constructInsertData(std::shared_ptr<OctreeNode> node, 
        gaiamission::RenderOption option);

    std::unique_ptr<OctreeNode> _root;
    std::unique_ptr<OctreeCuller> _culler;
    std::stack<int> _freeSpotsInBuffer;
    std::set<int> _removedKeysInPrevCall;

    size_t _totalDepth;
    size_t _numLeafNodes;
    size_t _numInnerNodes;
    size_t _biggestChunkIndexInUse;
    size_t _valuesPerStar;
    
    int _maxStackSize;
    bool _rebuildBuffer;
    bool _useVBO;

}; // class OctreeManager

}  // namespace openspace

#endif // __OPENSPACE_MODULE_GAIAMISSION___OCTREEMANAGER___H__

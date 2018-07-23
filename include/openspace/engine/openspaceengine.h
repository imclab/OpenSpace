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

#ifndef __OPENSPACE_CORE___OPENSPACEENGINE___H__
#define __OPENSPACE_CORE___OPENSPACEENGINE___H__

#include <openspace/engine/windowdelegate.h>
#include <openspace/properties/stringproperty.h>
#include <openspace/util/keys.h>
#include <openspace/util/mouse.h>
#include <ghoul/glm.h>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace ghoul { class Dictionary; }
namespace ghoul::cmdparser { class CommandlineParser; }

namespace openspace {

class AssetManager;
class GUI;
class LoadingScreen;
class Scene;

namespace interaction { struct JoystickInputStates; }
namespace gui { class GUI; }
namespace properties { class PropertyOwner; }
namespace scripting { struct LuaLibrary; }

  // Structure that is responsible for the delayed shutdown of the application
struct ShutdownInformation {
    // Whether the application is currently in shutdown mode (i.e. counting down the
    // timer and closing it at '0'
    bool inShutdown = false;
    // Total amount of time the application will wait before actually shutting down
    float waitTime = 0.f;
    // Current state of the countdown; if it reaches '0', the application will
    // close
    float timer = 0.f;
};

class OpenSpaceEngine {
public:
    static void create(int argc, char** argv,
        WindowDelegate windowDelegate,
        std::vector<std::string>& sgctArguments, bool& requestClose,
        bool consoleLog = true);

    static void destroy();
    static OpenSpaceEngine& ref();
    static bool isCreated();

    ~OpenSpaceEngine() = default;

    void initialize();
    void initializeGL();
    void deinitialize();
    void preSynchronization();
    void postSynchronizationPreDraw();
    void render(const glm::mat4& sceneMatrix, const glm::mat4& viewMatrix,
        const glm::mat4& projectionMatrix);
    void drawOverlays();
    void postDraw();
    void keyboardCallback(Key key, KeyModifier mod, KeyAction action);
    void charCallback(unsigned int codepoint, KeyModifier modifier);
    void mouseButtonCallback(MouseButton button, MouseAction action);
    void mousePositionCallback(double x, double y);
    void mouseScrollWheelCallback(double posX, double posY);
    void setJoystickInputStates(interaction::JoystickInputStates& states);
    void externalControlCallback(const char* receivedChars, int size, int clientId);
    void encode();
    void decode();

    void scheduleLoadSingleAsset(std::string assetPath);
    void toggleShutdownMode();

    // Guaranteed to return a valid pointer
    AssetManager& assetManager();
    LoadingScreen& loadingScreen();

    void writeSceneDocumentation();
    void writeStaticDocumentation();

    /**
     * Returns the Lua library that contains all Lua functions available to affect the
     * application.
     */
    static scripting::LuaLibrary luaLibrary();

private:
    OpenSpaceEngine(std::string programName);

    void loadSingleAsset(const std::string& assetPath);
    void loadFonts();

    void configureLogging(bool consoleLog);

    void runGlobalCustomizationScripts();
    void configureLogging();

    std::unique_ptr<Scene> _scene;
    std::unique_ptr<AssetManager> _assetManager;
    std::unique_ptr<LoadingScreen> _loadingScreen;

    struct {
        properties::StringProperty versionString;
        properties::StringProperty sourceControlInformation;
    } _versionInformation;

    bool _hasScheduledAssetLoading = false;
    std::string _scheduledAssetPathToLoad;

    ShutdownInformation _shutdown;

    // The first frame might take some more time in the update loop, so we need to know to
    // disable the synchronization; otherwise a hardware sync will kill us after 1 minute
    bool _isFirstRenderingFirstFrame = true;

    glm::dvec2 _mousePosition;

    static OpenSpaceEngine* _engine;
};

#define OsEng (openspace::OpenSpaceEngine::ref())

} // namespace openspace

#endif // __OPENSPACE_CORE___OPENSPACEENGINE___H__

#include "kemena/kemena.h"

using namespace kemena;

int main()
{
    // Create window and renderer
    kWindow* window = createWindow(1024, 768, "Kemena3D Demo - Hello World");
    kRenderer* renderer = createRenderer(window);
    renderer->setClearColor(vec4(0.4f, 0.6f, 0.8f, 1.0f));

    // Create the asset manager, world and scene
    kAssetManager* assetManager = createAssetManager();
    kWorld* world = createWorld(assetManager);
    kScene* scene = world->createScene("My Scene");

    // Create a camera and a sun light
    kCamera* camera = scene->addCamera(glm::vec3(2.5f, 2.5f, 2.5f), glm::vec3(0.0f, 0.5f, 0.0f), kCameraType::CAMERA_TYPE_LOCKED);

    kShader* shader = assetManager->createShaderByFile("../../../Assets/shader/glsl/flat.vert", "../../../Assets/shader/glsl/flat.frag");

    kMaterial* mat = assetManager->createMaterial(shader);
    kTexture2D* diff = assetManager->loadTexture2D("../diffuse.png", "albedoMap");
    mat->addTexture(diff);

    // Load a 3D model and apply the material to it
    kMesh* mesh = scene->addMesh("../reptile_mage.obj");
	mesh->setRotation(vec3(0.0f, -0.4f, 0.0f));
    mesh->setMaterial(mat);

    // Game loop
    kSystemEvent event;
    while (window->getRunning())
    {
        if (event.hasEvent())
        {
            if (event.getType() == K_EVENT_QUIT)
            {
                window->setRunning(false);
            }
        }

        renderer->render(scene, 0, 0, window->getWindowWidth(), window->getWindowHeight(), window->getTimer()->getDeltaTime());
    }

    // Clean up
    renderer->destroy();
    window->destroy();
    return 0;
}
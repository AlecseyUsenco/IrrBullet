#include<irrlicht.h>
#include<irrbullet.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace gui;
using namespace io;
using namespace video;
using namespace std;

#ifdef _IRR_WINDOWS_
#pragma comment(lib, "irrlicht.lib")
#pragma comment(lib, "irrBullet.lib")
#pragma comment(lib, "BulletDynamics.lib")
#pragma comment(lib, "BulletSoftBody.lib")
#pragma comment(lib, "GIMPACTUtils.lib")
#pragma comment(lib, "LinearMath.lib")
#pragma comment(lib, "BulletCollision.lib")
#endif

class CReceiver : public IEventReceiver
{
private:
	bool Keys[KEY_KEY_CODES_COUNT];
	bool Mouse[EMIE_COUNT];
public:
	CReceiver()
	{
		for (int i = 0; i < KEY_KEY_CODES_COUNT; ++i)
		Keys[i] = false;

		for (u32 i = 0; i < EMIE_COUNT; ++i)
		Mouse[i] = false;
	}

	bool OnEvent(const SEvent& event)
	{
		if (event.EventType == EET_KEY_INPUT_EVENT)
			Keys[event.KeyInput.Key] = event.KeyInput.PressedDown;
		if (event.EventType == EET_MOUSE_INPUT_EVENT)
			for (s32 i = 0; i<EMIE_COUNT; ++i)
			Mouse[i] = i == event.MouseInput.Event;
		return false;
	}
	bool isKeyDown(u32 keyCode) { return Keys[keyCode]; }
	bool isMouseDown(u32 mouseCode) { return Mouse[mouseCode]; }
};

void createWorld(int rows, int colums, IrrlichtDevice* device, irrBulletWorld* world)
{
	IMeshSceneNode *ground = device->getSceneManager()->addCubeSceneNode(1.0);
	ground->setScale(vector3df(800,3,800));
	ground->setPosition(vector3df(20,3,30));
	ground->setMaterialFlag(EMF_LIGHTING, false);
	ground->setMaterialTexture(0, device->getVideoDriver()->getTexture("../rockwall.jpg"));
	ICollisionShape *shapeground = new IBoxShape(ground, 0, false);
	IRigidBody *bodyground = world->addRigidBody(shapeground);
	for (u32 j = 0; j < colums; j++)
	{
		for (u32 i = 0; i < rows; i++)
		{
			ISceneNode *Node = device->getSceneManager()->addCubeSceneNode(1.0);
			Node->setScale(core::vector3df(3, 3, 3));
			Node->setPosition(core::vector3df(3 * j, 0 + 3 * i + 3, 0));
			Node->setMaterialFlag(video::EMF_LIGHTING, false);
			Node->setMaterialTexture(0, device->getVideoDriver()->getTexture("../crate.jpg"));
			ICollisionShape *shape = new IBoxShape(Node, 3, false);
			IRigidBody *body = world->addRigidBody(shape);
		}
	}
}

int main()
{
	int rows = 10, colums = 10;
	printf("Number of stack rows: ");
	cin >> rows;
	printf("\nNumber of stack colums: ");
	cin >> colums;
	CReceiver receiver;
	IrrlichtDevice *device = createDevice(EDT_DIRECT3D9, dimension2d<u32>(640, 480), 16, false, false, false, &receiver);
	IVideoDriver *driver = device->getVideoDriver();
	ISceneManager *smgr = device->getSceneManager();
	IGUIEnvironment *guienv = device->getGUIEnvironment();
	device->setWindowCaption(L"HelloWorld");
	irrBulletWorld * world = createIrrBulletWorld(device, true, true);
	world->setDebugMode(EPDM_DrawAabb | EPDM_DrawContactPoints);
	world->setGravity(core::vector3df(0, -10, 0));
	createWorld(rows, colums, device, world);
	scene::ICameraSceneNode* camera = smgr->addCameraSceneNodeFPS();
	camera->setPosition(core::vector3df(50, 15, 200));
	u32 then = device->getTimer()->getTime();
	while (device->run())
	{
		const u32 now = device->getTimer()->getTime();
		const f32 frameDeltaTime = (f32)(now - then) / 1000.f; // в секундах
		then = now;
		if (receiver.isMouseDown(EMIE_RMOUSE_PRESSED_DOWN))
		{
			core::vector3df pos(camera->getPosition().X, camera->getPosition().Y, camera->getPosition().Z);
			scene::ISceneNode *Node = smgr->addCubeSceneNode();
			Node->setScale(core::vector3df(0.2, 0.2, 0.2));
			Node->setPosition(pos);
			Node->setMaterialFlag(video::EMF_LIGHTING, false);
			Node->setMaterialTexture(0, driver->getTexture("../rockwall.jpg"));
			ICollisionShape *shape = new IBoxShape(Node, 1, true);
			IRigidBody *body = world->addRigidBody(shape);
			body->setDamping(0.2, 0.2);
			body->setFriction(0.4f);
			body->setGravity(core::vector3df(0, -10, 0));
			core::vector3df rot = camera->getRotation();
			core::matrix4 mat;
			mat.setRotationDegrees(rot);
			core::vector3df forwardDir(core::vector3df(mat[8], mat[9], mat[10]) * 120);
			body->setLinearVelocity(forwardDir);
		}

		driver->beginScene(true, true, video::SColor(255, 100, 101, 140));
		world->stepSimulation(frameDeltaTime, 120);
		world->debugDrawWorld(true);
		world->debugDrawProperties(true);
		smgr->drawAll();
		guienv->drawAll();
		driver->endScene();

		if (world) delete world;
		device->drop();
		return 0;
	}
}
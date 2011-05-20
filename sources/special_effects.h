#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;



void createSFXImpactDebrie()
{

    video::IVideoDriver* driver = device->getVideoDriver();
	ISceneManager* smgr = device->getSceneManager();
	gui::IGUIEnvironment* guienv = device->getGUIEnvironment();


//create impact debrie

       debrieDustEffect = smgr->addParticleSystemSceneNode(false);

                scene::IParticleEmitter* em = debrieDustEffect->createBoxEmitter(
                core::aabbox3d<f32>(-7,0,-7,7,1,7), // emitter size
                core::vector3df(0.0f,1.1f,0.0f),    // initial direction
                20,70,                              // emit rate
                video::SColor(0,255,255,255),       // darkest color
                video::SColor(0,255,255,255),       // brightest color
                500,501,50,                         // min and max age, angle
                core::dimension2df(20.0f,20.0f),        // min size
                core::dimension2df(25.0f,25.0f));       // max size

        debrieDustEffect->setEmitter(em);
        em->drop();

        scene::IParticleAffector* paf = debrieDustEffect->createGravityAffector(core::vector3df(0,-0.1f,0), 1000);

        debrieDustEffect->addAffector(paf);
        paf->drop();

        debrieDustEffect->setPosition(intersection2);
        debrieDustEffect->setScale(core::vector3df(0.1f,0.1f,0.1f));
        debrieDustEffect->setMaterialFlag(video::EMF_LIGHTING, true);
        debrieDustEffect->setMaterialFlag(video::EMF_ZWRITE_ENABLE, false);
        debrieDustEffect->setMaterialTexture(0, driver->getTexture("../data/shaders/dust_001.png"));
        debrieDustEffect->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL);

		ISceneNodeAnimator *deleteTheDebrieDustEffect = smgr->createDeleteAnimator(150);
        debrieDustEffect->addAnimator(deleteTheDebrieDustEffect);


}
















void createSFXExplosion(int explosionSize, core::vector3df explosionPosition)
{

    video::IVideoDriver* driver = device->getVideoDriver();
	ISceneManager* smgr = device->getSceneManager();
	gui::IGUIEnvironment* guienv = device->getGUIEnvironment();

//create explosion medium

    startStoppingExplosionAnimators = true;

	ILightSceneNode *explosionLightSource = smgr->addLightSceneNode(0, explosionPosition, (8.5f,10.5f,10.5f), 1.0f, -1);
    ISceneNodeAnimator *deleteTheLightNode = smgr->createDeleteAnimator(4000);
    explosionLightSource->addAnimator(deleteTheLightNode);



		//big flare effect ------------------------------------------------------------------------------------------------
       explosionSmallEffect = smgr->addParticleSystemSceneNode(false);

                scene::IParticleEmitter* em2 = explosionSmallEffect->createBoxEmitter(
                core::aabbox3d<f32>(-7,0,-7,7,1,7), // emitter size
                core::vector3df(0.0f,0.01f,0.0f),    // initial direction
                10,20,                              // emit rate
                video::SColor(0,255,255,255),       // darkest color
                video::SColor(0,255,255,255),       // brightest color
                200,300,10,                         // min and max age, angle
                core::dimension2df(30.0f*explosionSize,30.0f*explosionSize),        // min size
                core::dimension2df(32.0f*explosionSize,32.0f*explosionSize));       // max size

        explosionSmallEffect->setEmitter(em2);
        em2->drop();


		scene::IParticleAffector* paf7 = explosionSmallEffect->createFadeOutParticleAffector((0,0,0,0), 150);
		explosionSmallEffect->addAffector(paf7);
        paf7->drop();


		explosionSmallEffect->setPosition(core::vector3df(intersection2));		
        explosionSmallEffect->setScale(core::vector3df(0.005f,0.005f,0.005f));
        explosionSmallEffect->setMaterialFlag(video::EMF_LIGHTING, false);
        explosionSmallEffect->setMaterialFlag(video::EMF_ZWRITE_ENABLE, false);
        explosionSmallEffect->setMaterialTexture(0, driver->getTexture("../data/shaders/explosion.bmp"));
        explosionSmallEffect->setMaterialType(video::EMT_TRANSPARENT_VERTEX_ALPHA);
		
		ISceneNodeAnimator *deleteTheExplosionEffect = smgr->createDeleteAnimator(150);
        explosionSmallEffect->addAnimator(deleteTheExplosionEffect);



	//create physX force
	physxManager->createExplosion(explosionPosition, 200.0f, 150000000000.0f, 200.0f, 0.1f);




	//create debrie everywhere  -------------------------------------------------------

       debrieDustEffect = smgr->addParticleSystemSceneNode(false);

                scene::IParticleEmitter* em = debrieDustEffect->createBoxEmitter(
                core::aabbox3d<f32>(-7,0,-7,7,1,7), // emitter size
                core::vector3df(0.0f,9.0f,0.0f),    // initial direction
                10,11,								// emit rate
                video::SColor(0,255,255,255),       // darkest color
                video::SColor(0,255,255,255),       // brightest color
                199,200,45,                         // min and max age, angle
                core::dimension2df(20.5f*explosionSize,20.5f*explosionSize),        // min size
                core::dimension2df(20.6f*explosionSize,20.5f*explosionSize));       // max size

        debrieDustEffect->setEmitter(em);
        em->drop();

		scene::IParticleAffector* pafA = debrieDustEffect->createFadeOutParticleAffector((0,0,0,0), 150);
		debrieDustEffect->addAffector(pafA);
        pafA->drop();


        scene::IParticleAffector* paf = debrieDustEffect->createGravityAffector(core::vector3df(0, -1.03f,0), 1000);
        debrieDustEffect->addAffector(paf);
        paf->drop();

        debrieDustEffect->setPosition(core::vector3df(intersection2));
        debrieDustEffect->setScale(core::vector3df(0.1f,0.1f,0.1f));
        debrieDustEffect->setMaterialFlag(video::EMF_LIGHTING, true);
        debrieDustEffect->setMaterialFlag(video::EMF_ZWRITE_ENABLE, false);
        debrieDustEffect->setMaterialTexture(0, driver->getTexture("../data/shaders/explosion.bmp"));
        debrieDustEffect->setMaterialType(video::EMT_TRANSPARENT_VERTEX_ALPHA);

		ISceneNodeAnimator *deleteTheDebrieDustEffect = smgr->createDeleteAnimator(200);
        debrieDustEffect->addAnimator(deleteTheDebrieDustEffect);









	   //burn effect for 10 seconds ------------------------------------------------------------------------------------------------
       burnSmallEffect = smgr->addParticleSystemSceneNode(false);

                scene::IParticleEmitter* em3 = burnSmallEffect->createBoxEmitter(
                core::aabbox3d<f32>(-7,0,-7,7,1,7), // emitter size // was -7 0 -7 7 1 7
                core::vector3df(0.0f,1.1f,0.01f),    // initial direction
                10,50,                              // emit rate
                video::SColor(0,255,255,255),       // darkest color
                video::SColor(0,255,255,255),       // brightest color
                500,501,50,                         // min and max age, angle
                core::dimension2df(2.0f*explosionSize, 2.0f*explosionSize),        // min size
                core::dimension2df(10.0f*explosionSize, 10.0f*explosionSize));       // max size

        burnSmallEffect->setEmitter(em3);
        em3->drop();

        scene::IParticleAffector* paf3 = burnSmallEffect->createGravityAffector(core::vector3df(0,0.0,0), 1000);
        burnSmallEffect->addAffector(paf3);
        paf3->drop();

		scene::IParticleAffector* paf5 = burnSmallEffect->createFadeOutParticleAffector((0,0,0,0), 100);
		burnSmallEffect->addAffector(paf5);
        paf5->drop();

		burnSmallEffect->setPosition(core::vector3df(intersection2.X, intersection2.Y-10, intersection2.Z));
		burnSmallEffect->setScale(core::vector3df(0.05f,0.05f,0.05f));
        burnSmallEffect->setMaterialFlag(video::EMF_LIGHTING, false);
        burnSmallEffect->setMaterialFlag(video::EMF_ZWRITE_ENABLE, false);
        burnSmallEffect->setMaterialTexture(0, driver->getTexture("../data/shaders/fire_003.bmp"));
        burnSmallEffect->setMaterialType(video::EMT_TRANSPARENT_VERTEX_ALPHA);
		
		ISceneNodeAnimator *deleteTheBurnEffect = smgr->createDeleteAnimator(10000);
        burnSmallEffect->addAnimator(deleteTheBurnEffect);





		//smoke for 20 seconds ------------------------------------------------------------------------------------------------

		 smokeSmallEffect = smgr->addParticleSystemSceneNode(false);

                scene::IParticleEmitter* em4 = smokeSmallEffect->createBoxEmitter(
                core::aabbox3d<f32>(-7,0,-7,7,1,7), // emitter size
                core::vector3df(0.0f,0.02f,0.07f),    // initial direction //was 0.0 0.03 0.01
                10,10,                              // emit rate
                video::SColor(0,0,0,0),             // darkest color
                video::SColor(0,50,50,50),       // brightest color
                1000,10500,10,                         // min and max age, angle
                core::dimension2df(10.0f*explosionSize, 10.0f*explosionSize),        // min size
                core::dimension2df(10.0f*explosionSize, 10.0f*explosionSize));       // max size

        smokeSmallEffect->setEmitter(em4);
        em4->drop();

		scene::IParticleAffector* paf6 = smokeSmallEffect->createFadeOutParticleAffector((0,0,0,0), 10000);
		smokeSmallEffect->addAffector(paf6);
        paf6->drop();


		smokeSmallEffect->setPosition(core::vector3df(intersection2.X, intersection2.Y-3, intersection2.Z));		
        smokeSmallEffect->setScale(core::vector3df(0.5f, 10.05f, 0.5f));
        smokeSmallEffect->setMaterialFlag(video::EMF_LIGHTING, false);
        smokeSmallEffect->setMaterialFlag(video::EMF_ZWRITE_ENABLE, false);
        smokeSmallEffect->setMaterialTexture(0, driver->getTexture("../data/shaders/smoke_001.bmp"));
        smokeSmallEffect->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
		
		ISceneNodeAnimator *deleteTheSmokeEffect = smgr->createDeleteAnimator(20000);
        smokeSmallEffect->addAnimator(deleteTheSmokeEffect);

		shakeCameraExplosion();

}
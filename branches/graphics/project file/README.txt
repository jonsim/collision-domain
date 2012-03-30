The following environment variables are needed, I may have missed one...

OGREBULLET_ROOT    // top level of ogrebullet folder (probably also containing OgreBullet_VC8.sln)
OGRE_HOME          // i.e. C:\CollisionDomain\OgreSDK_vc10_v1-7-3
BULLET_HOME        // i.e. C:\CollisionDomain\bullet-2.79-rev2440
CD_CHECKOUT_DIR    // the checkout directory containing client,server,shared folders

Lastly, the files along the side in solution explorer are by the way VS works "hardcoded", so you should delete these from solution explorer and drag yours back in from your checkout directory.

Alternatively you can adjust to what I was using (only if you really, really want to)

some collision domain folder
	"CD_CHECKOUT_DIR"
	.svn
	solution
		VS project
	client
	server
	shared
	
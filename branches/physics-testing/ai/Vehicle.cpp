#include "Vehicle.h"

using namespace Ogre;
using namespace std;

Vehicle::Vehicle(Vector3 position, double rotation,
            Vector3 velocity, double mass,
            double max_force, double max_speed,
            double max_turn_rate, double scale, SceneManager *sceneManager,
            PhysicsCore *physicsCore, int id)
: MovingEntity(position, scale, velocity, max_speed, velocity.perpendicular(), mass, Vector3(scale,scale,scale), max_turn_rate, max_force)
 //m_vFeelers(3)
{
	  m_pSteering     = new SteeringBehaviour(this);
	  m_dTimeElapsed  = 0.0;
	  m_dDBoxLength = 10.0;
	  mSceneMgr       = sceneManager;
	  mCar = (Car*) new BulletBuggyCar(mSceneMgr, physicsCore->mWorld, id);
	  mCar->moveTo(btVector3(position.x, position.y, position.z));
	  m_pCollision    = new CollisionDetection(mSceneMgr);

	  /*m_vWallHitPosition = Vector3::ZERO;
	  m_vWallNormal      = Vector3::ZERO;
	  m_vFeelerPos       = Vector3::ZERO;*/

	  //set up the smoother
	  m_pHeadingSmoother = new Smoother<Vector3>(10, Vector3(0, 0, 0));
}

Vehicle::~Vehicle()
{
	if(m_pCollision)
		delete m_pCollision;
}

void Vehicle::Update(double time_elapsed)
{
    m_dTimeElapsed = time_elapsed;

    //Wall detection
    /*if(FindWalls()==Vector3::ZERO)
	{
		m_vFeelerPos       = Vector3::ZERO;
		m_vWallHitPosition = Vector3::ZERO;
		m_vWallNormal      = Vector3::ZERO;

     */
    //FindCloseObstacle();

    Vector3 SteeringForce = m_pSteering->Calculate();
    Vector3 acceleration = SteeringForce / m_dMass;

    m_vVelocity += acceleration * time_elapsed;
    m_vVelocity = Truncate(m_vVelocity, m_dMaxSpeed);
    m_vPos += m_vVelocity * time_elapsed;

    if(m_vVelocity.squaredLength() > 0.000000001)
    {
    	m_vHeading = m_vVelocity.normalisedCopy();
    	m_vSide    = m_vHeading.perpendicular();
    	mCar->moveTo(btVector3(m_vPos.x, m_vPos.y, m_vPos.z));
    }

    //use the smoother to prevent jittering behaviour
    m_vSmoothedHeading = m_pHeadingSmoother->Update(GetHeading());
}

/*void Vehicle::CreateFeelers(void)
{
  double range = m_dWallDetectionFeelerLength + (Speed()/m_dMaxSpeed) * m_dWallDetectionFeelerLength;
  m_vFeelers[0] = m_vPos + range * m_vHeading;
  m_vFeelers[1] = m_vPos + range/2.0f * m_vHeading.crossProduct( m_vHeading.perpendicular());
  m_vFeelers[2] = m_vPos + range/2.0f * m_vHeading.crossProduct(-m_vHeading.perpendicular());
}

Vector3 Vehicle::FindWalls(void)
{
  double DistToThisIP    = 0.0;
  double DistToClosestIP = MaxDouble;

  Vector3 point, ClosestPoint;
  point = ClosestPoint = Vector3::ZERO;

  String entName;
  Entity *wallEnt;

  CreateFeelers();
  SceneNode *feeler = NULL;

  for(unsigned int flr = 0; flr < m_vFeelers.size(); ++flr)
  {
    Vector3 heading = Vector3::ZERO;
    heading = (m_vFeelers[flr]+m_vPos).normalisedCopy();
    //heading = (m_vPos - m_vFeelers[flr]).normalisedCopy();

    if(m_pCollision->RaycastFromPoint(m_vFeelers[flr], heading, wall_type, point, entName))
    {
      DistToThisIP = m_vFeelers[flr].distance(point);
      if(DistToThisIP < DistToClosestIP)
      {
        DistToClosestIP = DistToThisIP;
        if(DistToClosestIP < m_dWallDetectionFeelerLength)
        {
          wallEnt            = mSceneMgr->getEntity(entName);
          m_pFoundWall       = static_cast<Wall*>(wallEnt->getUserObject());
          m_vWallHitPosition = point;
          m_vWallNormal      = m_pFoundWall->Normal();
          m_vFeelerPos       = m_vFeelers[flr];
        }
        ClosestPoint       = point;
      }
    }
  }

  if(DistToClosestIP  < m_dWallDetectionFeelerLength)
    return ClosestPoint;
  else return Vector3::ZERO;
}*/

Vector3 Vehicle::FindCloseObstacle(void)
{
	Vector3 foundPos = Vector3::ZERO;
	String foundName;

    //Find obstacles
	m_dDBoxLength = m_pSteering->DBoxLength() + (GetSpeed()/m_dMaxSpeed) * m_pSteering->DBoxLength();
	cout << m_dDBoxLength << endl;
	m_pFoundObstacle = NULL;

	if(m_pCollision->RaycastFromPoint(m_vPos, m_vHeading, static_type, foundPos, foundName))
	{
		if(m_vPos.distance(foundPos) < m_dDBoxLength)
		{
			m_pFoundObstacle = any_cast<Obstacle*>(mSceneMgr->getEntity(foundName)->getUserAny());
			return foundPos;
		}
	}

	return Vector3::ZERO;
}

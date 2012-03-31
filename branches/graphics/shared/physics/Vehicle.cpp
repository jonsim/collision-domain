#include "stdafx.h"
#include "SharedIncludes.h"

struct btWheelContactPoint
{
    btRigidBody* m_body0;
    btRigidBody* m_body1;
    btVector3    m_frictionPositionWorld;
    btVector3    m_frictionDirectionWorld;
    btScalar    m_jacDiagABInv;
    btScalar    m_maxImpulse;


    btWheelContactPoint(btRigidBody* body0,btRigidBody* body1,const btVector3& frictionPosWorld,const btVector3& frictionDirectionWorld, btScalar maxImpulse)
        :m_body0(body0),
        m_body1(body1),
        m_frictionPositionWorld(frictionPosWorld),
        m_frictionDirectionWorld(frictionDirectionWorld),
        m_maxImpulse(maxImpulse)
    {
        btScalar denom0 = body0->computeImpulseDenominator(frictionPosWorld,frictionDirectionWorld);
        btScalar denom1 = body1->computeImpulseDenominator(frictionPosWorld,frictionDirectionWorld);
        btScalar    relaxation = 1.f;
        m_jacDiagABInv = relaxation/(denom0+denom1);
    }
};

btScalar calcContactFriction(btWheelContactPoint& contactPoint);


/* Notes to self

    - Use engine force to rotate wheels
    - Work out slip of rear wheels - linearly reduce engine force until we have traction
        = actual velocity (front wheels) - target velocity (rear wheels)
    - Possibly need to divide by friction slip parameter - take veldifference.length[2?]() and get proportion
    - Different if brakes are on - front wheel vel = 0 then so probably need to use car linear velocity

    - Sliding is fucked. Why do you do this to me Bullet?
    - Some crazy equation of linear / angular velocity * slipangle might work
*/

void Vehicle::updateVehicle( btScalar step )
{
    int i = 0;

    // Make sure wheel rays are cast in correct place
    for( i = 0; i < getNumWheels(); i ++ )
        updateWheelTransform( i, false );

    // Get vehicle speed
    m_currentVehicleSpeedKmHour = btScalar(3.6) * getRigidBody()->getLinearVelocity().length();
    // and position
    const btTransform& chassisTrans = getChassisWorldTransform();

    // Work out direction of forward motion
    btVector3 forwardW (
        chassisTrans.getBasis()[0][getForwardAxis()],
        chassisTrans.getBasis()[1][getForwardAxis()],
        chassisTrans.getBasis()[2][getForwardAxis()] );

    // Speed *= -1 if we're going backwards
    if( forwardW.dot( getRigidBody()->getLinearVelocity() ) < btScalar(0.) )
        m_currentVehicleSpeedKmHour *= btScalar(-1.);


    // SUSPENSION
    for( i = 0; i < m_wheelInfo.size(); i ++ )
        rayCast( m_wheelInfo[i] );

    updateSuspension( step );

    for( i = 0; i < m_wheelInfo.size(); i ++ )
    {
        btWheelInfo& wheel = m_wheelInfo[i];
        btScalar suspensionForce = wheel.m_wheelsSuspensionForce;
        
        // Restrict to given max suspension force
        if( suspensionForce > wheel.m_maxSuspensionForce )
            suspensionForce = wheel.m_maxSuspensionForce;

        btVector3 impulse = wheel.m_raycastInfo.m_contactNormalWS * suspensionForce * step;
        btVector3 relpos = wheel.m_raycastInfo.m_contactPointWS - getRigidBody()->getCenterOfMassPosition();
        
        getRigidBody()->applyImpulse(impulse, relpos);
    }

    updateFriction( step );

    // WHEEL ROTATION
    // TODO: change so that this works from engine force rather than velocity
    for( i=0; i < m_wheelInfo.size(); i++ )
    {
        btWheelInfo& wheel = m_wheelInfo[i];
        btVector3 relpos = wheel.m_raycastInfo.m_hardPointWS - getRigidBody()->getCenterOfMassPosition();
        btVector3 vel = getRigidBody()->getVelocityInLocalPoint( relpos );

        if (wheel.m_raycastInfo.m_isInContact)
        {
            btVector3 fwd = forwardW;
            btScalar proj = fwd.dot( wheel.m_raycastInfo.m_contactNormalWS );
            fwd -= wheel.m_raycastInfo.m_contactNormalWS * proj;

            btScalar proj2 = fwd.dot(vel);
            
            wheel.m_deltaRotation = (proj2 * step) / (wheel.m_wheelsRadius);
            wheel.m_rotation += wheel.m_deltaRotation;

        }
        else
        {
            wheel.m_rotation += wheel.m_deltaRotation;
        }
        
        wheel.m_deltaRotation *= btScalar(0.99);//damping of rotation when not in contact

    }
}

void Vehicle::updateFriction( btScalar timeStep )
{
    int numWheel = getNumWheels();
    if (!numWheel)
        return;

    m_forwardWS.resize(numWheel);
    m_axle.resize(numWheel);
    m_forwardImpulse.resize(numWheel);
    m_sideImpulse.resize(numWheel);
    
    int numWheelsOnGround = 0;
    
    btScalar rollingFriction = 0.0f, brakingFriction = 0.0f, engineFriction = 0.0f;

    for (int i = 0; i < getNumWheels(); i ++ )
    {
        btWheelInfo& wheelInfo = m_wheelInfo[i];
        
        m_sideImpulse[i] = btScalar( 0.0f );
        m_forwardImpulse[i] = btScalar( 0.0f );
        
        const btTransform& wheelTrans = getWheelTransformWS( i );

            btMatrix3x3 wheelBasis0 = wheelTrans.getBasis();
            m_axle[i] = btVector3(    
                wheelBasis0[0][getRightAxis()],
                wheelBasis0[1][getRightAxis()],
                wheelBasis0[2][getRightAxis()] );
            
            const btVector3& surfNormalWS = wheelInfo.m_raycastInfo.m_contactNormalWS;
            btScalar proj = m_axle[i].dot( surfNormalWS );
            m_axle[i] -= surfNormalWS * proj;
            m_axle[i] = m_axle[i].normalize();
            
            m_forwardWS[i] = surfNormalWS.cross( m_axle[i] );
            m_forwardWS[i].normalize();
            
        class btRigidBody* groundObject = (class btRigidBody*) wheelInfo.m_raycastInfo.m_groundObject;
        
        if( groundObject )
        {
            numWheelsOnGround++;
            
            btVector3 rel_pos = wheelInfo.m_raycastInfo.m_contactPointWS - getRigidBody()->getCenterOfMassPosition();
            btVector3 rel_pos_ground = wheelInfo.m_raycastInfo.m_contactPointWS - groundObject->getCenterOfMassPosition();
            

            ///////////////////////////////////////
            // Rolling resistance force
            ///////////////////////////////////////
            
            // http://www.engineeringtoolbox.com/rolling-friction-resistance-d_1303.html
            double speed01 = getCurrentSpeedKmHour() * 0.01;
            double roll = 0.005 + 1.0 / 2.0 * ( 0.01 + 0.0095 * ( speed01 * speed01 ) );
            
            btWheelContactPoint contactPtRoll( getRigidBody(), groundObject, 
                wheelInfo.m_raycastInfo.m_contactPointWS, m_forwardWS[i], 
                btScalar( roll * wheelInfo.m_wheelsSuspensionForce * timeStep ) );
                
            rollingFriction = calcContactFriction( contactPtRoll );

            
            ///////////////////////////////////////
            // Braking force
            ///////////////////////////////////////
            
            if( wheelInfo.m_brake )
            {
                btWheelContactPoint contactPtBr( getRigidBody(), groundObject, 
                    wheelInfo.m_raycastInfo.m_contactPointWS, m_forwardWS[i], 
                    wheelInfo.m_brake );
                    
                brakingFriction = calcContactFriction( contactPtBr );
            }

            
            ///////////////////////////////////////
            // Engine force
            ///////////////////////////////////////
            
            if ( wheelInfo.m_engineForce != 0.f )
                engineFriction = wheelInfo.m_engineForce * timeStep;
        

            ///////////////////////////////////////
            // Side Impulse
            ///////////////////////////////////////
            
            // Calculate the wheel's slip angle
            btVector3 wheelVel = getRigidBody()->getVelocityInLocalPoint( rel_pos );
            btScalar velX =  m_axle[i].dot( wheelVel );
            btScalar velZ = -m_axle[i].dot( wheelVel );
            btScalar slipAngle = btFabs( btAtan2( velX, btFabs( velZ ) ) );

            resolveSingleBilateral( *getRigidBody(), 
                wheelInfo.m_raycastInfo.m_contactPointWS, *groundObject,
                wheelInfo.m_raycastInfo.m_contactPointWS, btScalar(0.), 
                m_axle[i], m_sideImpulse[i], timeStep );


            ///////////////////////////////////////
            // Forward Impulse
            ///////////////////////////////////////

            m_forwardImpulse[i] = rollingFriction + engineFriction + brakingFriction;

            m_wheelInfo[i].m_skidInfo= btScalar(1.);
            
            btScalar maximp = wheelInfo.m_wheelsSuspensionForce * timeStep * wheelInfo.m_frictionSlip;
            btScalar maximpSide = maximp;

            btScalar maximpSquared = maximp * maximpSide;

            btScalar x = ( m_forwardImpulse[i] ) * 0.5f;
            btScalar y = ( m_sideImpulse[i] ) * 1.3f;
            
            btScalar impulseSquared = ( x * x + y * y );

            if( impulseSquared > maximpSquared )
            {        
                btScalar factor = maximp / btSqrt( impulseSquared );        
                m_wheelInfo[i].m_skidInfo *= factor;
                if( m_wheelInfo[i].m_skidInfo< btScalar(1.) )
                {
                    m_forwardImpulse[i] *= m_wheelInfo[i].m_skidInfo;
                    m_sideImpulse[i]    *= m_wheelInfo[i].m_skidInfo;
                }
            }

            if( slipAngle > btScalar(1.) )
            {
                //m_forwardImpulse[i] *= slipAngle;
                m_sideImpulse[i]    *= slipAngle;
            }


            ///////////////////////////////////////
            // Apply Impulses
            ///////////////////////////////////////

            btVector3 fwdVector = m_forwardWS[i] * m_forwardImpulse[i];
            
            if( !fwdVector.fuzzyZero() )
                getRigidBody()->applyImpulse( fwdVector, rel_pos );
                
            groundObject->applyImpulse( -fwdVector, rel_pos_ground );

            btVector3 sideImpulse = m_axle[i] * m_sideImpulse[i] * slipAngle;
            if( !sideImpulse.fuzzyZero() )
            {
                //btVector3 vChassisWorldUp = getRigidBody()->getCenterOfMassTransform().getBasis().getColumn( getUpAxis() );
                //btVector3 new_rel = rel_pos - vChassisWorldUp * ( vChassisWorldUp.dot( rel_pos ) * ( 1.f-wheelInfo.m_rollInfluence ) );
                
                getRigidBody()->applyImpulse( sideImpulse, rel_pos );

                //apply friction impulse on the ground
                groundObject->applyImpulse( -sideImpulse, rel_pos_ground );
            }
            
            ///////////////////////////////////////
            // Air resistance
            ///////////////////////////////////////
            
            btScalar constDrag = 0.0f, dragCoef = 0.36f, area = 1.549f, airDensity = 1.22f;
            btVector3 carVel = getRigidBody()->getLinearVelocity();
            
            constDrag = btScalar( 0.5f ) * dragCoef * area * airDensity;
            
            if( !carVel.fuzzyZero() )
            {
                btScalar drag = constDrag * carVel.length2();
                btVector3 carDrag = -1.0f * drag * carVel.normalized();
                getRigidBody()->applyCentralImpulse( carDrag * timeStep );
            }
        }
    }
}


btScalar calcContactFriction(btWheelContactPoint& contactPoint)
{

    btScalar j1=0.f;
    btScalar maxImpulse  = contactPoint.m_maxImpulse;
    
    const btVector3& contactPosWorld = contactPoint.m_frictionPositionWorld;

    btVector3 rel_pos1 = contactPosWorld - contactPoint.m_body0->getCenterOfMassPosition(); 
    btVector3 rel_pos2 = contactPosWorld - contactPoint.m_body1->getCenterOfMassPosition();
    
    btVector3 vel1 = contactPoint.m_body0->getVelocityInLocalPoint( rel_pos1 );
    btVector3 vel2 = contactPoint.m_body1->getVelocityInLocalPoint( rel_pos2 );
    btVector3 vel = vel1 - vel2;
    
    if( vel.fuzzyZero() )
        return btScalar( 0.0f );

    btScalar vrel = contactPoint.m_frictionDirectionWorld.dot( vel );

    // calculate j that moves us to zero relative velocity
    j1 = -vrel * contactPoint.m_jacDiagABInv;
    btSetMin( j1,  maxImpulse );
    btSetMax( j1, -maxImpulse );

    return j1;
}





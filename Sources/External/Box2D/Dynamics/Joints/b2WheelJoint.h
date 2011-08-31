/*
* Copyright (c) 2006-2011 Erin Catto http://www.box2d.org
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#ifndef B2_WHEEL_JOINT_H
#define B2_WHEEL_JOINT_H

#include <Box2D/Dynamics/Joints/b2Joint.h>

/// Wheel joint definition. This requires defining a line of
/// motion using an axis and an anchor point. The definition uses local
/// anchor points and a local axis so that the initial configuration
/// can violate the constraint slightly. The joint translation is zero
/// when the local anchor points coincide in world space. Using local
/// anchors and a local axis helps when saving and loading a game.
struct b2WheelJointDef : public b2JointDef
{
	b2WheelJointDef()
	{
		type = e_wheelJoint;
		localAnchorA.SetZero();
		localAnchorB.SetZero();
		localAxisA.Set(1.0f, 0.0f);
		enableMotor = false;
		maxMotorTorque = 0.0f;
		motorSpeed = 0.0f;
		frequencyHz = 2.0f;
		dampingRatio = 0.7f;
	}

	/// Initialize the bodies, anchors, axis, and reference angle using the world
	/// anchor and world axis.
	void Initialize(b2Body* bodyA, b2Body* bodyB, const b2Vec2& anchor, const b2Vec2& axis);

	/// The local anchor point relative to body1's origin.
	b2Vec2 localAnchorA;

	/// The local anchor point relative to body2's origin.
	b2Vec2 localAnchorB;

	/// The local translation axis in body1.
	b2Vec2 localAxisA;

	/// Enable/disable the joint motor.
	bool enableMotor;

	/// The maximum motor torque, usually in N-m.
	b2_float32 maxMotorTorque;

	/// The desired motor speed in radians per second.
	b2_float32 motorSpeed;

	/// Suspension frequency, zero indicates no suspension
	b2_float32 frequencyHz;

	/// Suspension damping ratio, one indicates critical damping
	b2_float32 dampingRatio;
};

/// A wheel joint. This joint provides two degrees of freedom: translation
/// along an axis fixed in body1 and rotation in the plane. You can use a
/// joint limit to restrict the range of motion and a joint motor to drive
/// the rotation or to model rotational friction.
/// This joint is designed for vehicle suspensions.
class b2WheelJoint : public b2Joint
{
public:
	b2Vec2 GetAnchorA() const;
	b2Vec2 GetAnchorB() const;

	b2Vec2 GetReactionForce(b2_float32 inv_dt) const;
	b2_float32 GetReactionTorque(b2_float32 inv_dt) const;

	/// Get the current joint translation, usually in meters.
	b2_float32 GetJointTranslation() const;

	/// Get the current joint translation speed, usually in meters per second.
	b2_float32 GetJointSpeed() const;

	/// Is the joint motor enabled?
	bool IsMotorEnabled() const;

	/// Enable/disable the joint motor.
	void EnableMotor(bool flag);

	/// Set the motor speed, usually in radians per second.
	void SetMotorSpeed(b2_float32 speed);

	/// Get the motor speed, usually in radians per second.
	b2_float32 GetMotorSpeed() const;

	/// Set/Get the maximum motor force, usually in N-m.
	void SetMaxMotorTorque(b2_float32 torque);
	b2_float32 GetMaxMotorTorque() const;

	/// Get the current motor torque given the inverse time step, usually in N-m.
	b2_float32 GetMotorTorque(b2_float32 inv_dt) const;

	/// Set/Get the spring frequency in hertz. Setting the frequency to zero disables the spring.
	void SetSpringFrequencyHz(b2_float32 hz);
	b2_float32 GetSpringFrequencyHz() const;

	/// Set/Get the spring damping ratio
	void SetSpringDampingRatio(b2_float32 ratio);
	b2_float32 GetSpringDampingRatio() const;

protected:

	friend class b2Joint;
	b2WheelJoint(const b2WheelJointDef* def);

	void InitVelocityConstraints(const b2SolverData& data);
	void SolveVelocityConstraints(const b2SolverData& data);
	bool SolvePositionConstraints(const b2SolverData& data);

	b2_float32 m_frequencyHz;
	b2_float32 m_dampingRatio;

	// Solver shared
	b2Vec2 m_localAnchorA;
	b2Vec2 m_localAnchorB;
	b2Vec2 m_localXAxisA;
	b2Vec2 m_localYAxisA;

	b2_float32 m_impulse;
	b2_float32 m_motorImpulse;
	b2_float32 m_springImpulse;

	b2_float32 m_maxMotorTorque;
	b2_float32 m_motorSpeed;
	bool m_enableMotor;

	// Solver temp
	b2_int32 m_indexA;
	b2_int32 m_indexB;
	b2Vec2 m_localCenterA;
	b2Vec2 m_localCenterB;
	b2_float32 m_invMassA;
	b2_float32 m_invMassB;
	b2_float32 m_invIA;
	b2_float32 m_invIB;

	b2Vec2 m_ax, m_ay;
	b2_float32 m_sAx, m_sBx;
	b2_float32 m_sAy, m_sBy;

	b2_float32 m_mass;
	b2_float32 m_motorMass;
	b2_float32 m_springMass;

	b2_float32 m_bias;
	b2_float32 m_gamma;
};

inline b2_float32 b2WheelJoint::GetMotorSpeed() const
{
	return m_motorSpeed;
}

inline b2_float32 b2WheelJoint::GetMaxMotorTorque() const
{
	return m_maxMotorTorque;
}

inline void b2WheelJoint::SetSpringFrequencyHz(b2_float32 hz)
{
	m_frequencyHz = hz;
}

inline b2_float32 b2WheelJoint::GetSpringFrequencyHz() const
{
	return m_frequencyHz;
}

inline void b2WheelJoint::SetSpringDampingRatio(b2_float32 ratio)
{
	m_dampingRatio = ratio;
}

inline b2_float32 b2WheelJoint::GetSpringDampingRatio() const
{
	return m_dampingRatio;
}

#endif

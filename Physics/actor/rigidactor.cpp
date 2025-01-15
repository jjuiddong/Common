
#include "stdafx.h"
#include "rigidactor.h"
#include "../manager/physicsengine.h"

using namespace phys;
using namespace physx;


cRigidActor::cRigidActor()
	: m_id(common::GenerateId())
	, m_actor(nullptr)
	, m_type(eRigidType::None)
	, m_shape(eShapeType::None)
{
}

cRigidActor::~cRigidActor()
{
	Clear();
}


bool cRigidActor::CreatePlane(cPhysicsEngine &physics
	, const Vector3 &norm
	, graphic::cNode *node //= nullptr
)
{
	PxSceneWriteLock scopedLock(*physics.m_scene);
	PxRigidStatic* plane = PxCreatePlane(*physics.m_physics
		, PxPlane(*(PxVec3*)&norm, 0), *physics.m_material);
	if (!plane)
		return false;

	plane->setActorFlag(PxActorFlag::eVISUALIZATION, true);

	m_type = eRigidType::Static;
	m_shape = eShapeType::Plane;
	m_actor = plane;
	if (node)
	{
		sMesh mesh;
		mesh.node = node;
		mesh.shape = eShapeType::Plane;
		mesh.local = Matrix44::Identity;
		m_meshes.push_back(mesh);
	}
	return true;
}


// create box
bool cRigidActor::CreateBox(cPhysicsEngine &physics
	, const Transform &tfm
	, const Vector3* linVel //= nullptr
	, const float density //= 1.f
	, const bool isKinematic //=false
	, graphic::cNode *node //= nullptr
)
{
	Quaternion q = tfm.rot;
	q.Normalize();
	const PxQuat rot = *(PxQuat*)&q;

	PxSceneWriteLock scopedLock(*physics.m_scene);
	PxRigidDynamic* box = PxCreateDynamic(*physics.m_physics
		, PxTransform(*(PxVec3*)&tfm.pos, rot)
		, PxBoxGeometry(*(PxVec3*)&tfm.scale), *physics.m_material, density);
	PX_ASSERT(box);

	DefaultRigidActorConfiguration(box);

	box->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, isKinematic);
	if (linVel)
		box->setLinearVelocity(*(PxVec3*)linVel);

	m_type = eRigidType::Dynamic;
	m_shape = eShapeType::Box;
	m_actor = box;
	if (node)
	{
		sMesh mesh;
		mesh.node = node;
		mesh.shape = eShapeType::Box;
		mesh.local = Matrix44::Identity;
		m_meshes.push_back(mesh);
	}	return true;
}


// create sphere rigidbody
bool cRigidActor::CreateSphere(cPhysicsEngine &physics
	, const Transform &tfm
	, const float radius
	, const Vector3* linVel //= nullptr
	, const float density //= 1.f
	, const bool isKinematic //=false
	, graphic::cNode *node //= nullptr
)
{
	PxSceneWriteLock scopedLock(*physics.m_scene);

	Quaternion q = tfm.rot;
	q.Normalize();
	const PxQuat rot = *(PxQuat*)&q;

	PxRigidDynamic* sphere = PxCreateDynamic(*physics.m_physics
		, PxTransform(*(PxVec3*)&tfm.pos, rot)
		, PxSphereGeometry(radius), *physics.m_material, density);
	PX_ASSERT(sphere);

	DefaultRigidActorConfiguration(sphere);

	sphere->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, isKinematic);
	if (linVel)
		sphere->setLinearVelocity(*(PxVec3*)linVel);

	m_type = eRigidType::Dynamic;
	m_shape = eShapeType::Sphere;
	m_actor = sphere;
	if (node)
	{
		sMesh mesh;
		mesh.node = node;
		mesh.shape = eShapeType::Sphere;
		mesh.local = Matrix44::Identity;
		m_meshes.push_back(mesh);
	}
	return true;
}


// create capsuel rigidbody
bool cRigidActor::CreateCapsule(cPhysicsEngine &physics
	, const Transform &tfm
	, const float radius
	, const float halfHeight
	, const Vector3* linVel //= nullptr
	, const float density //= 1.f
	, const bool isKinematic //=false
	, graphic::cNode *node //= nullptr
)
{
	Quaternion q = tfm.rot;
	q.Normalize();
	const PxQuat rot = *(PxQuat*)&q;

	PxSceneWriteLock scopedLock(*physics.m_scene);

	PxRigidDynamic* capsule = PxCreateDynamic(*physics.m_physics
		, PxTransform(*(PxVec3*)&tfm.pos, rot)
		, PxCapsuleGeometry(radius, halfHeight), *physics.m_material, density);
	PX_ASSERT(capsule);

	DefaultRigidActorConfiguration(capsule);
	capsule->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, isKinematic);

	if (linVel)
		capsule->setLinearVelocity(*(PxVec3*)linVel);

	m_type = eRigidType::Dynamic;
	m_shape = eShapeType::Capsule;
	m_actor = capsule;
	if (node)
	{
		sMesh mesh;
		mesh.node = node;
		mesh.shape = eShapeType::Capsule;
		mesh.local = Matrix44::Identity;
		m_meshes.push_back(mesh);
	}
	return true;
}


// create cylinder rigidbody
bool cRigidActor::CreateCylinder(cPhysicsEngine &physics
	, const Transform &tfm
	, const float radius
	, const float height
	, const Vector3* linVel //= nullptr
	, const float density //= 1.f
	, const bool isKinematic //= false
	, graphic::cNode *node //= nullptr
)
{
	using namespace physx;

	Quaternion q = tfm.rot;
	q.Normalize();
	const PxQuat rot = *(PxQuat*)&q;

	PxSceneWriteLock scopedLock(*physics.m_scene);
	
	PxConvexMesh* convexMesh = GenerateCylinderMesh(physics, radius, height);

	PxRigidDynamic* convex = PxCreateDynamic(*physics.m_physics
		, PxTransform(*(PxVec3*)&tfm.pos, rot)
		, PxConvexMeshGeometry(convexMesh), *physics.m_material, density);
	PX_ASSERT(convex);

	DefaultRigidActorConfiguration(convex);

	convex->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, isKinematic);
	if (linVel)
		convex->setLinearVelocity(*(PxVec3*)linVel);

	m_type = eRigidType::Dynamic;
	m_shape = eShapeType::Cylinder;
	m_actor = convex;
	if (node)
	{
		sMesh mesh;
		mesh.node = node;
		mesh.shape = eShapeType::Cylinder;
		mesh.local = Matrix44::Identity;
		m_meshes.push_back(mesh);
	}
	return true;
}


// create cylinder convex mesh
physx::PxConvexMesh* cRigidActor::GenerateCylinderMesh(cPhysicsEngine &physics
	, const float radius, const float height)
{
	const int slices = 10;
	graphic::cCylinderShape shape;
	const int numConeVertices = (slices * 2 + 1) * 2;
	const int numConeIndices = slices * 2 * 6;
	const int numCapsuleVertices = numConeVertices;
	const int numCapsuleIndices = numConeIndices;

	vector<Vector3> conePositions(numConeVertices);
	vector<unsigned short> coneIndices(numConeIndices);
	shape.GenerateConeMesh2(slices, radius, height, &conePositions[0], &coneIndices[0], 0, false);

	PxConvexMeshDesc convexDesc;
	convexDesc.points.count = numConeVertices;
	convexDesc.points.stride = sizeof(PxVec3);
	convexDesc.points.data = &conePositions[0];
	convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;
	PxConvexMesh* convexMesh = physics.m_cooking->createConvexMesh(convexDesc
		, physics.m_physics->getPhysicsInsertionCallback());

	return convexMesh;
}


void cRigidActor::DefaultRigidActorConfiguration(physx::PxRigidDynamic *actor)
{
	actor->setLinearDamping(0.5f);
	actor->setAngularDamping(0.5f);
	actor->setActorFlag(PxActorFlag::eVISUALIZATION, true);
}


// change geometry shape
bool cRigidActor::ChangeDimension(cPhysicsEngine &physics, const Vector3 &dim)
{
	RETV(!m_actor, false);

	// 1. remove old shape
	const uint count = m_actor->getNbShapes();
	if (count == 0)
		return false;

	PxShape **shapes = new PxShape*[count];
	const uint result = m_actor->getShapes(shapes, count);
	for (uint i = 0; i < result; ++i)
		m_actor->detachShape(*shapes[i]);
	delete[] shapes;

	// 2. generate new shape
	switch (m_shape)
	{
	case eShapeType::Box:
		physx::PxRigidActorExt::createExclusiveShape(
			*m_actor, PxBoxGeometry(*(PxVec3*)&dim), *physics.m_material);
		break;
	case eShapeType::Sphere:
		physx::PxRigidActorExt::createExclusiveShape(
			*m_actor, PxSphereGeometry(dim.x), *physics.m_material);
		break;
	case eShapeType::Capsule:
		physx::PxRigidActorExt::createExclusiveShape(
			*m_actor, PxCapsuleGeometry(dim.y, dim.x), *physics.m_material);
		break;
	case eShapeType::Cylinder:
	{
		const float radius = dim.y;
		const float height = dim.x;
		PxConvexMesh* convexMesh = GenerateCylinderMesh(physics, radius, height);
		physx::PxRigidActorExt::createExclusiveShape(
			*m_actor, PxConvexMeshGeometry(convexMesh), *physics.m_material);
	}
	break;

	case eShapeType::Plane: // not support
	case eShapeType::Convex: // not support
		assert(0);
		break;
	default: assert(0); break;
	}

	// 3. update mass
	// todo: density modified
	const float  density = 1.f;
	if (PxRigidBody *p = m_actor->is<PxRigidBody>())
		PxRigidBodyExt::updateMassAndInertia(*p, density);

	// update joint localTransform
	for (auto &j : m_joints)
	{
		Transform &tfm = (j->m_actor0 == this) ?
			j->m_actorLocal0 : j->m_actorLocal1;
		switch (m_shape)
		{
		case eShapeType::Box:
			tfm.scale = dim;
			break;
		case eShapeType::Sphere:
			tfm.scale = Vector3::Ones * dim.x;
			break;
		case eShapeType::Capsule:
			tfm.scale = Vector3(dim.x + dim.y, dim.y, dim.y);
			break;

		case eShapeType::Plane: // not support
		case eShapeType::Convex: // not support
			assert(0);
			break;
		default: assert(0); break;
		}
	}

	return true;
}


// Compound this and src
// move src mesh to this actor
bool cRigidActor::Compound(cPhysicsEngine &physics, cRigidActor *src)
{
	const uint count = src->m_actor->getNbShapes();
	if (count == 0)
		return false;
	if (count != src->m_meshes.size())
		return false; // error occurred!

	PxShape **shapes = new PxShape*[count];
	if (src->m_actor->getShapes(shapes, count) == 0)
	{
		delete[] shapes;
		return true;
	}
	
	const Matrix44 rtm0 = Transform(m_meshes[0].node->m_transform.pos
		, m_meshes[0].node->m_transform.rot).GetMatrix().Inverse();
	
	for (uint i=0; i < src->m_meshes.size(); ++i)
	{
		const sMesh &mesh = src->m_meshes[i];
		physx::PxShape *shape = shapes[i];

		// calc local tm
		sMesh newMesh = mesh;
		const Matrix44 tm = Transform(mesh.node->m_transform.pos
			, mesh.node->m_transform.rot).GetMatrix() * rtm0;
		newMesh.local = tm;

		const Transform tfm(tm.GetPosition(), tm.GetQuaternion());
		const PxTransform target = PxTransform(*(PxVec3*)&tfm.pos, *(PxQuat*)&tfm.rot);

		physx::PxShape* newShape = nullptr;
		switch (mesh.shape)
		{
		case eShapeType::Box:
			newShape = PxRigidActorExt::createExclusiveShape(*m_actor
				, shape->getGeometry().box()
				, *physics.m_material);
			break;
		case eShapeType::Sphere:
			newShape = PxRigidActorExt::createExclusiveShape(*m_actor
				, shape->getGeometry().sphere()
				, *physics.m_material);
			break;
		case eShapeType::Capsule:
			newShape = PxRigidActorExt::createExclusiveShape(*m_actor
				, shape->getGeometry().capsule()
				, *physics.m_material);
			break;
		case eShapeType::Cylinder:
		{
			newShape = PxRigidActorExt::createExclusiveShape(*m_actor
				, shape->getGeometry().convexMesh()
				, *physics.m_material);
		}
		break;

		case eShapeType::Plane: // not support
		case eShapeType::Convex: // not support
			assert(0);
			break;
		default: assert(0); break;
		}

		newShape->setLocalPose(target);
		m_meshes.push_back(newMesh);
	}

	// todo: density modified
	const float density = 1.f;
	if (PxRigidBody *p = m_actor->is<PxRigidBody>())
		PxRigidBodyExt::updateMassAndInertia(*p, density);

	src->m_meshes.clear();
	delete[] shapes;
	return true;
}


bool cRigidActor::SetGlobalPose(const physx::PxTransform &tfm)
{
	if (PxRigidDynamic *p = m_actor->is<PxRigidDynamic>())
		p->setGlobalPose(tfm);

	// not working setKinematicTarget() function
	//const PxRigidBodyFlags flags = p->getRigidBodyFlags();
	//const bool isKinematic = flags.isSet(PxRigidBodyFlag::eKINEMATIC);
	//if (isKinematic)
	//	p->setKinematicTarget(tfm);
	return true;
}


bool cRigidActor::SetGlobalPose(const Transform &tfm)
{
	const PxTransform target = PxTransform(*(PxVec3*)&tfm.pos, *(PxQuat*)&tfm.rot);
	return SetGlobalPose(target);
}


Transform cRigidActor::GetGlobalPose()
{
	const PxTransform tm = m_actor->getGlobalPose();
	const Transform tfm = Transform(*(Vector3*)&tm.p) * Transform(*(Quaternion*)&tm.q);
	return tfm;
}


bool cRigidActor::SetKinematic(const bool isKinematic)
{
	RETV(!m_actor, false);
	RETV(m_type != eRigidType::Dynamic, false);

	if (PxRigidDynamic *p = m_actor->is<PxRigidDynamic>())
		p->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, isKinematic);

	if (!isKinematic)
		WakeUp();
	return true;
}


bool cRigidActor::IsKinematic() const
{
	RETV(!m_actor, false);
	RETV(m_type != eRigidType::Dynamic, false);

	if (PxRigidDynamic *p = m_actor->is<PxRigidDynamic>())
	{
		const PxRigidBodyFlags flags = p->getRigidBodyFlags();
		const bool isKinematic = flags.isSet(PxRigidBodyFlag::eKINEMATIC);
		return isKinematic;
	}
	return true;
}


float cRigidActor::GetMass() const
{
	if (PxRigidDynamic *p = m_actor->is<PxRigidDynamic>())
		return p->getMass();
	return 0.f;
}


bool cRigidActor::SetMass(const float mass)
{
	if (PxRigidDynamic *p = m_actor->is<PxRigidDynamic>())
		p->setMass(mass);
	return true;
}


float cRigidActor::GetLinearDamping() const
{
	if (PxRigidDynamic *p = m_actor->is<PxRigidDynamic>())
		return p->getLinearDamping();
	return 0.f;
}


bool cRigidActor::SetLinearDamping(const float damping)
{
	if (PxRigidDynamic *p = m_actor->is<PxRigidDynamic>())
		p->setLinearDamping(damping);
	return true;
}


float cRigidActor::GetAngularDamping() const
{
	if (PxRigidDynamic *p = m_actor->is<PxRigidDynamic>())
		return p->getAngularDamping();
	return 0.f;
}


bool cRigidActor::SetAngularDamping(const float damping)
{
	if (PxRigidDynamic *p = m_actor->is<PxRigidDynamic>())
		p->setAngularDamping(damping);
	return true;
}


bool cRigidActor::SetMaxAngularVelocity(const float maxVel)
{
	if (PxRigidDynamic *p = m_actor->is<PxRigidDynamic>())
		p->setMaxAngularVelocity(maxVel);
	return true;
}


float cRigidActor::GetMaxAngularVelocity()
{
	if (PxRigidDynamic *p = m_actor->is<PxRigidDynamic>())
		return p->getMaxAngularVelocity();
	return 0.f;
}


bool cRigidActor::SetLinearVelocity(const Vector3 velocity)
{
	if (PxRigidDynamic *p = m_actor->is<PxRigidDynamic>())
		p->setLinearVelocity(*(PxVec3*)&velocity);
	return true;
}


bool cRigidActor::SetAngularVelocity(const Vector3 velocity)
{
	if (PxRigidDynamic *p = m_actor->is<PxRigidDynamic>())
		p->setAngularVelocity(*(PxVec3*)&velocity);
	return true;
}


// clear linear, torque force
bool cRigidActor::ClearForce()
{
	if (PxRigidDynamic *p = m_actor->is<PxRigidDynamic>())
	{
		p->clearForce();
		p->clearTorque();
	}
	return true;
}


bool cRigidActor::WakeUp()
{
	if (PxRigidDynamic *p = m_actor->is<PxRigidDynamic>())
		p->wakeUp();
	return true;
}


// add joint reference
// joint is this rigid actor connection joint
bool cRigidActor::AddJoint(cJoint *j1)
{
	if (m_joints.end() == std::find(m_joints.begin(), m_joints.end(), j1))
	{
		m_joints.push_back(j1);
		return true;
	}
	return false;
}


// remove joint reference
bool cRigidActor::RemoveJoint(cJoint *j1)
{
	if (m_joints.end() != std::find(m_joints.begin(), m_joints.end(), j1))
	{
		common::removevector(m_joints, j1);
		return true;
	}
	return false;
}


void cRigidActor::Clear()
{
	m_joints.clear();
	PHY_SAFE_RELEASE(m_actor);
	for (auto &mesh : m_meshes)
		SAFE_DELETE(mesh.node);
	m_meshes.clear();
}

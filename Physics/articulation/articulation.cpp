
#include "stdafx.h"
#include "articulation.h"

using namespace phys;
using namespace physx;


cArticulation::cArticulation()
	: m_art(nullptr)
{
}

cArticulation::~cArticulation()
{
	Clear();
}


// initialize articulation object
bool cArticulation::Create(cPhysicsEngine& physics
	, const bool isFixed //= true
	, const int solverIterationCount //= 32,
)
{
	RETV(!physics.m_physics, false);
	RETV(!physics.m_scene, false);

	Clear();

	m_art = physics.m_physics->createArticulationReducedCoordinate();
	if (isFixed)
		m_art->setArticulationFlag(PxArticulationFlag::eFIX_BASE, true);
	m_art->setSolverIterationCounts(solverIterationCount);
	return true;
}


// create and add box link
// parentLinkId: parent link id, -1=root
// tfm: link pos, scale, rotation
// mass: link mass
// return: link id, -1:fail return
int cArticulation::AddBoxLink(cPhysicsEngine& physics
	, const int parentLinkId, const Transform& tfm
	, const float mass //= 1.f
	, physx::PxMaterial* material //= nullptr
)
{
	RETV(!m_art, -1);

	PxArticulationLink* parent = nullptr;
	if (parentLinkId >= 0)
		parent = GetLink(parentLinkId);

	PxArticulationLink* link = m_art->createLink(parent
		, PxTransform(*(PxVec3*)&tfm.pos, *(PxQuat*)&tfm.rot));

	PxRigidActorExt::createExclusiveShape(*link
		, PxBoxGeometry(tfm.scale.x, tfm.scale.y, tfm.scale.z)
		, material? *material : *physics.m_material);

	PxRigidBodyExt::updateMassAndInertia(*link, mass);

	const int id = common::GenerateId();
	sLinkInfo info;
	info.type = eShapeType::Box;
	info.link = link;
	info.scale = tfm.scale;
	m_links.insert({ id, info });
	return id;
}


// create and add sphere link
// parentLinkId: parent link id, -1=root
// tfm: link pos, scale, rotation
// radius: sphere radius
// mass: link mass
// return: link id, -1:fail return
int cArticulation::AddSphereLink(cPhysicsEngine& physics
	, const int parentLinkId
	, const Transform& tfm
	, const float radius
	, const float mass //= 1.f
	, physx::PxMaterial* material //= nullptr
)
{
	RETV(!m_art, -1);

	PxArticulationLink* parent = nullptr;
	if (parentLinkId >= 0)
		parent = GetLink(parentLinkId);

	PxArticulationLink* link = m_art->createLink(parent
		, PxTransform(*(PxVec3*)&tfm.pos, *(PxQuat*)&tfm.rot));

	physx::PxRigidActorExt::createExclusiveShape(*link
		, PxSphereGeometry(radius)
		, material ? *material : *physics.m_material);

	PxRigidBodyExt::updateMassAndInertia(*link, mass);

	const int id = common::GenerateId();
	sLinkInfo info;
	info.type = eShapeType::Sphere;
	info.link = link;
	info.radius = radius;
	m_links.insert({ id, info });
	return id;
}


// create and add capsule link
// parentLinkId: parent link id, -1=root
// tfm: link pos, scale, rotation
// radius: capsule radius
// halfHeight: capsule half height
// mass: link mass
// return: link id, -1:fail return
int cArticulation::AddCapsuleLink(cPhysicsEngine& physics
	, const int parentLinkId
	, const Transform& tfm
	, const float radius
	, const float halfHeight
	, const float mass //= 1.f
	, physx::PxMaterial* material //= nullptr
)
{
	RETV(!m_art, -1);

	PxArticulationLink* parent = nullptr;
	if (parentLinkId >= 0)
		parent = GetLink(parentLinkId);

	PxArticulationLink* link = m_art->createLink(parent
		, PxTransform(*(PxVec3*)&tfm.pos, *(PxQuat*)&tfm.rot));

	physx::PxRigidActorExt::createExclusiveShape(*link
		, PxCapsuleGeometry(radius, halfHeight)
		, material ? *material : *physics.m_material);

	PxRigidBodyExt::updateMassAndInertia(*link, mass);

	const int id = common::GenerateId();
	sLinkInfo info;
	info.type = eShapeType::Capsule;
	info.link = link;
	info.radius = radius;
	info.halfHeight = halfHeight;
	m_links.insert({ id, info });
	return id;
}


// create and add cylinder link
// parentLinkId: parent link id, -1=root
// tfm: link pos, scale, rotation
// radius: cylinder radius
// height: cylinder height
// mass: link mass
// return: link id, -1:fail return
int cArticulation::AddCylinderLink(cPhysicsEngine& physics
	, const int parentLinkId
	, const Transform& tfm
	, const float radius
	, const float height
	, const float mass //= 1.f
	, physx::PxMaterial* material //= nullptr
)
{
	RETV(!m_art, -1);

	PxArticulationLink* parent = nullptr;
	if (parentLinkId >= 0)
		parent = GetLink(parentLinkId);

	PxArticulationLink* link = m_art->createLink(parent
		, PxTransform(*(PxVec3*)&tfm.pos, *(PxQuat*)&tfm.rot));

	PxConvexMesh* convexMesh = GenerateCylinderMesh(physics, radius, height);
	physx::PxRigidActorExt::createExclusiveShape(*link
		, PxConvexMeshGeometry(convexMesh)
		, material ? *material : *physics.m_material);

	PxRigidBodyExt::updateMassAndInertia(*link, mass);

	const int id = common::GenerateId();
	sLinkInfo info;
	info.type = eShapeType::Cylinder;
	info.link = link;
	info.radius = radius;
	info.height = height;
	m_links.insert({ id, info });
	return id;
}


// add articulation to scene
bool cArticulation::AddScene(cPhysicsEngine& physics)
{
	RETV(!physics.m_scene || !m_art, false);
	physics.m_scene->addArticulation(*m_art);
	return true;
}


// return link
physx::PxArticulationLink* cArticulation::GetLink(const int linkId)
{
	auto it = m_links.find(linkId);
	if (m_links.end() == it)
		return nullptr;
	return it->second.link;
}


// set attribute
bool cArticulation::SetAttribute(const float linearDamping, const float angularDamping
	, const float maxLinearVelocity, const float maxAngularVelocity)
{
	RETV(!m_art, false);

	for (uint i = 0; i < m_art->getNbLinks(); ++i)
	{
		PxArticulationLink* link;
		m_art->getLinks(&link, 1, i);

		link->setLinearDamping(linearDamping);
		link->setAngularDamping(angularDamping);

		link->setMaxAngularVelocity(maxAngularVelocity);
		link->setMaxLinearVelocity(maxLinearVelocity);
	}
	return true;
}


// add joint
physx::PxArticulationJointReducedCoordinate* cArticulation::AddJoint(
	const int childLinkId
	, const eJointType jointType
	, const Vector3& parentPivot0
	, const Vector3& childPivot1
	, const Vector3 axis //= Vector3::Zeroes
)
{
	RETV(!m_art, nullptr);
	PxArticulationLink* child = GetLink(childLinkId);
	RETV(!child, nullptr);

	PxArticulationJointReducedCoordinate* joint = child->getInboundJoint();
	PxArticulationLink* parent = &joint->getParentArticulationLink();

	const PxTransform parentTm = parent->getGlobalPose();
	const PxTransform childTm = child->getGlobalPose();

	Transform worldTfm0;
	worldTfm0.pos = *(Vector3*)&parentTm.p;
	worldTfm0.rot = *(Quaternion*)&parentTm.q;
	Transform worldTfm1;
	worldTfm1.pos = *(Vector3*)&childTm.p;
	worldTfm1.rot = *(Quaternion*)&childTm.q;

	PxTransform localFrame0, localFrame1;
	const Vector3 jointPos = (parentPivot0 + childPivot1) / 2.f;

	Vector3 revoluteAxis;
	PxArticulationJointType::Enum jtype;

	switch (jointType)
	{
	case eJointType::Fixed:
		jtype = PxArticulationJointType::eFIX;
		break;
	case eJointType::Spherical:
		jtype = PxArticulationJointType::eSPHERICAL;
		break;
	case eJointType::Revolute:
		revoluteAxis = axis;
		jtype = PxArticulationJointType::eREVOLUTE;
		break;
	case eJointType::Prismatic:
		jtype = PxArticulationJointType::ePRISMATIC;
		break;
	case eJointType::Distance:
	case eJointType::D6:
		return nullptr; // error return
	default: break; // nothing
	}

	GetLocalFrame(worldTfm0, worldTfm1, jointPos, revoluteAxis
		, localFrame0, localFrame1);

	joint->setJointType(jtype);
	joint->setParentPose(localFrame0);
	joint->setChildPose(localFrame1);
	return joint;
}


// add d6 joint
physx::PxD6Joint* cArticulation::AddD6Joint(
	cPhysicsEngine& physics
	, const int linkId0, const Vector3& pivot0
	, const int linkId1, const Vector3& pivot1
)
{
	RETV(!m_art, nullptr);
	PxArticulationLink* link0 = GetLink(linkId0);
	PxArticulationLink* link1 = GetLink(linkId1);
	RETV(!link0 || !link1, nullptr);

	const PxTransform linkTm0 = link0->getGlobalPose();
	const PxTransform linkTm1 = link1->getGlobalPose();

	Transform worldTfm0;
	worldTfm0.pos = *(Vector3*)&linkTm0.p;
	worldTfm0.rot = *(Quaternion*)&linkTm0.q;
	Transform worldTfm1;
	worldTfm1.pos = *(Vector3*)&linkTm1.p;
	worldTfm1.rot = *(Quaternion*)&linkTm1.q;

	PxTransform localFrame0, localFrame1;
	const Vector3 jointPos = (pivot0 + pivot1) / 2.f;
	GetLocalFrame(worldTfm0, worldTfm1, jointPos
		, Vector3::Zeroes, localFrame0, localFrame1);

	PxD6Joint* joint = PxD6JointCreate(*physics.m_physics
		, link0, localFrame0, link1, localFrame1);

	return joint;
}


// set joint velocity drive parameter
bool cArticulation::SetJointDriveVelocityByLink(const int linkId
	, const float damping, const float stiffness, const float maxForce, const float velocity)
{
	PxArticulationLink *link = GetLink(linkId);
	if (!link) return false; // error return

	PxArticulationJointReducedCoordinate* joint = link->getInboundJoint();
	return SetJointDriveVelocity(joint, damping, stiffness, maxForce, velocity);
}


// set joint velocity drive parameter
bool cArticulation::SetJointDriveVelocity(physx::PxArticulationJointReducedCoordinate* joint
	, const float damping, const float stiffness, const float maxForce, const float velocity)
{
	PxArticulationDrive param;
	param.stiffness = stiffness;
	param.damping = damping;
	param.maxForce = maxForce;
	param.driveType = PxArticulationDriveType::eVELOCITY;

	joint->setDriveParams(PxArticulationAxis::eTWIST, param);
	joint->setMotion(PxArticulationAxis::eTWIST, PxArticulationMotion::eFREE);
	joint->setDriveVelocity(PxArticulationAxis::eTWIST, velocity);
	return true;
}


// set joint drive target parameter by link id
bool cArticulation::SetJointDriveTargetByLink(const int linkId
	, const float damping, const float stiffness, const float maxForce, const float target)
{
	PxArticulationLink* link = GetLink(linkId);
	if (!link) return false; // error return

	PxArticulationJointReducedCoordinate* joint = link->getInboundJoint();
	return SetJointDriveTarget(joint, damping, stiffness, maxForce, target);
}


// set joint drive target parameter
bool cArticulation::SetJointDriveTarget(physx::PxArticulationJointReducedCoordinate* joint
	, const float damping, const float stiffness, const float maxForce, const float target)
{
	PxArticulationDrive param;
	param.stiffness = stiffness;
	param.damping = damping;
	param.maxForce = maxForce;
	param.driveType = PxArticulationDriveType::eTARGET;

	joint->setDriveParams(PxArticulationAxis::eTWIST, param);
	joint->setMotion(PxArticulationAxis::eTWIST, PxArticulationMotion::eFREE);
	joint->setDriveTarget(PxArticulationAxis::eTWIST, target);
	return true;
}


// render articulation object  (wire frame render)
bool cArticulation::Render(graphic::cRenderer & renderer
	, const XMMATRIX & parentTm //= XMIdentity
	, const int flags //= 1
)
{
	using namespace graphic;

	renderer.m_cube.SetColor(cColor::WHITE);
	renderer.m_sphere.SetColor(cColor::WHITE);
	renderer.m_capsule.SetColor(cColor::WHITE);
	renderer.m_cylinder.SetColor(cColor::WHITE);

	for (auto& kv : m_links)
	{
		const phys::cArticulation::sLinkInfo& info = kv.second;
		switch (info.type)
		{
		case phys::eShapeType::Box:
		{
			const PxTransform pose = info.link->getGlobalPose();
			const Transform tm = Transform(Vector3(), info.scale) *
				Transform(*(Quaternion*)&pose.q) * Transform(*(Vector3*)&pose.p);
			renderer.m_cube.SetCube(tm);
			renderer.m_cube.Render(renderer, parentTm, flags);
			renderer.m_cube.Render(renderer, parentTm, flags | eRenderFlag::WIREFRAME);
		}
		break;

		case phys::eShapeType::Sphere:
		{
			const PxTransform pose = info.link->getGlobalPose();
			renderer.m_sphere.SetPos(*(Vector3*)&pose.p);
			renderer.m_sphere.SetRadius(info.radius);
			renderer.m_sphere.Render(renderer, parentTm, flags);
			renderer.m_sphere.Render(renderer, parentTm, flags | eRenderFlag::WIREFRAME);
		}
		break;

		case phys::eShapeType::Capsule:
		{
			const PxTransform pose = info.link->getGlobalPose();
			const Transform tm = Transform(*(Quaternion*)&pose.q) * Transform(*(Vector3*)&pose.p);
			renderer.m_capsule.m_transform.pos = tm.pos;
			renderer.m_capsule.m_transform.rot = tm.rot;
			renderer.m_capsule.SetDimension(info.radius, info.halfHeight);
			renderer.m_capsule.Render(renderer, parentTm, flags);
			renderer.m_capsule.Render(renderer, parentTm, flags | eRenderFlag::WIREFRAME);
		}
		break;

		case phys::eShapeType::Cylinder:
		{
			const PxTransform pose = info.link->getGlobalPose();
			const Transform tm = Transform(*(Quaternion*)&pose.q) * Transform(*(Vector3*)&pose.p);
			renderer.m_cylinder.m_transform.pos = tm.pos;
			renderer.m_cylinder.m_transform.rot = tm.rot;
			renderer.m_cylinder.SetDimension(info.radius, info.height);
			renderer.m_cylinder.Render(renderer, parentTm, flags);
			renderer.m_cylinder.Render(renderer, parentTm, flags | eRenderFlag::WIREFRAME);
		}
		break;

		default: break;
		}
	}

	return true;
}


// set kinematic
bool cArticulation::SetKinematic(const bool isKinematic)
{
	RETV(!m_art, false);
	for (auto& kv : m_links)
	{
		sLinkInfo& info = kv.second;
		info.link->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, isKinematic);
	}
	return true;
}


// set global pose root link
bool cArticulation::SetGlobalPose(const Transform& tfm)
{
	RETV(!m_art, false);

	using namespace physx;
	PxTransform tm;
	tm.p = *(PxVec3*)&tfm.pos;
	tm.q = *(PxQuat*)&tfm.rot;
	m_art->setRootGlobalPose(tm);
	return true;
}


// return global pose root link
Transform cArticulation::GetGlobalPose() const
{
	Transform tfm;
	RETV(!m_art, tfm);

	using namespace physx;
	PxTransform tm = m_art->getRootGlobalPose();
	tfm.pos = *(Vector3*)&tm.p;
	tfm.rot = *(Quaternion*)&tm.q;
	return tfm;
}


// create cylinder convex mesh
physx::PxConvexMesh* cArticulation::GenerateCylinderMesh(cPhysicsEngine& physics
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

	PxTolerancesScale scale;
	PxCookingParams params(scale);
	PxConvexMeshDesc convexDesc;
	convexDesc.points.count = numConeVertices;
	convexDesc.points.stride = sizeof(PxVec3);
	convexDesc.points.data = &conePositions[0];
	convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;
	PxConvexMesh* convexMesh = PxCreateConvexMesh(params, convexDesc
		, physics.m_physics->getPhysicsInsertionCallback());
	return convexMesh;
}


// calc localFrame for PxJoint~ Function seriese
// worldTm0 : actor0 world transform
// worldTm1 : actor1 world transform
// revoluteAxis : revolution Axis
//			      if ZeroVector, ignore revolute axis
// out0 : return actor0 localFrame
// out1 : return actor1 localFrame
void cArticulation::GetLocalFrame(const Transform& worldTm0, const Transform& worldTm1
	, const Vector3& jointPos, const Vector3& revoluteAxis
	, OUT physx::PxTransform& out0, OUT physx::PxTransform& out1)
{
	Transform tfm0 = worldTm0;
	Transform tfm1 = worldTm1;

	Vector3 p0 = jointPos - tfm0.pos;
	Vector3 p1 = jointPos - tfm1.pos;
	Quaternion q0 = tfm0.rot.Inverse();
	Quaternion q1 = tfm1.rot.Inverse();

	if (revoluteAxis != Vector3::Zeroes)
	{
		Quaternion rot(revoluteAxis, Vector3(1, 0, 0));
		tfm0.rot *= rot;
		tfm1.rot *= rot;

		p0 = (tfm0.pos - jointPos) * rot + jointPos;
		p1 = (tfm1.pos - jointPos) * rot + jointPos;
		p0 = jointPos - p0;
		p1 = jointPos - p1;
		q0 = tfm0.rot.Inverse();
		q1 = tfm1.rot.Inverse();
	}

	const PxTransform localFrame0 = PxTransform(*(PxQuat*)&q0) * PxTransform(*(PxVec3*)&p0);
	const PxTransform localFrame1 = PxTransform(*(PxQuat*)&q1) * PxTransform(*(PxVec3*)&p1);

	out0 = localFrame0;
	out1 = localFrame1;
}


// clear
void cArticulation::Clear(cPhysicsEngine *physics)
{
	m_links.clear();

	if (physics && physics->m_scene && m_art)
		physics->m_scene->removeArticulation(*m_art);
	SAFE_RELEASE2(m_art);
}


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
	RETV(!physics.s_physics, false);
	RETV(!physics.m_scene, false);

	Clear();

	m_art = physics.s_physics->createArticulationReducedCoordinate();
	if (isFixed)
		m_art->setArticulationFlag(PxArticulationFlag::eFIX_BASE, true);
	m_art->setSolverIterationCounts(solverIterationCount);
	return true;
}


// create and add box link
// parentLinkId: parent link id, -1=root
// tfm: box pos, dimension, rotation (world space)
// mass: box mass
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
	RETV(!link, -1);

	PxRigidActorExt::createExclusiveShape(*link
		, PxBoxGeometry(tfm.scale.x / 2.f, tfm.scale.y / 2.f, tfm.scale.z / 2.f)
		, material? *material : *physics.m_material);

	PxRigidBodyExt::updateMassAndInertia(*link, mass);

	const int id = common::GenerateId();
	sLinkInfo info;
	info.id = id;
	info.type = eShapeType::Box;
	info.link = link;
	info.scale = tfm.scale;
	m_links.insert({ id, info });
	return id;
}


// create and add sphere link
// parentLinkId: parent link id, -1=root
// tfm: link pos, scale, rotation (world space)
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
	RETV(!link, -1);

	physx::PxRigidActorExt::createExclusiveShape(*link
		, PxSphereGeometry(radius)
		, material ? *material : *physics.m_material);

	PxRigidBodyExt::updateMassAndInertia(*link, mass);

	const int id = common::GenerateId();
	sLinkInfo info;
	info.id = id;
	info.type = eShapeType::Sphere;
	info.link = link;
	info.radius = radius;
	m_links.insert({ id, info });
	return id;
}


// create and add capsule link
// parentLinkId: parent link id, -1=root
// tfm: link pos, scale, rotation (world space)
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
	RETV(!link, -1);

	physx::PxRigidActorExt::createExclusiveShape(*link
		, PxCapsuleGeometry(radius, halfHeight)
		, material ? *material : *physics.m_material);

	PxRigidBodyExt::updateMassAndInertia(*link, mass);

	const int id = common::GenerateId();
	sLinkInfo info;
	info.id = id;
	info.type = eShapeType::Capsule;
	info.link = link;
	info.radius = radius;
	info.halfHeight = halfHeight;
	m_links.insert({ id, info });
	return id;
}


// create and add cylinder link (y-axis height cylinder)
// parentLinkId: parent link id, -1=root
// tfm: link pos, scale, rotation (world space)
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
	RETV(!link, -1);

	PxConvexMesh* convexMesh = GenerateCylinderMesh(physics, radius, height);
	physx::PxRigidActorExt::createExclusiveShape(*link
		, PxConvexMeshGeometry(convexMesh)
		, material ? *material : *physics.m_material);

	PxRigidBodyExt::updateMassAndInertia(*link, mass);

	const int id = common::GenerateId();
	sLinkInfo info;
	info.id = id;
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
// parentPivot0, childPivot1: world space
// axis: rotate axis, world space
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
	RETV(!joint, nullptr);
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
	m_joints.push_back(joint);
	return joint;
}


// add d6 joint
// pivot0, pivot1: (world space)
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

	PxD6Joint* joint = PxD6JointCreate(*physics.s_physics
		, link0, localFrame0, link1, localFrame1);

	return joint;
}


// set joint velocity drive parameter
bool cArticulation::SetJointDriveVelocityByLink(const int linkId
	, const float stiffness, const float damping, const float maxForce, const float velocity)
{
	PxArticulationLink *link = GetLink(linkId);
	if (!link) return false; // error return

	PxArticulationJointReducedCoordinate* joint = link->getInboundJoint();
	return SetJointDriveVelocity(joint, stiffness, damping, maxForce, velocity);
}


// set joint velocity drive parameter
bool cArticulation::SetJointDriveVelocity(physx::PxArticulationJointReducedCoordinate* joint
	, const float stiffness, const float damping, const float maxForce, const float velocity)
{
	PxArticulationDrive param;
	param.stiffness = stiffness;
	param.damping = damping;
	param.maxForce = maxForce;
	param.driveType = PxArticulationDriveType::eACCELERATION;

	joint->setDriveParams(PxArticulationAxis::eTWIST, param);
	joint->setMotion(PxArticulationAxis::eTWIST, PxArticulationMotion::eFREE);
	joint->setDriveVelocity(PxArticulationAxis::eTWIST, velocity);
	return true;
}


// set joint drive target parameter by link id
bool cArticulation::SetJointDriveTargetByLink(const int linkId
	, const float stiffness, const float damping, const float maxForce, const float target)
{
	PxArticulationLink* link = GetLink(linkId);
	if (!link) return false; // error return

	PxArticulationJointReducedCoordinate* joint = link->getInboundJoint();
	return SetJointDriveTarget(joint, stiffness, damping, maxForce, target);
}


// set joint drive target parameter
bool cArticulation::SetJointDriveTarget(physx::PxArticulationJointReducedCoordinate* joint
	, const float stiffness, const float damping, const float maxForce, const float target)
{
	PxArticulationDrive param;
	param.stiffness = stiffness;
	param.damping = damping;
	param.maxForce = maxForce;
	param.driveType = PxArticulationDriveType::eACCELERATION;

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
	renderer.m_cylinder2.SetColor(cColor::WHITE);

	for (auto& kv : m_links)
	{
		const phys::cArticulation::sLinkInfo& info = kv.second;
		switch (info.type)
		{
		case phys::eShapeType::Box:
		{
			const PxTransform pose = info.link->getGlobalPose();
			const Transform tm = Transform(Vector3(), info.scale * 0.5f) *
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
			renderer.m_capsule.m_transform.pos = *(Vector3*)&pose.p;
			renderer.m_capsule.m_transform.rot = *(Quaternion*)&pose.q;
			renderer.m_capsule.SetDimension(info.radius, info.halfHeight);
			renderer.m_capsule.Render(renderer, parentTm, flags);
			renderer.m_capsule.Render(renderer, parentTm, flags | eRenderFlag::WIREFRAME);
		}
		break;

		case phys::eShapeType::Cylinder:
		{
			const PxTransform pose = info.link->getGlobalPose();
			const Transform tm = Transform(*(Vector3*)&pose.p, *(Quaternion*)&pose.q);

			renderer.m_cylinder2.m_transform.pos = tm.pos;
			renderer.m_cylinder2.m_transform.rot = tm.rot;
			renderer.m_cylinder2.SetDimension(info.radius, info.height);
			renderer.m_cylinder2.Render(renderer, parentTm, flags);
			renderer.m_cylinder2.Render(renderer, parentTm, flags | eRenderFlag::WIREFRAME);
		}
		break;

		default: break;
		}
	}

	return true;
}


// get link shape info
bool cArticulation::GetShapeInfo(OUT vector<float>& out
	, const uint maxSize //= 100
)
{
	using namespace graphic;

	for (auto& kv : m_links)
	{
		const phys::cArticulation::sLinkInfo& info = kv.second;
		switch (info.type)
		{
		case phys::eShapeType::Box:
		{
			if (((uint)out.size() + 11) > maxSize)
				break;

			const PxTransform pose = info.link->getGlobalPose();
			const Transform tm = Transform(Vector3(), info.scale) *
				Transform(*(Quaternion*)&pose.q) * Transform(*(Vector3*)&pose.p);
			Transform tm2 = tm;
		
			out.push_back(0.f); // box
			out.push_back(tm2.pos.x);
			out.push_back(tm2.pos.y);
			out.push_back(tm2.pos.z);
			out.push_back(tm2.scale.x);
			out.push_back(tm2.scale.y);
			out.push_back(tm2.scale.z);
			out.push_back(tm2.rot.x);
			out.push_back(tm2.rot.y);
			out.push_back(tm2.rot.z);
			out.push_back(tm2.rot.w);
		}
		break;

		case phys::eShapeType::Sphere:
		{
			if (((uint)out.size() + 5) > maxSize)
				break;

			const PxTransform pose = info.link->getGlobalPose();
			Vector3 pos = (*(Vector3*)&pose.p);

			out.push_back(1.f); // sphere
			out.push_back(pos.x);
			out.push_back(pos.y);
			out.push_back(pos.z);
			out.push_back(info.radius);
		}
		break;

		case phys::eShapeType::Capsule:
		{
			if (((uint)out.size() + 10) > maxSize)
				break;

			const PxTransform pose = info.link->getGlobalPose();
			Transform tm2;
			tm2.pos = (*(Vector3*)&pose.p);
			tm2.rot = Quaternion(pose.q.x, pose.q.y, pose.q.z, pose.q.w);

			out.push_back(2.f); // capsule
			out.push_back(tm2.pos.x);
			out.push_back(tm2.pos.y);
			out.push_back(tm2.pos.z);
			out.push_back(tm2.rot.x);
			out.push_back(tm2.rot.y);
			out.push_back(tm2.rot.z);
			out.push_back(tm2.rot.w);
			out.push_back(info.radius);
			out.push_back(info.halfHeight);
		}
		break;

		case phys::eShapeType::Cylinder:
		{
			if (((uint)out.size() + 10) > maxSize)
				break;

			const PxTransform pose = info.link->getGlobalPose();
			Transform tm2;
			tm2.pos = (*(Vector3*)&pose.p);
			tm2.rot = Quaternion(pose.q.x, pose.q.y, pose.q.z, pose.q.w);

			out.push_back(3.f); // cylinder
			out.push_back(tm2.pos.x);
			out.push_back(tm2.pos.y);
			out.push_back(tm2.pos.z);
			out.push_back(tm2.rot.x);
			out.push_back(tm2.rot.y);
			out.push_back(tm2.rot.z);
			out.push_back(tm2.rot.w);
			out.push_back(info.radius);
			out.push_back(info.height);
		}
		break;

		default: break;
		}
	}

	return true;
}


// return articulation link shape info
// return bytes size, if dst null, return total byte size
int cArticulation::GetShapeInfo2(INOUT BYTE* dst)
{
	using namespace graphic;

#define SET_VALUE(pdst, value, type) \
	*(type*)pdst = (type)value; \
	pdst += sizeof(type);

	int writeSize = 0;

	for (auto& kv : m_links)
	{
		const phys::cArticulation::sLinkInfo& info = kv.second;
		switch (info.type)
		{
		case phys::eShapeType::Box:
		{
			if (dst)
			{
				const PxTransform pose = info.link->getGlobalPose();
				const Transform tm = Transform(Vector3(), info.scale) *
					Transform(*(Quaternion*)&pose.q) * Transform(*(Vector3*)&pose.p);

				SET_VALUE(dst, info.id, int);
				SET_VALUE(dst, phys::eShapeType::Box, int);
				SET_VALUE(dst, tm.pos.x, float);
				SET_VALUE(dst, tm.pos.y, float);
				SET_VALUE(dst, tm.pos.z, float);
				SET_VALUE(dst, tm.scale.x, float);
				SET_VALUE(dst, tm.scale.y, float);
				SET_VALUE(dst, tm.scale.z, float);
				SET_VALUE(dst, tm.rot.x, float);
				SET_VALUE(dst, tm.rot.y, float);
				SET_VALUE(dst, tm.rot.z, float);
				SET_VALUE(dst, tm.rot.w, float);
			}
			writeSize += 48;
		}
		break;

		case phys::eShapeType::Sphere:
		{
			if (dst)
			{
				const PxTransform pose = info.link->getGlobalPose();
				Vector3 pos = (*(Vector3*)&pose.p);

				SET_VALUE(dst, info.id, int);
				SET_VALUE(dst, phys::eShapeType::Sphere, int);
				SET_VALUE(dst, pos.x, float);
				SET_VALUE(dst, pos.y, float);
				SET_VALUE(dst, pos.z, float);
				SET_VALUE(dst, info.radius, float);
			}
			writeSize += 24;
		}
		break;

		case phys::eShapeType::Capsule:
		{
			if (dst)
			{
				const PxTransform pose = info.link->getGlobalPose();
				Transform tm;
				tm.pos = (*(Vector3*)&pose.p);
				tm.rot = Quaternion(pose.q.x, pose.q.y, pose.q.z, pose.q.w);

				SET_VALUE(dst, info.id, int);
				SET_VALUE(dst, info.type, int);
				SET_VALUE(dst, tm.pos.x, float);
				SET_VALUE(dst, tm.pos.y, float);
				SET_VALUE(dst, tm.pos.z, float);
				SET_VALUE(dst, tm.rot.x, float);
				SET_VALUE(dst, tm.rot.y, float);
				SET_VALUE(dst, tm.rot.z, float);
				SET_VALUE(dst, tm.rot.w, float);
				SET_VALUE(dst, info.radius, float);
				SET_VALUE(dst, info.halfHeight, float);
			}
			writeSize += 44;
		}
		break;

		case phys::eShapeType::Cylinder:
		{
			if (dst)
			{
				const PxTransform pose = info.link->getGlobalPose();
				Transform tm;
				tm.pos = (*(Vector3*)&pose.p);
				tm.rot = Quaternion(pose.q.x, pose.q.y, pose.q.z, pose.q.w);

				SET_VALUE(dst, info.id, int);
				SET_VALUE(dst, info.type, int);
				SET_VALUE(dst, tm.pos.x, float);
				SET_VALUE(dst, tm.pos.y, float);
				SET_VALUE(dst, tm.pos.z, float);
				SET_VALUE(dst, tm.rot.x, float);
				SET_VALUE(dst, tm.rot.y, float);
				SET_VALUE(dst, tm.rot.z, float);
				SET_VALUE(dst, tm.rot.w, float);
				SET_VALUE(dst, info.radius, float);
				SET_VALUE(dst, info.height, float);
			}
			writeSize += 44;
		}
		break;

		default: break;
		}
	}
	return writeSize;
}


// return all joint value
bool cArticulation::GetJointValues(OUT vector<float>& out)
{
	for (auto& joint : m_joints)
	{
		const float value = joint->getJointPosition(PxArticulationAxis::eTWIST);
		out.push_back(value);
	}
	return true;
}


// update articulation link sync reference
bool cArticulation::UpdateSyncInfo(sSyncInfo* sync)
{
	for (auto& kv : m_links)
	{
		if (kv.second.link)
			kv.second.link->userData = sync;
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
	if (!m_art->getScene())
		return tfm;

	PxTransform tm = m_art->getRootGlobalPose();
	tfm.pos = *(Vector3*)&tm.p;
	tfm.rot = *(Quaternion*)&tm.q;
	return tfm;
}


// create cylinder convex mesh
// y-axis height cylinder
physx::PxConvexMesh* cArticulation::GenerateCylinderMesh(cPhysicsEngine& physics
	, const float radius, const float height)
{
	const int slices = 20;
	graphic::cCylinderShape shape;
	const int sideVtxCnt = slices * 2;
	vector<Vector3> positions(sideVtxCnt);
	shape.GenerateCylinderMesh(slices, radius, height, &positions[0], nullptr, false
		, graphic::eCylinderType::AxisY);

	PxTolerancesScale scale;
	PxCookingParams params(scale);
	PxConvexMeshDesc convexDesc;
	convexDesc.points.count = (uint)positions.size();
	convexDesc.points.stride = sizeof(PxVec3);
	convexDesc.points.data = &positions[0];
	convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;
	PxConvexMesh* convexMesh = PxCreateConvexMesh(params, convexDesc
		, physics.s_physics->getPhysicsInsertionCallback());
	return convexMesh;
}


// calc localFrame for PxJoint~ Function seriese
// worldTm0 : actor0 world transform
// worldTm1 : actor1 world transform
// jointPos : joint pos, world space
// revoluteAxis : revolution Axis, world space
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
	m_joints.clear();

	if (physics && physics->m_scene && m_art)
		physics->m_scene->removeArticulation(*m_art);
	SAFE_RELEASE2(m_art);
}

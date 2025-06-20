#pragma once
#include "Gathering_Code.h"
class hkpSimpleConstraintContactMgr;
class hkMatrix3x4;
class NiMatrix33;
class NiVector3;
class hkpWorldObject;
class bhkShape;
class hkpRigidBody;
template <typename T_Data> class hkArray
{
public:
	enum
	{
		kFlag_NoDealloc = 0x80000000,	// data not owned by us
		kFlag_Locked = 0x40000000,	// will never be destroyed
	};

	T_Data* data;				// 00
	UInt32		size;				// 04
	UInt32		capacityAndFlags;	// 08

	UInt32 Size() const { return size; }
	bool Empty() const { return !size; }
	T_Data* Data() const { return const_cast<T_Data*>(data); }

	T_Data& operator[](UInt32 index) { return data[index]; }

	class Iterator
	{
		T_Data* pData;
		UInt32		count;

	public:
		explicit operator bool() const { return count != 0; }
		void operator++()
		{
			pData++;
			count--;
		}

		T_Data& operator*() const { return *pData; }
		T_Data& operator->() const { return *pData; }
		T_Data& Get() const { return *pData; }

		Iterator(hkArray& source) : pData(source.data), count(source.size) {}
	};

	Iterator Begin() { return Iterator(*this); }
};

struct NiPoint2
{
	float	x, y;

	NiPoint2() {}
	__forceinline NiPoint2(float _x, float _y) : x(_x), y(_y) {}
	__forceinline NiPoint2(const NiPoint2& rhs) { *this = rhs; }
	__forceinline explicit NiPoint2(const __m128 rhs) { SetPS(rhs); }

	__forceinline void operator=(NiPoint2&& rhs)
	{
		x = rhs.x;
		y = rhs.y;
	}
	__forceinline void operator=(const NiPoint2& rhs) { _mm_storeu_si64(this, _mm_loadu_si64(&rhs)); }

	__forceinline NiPoint2& SetPS(const __m128 rhs)
	{
		_mm_storeu_si64(this, _mm_castps_si128(rhs));
		return *this;
	}

	inline operator float* () { return &x; }

	__forceinline __m128 PS() const { return _mm_castsi128_ps(_mm_loadu_si64(this)); }

	void Dump() const;
};

struct hkDiagonal
{
	hkVector4	pt0;
	hkVector4	pt1;
};

class hkBaseObject
{
public:
	/*00*/virtual void		Destroy(bool doFree);
};

struct alignas(16) hkQuaternion
{
	float	x, y, z, w;

	hkQuaternion() {}
	__forceinline hkQuaternion(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
	__forceinline hkQuaternion(const hkQuaternion& from) { *this = from; }
	__forceinline explicit hkQuaternion(const NiVector3& pry) { *this = pry; }
	__forceinline explicit hkQuaternion(const NiMatrix33& rotMat) { *this = rotMat; }
	__forceinline explicit hkQuaternion(const hkMatrix3x4& rotMat) { *this = rotMat; }
	__forceinline hkQuaternion(const hkVector4& from) { *this = from; }
	__forceinline explicit hkQuaternion(const AxisAngle& axisAngle) { *this = axisAngle; }
	__forceinline hkQuaternion(const NiQuaternion& niQt) { *this = niQt; }
	__forceinline explicit hkQuaternion(const __m128 rhs) { SetPS(rhs); }

	__forceinline void operator=(hkQuaternion&& from)
	{
		x = from.x;
		y = from.y;
		z = from.z;
		w = from.w;
	}
	__forceinline void operator=(const hkQuaternion& from) { SetPS(from.PS()); }
	__forceinline void operator=(const NiVector3& pry) { FromEulerPRY(pry.PS()); }
	__forceinline void operator=(const NiMatrix33& rotMat) { FromRotationMatrix(rotMat); }
	__forceinline void operator=(const hkMatrix3x4& rotMat) { ThisStdCall(0xCB26E0, this, &rotMat); }
	__forceinline void operator=(const hkVector4& from) { SetPS(from.PS()); }
	__forceinline void operator=(const AxisAngle& axisAngle) { FromAxisAngle(axisAngle); }
	__forceinline void operator=(const NiQuaternion& niQt)
	{
		__m128 m = niQt.PS();
		SetPS(_mm_shuffle_ps(m, m, 0x39));
	}

	__forceinline hkQuaternion& SetPS(const __m128 rhs)
	{
		_mm_store_ps(&x, rhs);
		return *this;
	}

	__forceinline __m128 operator+(const hkQuaternion& rhs) const { return PS() + rhs.PS(); }
	__forceinline __m128 operator-(const hkQuaternion& rhs) const { return PS() - rhs.PS(); }

	__forceinline __m128 operator*(float s) const { return PS() * _mm_set_ps1(s); }
	__forceinline __m128 operator*(const hkQuaternion& rhs) const { return MultiplyQuaternion(rhs); }
	__forceinline __m128 operator*(__m128 vec) const { return MultiplyVector(vec); }

	__forceinline hkQuaternion& operator+=(const hkQuaternion& rhs) { return SetPS(*this + rhs); }

	__forceinline hkQuaternion& operator-=(const hkQuaternion& rhs) { return SetPS(*this - rhs); }

	__forceinline hkQuaternion& operator*=(float s) { return SetPS(*this * s); }

	__forceinline hkQuaternion& operator*=(const hkQuaternion& rhs) { return SetPS(*this * rhs); }

	inline operator float* () { return &x; }
	__forceinline __m128 PS() const { return _mm_load_ps(&x); }

	__forceinline bool operator==(const hkQuaternion& rhs) const { return Equal_V4(PS(), rhs.PS()); }
	__forceinline bool operator!=(const hkQuaternion& rhs) const { return !(*this == rhs); }

	hkQuaternion& __vectorcall FromEulerPRY(__m128 pry);
	hkQuaternion& __fastcall FromRotationMatrix(const NiMatrix33& rotMat);
	hkQuaternion& __fastcall FromAxisAngle(const AxisAngle& axisAngle);

	__m128 __vectorcall MultiplyVector(__m128 vec) const;
	__m128 __vectorcall MultiplyQuaternion(const hkQuaternion& rhs) const;

	__forceinline __m128 Invert() const { return PS() ^ _mm_load_ps((const float*)0x10C4C00); }

	__forceinline __m128 Negate() const { return PS() ^ GET_PS(2); }

	__forceinline float __vectorcall DotProduct(const hkQuaternion& rhs) const
	{
		__m128 k = _mm_setzero_ps();
		return _mm_cvtss_f32(_mm_hadd_ps(_mm_hadd_ps(PS() * rhs.PS(), k), k));
	}

	hkQuaternion& Normalize() { return SetPS(Normalize_V4(PS())); }

	__m128 __vectorcall ToEulerPRY() const;
	__m128 __vectorcall ToEulerYPR() const;

	void Dump() const;
};

struct alignas(16) hkMatrix3x4
{
	float	cr[3][4];

	hkMatrix3x4() {}
	__forceinline hkMatrix3x4(float m00, float m10, float m20, float m01, float m11, float m21, float m02, float m12, float m22)
	{
		cr[0][0] = m00; cr[0][1] = m10; cr[0][2] = m20;
		cr[0][4] = m01; cr[0][5] = m11; cr[0][6] = m21;
		cr[0][8] = m02; cr[0][9] = m12; cr[0][10] = m22;
	}
	__forceinline explicit hkMatrix3x4(const NiMatrix33& inMatrix) { *this = inMatrix; }
	__forceinline explicit hkMatrix3x4(const hkQuaternion& inQuaternion) { *this = inQuaternion; }
	__forceinline explicit hkMatrix3x4(const hkMatrix3x4& inMatrix) { *this = inMatrix; }

	__forceinline void operator=(hkMatrix3x4&& rhs)
	{
		cr[0][0] = rhs.cr[0][0]; cr[0][1] = rhs.cr[0][1]; cr[0][2] = rhs.cr[0][2];
		cr[0][4] = rhs.cr[0][4]; cr[0][5] = rhs.cr[0][5]; cr[0][6] = rhs.cr[0][6];
		cr[0][8] = rhs.cr[0][8]; cr[0][9] = rhs.cr[0][9]; cr[0][10] = rhs.cr[0][10];
	}
	__forceinline void operator=(const hkMatrix3x4& from)
	{
		_mm_store_ps(&cr[0][0], _mm_load_ps(&from.cr[0][0]));
		_mm_store_ps(&cr[1][0], _mm_load_ps(&from.cr[1][0]));
		_mm_store_ps(&cr[2][0], _mm_load_ps(&from.cr[2][0]));
	}
	void __fastcall operator=(const NiMatrix33& inMatrix);
	void __fastcall operator=(const hkQuaternion& inQuaternion);

	inline operator float* () const { return (float*)this; }

	__m128 __vectorcall MultiplyVector(__m128 vec) const;
	__m128 __vectorcall MultiplyVectorInv(__m128 vec) const;

	void Dump() const;
};

class bhkRefObject : public NiObject
{
public:
	/*8C*/virtual void		SetObject(void* object);
	/*90*/virtual void		UpdateRefCount(bool incRef);	// inc/dec ref, depending on arg

	void* refObject;		// 08
};

// 10
class bhkSerializable : public bhkRefObject
{
public:
	/*94*/virtual void		Unk_25(UInt32 arg);
	/*98*/virtual void* GetWorld(void);					// return bhkWorld
	/*9C*/virtual bool		SetWorld(void* inWorld);// arg bhkWorld
	/*A0*/virtual bool		Unk_28(void);
	/*A4*/virtual void		FreeData(bool del);	// free hkData
	/*A8*/virtual UInt32	Unk_2A(void);
	/*AC*/virtual void		LoadHavokData(NiStream* stream);	// called from bhkSerializable::Load after primary load is done
	/*B0*/virtual void		Unk_2C(void);	// create object
	/*B4*/virtual void* CreateHavokData(UInt8* arg);	// create Cinfo, return hkData
	/*B8*/virtual void		Unk_2E(void);		// destroy object
	/*BC*/virtual void		Unk_2F(void);
	/*C0*/virtual void		Unk_30(void);

	void* hkData;	// 0C - stores hkConstraintData (descriptor used to build hkObj)
};

class bhkShape : public bhkSerializable
{
public:
	/*C4*/virtual void		Unk_31(void);
	/*C8*/virtual void		Unk_32(void);
	/*CC*/virtual void		Unk_33(void);
	/*D0*/virtual void		Unk_34(void);
	/*D4*/virtual void		Unk_35(void);
	/*D8*/virtual void		Unk_36(void);
	/*DC*/virtual void		Unk_37(void);
	/*E0*/virtual void		Unk_38(void);

	UInt32			materialType;	// 10
};

// 14
class bhkWorldObject : public bhkSerializable
{
public:
	/*C4*/virtual void		UpdateCollisionFilter();
	/*C8*/virtual void		Unk_32(void);
	/*CC*/virtual void		Unk_33(void);
	/*D0*/virtual void		Unk_34(void);

	UInt32				bodyFlags;		// 10

	void __fastcall ApplyForce(const NiVector4& forceVector);
};

// 18
class bhkPhantom : public bhkWorldObject
{
public:
	UInt32				unk14;		// 14
};

// 18
class bhkAabbPhantom : public bhkPhantom
{
public:
};

// 18
class bhkShapePhantom : public bhkPhantom
{
public:
};

class bhkCachingShapePhantom : public bhkShapePhantom
{
public:
};


struct hkTransform
{
	hkMatrix3x4	rotation;		// 00
	hkVector4	translation;	// 30
};

struct hkSweptTransform
{
	hkVector4		centerOfMass0;	// 00 (40)
	hkVector4		centerOfMass1;	// 10 (50)
	hkQuaternion	rotation0;		// 20 (60)
	hkQuaternion	rotation1;		// 30 (70)
	hkVector4		centerOfMassLoc;// 40 (80)
};

struct hkMotionState
{
	enum SolverDeactivation
	{
		kSolver_Max,
		kSolver_Off,
		kSolver_Low,
		kSolver_Medium,
		kSolver_High
	};

	hkTransform			transform;			// 00
	hkSweptTransform	sweptTransform;		// 40
	hkVector4			deltaAngle;			// 90
	float				objectRadius;		// A0	For spheres: actual radius * tan(PI/3)
	float				linearDamping;		// A4
	float				angularDamping;		// A8
	UInt8				byteAC;				// AC
	UInt8				byteAD;				// AD
	UInt8				solverDeactivation;	// AE
	UInt8				byteAF;				// AF
};

class hkReferencedObject : public hkBaseObject
{
public:
	/*04*/virtual void		CalcStatistics(void* collector); // void* collector - hkStatisticsCollector
	/*08*/virtual void		Unk_02(void);

	UInt16			sizeAndFlags;		// 04
	UInt16			refCount;			// 06
};

class bhkSphereRepShape : public bhkShape
{
public:
};

// 14
class bhkConvexShape : public bhkSphereRepShape
{
public:
};

// 14
class bhkCapsuleShape : public bhkConvexShape
{
public:
};

class bhkConvexVerticesShape : public bhkConvexShape
{
public:
};

// 14
class bhkCharControllerShape : public bhkConvexVerticesShape
{
public:
};

class hkpMotion : public hkReferencedObject
{
public:
	/*0C*/virtual void		SetBodyMass(float _bodyMass);
	/*10*/virtual void		SetBodyMassInv(float _bodyMassInv);
	/*14*/virtual void		GetInertiaLocal(hkMatrix3x4* outInertia);
	/*18*/virtual void		GetInertiaWorld(hkMatrix3x4* outInertia);
	/*1C*/virtual void		SetInertiaLocal(hkMatrix3x4* inInertia);
	/*20*/virtual void		SetInertiaInvLocal(hkMatrix3x4* inInertia);
	/*24*/virtual void		GetInertiaInvLocal(hkMatrix3x4* outInertia);
	/*28*/virtual void		GetInertiaInvWorld(hkMatrix3x4* outInertia);
	/*2C*/virtual void		SetCenterOfMassInLocal(hkVector4* centerOfMass);
	/*30*/virtual void		SetPosition(hkVector4* position);
	/*34*/virtual void		SetRotation(hkQuaternion* rotation);
	/*38*/virtual void		SetPositionAndRotation(hkVector4* position, hkQuaternion* rotation);
	/*3C*/virtual void		SetTransform(hkTransform* transform);
	/*40*/virtual void		SetLinearVelocity(hkVector4* velocity);
	/*44*/virtual void		SetAngularVelocity(hkVector4* velocity);
	/*48*/virtual void		GetProjectedPointVelocity(hkVector4* point, hkVector4* normal, float* outVel, float* outInvVirtMass);
	/*4C*/virtual void		ApplyLinearImpulse(hkVector4* impulse);
	/*50*/virtual void		ApplyPointImpulse(hkVector4* impulse, hkVector4* point);
	/*54*/virtual void		ApplyAngularImpulse(hkVector4* impulse);
	/*58*/virtual void		ApplyPointForce(float deltaTime, hkVector4* force, hkVector4* point);
	/*5C*/virtual void		ApplyForce(float deltaTime, hkVector4* force);
	/*60*/virtual void		ApplyTorque(float deltaTime, hkVector4* torque);
	/*64*/virtual void		GetMotionStateAndVelocitiesAndDeactivationType(hkpMotion* outMotion);

	enum MotionType
	{
		kMotionType_Sphere = 2,
		kMotionType_Box,
		kMotionType_KeyframedRigid,
		kMotionType_FixedRigid,
		kMotionType_ThinBox,
		kMotionType_Character,
	};

	UInt8			type;				// 008 (0E8)
	UInt8			deactivationType;	// 009 (0E9)
	UInt16			word00A;			// 00A (0EA)
	UInt16			word00C;			// 00C (0EC)
	UInt8			pad00E[2];			// 00E (0EE)
	hkMotionState	motionState;		// 010 (0F0)
	NiVector3		inertia;			// 0C0 (1A0)
	float			bodyMassInv;		// 0CC (1AC)
	hkVector4		linVelocity;		// 0D0 (1B0)
	hkVector4		angVelocity;		// 0E0 (1C0)
	hkVector4		deactivationPos[2];	// 0F0 (1D0)
	UInt32			unk110[12];			// 110 (1F0)

	void Dump();
};

class hkpShape : public hkReferencedObject
{
public:
	/*0C*/virtual void		Unk_03(hkVector4* arg1);
	/*10*/virtual UInt32	Unk_04();
	/*14*/virtual bool		IsConvex();
	/*18*/virtual UInt32	Unk_06(UInt32 arg1, UInt32 arg2);
	/*1C*/virtual void		GetSpaceDiagonal(hkTransform* arg1, float arg2, hkDiagonal* resPtr);
	/*20*/virtual void* Unk_08(void* arg1, hkVector4* arg2, void* arg3);
	/*24*/virtual void		Unk_09(void* arg1, void* arg2, void* arg3);
	/*28*/virtual void		Unk_0A(hkVector4* arg1, hkVector4* arg2, void* arg3, hkVector4* arg4);
	/*2C*/virtual UInt32	Unk_0B();

	bhkShape* shape;		// 08
	UInt32			unk0C;		// 0C
};
static_assert(sizeof(hkpShape) == 0x10);

struct hkCdBody
{
	hkpShape* shape;			// 00
	UInt32			shapeKey;		// 04
	hkMotionState* motion;		// 08
	hkCdBody* parent;		// 0C

	hkpWorldObject* GetWorldObj() { return (hkpWorldObject*)((UInt8*)this - 0x10); }
};


enum CollisionLayerTypes : UINT8
{
	LAYER_STATIC = 1,
	LAYER_ANIM_STATIC,
	LAYER_TRANSPARENT,
	LAYER_CLUTTER,
	LAYER_WEAPON,
	LAYER_PROJECTILE,
	LAYER_SPELL,
	LAYER_BIPED,
	LAYER_TREES,
	LAYER_PROPS,
	LAYER_WATER,
	LAYER_TRIGGER,
	LAYER_TERRAIN,
	LAYER_TRAP,
	LAYER_NONCOLLIDABLE,
	LAYER_CLOUD_TRAP,
	LAYER_GROUND,
	LAYER_PORTAL,
	LAYER_DEBRIS_SMALL,
	LAYER_DEBRIS_LARGE,
	LAYER_ACOUSTIC_SPACE,
	LAYER_ACTORZONE,
	LAYER_PROJECTILEZONE,
	LAYER_GASTRAP,
	LAYER_SHELLCASING,
	LAYER_TRANSPARENT_SMALL,
	LAYER_INVISIBLE_WALL,
	LAYER_TRANSPARENT_SMALL_ANIM,
	LAYER_DEADBIP,
	LAYER_CHARCONTROLLER,
	LAYER_AVOIDBOX,
	LAYER_COLLISIONBOX,
	LAYER_CAMERASPHERE,
	LAYER_DOORDETECTION,
	LAYER_CAMERAPICK,
	LAYER_ITEMPICK,
	LAYER_LINEOFSIGHT,
	LAYER_PATHPICK,
	LAYER_CUSTOMPICK1,
	LAYER_CUSTOMPICK2,
	LAYER_SPELLEXPLOSION,
	LAYER_DROPPINGPICK,
	LAYER_MAX
};

struct ContactData
{
	UInt16							unk00;
	UInt16							flags;
	UInt32							key;
	hkpSimpleConstraintContactMgr* contactMgr;
	UInt32							unk0C;
	hkCdBody* cdBody1;
	hkCdBody* cdBody2;
	UInt32							unk18[26];
};

struct CdBodyLinker
{
	ContactData* data;
	hkCdBody* cdBody;
};

struct CdParentObj
{
	NiRTTI* type;		// 00	0x1267B70 (bhkNiCollisionObject) / 0x11F4280 (NiAVObject) / 0x11F4A20 (NiTriStrips)
	UInt32		unk04;		// 04
	NiObject* object;	// 08	bhkNiCollisionObject or NiAVObject
	UInt32		unk0C;		// 0C
};


class hkpWorldObject : public hkReferencedObject
{
public:
	/*0C*/virtual void		SetShape(hkpShape* shape);
	/*10*/virtual hkMotionState* GetMotionState();

	enum FilterFlags
	{
		kFilterFlag_Scaled = 0x20,
		kFilterFlag_NoCollision = 0x40,
		kFilterFlag_Linked = 0x80
	};

	void* pWorld;		// 08 - hkpWorld
	bhkWorldObject* object;		// 0C
	hkCdBody				cdBody;			// 10
	SInt8					byte20;			// 20	Offset modifier from cdBody offset to object begin offset
	UInt8					byte21;			// 21
	UInt16					word22;			// 22
	UInt32					unk24;			// 24
	UInt8					collisionType;	// 28	Known: 1 - hkpRigidBody, 2 - hkpPhantom
	UInt8					byte29;			// 29
	UInt8					byte2A;			// 2A
	UInt8					pad2B;			// 2B
	UInt8					layerType;		// 2C
	UInt8					filterFlags;	// 2D
	UInt16					group;			// 2E
	NiVector4				vector30;		// 30
	NiVector4				vector40;		// 40
	UInt32					unk50;			// 50
	UInt32* unk54;			// 54
	UInt32* unk58;			// 58
	float					penetrateDepth;	// 5C
	hkArray<CdBodyLinker>	cdRgdBodies;	// 60
	UInt32					unk6C[3];		// 6C
	hkArray<CdParentObj>	parentObjs;		// 78
	UInt32					unk84[2];		// 84

	NiNode* GetParentNode();
	TESObjectREFR* GetParentRef();
};
static_assert(sizeof(hkpWorldObject) == 0x8C);

class hkpEntity : public hkpWorldObject
{
public:
	/*14*/virtual void		Unk_05(void);
};

class hkpConstraintData : public hkReferencedObject
{
public:
	/*0C*/virtual void		Unk_03(void);
	/*10*/virtual void		Unk_04(void);
	/*14*/virtual void		Unk_05(void);
	/*18*/virtual void		Unk_06(void);
	/*1C*/virtual void		Unk_07(void);
	/*20*/virtual void		Unk_08(void);
	/*24*/virtual void		Unk_09(void);
	/*28*/virtual void		Unk_0A(void);
	/*2C*/virtual void		Unk_0B(void);
	/*30*/virtual void		Unk_0C(void);
	/*34*/virtual void		Unk_0D(void);
	/*38*/virtual void		Unk_0E(void);
	/*3C*/virtual void		Unk_0F(void);
};

class hkpConstraintOwner : public hkReferencedObject
{
public:
	/*0C*/virtual void		Unk_03(void);
	/*10*/virtual void		Unk_04(void);
	/*14*/virtual void		Unk_05(void);
	/*18*/virtual void		Unk_06(void);
};

class hkpSimulationIsland : public hkpConstraintOwner
{
public:
	UInt32					unk08[4];		// 08
	void* world;			// 18 - ahkpWorld
	UInt32					unk1C[11];		// 1C
	hkArray<hkpRigidBody*>	rigidBodies;	// 48
	hkpRigidBody*			rgdBody54;		// 54
	UInt32					unk58;			// 58
	hkArray<ContactData*>	contactData;	// 5C
	ContactData* data68;		// 68
};

// 38
class hkpConstraintInstance : public hkReferencedObject
{
public:
	/*0C*/virtual void		Unk_03(void);
	/*10*/virtual void		Unk_04(void);
	/*14*/virtual void		Unk_05(void);
	/*18*/virtual void		Unk_06(void);
	/*1C*/virtual void		Unk_07(void);

	hkpSimulationIsland*		simIsle;			// 08
	hkpConstraintData*			constraintData;		// 0C
	UInt32						unk10;				// 10
	hkpRigidBody*				contactBody;		// 14
	hkpRigidBody*				parentBody;			// 18
	UInt32						unk1C[5];			// 1C
	void*						ptr30;				// 30
	UInt32						unk34;				// 34
};

struct ConstraintContact
{
	hkpConstraintInstance* instance;		// 00
	hkpRigidBody* parentBody;	// 04
	hkpRigidBody* contactBody;	// 08
	void* ptr0C;			// 0C
	UInt32					unk10[5];		// 10
	float					flt24[3];		// 24
};

class hkpRigidBody : public hkpEntity
{
public:
	/*18*/virtual void		Unk_06(void);

	UInt8							byte8C;			// 8C
	UInt8							pad8D[3];		// 8D
	float							friction;		// 90
	float							restitution;	// 94
	UInt32							unk98[5];		// 98
	ConstraintContact* contactsArr;	// AC
	UInt16							contactsSize;	// B0
	UInt16							contactsCap;	// B2
	hkArray<hkpConstraintInstance*>	constraintInst;	// B4
	UInt32							unkC0[3];		// C0
	hkpSimulationIsland* simIsle;		// CC
	UInt32							unkD0[4];		// D0
	hkpMotion						motion;			// E0

	bool IsMobile() const { return (motion.type & 2) != 0; }

	void __vectorcall SetVelocity(__m128 vel, UInt32 flags);	// bit0 - set global; bit1 - add value; bit4 - angular

	__forceinline void ResetCollision() { CdeclCall(0xCA8700, this); }

	__forceinline void UpdateMotion() { ThisStdCall(0xC9C1D0, this); }

	//void __fastcall SetCollisionProperty(UInt32 propID, FltAndInt value);
};


class bhkNiCollisionObject : public NiCollisionObject
{
public:
	/*A0*/virtual NiVector3* GetLinearVelocityInGameUnits(NiVector3* outVel);
	/*A4*/virtual void		CollisionObjTransformToTargetNode();
	/*A8*/virtual void		TargetNodeTransformToCollisionObj();
	/*AC*/virtual void		UpdateVelocity();
	/*B0*/virtual void		SetMotionType(UInt32 moType, void* body, bool updateMotion);	// void* body - bhkRigidBody
	/*B4*/virtual bool		IsFixedMotion();
	/*B8*/virtual void		SetTargetNodeTransform(NiTransform* transform);
	/*BC*/virtual bool		Unk_2F();
	/*C0*/virtual void		ToggleDebugDisplay(bool toggle);	//	TCG

	enum Flags
	{
		kFlag_Active = 1,
		kFlag_Notify = 4,
		kFlag_SetLocal = 8,
		kFlag_DebugDisplay = 0x10,	//	TCG
		kFlag_UseVelocity = 0x20,
		kFlag_Reset = 0x40,
		kFlag_SyncOnUpdate = 0x80,
		kFlag_AnimTargeted = 0x400,
		kFlag_DismemberLimb = 0x800
	};

	UInt16			flags;			// 0C
	UInt16			word0E;			// 0E
	bhkWorldObject* worldObj;		// 10

	//	Callbacks array @ 0x11AFE88
};

class hkpCharacterState : public hkReferencedObject
{
public:
	enum StateType
	{
		kState_OnGround,
		kState_Jumping,
		kState_InAir,
		kState_Climbing,
		kState_Flying,
		kState_Swimming,
		kState_Projectile
	};

	/*0C*/virtual UInt32	GetStateType();
	/*10*/virtual void		Unk_04(void);
	/*14*/virtual void		Unk_05(void);
	/*18*/virtual void		Unk_06(void);
	/*1C*/virtual void		Unk_07(void);
	/*20*/virtual void		UpdateVelocity(bhkCharacterController* charCtrl);
};

class bhkCharacterState : public hkpCharacterState
{
public:
};

class bhkCharacterStateOnGround : public bhkCharacterState
{
public:
};

class bhkCharacterStateJumping : public bhkCharacterState
{
public:
};

class bhkCharacterStateInAir : public bhkCharacterState
{
public:
};

class bhkCharacterStateClimbing : public bhkCharacterState
{
public:
};

class bhkCharacterStateFlying : public bhkCharacterState
{
public:
};

class bhkCharacterStateSwimming : public bhkCharacterState 
{
public:
};

class bhkCharacterStateProjectile : public bhkCharacterState 
{
public:
};

// 34
class hkpCharacterStateManager : public hkReferencedObject 
{
public:
	bhkCharacterStateOnGround* stateOnGround;		// 08 
	bhkCharacterStateJumping* stateJumping;		// 0C 
	bhkCharacterStateInAir* stateInAir;		// 10
	bhkCharacterStateClimbing* stateClimbing;		// 14
	bhkCharacterStateFlying* stateFlying;		// 18
	bhkCharacterStateSwimming* stateSwimming;		// 1C
	bhkCharacterStateProjectile* stateProjectile;	// 20
	UInt32							unk24[4];			// 24
};

// 30
class hkpCharacterContext : public hkReferencedObject
{
public:
	UInt32						unk08;			// 08
	hkpCharacterStateManager* stateMngr;		// 0C
	UInt32						hkState;		// 10
	UInt32						unk14[7];		// 14
};

class hkpCharacterProxyListener
{
public:
	/*00*/virtual void		Unk_00(void);
	/*04*/virtual void		Unk_01(void);
	/*08*/virtual void		Unk_02(void);
	/*0C*/virtual void		Unk_03(void);
	/*10*/virtual void		Unk_04(void);
	/*14*/virtual void		Unk_05(void);
};

// 70
class bhkCharacterListener : public hkpCharacterProxyListener 
{
public:
	enum
	{
		kJumping = 0x400,
		kIsSceneComplex = 0x100000,
		kTiltFrontBack = 0x2000000,
		kTiltLeftRight = 0x4000000,
		kIsUsingFurniture = 0x8000000,
	};

	UInt32			flags;			// 04 (414)
	UInt32			unk08[6];		// 08
	float			flt20;			// 20 (430)
	UInt32			surfaceMaterial;// 24 (434)
	UInt32			unk28;			// 28 (438)
	UInt32			unk2C[5];		// 2C
	hkVector4		vector40;		// 40 (450)
	UInt32			unk50[8];		// 50
};


class hkpCdPointCollector
{
public:
	/*00*/virtual void		Unk_00(void);
	/*04*/virtual void		Unk_01(void);
	/*08*/virtual void		Unk_02(void);

	float			earlyOutDistance;	// 04
};

struct hkpSurfaceInfo
{
	UInt32			supportedState;		// 00
	UInt32			unk04;				// 04
	UInt32			unk08;				// 08
	UInt32			unk0C;				// 0C
	hkVector4		surfaceNormal;		// 10
	hkVector4		surfaceVelocity;	// 20
	float			flt30;				// 30
	UInt32			unk34[3];			// 34
};
static_assert(sizeof(hkpSurfaceInfo) == 0x40);

struct hkStepInfo
{
	float		startTime;		// 00
	float		endTime;		// 04
	float		deltaTime;		// 08
	float		invDeltaTime;	// 0C
};

struct hkCdPoint
{
	NiVector4		point00;		// 00
	NiVector4		point10;		// 10
	hkCdBody* cdBody1;		// 20
	UInt8			body1Key;		// 24
	UInt8			pad25[2];		// 25
	UInt8			obj1Flag;		// 27
	NiVector4		point28;		// 28
	NiVector4		point38;		// 38
	hkCdBody* cdBody2;		// 48
	UInt8			body2Key;		// 4C
	UInt8			pad4D[2];		// 4D
	UInt8			obj2Flag;		// 4F
	NiVector4		point50;		// 50
	NiVector4		point60;		// 60
};

class hkpAllCdPointCollector : public hkpCdPointCollector
{
public:
	/*0C*/virtual void		Unk_03(void);

	UInt32					unk008[2];		// 008
	hkArray<hkCdPoint>		cdPoints;		// 010
	UInt32					unk01C;			// 01C
	hkCdPoint				cdPointsLoc[8];	// 020
};
static_assert(sizeof(hkpAllCdPointCollector) == 0x3A0);

class bhkCharacterPointCollector : public hkpAllCdPointCollector
{
public:
	UInt32						unk3A0;			// 3A0
	hkArray<hkpWorldObject*>	contactBodies;	// 3A4
	hkArray<UInt32>				arr3B0;			// 3B0
	hkArray<float>				arr3BC;			// 3BC
};
static_assert(sizeof(bhkCharacterPointCollector) == 0x3C8);

class bhkCharacterProxy : public bhkSerializable
{
public:
	bhkCharacterPointCollector	pointCollector;		// 10
	UInt32						unk3D8[2];
};
static_assert(sizeof(bhkCharacterProxy) == 0x3E0);

class bhkCharacterController : public bhkCharacterProxy
{
public:
	/*C4*/virtual void		Unk_31(void);
	/*C8*/virtual void		Unk_32(void);
	/*CC*/virtual void		Unk_33(void);
	/*D0*/virtual void		Unk_34(void);

	hkpCharacterContext		chrContext;			// 3E0
	bhkCharacterListener	chrListener;		// 410

	hkpSurfaceInfo			surfaceInfo;		// 480
	hkVector4				upVec;				// 4C0
	hkVector4				forwardVec;			// 4D0
	hkStepInfo				stepInfo;			// 4E0
	hkVector4				velocity;			// 4F0
	hkVector4				throwbackVelocity;	// 500
	hkVector4				direction;			// 510
	UInt32					wantState;			// 520
	float					throwbackTimer;		// 524
	float					rotMod;				// 528
	float					rotModTime;			// 52C
	float					calculatePitchTimer;// 530
	float					acrobatics;			// 534	Always 100.0
	float					center;				// 538
	float					waterHeight;		// 53C
	float					jumpHeight;			// 540
	float					startingHeight;		// 544
	float					fallTimeElapsed;	// 548
	float					gravityMult;		// 54C
	NiPoint2				tiltAngle;			// 550
	float					pitchMult;			// 558
	float					scale;				// 55C
	float					swimFloatHeight;	// 560	Always 1.6
	float					actorHeight;		// 564	ControllerShape total length (z)
	float					speedPct;			// 568
	float					flt56C;				// 56C
	hkVector4				rotCenter;			// 570
	float					flt580;				// 580
	float					flt584;				// 584
	float					flt588;				// 588
	float					flt58C;				// 58C
	float					flt590;				// 590
	bhkCachingShapePhantom* chrPhantom;		// 594
	UInt32					unk598;				// 598
	UInt32					usedShape;			// 59C	0 - 5A4; 1 - 5A8
	UInt32					unk5A0;				// 5A0
	bhkCapsuleShape* capsuleShape;		// 5A4
	bhkCharControllerShape* charCtrlShape;		// 5A8
	float					unk5AC[17];			// 5AC
	float					shapeDefaultRadius;	// 5F0
	float					shapeHeight;		// 5F4
	float					shapeRadius;		// 5F8
	float					flt5FC;				// 5FC
	UInt32					unk600[2];			// 600
	UInt8					byte608;			// 608
	UInt8					pad609[3];			// 609
	hkpRigidBody* bodyUnderFeet;		// 60C
	UInt8					bFakeSupport;		// 610
	UInt8					pad611[3];			// 611
	UInt32					unk614[3];			// 614
	hkVector4				vector620;			// 620
	void* bumpedBody;		// 630
	float					bumpForce;			// 634
	void* bumpedCollObj;		// 638
	hkArray<hkCdPoint>		arr63C;				// 63C
	tList<void>				damageImpacts;		// 648
};
static_assert(sizeof(bhkCharacterController) == 0x650);

class ProjectileListener : public bhkCharacterController
{
public:
	/*D4*/virtual void		Unk_35(void);

	UInt32			unk650;		// 650
};
#pragma once
#include <vector>
#include <sstream>
#include "vmt.h"
#include "crc.h"
#include "pattern.h"
#include "hooks.h"
#include <assert.h>

enum ItemDefinitionIndex
{
	ITEM_NONE = 0,
	WEAPON_DEAGLE = 1,
	WEAPON_DUALS = 2,
	WEAPON_FIVE7 = 3,
	WEAPON_GLOCK = 4,
	WEAPON_AK47 = 7,
	WEAPON_AUG = 8,
	WEAPON_AWP = 9,
	WEAPON_FAMAS = 10,
	WEAPON_G3SG1 = 11,
	WEAPON_GALIL = 13,
	WEAPON_M249 = 14,
	WEAPON_M4A1 = 16,
	WEAPON_MAC10 = 17,
	WEAPON_P90 = 19,
	WEAPON_UMP45 = 24,
	WEAPON_XM1014 = 25,
	WEAPON_BIZON = 26,
	WEAPON_MAG7 = 27,
	WEAPON_NEGEV = 28,
	WEAPON_SAWEDOFF = 29,
	WEAPON_TEC9 = 30,
	WEAPON_TASER = 31,
	WEAPON_P2000 = 32,
	WEAPON_MP7 = 33,
	WEAPON_MP9 = 34,
	WEAPON_NOVA = 35,
	WEAPON_P250 = 36,
	WEAPON_SCAR20 = 38,
	WEAPON_SG553 = 39,
	WEAPON_SCOUT = 40,
	WEAPON_KNIFE_T = 42,
	WEAPON_FLASH = 43,
	WEAPON_HE = 44,
	WEAPON_SMOKE = 45,
	WEAPON_MOLOTOV = 46,
	WEAPON_DECOY = 47,
	WEAPON_INC = 48,
	WEAPON_C4 = 49,
	WEAPON_KNIFE_CT = 59,
	WEAPON_M4A1S = 60,
	WEAPON_USPS = 61,
	WEAPON_CZ75 = 63,
	WEAPON_REVOLVER = 64,
	WEAPON_KNIFE_BAYONET = 500,
	WEAPON_KNIFE_FLIP = 505,
	WEAPON_KNIFE_GUT = 506,
	WEAPON_KNIFE_KARAMBIT = 507,
	WEAPON_KNIFE_M9BAYONET = 508,
	WEAPON_KNIFE_HUNTSMAN = 509,
	WEAPON_KNIFE_FALCHION = 512,
	WEAPON_KNIFE_BOWIE = 514,
	WEAPON_KNIFE_BUTTERFLY = 515,
	WEAPON_KNIFE_DAGGER = 516,
	WEAPON_MAX
};

// -----------------------------------------------------
// spatial content masks - used for spatial queries (traceline,etc.)
// -----------------------------------------------------
#define	MASK_ALL					(0xFFFFFFFF)
// everything that is normally solid
#define	MASK_SOLID					(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
// everything that blocks player movement
#define	MASK_PLAYERSOLID			(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_PLAYERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
// blocks npc movement
#define	MASK_NPCSOLID				(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
// water physics in these contents
#define	MASK_WATER					(CONTENTS_WATER|CONTENTS_MOVEABLE|CONTENTS_SLIME)
// everything that blocks lighting
#define	MASK_OPAQUE					(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_OPAQUE)
// everything that blocks lighting, but with monsters added.
#define MASK_OPAQUE_AND_NPCS		(MASK_OPAQUE|CONTENTS_MONSTER)
// everything that blocks line of sight for AI
#define MASK_BLOCKLOS				(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_BLOCKLOS)
// everything that blocks line of sight for AI plus NPCs
#define MASK_BLOCKLOS_AND_NPCS		(MASK_BLOCKLOS|CONTENTS_MONSTER)
// everything that blocks line of sight for players
#define	MASK_VISIBLE					(MASK_OPAQUE|CONTENTS_IGNORE_NODRAW_OPAQUE)
// everything that blocks line of sight for players, but with monsters added.
#define MASK_VISIBLE_AND_NPCS		(MASK_OPAQUE_AND_NPCS|CONTENTS_IGNORE_NODRAW_OPAQUE)
// bullets see these as solid
#define	MASK_SHOT					(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEBRIS|CONTENTS_HITBOX)
// non-raycasted weapons see this as solid (includes grates)
#define MASK_SHOT_HULL				(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEBRIS|CONTENTS_GRATE)
// hits solids (not grates) and passes through everything else
#define MASK_SHOT_PORTAL			(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTER)
// everything normally solid, except monsters (world+brush only)
#define MASK_SOLID_BRUSHONLY		(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_GRATE)
// everything normally solid for player movement, except monsters (world+brush only)
#define MASK_PLAYERSOLID_BRUSHONLY	(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_PLAYERCLIP|CONTENTS_GRATE)
// everything normally solid for npc movement, except monsters (world+brush only)
#define MASK_NPCSOLID_BRUSHONLY		(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP|CONTENTS_GRATE)
// just the world, used for route rebuilding
#define MASK_NPCWORLDSTATIC			(CONTENTS_SOLID|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP|CONTENTS_GRATE)
// These are things that can split areaportals
#define MASK_SPLITAREAPORTAL		(CONTENTS_WATER|CONTENTS_SLIME)

// UNDONE: This is untested, any moving water
#define MASK_CURRENT				(CONTENTS_CURRENT_0|CONTENTS_CURRENT_90|CONTENTS_CURRENT_180|CONTENTS_CURRENT_270|CONTENTS_CURRENT_UP|CONTENTS_CURRENT_DOWN)

// everything that blocks corpse movement
// UNDONE: Not used yet / may be deleted
#define	MASK_DEADSOLID				(CONTENTS_SOLID|CONTENTS_PLAYERCLIP|CONTENTS_WINDOW|CONTENTS_GRATE)


#define NUM_ENT_ENTRY_BITS         (11 + 2)
#define NUM_ENT_ENTRIES            (1 << NUM_ENT_ENTRY_BITS)
#define INVALID_EHANDLE_INDEX       0xFFFFFFFF
#define NUM_SERIAL_NUM_BITS        16 // (32 - NUM_ENT_ENTRY_BITS)
#define NUM_SERIAL_NUM_SHIFT_BITS (32 - NUM_SERIAL_NUM_BITS)
#define ENT_ENTRY_MASK             (( 1 << NUM_SERIAL_NUM_BITS) - 1)

class CBaseHandle;

class IHandleEntity
{
public:
	virtual ~IHandleEntity() {}
	virtual void SetRefEHandle(const CBaseHandle &handle) = 0;
	virtual const CBaseHandle& GetRefEHandle() const = 0;
};

class CBaseHandle
{
public:
	CBaseHandle();
	CBaseHandle(const CBaseHandle &other);
	CBaseHandle(unsigned long value);
	CBaseHandle(int iEntry, int iSerialNumber);

	void Init(int iEntry, int iSerialNumber);
	void Term();

	// Even if this returns true, Get() still can return return a non-null value.
	// This just tells if the handle has been initted with any values.
	bool IsValid() const;

	int GetEntryIndex() const;
	int GetSerialNumber() const;

	int ToInt() const;
	bool operator !=(const CBaseHandle &other) const;
	bool operator ==(const CBaseHandle &other) const;
	bool operator ==(const IHandleEntity* pEnt) const;
	bool operator !=(const IHandleEntity* pEnt) const;
	bool operator <(const CBaseHandle &other) const;
	bool operator <(const IHandleEntity* pEnt) const;

	// Assign a value to the handle.
	const CBaseHandle& operator=(const IHandleEntity *pEntity);
	const CBaseHandle& Set(const IHandleEntity *pEntity);

	// Use this to dereference the handle.
	// Note: this is implemented in game code (ehandle.h)
	IHandleEntity* Get() const;

protected:
	// The low NUM_SERIAL_BITS hold the index. If this value is less than MAX_EDICTS, then the entity is networkable.
	// The high NUM_SERIAL_NUM_BITS bits are the serial number.
	unsigned long	m_Index;
};

inline CBaseHandle::CBaseHandle()
{
	m_Index = INVALID_EHANDLE_INDEX;
}

inline CBaseHandle::CBaseHandle(const CBaseHandle &other)
{
	m_Index = other.m_Index;
}

inline CBaseHandle::CBaseHandle(unsigned long value)
{
	m_Index = value;
}

inline CBaseHandle::CBaseHandle(int iEntry, int iSerialNumber)
{
	Init(iEntry, iSerialNumber);
}

inline void CBaseHandle::Init(int iEntry, int iSerialNumber)
{
	m_Index = iEntry | (iSerialNumber << NUM_ENT_ENTRY_BITS);
}

inline void CBaseHandle::Term()
{
	m_Index = INVALID_EHANDLE_INDEX;
}

inline bool CBaseHandle::IsValid() const
{
	return m_Index != INVALID_EHANDLE_INDEX;
}

inline int CBaseHandle::GetEntryIndex() const
{
	return m_Index & ENT_ENTRY_MASK;
}

inline int CBaseHandle::GetSerialNumber() const
{
	return m_Index >> NUM_ENT_ENTRY_BITS;
}

inline int CBaseHandle::ToInt() const
{
	return (int)m_Index;
}

inline bool CBaseHandle::operator !=(const CBaseHandle &other) const
{
	return m_Index != other.m_Index;
}

inline bool CBaseHandle::operator ==(const CBaseHandle &other) const
{
	return m_Index == other.m_Index;
}

inline bool CBaseHandle::operator ==(const IHandleEntity* pEnt) const
{
	return Get() == pEnt;
}

inline bool CBaseHandle::operator !=(const IHandleEntity* pEnt) const
{
	return Get() != pEnt;
}

inline bool CBaseHandle::operator <(const CBaseHandle &other) const
{
	return m_Index < other.m_Index;
}

inline bool CBaseHandle::operator <(const IHandleEntity *pEntity) const
{
	unsigned long otherIndex = (pEntity) ? pEntity->GetRefEHandle().m_Index : INVALID_EHANDLE_INDEX;
	return m_Index < otherIndex;
}

inline const CBaseHandle& CBaseHandle::operator=(const IHandleEntity *pEntity)
{
	return Set(pEntity);
}

inline const CBaseHandle& CBaseHandle::Set(const IHandleEntity *pEntity)
{
	if (pEntity) {
		*this = pEntity->GetRefEHandle();
	}
	else {
		m_Index = INVALID_EHANDLE_INDEX;
	}

	return *this;
}

template< class T >
class CHandle : public CBaseHandle
{
public:

	CHandle();
	CHandle(int iEntry, int iSerialNumber);
	CHandle(const CBaseHandle &handle);
	CHandle(T *pVal);

	// The index should have come from a call to ToInt(). If it hasn't, you're in trouble.
	static CHandle<T> FromIndex(int index);

	T* Get() const;
	void Set(const T* pVal);

	operator T*();
	operator T*() const;

	bool operator !() const;
	bool operator==(T *val) const;
	bool operator!=(T *val) const;
	const CBaseHandle& operator=(const T *val);

	T* operator->() const;
};


// ----------------------------------------------------------------------- //
// Inlines.
// ----------------------------------------------------------------------- //

template<class T>
CHandle<T>::CHandle()
{
}


template<class T>
CHandle<T>::CHandle(int iEntry, int iSerialNumber)
{
	Init(iEntry, iSerialNumber);
}


template<class T>
CHandle<T>::CHandle(const CBaseHandle &handle)
	: CBaseHandle(handle)
{
}


template<class T>
CHandle<T>::CHandle(T *pObj)
{
	Term();
	Set(pObj);
}


template<class T>
inline CHandle<T> CHandle<T>::FromIndex(int index)
{
	CHandle<T> ret;
	ret.m_Index = index;
	return ret;
}


template<class T>
inline T* CHandle<T>::Get() const
{
	return (T*)CBaseHandle::Get();
}


template<class T>
inline CHandle<T>::operator T *()
{
	return Get();
}

template<class T>
inline CHandle<T>::operator T *() const
{
	return Get();
}


template<class T>
inline bool CHandle<T>::operator !() const
{
	return !Get();
}

template<class T>
inline bool CHandle<T>::operator==(T *val) const
{
	return Get() == val;
}

template<class T>
inline bool CHandle<T>::operator!=(T *val) const
{
	return Get() != val;
}

template<class T>
void CHandle<T>::Set(const T* pVal)
{
	CBaseHandle::Set(reinterpret_cast<const IHandleEntity*>(pVal));
}

template<class T>
inline const CBaseHandle& CHandle<T>::operator=(const T *val)
{
	Set(val);
	return *this;
}

template<class T>
T* CHandle<T>::operator -> () const
{
	return Get();
}

class bf_write;
class bf_read;

typedef void* (*CreateInterfaceFn)(const char *pName, int *pReturnCode);
typedef void* (*InstantiateInterfaceFn)();

class IAppSystem
{
public:
	virtual bool                            Connect(CreateInterfaceFn factory) = 0;                                     // 0
	virtual void                            Disconnect() = 0;                                                           // 1
	virtual void*                           QueryInterface(const char *pInterfaceName) = 0;                             // 2
	virtual int /*InitReturnVal_t*/         Init() = 0;                                                                 // 3
	virtual void                            Shutdown() = 0;                                                             // 4
	virtual const void* /*AppSystemInfo_t*/ GetDependencies() = 0;                                                      // 5
	virtual int /*AppSystemTier_t*/         GetTier() = 0;                                                              // 6
	virtual void                            Reconnect(CreateInterfaceFn factory, const char *pInterfaceName) = 0;       // 7
	virtual void                            UnkFunc() = 0;                                                              // 8
};

#define DECLARE_POINTER_HANDLE(name) struct name##__ { int unused; }; typedef struct name##__ *name
#define MAXSTUDIOSKINS		32

#define TEXTURE_GROUP_LIGHTMAP "Lightmaps"
#define TEXTURE_GROUP_WORLD "World textures"
#define TEXTURE_GROUP_MODEL "Model textures"
#define TEXTURE_GROUP_VGUI "VGUI textures"
#define TEXTURE_GROUP_PARTICLE "Particle textures"
#define TEXTURE_GROUP_DECAL "Decal textures"
#define TEXTURE_GROUP_SKYBOX "SkyBox textures"
#define TEXTURE_GROUP_CLIENT_EFFECTS "ClientEffect textures"
#define TEXTURE_GROUP_OTHER "Other textures"
#define TEXTURE_GROUP_PRECACHED "Precached"
#define TEXTURE_GROUP_CUBE_MAP "CubeMap textures"
#define TEXTURE_GROUP_RENDER_TARGET "RenderTargets"
#define TEXTURE_GROUP_UNACCOUNTED "Unaccounted textures"
//#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER		  "Static Vertex"
#define TEXTURE_GROUP_STATIC_INDEX_BUFFER "Static Indices"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_DISP "Displacement Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_COLOR "Lighting Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_WORLD "World Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_MODELS "Model Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_OTHER "Other Verts"
#define TEXTURE_GROUP_DYNAMIC_INDEX_BUFFER "Dynamic Indices"
#define TEXTURE_GROUP_DYNAMIC_VERTEX_BUFFER "Dynamic Verts"
#define TEXTURE_GROUP_DEPTH_BUFFER "DepthBuffer"
#define TEXTURE_GROUP_VIEW_MODEL "ViewModel"
#define TEXTURE_GROUP_PIXEL_SHADERS "Pixel Shaders"
#define TEXTURE_GROUP_VERTEX_SHADERS "Vertex Shaders"
#define TEXTURE_GROUP_RENDER_TARGET_SURFACE "RenderTarget Surfaces"
#define TEXTURE_GROUP_MORPH_TARGETS "Morph Targets"

class QAngle
{
public:
	QAngle(void)
	{
		Init();
	}
	QAngle(float X, float Y, float Z)
	{
		Init(X, Y, Z);
	}
	QAngle(const float* clr)
	{
		Init(clr[0], clr[1], clr[2]);
	}

	void Init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f)
	{
		pitch = ix;
		yaw = iy;
		roll = iz;
	}

	float operator[](int i) const
	{
		return ((float*)this)[i];
	}
	float& operator[](int i)
	{
		return ((float*)this)[i];
	}

	QAngle& operator+=(const QAngle& v)
	{
		pitch += v.pitch; yaw += v.yaw; roll += v.roll;
		return *this;
	}
	QAngle& operator-=(const QAngle& v)
	{
		pitch -= v.pitch; yaw -= v.yaw; roll -= v.roll;
		return *this;
	}
	QAngle& operator*=(float fl)
	{
		pitch *= fl;
		yaw *= fl;
		roll *= fl;
		return *this;
	}
	QAngle& operator*=(const QAngle& v)
	{
		pitch *= v.pitch;
		yaw *= v.yaw;
		roll *= v.roll;
		return *this;
	}
	QAngle& operator/=(const QAngle& v)
	{
		pitch /= v.pitch;
		yaw /= v.yaw;
		roll /= v.roll;
		return *this;
	}
	QAngle& operator+=(float fl)
	{
		pitch += fl;
		yaw += fl;
		roll += fl;
		return *this;
	}
	QAngle& operator/=(float fl)
	{
		pitch /= fl;
		yaw /= fl;
		roll /= fl;
		return *this;
	}
	QAngle& operator-=(float fl)
	{
		pitch -= fl;
		yaw -= fl;
		roll -= fl;
		return *this;
	}

	QAngle& operator=(const QAngle &vOther)
	{
		pitch = vOther.pitch; yaw = vOther.yaw; roll = vOther.roll;
		return *this;
	}

	QAngle operator-(void) const
	{
		return QAngle(-pitch, -yaw, -roll);
	}
	QAngle operator+(const QAngle& v) const
	{
		return QAngle(pitch + v.pitch, yaw + v.yaw, roll + v.roll);
	}
	QAngle operator-(const QAngle& v) const
	{
		return QAngle(pitch - v.pitch, yaw - v.yaw, roll - v.roll);
	}
	QAngle operator*(float fl) const
	{
		return QAngle(pitch * fl, yaw * fl, roll * fl);
	}
	QAngle operator*(const QAngle& v) const
	{
		return QAngle(pitch * v.pitch, yaw * v.yaw, roll * v.roll);
	}
	QAngle operator/(float fl) const
	{
		return QAngle(pitch / fl, yaw / fl, roll / fl);
	}
	QAngle operator/(const QAngle& v) const
	{
		return QAngle(pitch / v.pitch, yaw / v.yaw, roll / v.roll);
	}

	float Length() const
	{
		return sqrt(pitch*pitch + yaw*yaw + roll*roll);
	}
	float LengthSqr(void) const
	{
		return (pitch*pitch + yaw*yaw + roll*roll);
	}
	bool IsZero(float tolerance = 0.01f) const
	{
		return (pitch > -tolerance && pitch < tolerance &&
			yaw > -tolerance && yaw < tolerance &&
			roll > -tolerance && roll < tolerance);
	}

	float pitch;
	float yaw;
	float roll;
};

inline QAngle operator*(float lhs, const QAngle& rhs)
{
	return rhs * lhs;
}
inline QAngle operator/(float lhs, const QAngle& rhs)
{
	return rhs / lhs;
}

class Vector
{
public:
	Vector(void)
	{
		Invalidate();
	}
	Vector(float X, float Y, float Z)
	{
		x = X;
		y = Y;
		z = Z;
	}
	Vector(const float* clr)
	{
		x = clr[0];
		y = clr[1];
		z = clr[2];
	}

	void Init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f)
	{
		x = ix; y = iy; z = iz;
	}
	bool IsValid() const
	{
		return std::isfinite(x) && std::isfinite(y) && std::isfinite(z);
	}
	void Invalidate()
	{
		x = y = z = std::numeric_limits<float>::infinity();
	}

	float& operator[](int i)
	{
		return ((float*)this)[i];
	}
	float operator[](int i) const
	{
		return ((float*)this)[i];
	}

	void Zero()
	{
		x = y = z = 0.0f;
	}

	bool operator==(const Vector& src) const
	{
		return (src.x == x) && (src.y == y) && (src.z == z);
	}
	bool operator!=(const Vector& src) const
	{
		return (src.x != x) || (src.y != y) || (src.z != z);
	}

	Vector& operator+=(const Vector& v)
	{
		x += v.x; y += v.y; z += v.z;
		return *this;
	}
	Vector& operator-=(const Vector& v)
	{
		x -= v.x; y -= v.y; z -= v.z;
		return *this;
	}
	Vector& operator*=(float fl)
	{
		x *= fl;
		y *= fl;
		z *= fl;
		return *this;
	}
	Vector& operator*=(const Vector& v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}
	Vector& operator/=(const Vector& v)
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
		return *this;
	}
	Vector& operator+=(float fl)
	{
		x += fl;
		y += fl;
		z += fl;
		return *this;
	}
	Vector& operator/=(float fl)
	{
		x /= fl;
		y /= fl;
		z /= fl;
		return *this;
	}
	Vector& operator-=(float fl)
	{
		x -= fl;
		y -= fl;
		z -= fl;
		return *this;
	}

	void NormalizeInPlace()
	{
		*this = Normalized();
	}
	Vector Normalized() const
	{
		Vector res = *this;
		float l = res.Length();
		if (l != 0.0f) {
			res /= l;
		}
		else {
			res.x = res.y = res.z = 0.0f;
		}
		return res;
	}

	float DistTo(const Vector &vOther) const
	{
		Vector delta;

		delta.x = x - vOther.x;
		delta.y = y - vOther.y;
		delta.z = z - vOther.z;

		return delta.Length();
	}
	float DistToSqr(const Vector &vOther) const
	{
		Vector delta;

		delta.x = x - vOther.x;
		delta.y = y - vOther.y;
		delta.z = z - vOther.z;

		return delta.LengthSqr();
	}
	float Dot(const Vector& vOther) const
	{
		return (x*vOther.x + y*vOther.y + z*vOther.z);
	}
	float Length() const
	{
		return sqrt(x*x + y*y + z*z);
	}
	float LengthSqr(void) const
	{
		return (x*x + y*y + z*z);
	}
	float Length2D() const
	{
		return sqrt(x*x + y*y);
	}

	Vector& operator=(const Vector &vOther)
	{
		x = vOther.x; y = vOther.y; z = vOther.z;
		return *this;
	}

	Vector Vector::operator-(void) const
	{
		return Vector(-x, -y, -z);
	}
	Vector Vector::operator+(const Vector& v) const
	{
		return Vector(x + v.x, y + v.y, z + v.z);
	}
	Vector Vector::operator-(const Vector& v) const
	{
		return Vector(x - v.x, y - v.y, z - v.z);
	}
	Vector Vector::operator*(float fl) const
	{
		return Vector(x * fl, y * fl, z * fl);
	}
	Vector Vector::operator*(const Vector& v) const
	{
		return Vector(x * v.x, y * v.y, z * v.z);
	}
	Vector Vector::operator/(float fl) const
	{
		return Vector(x / fl, y / fl, z / fl);
	}
	Vector Vector::operator/(const Vector& v) const
	{
		return Vector(x / v.x, y / v.y, z / v.z);
	}

	float x, y, z;
};

inline Vector operator*(float lhs, const Vector& rhs)
{
	return rhs * lhs;
}
inline Vector operator/(float lhs, const Vector& rhs)
{
	return rhs / lhs;
}

class __declspec(align(16)) VectorAligned : public Vector
{
public:
	inline VectorAligned(void) {};
	inline VectorAligned(float X, float Y, float Z)
	{
		Init(X, Y, Z);
	}

public:
	explicit VectorAligned(const Vector &vOther)
	{
		Init(vOther.x, vOther.y, vOther.z);
	}

	VectorAligned& operator=(const Vector &vOther)
	{
		Init(vOther.x, vOther.y, vOther.z);
		return *this;
	}

	VectorAligned& operator=(const VectorAligned &vOther)
	{
		Init(vOther.x, vOther.y, vOther.z);
		return *this;
	}

	float w;
};

struct cplane_t
{
	Vector normal;
	float dist;
	uint8_t type;   // for fast side tests
	uint8_t signbits;  // signx + (signy<<1) + (signz<<1)
	uint8_t pad[2];

};

class matrix3x4_t
{
public:
	matrix3x4_t() {}
	matrix3x4_t(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23)
	{
		m_flMatVal[0][0] = m00; m_flMatVal[0][1] = m01; m_flMatVal[0][2] = m02; m_flMatVal[0][3] = m03;
		m_flMatVal[1][0] = m10; m_flMatVal[1][1] = m11; m_flMatVal[1][2] = m12; m_flMatVal[1][3] = m13;
		m_flMatVal[2][0] = m20; m_flMatVal[2][1] = m21; m_flMatVal[2][2] = m22; m_flMatVal[2][3] = m23;
	}
	//-----------------------------------------------------------------------------
	// Creates a matrix where the X axis = forward
	// the Y axis = left, and the Z axis = up
	//-----------------------------------------------------------------------------
	void Init(const Vector& xAxis, const Vector& yAxis, const Vector& zAxis, const Vector &vecOrigin)
	{
		m_flMatVal[0][0] = xAxis.x; m_flMatVal[0][1] = yAxis.x; m_flMatVal[0][2] = zAxis.x; m_flMatVal[0][3] = vecOrigin.x;
		m_flMatVal[1][0] = xAxis.y; m_flMatVal[1][1] = yAxis.y; m_flMatVal[1][2] = zAxis.y; m_flMatVal[1][3] = vecOrigin.y;
		m_flMatVal[2][0] = xAxis.z; m_flMatVal[2][1] = yAxis.z; m_flMatVal[2][2] = zAxis.z; m_flMatVal[2][3] = vecOrigin.z;
	}

	//-----------------------------------------------------------------------------
	// Creates a matrix where the X axis = forward
	// the Y axis = left, and the Z axis = up
	//-----------------------------------------------------------------------------
	matrix3x4_t(const Vector& xAxis, const Vector& yAxis, const Vector& zAxis, const Vector &vecOrigin)
	{
		Init(xAxis, yAxis, zAxis, vecOrigin);
	}

	inline void SetOrigin(Vector const & p)
	{
		m_flMatVal[0][3] = p.x;
		m_flMatVal[1][3] = p.y;
		m_flMatVal[2][3] = p.z;
	}

	inline void Invalidate(void)
	{
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 4; j++) {
				m_flMatVal[i][j] = std::numeric_limits<float>::infinity();;
			}
		}
	}

	Vector GetXAxis()  const { return at(0); }
	Vector GetYAxis()  const { return at(1); }
	Vector GetZAxis()  const { return at(2); }
	Vector GetOrigin() const { return at(3); }

	Vector at(int i) const { return Vector{ m_flMatVal[0][i], m_flMatVal[1][i], m_flMatVal[2][i] }; }

	float *operator[](int i) { return m_flMatVal[i]; }
	const float *operator[](int i) const { return m_flMatVal[i]; }
	float *Base() { return &m_flMatVal[0][0]; }
	const float *Base() const { return &m_flMatVal[0][0]; }

	float m_flMatVal[3][4];
};

class VMatrix
{
public:

	VMatrix();
	VMatrix(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33
	);

	// Creates a matrix where the X axis = forward
	// the Y axis = left, and the Z axis = up
	VMatrix(const Vector& forward, const Vector& left, const Vector& up);

	// Construct from a 3x4 matrix
	VMatrix(const matrix3x4_t& matrix3x4);

	// Set the values in the matrix.
	void  Init(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33
	);


	// Initialize from a 3x4
	void  Init(const matrix3x4_t& matrix3x4);

	// array access
	inline float* operator[](int i)
	{
		return m[i];
	}

	inline const float* operator[](int i) const
	{
		return m[i];
	}

	// Get a pointer to m[0][0]
	inline float *Base()
	{
		return &m[0][0];
	}

	inline const float *Base() const
	{
		return &m[0][0];
	}

	void  SetLeft(const Vector &vLeft);
	void  SetUp(const Vector &vUp);
	void  SetForward(const Vector &vForward);

	void  GetBasisVectors(Vector &vForward, Vector &vLeft, Vector &vUp) const;
	void  SetBasisVectors(const Vector &vForward, const Vector &vLeft, const Vector &vUp);

	// Get/Set the translation.
	Vector & GetTranslation(Vector &vTrans) const;
	void  SetTranslation(const Vector &vTrans);

	void  PreTranslate(const Vector &vTrans);
	void  PostTranslate(const Vector &vTrans);

	matrix3x4_t& As3x4();
	const matrix3x4_t& As3x4() const;
	void  CopyFrom3x4(const matrix3x4_t &m3x4);
	void  Set3x4(matrix3x4_t& matrix3x4) const;

	bool  operator==(const VMatrix& src) const;
	bool  operator!=(const VMatrix& src) const { return !(*this == src); }

	// Access the basis vectors.
	Vector  GetLeft() const;
	Vector  GetUp() const;
	Vector  GetForward() const;
	Vector  GetTranslation() const;


	// Matrix->vector operations.
public:
	// Multiply by a 3D vector (same as operator*).
	void  V3Mul(const Vector &vIn, Vector &vOut) const;

	// Multiply by a 4D vector.
	//void  V4Mul( const Vector4D &vIn, Vector4D &vOut ) const;

	// Applies the rotation (ignores translation in the matrix). (This just calls VMul3x3).
	Vector  ApplyRotation(const Vector &vVec) const;

	// Multiply by a vector (divides by w, assumes input w is 1).
	Vector  operator*(const Vector &vVec) const;

	// Multiply by the upper 3x3 part of the matrix (ie: only apply rotation).
	Vector  VMul3x3(const Vector &vVec) const;

	// Apply the inverse (transposed) rotation (only works on pure rotation matrix)
	Vector  VMul3x3Transpose(const Vector &vVec) const;

	// Multiply by the upper 3 rows.
	Vector  VMul4x3(const Vector &vVec) const;

	// Apply the inverse (transposed) transformation (only works on pure rotation/translation)
	Vector  VMul4x3Transpose(const Vector &vVec) const;


	// Matrix->plane operations.
	//public:
	// Transform the plane. The matrix can only contain translation and rotation.
	//void  TransformPlane( const VPlane &inPlane, VPlane &outPlane ) const;

	// Just calls TransformPlane and returns the result.
	//VPlane  operator*(const VPlane &thePlane) const;

	// Matrix->matrix operations.
public:

	VMatrix& operator=(const VMatrix &mOther);

	// Add two matrices.
	const VMatrix& operator+=(const VMatrix &other);

	// Add/Subtract two matrices.
	VMatrix  operator+(const VMatrix &other) const;
	VMatrix  operator-(const VMatrix &other) const;

	// Negation.
	VMatrix  operator-() const;

	// Return inverse matrix. Be careful because the results are undefined 
	// if the matrix doesn't have an inverse (ie: InverseGeneral returns false).
	VMatrix  operator~() const;

	// Matrix operations.
public:
	// Set to identity.
	void  Identity();
	bool  IsIdentity() const;
public:
	// The matrix.
	float  m[4][4];
};

inline void MatrixGetColumn(const matrix3x4_t &src, int nCol, Vector& pColumn)
{
	pColumn.x = src[0][nCol];
	pColumn.y = src[1][nCol];
	pColumn.z = src[2][nCol];
}

inline void MatrixPosition(const matrix3x4_t &matrix, Vector &position)
{
	MatrixGetColumn(matrix, 3, position);
}

#define SIGNONSTATE_NONE		0	// no state yet, about to connect
#define SIGNONSTATE_CHALLENGE	1	// client challenging server, all OOB packets
#define SIGNONSTATE_CONNECTED	2	// client is connected to server, netchans ready
#define SIGNONSTATE_NEW			3	// just got serverinfo and string tables
#define SIGNONSTATE_PRESPAWN	4	// received signon buffers
#define SIGNONSTATE_SPAWN		5	// ready to receive entity packets
#define SIGNONSTATE_FULL		6	// we are fully connected, first non-delta packet received
#define SIGNONSTATE_CHANGELEVEL	7	// server is changing level, please wait

enum PreviewImageRetVal_t
{
	MATERIAL_PREVIEW_IMAGE_BAD = 0,
	MATERIAL_PREVIEW_IMAGE_OK,
	MATERIAL_NO_PREVIEW_IMAGE,
};

enum MaterialVarFlags_t
{
	MATERIAL_VAR_DEBUG = (1 << 0),
	MATERIAL_VAR_NO_DEBUG_OVERRIDE = (1 << 1),
	MATERIAL_VAR_NO_DRAW = (1 << 2),
	MATERIAL_VAR_USE_IN_FILLRATE_MODE = (1 << 3),
	MATERIAL_VAR_VERTEXCOLOR = (1 << 4),
	MATERIAL_VAR_VERTEXALPHA = (1 << 5),
	MATERIAL_VAR_SELFILLUM = (1 << 6),
	MATERIAL_VAR_ADDITIVE = (1 << 7),
	MATERIAL_VAR_ALPHATEST = (1 << 8),
	//MATERIAL_VAR_UNUSED = (1 << 9),
	MATERIAL_VAR_ZNEARER = (1 << 10),
	MATERIAL_VAR_MODEL = (1 << 11),
	MATERIAL_VAR_FLAT = (1 << 12),
	MATERIAL_VAR_NOCULL = (1 << 13),
	MATERIAL_VAR_NOFOG = (1 << 14),
	MATERIAL_VAR_IGNOREZ = (1 << 15),
	MATERIAL_VAR_DECAL = (1 << 16),
	MATERIAL_VAR_ENVMAPSPHERE = (1 << 17), // OBSOLETE
	MATERIAL_VAR_UNUSED = (1 << 18), // UNUSED
	MATERIAL_VAR_ENVMAPCAMERASPACE = (1 << 19), // OBSOLETE
	MATERIAL_VAR_BASEALPHAENVMAPMASK = (1 << 20),
	MATERIAL_VAR_TRANSLUCENT = (1 << 21),
	MATERIAL_VAR_NORMALMAPALPHAENVMAPMASK = (1 << 22),
	MATERIAL_VAR_NEEDS_SOFTWARE_SKINNING = (1 << 23), // OBSOLETE
	MATERIAL_VAR_OPAQUETEXTURE = (1 << 24),
	MATERIAL_VAR_ENVMAPMODE = (1 << 25), // OBSOLETE
	MATERIAL_VAR_SUPPRESS_DECALS = (1 << 26),
	MATERIAL_VAR_HALFLAMBERT = (1 << 27),
	MATERIAL_VAR_WIREFRAME = (1 << 28),
	MATERIAL_VAR_ALLOWALPHATOCOVERAGE = (1 << 29),
	MATERIAL_VAR_ALPHA_MODIFIED_BY_PROXY = (1 << 30),
	MATERIAL_VAR_VERTEXFOG = (1 << 31),
};

typedef unsigned short ModelInstanceHandle_t;
typedef void* LightCacheHandle_t;
typedef void* StudioDecalHandle_t;
typedef int OverrideType_t;
typedef int ImageFormat;
typedef int VertexFormat_t;
typedef int MaterialPropertyTypes_t;

class IClientRenderable;
class IMaterial;
class CStudioHdr;
class IMatRenderContext;
class DataCacheHandle_t;
class ITexture;
class IMaterialVar;
class KeyValues;
struct model_t;
struct mstudioanimdesc_t;
struct mstudioseqdesc_t;
struct Ray_t;
struct DrawModelInfo_t;
struct studiohwdata_t;
struct MaterialLightingState_t;
struct ColorMeshInfo_t;

typedef float Quaternion[4];
typedef float RadianEuler[3];

struct mstudiobone_t
{
	int     sznameindex;
	int     parent;		// parent bone
	int     bonecontroller[6];	// bone controller index, -1 == none
	Vector  pos;
	Quaternion  quat;
	RadianEuler rot;
	Vector      posscale;
	Vector      rotscale;

	matrix3x4_t poseToBone;
	Quaternion  qAlignment;
	int					flags;
};

struct mstudiobbox_t
{
	int         bone;
	int         group;
	Vector      bbmin;
	Vector      bbmax;
	int         szhitboxnameindex;
	int32_t     m_iPad01[3];
	float       m_flRadius;
	int32_t     m_iPad02[4];

	const char* GetName()
	{
		if (!szhitboxnameindex) return nullptr;
		return (const char*)((uint8_t*)this + szhitboxnameindex);
	}
};

struct model_t
{
	void*   fnHandle;               //0x0000 
	char    szName[260];            //0x0004 
	__int32 nLoadFlags;             //0x0108 
	__int32 nServerCount;           //0x010C 
	__int32 type;                   //0x0110 
	__int32 flags;                  //0x0114 
	Vector  vecMins;                //0x0118 
	Vector  vecMaxs;                //0x0124 
	float   radius;                 //0x0130 
	char    pad[0x1C];              //0x0134
};//Size=0x0150

#define MAX_QPATH  260

#define BONE_CALCULATE_MASK             0x1F
#define BONE_PHYSICALLY_SIMULATED       0x01    // bone is physically simulated when physics are active
#define BONE_PHYSICS_PROCEDURAL         0x02    // procedural when physics is active
#define BONE_ALWAYS_PROCEDURAL          0x04    // bone is always procedurally animated
#define BONE_SCREEN_ALIGN_SPHERE        0x08    // bone aligns to the screen, not constrained in motion.
#define BONE_SCREEN_ALIGN_CYLINDER      0x10    // bone aligns to the screen, constrained by it's own axis.

#define BONE_USED_MASK                  0x0007FF00
#define BONE_USED_BY_ANYTHING           0x0007FF00
#define BONE_USED_BY_HITBOX             0x00000100    // bone (or child) is used by a hit box
#define BONE_USED_BY_ATTACHMENT         0x00000200    // bone (or child) is used by an attachment point
#define BONE_USED_BY_VERTEX_MASK        0x0003FC00
#define BONE_USED_BY_VERTEX_LOD0        0x00000400    // bone (or child) is used by the toplevel model via skinned vertex
#define BONE_USED_BY_VERTEX_LOD1        0x00000800    
#define BONE_USED_BY_VERTEX_LOD2        0x00001000  
#define BONE_USED_BY_VERTEX_LOD3        0x00002000
#define BONE_USED_BY_VERTEX_LOD4        0x00004000
#define BONE_USED_BY_VERTEX_LOD5        0x00008000
#define BONE_USED_BY_VERTEX_LOD6        0x00010000
#define BONE_USED_BY_VERTEX_LOD7        0x00020000
#define BONE_USED_BY_BONE_MERGE         0x00040000    // bone is available for bone merge to occur against it

#define BONE_USED_BY_VERTEX_AT_LOD(lod) ( BONE_USED_BY_VERTEX_LOD0 << (lod) )
#define BONE_USED_BY_ANYTHING_AT_LOD(lod) ( ( BONE_USED_BY_ANYTHING & ~BONE_USED_BY_VERTEX_MASK ) | BONE_USED_BY_VERTEX_AT_LOD(lod) )

#define MAX_NUM_LODS 8
#define MAXSTUDIOBONES		128		// total bones actually used

#define BONE_TYPE_MASK                  0x00F00000
#define BONE_FIXED_ALIGNMENT            0x00100000    // bone can't spin 360 degrees, all interpolation is normalized around a fixed orientation

#define BONE_HAS_SAVEFRAME_POS          0x00200000    // Vector48
#define BONE_HAS_SAVEFRAME_ROT64        0x00400000    // Quaternion64
#define BONE_HAS_SAVEFRAME_ROT32        0x00800000    // Quaternion32


enum modtype_t
{
	mod_bad = 0,
	mod_brush,
	mod_sprite,
	mod_studio
};

enum Hitboxes
{
	HITBOX_HEAD,
	HITBOX_NECK,
	HITBOX_LOWER_NECK,
	HITBOX_PELVIS,
	HITBOX_STOMACH,
	HITBOX_LOWER_CHEST,
	HITBOX_CHEST,
	HITBOX_UPPER_CHEST,
	HITBOX_RIGHT_THIGH,
	HITBOX_LEFT_THIGH,
	HITBOX_RIGHT_CALF,
	HITBOX_LEFT_CALF,
	HITBOX_RIGHT_FOOT,
	HITBOX_LEFT_FOOT,
	HITBOX_RIGHT_HAND,
	HITBOX_LEFT_HAND,
	HITBOX_RIGHT_UPPER_ARM,
	HITBOX_RIGHT_FOREARM,
	HITBOX_LEFT_UPPER_ARM,
	HITBOX_LEFT_FOREARM,
	HITBOX_MAX
};

typedef unsigned short MDLHandle_t;

struct mstudiohitboxset_t
{
	int    sznameindex;
	int    numhitboxes;
	int    hitboxindex;

	const char* GetName()
	{
		if (!sznameindex) return nullptr;
		return (const char*)((uint8_t*)this + sznameindex);
	}

	mstudiobbox_t* GetHitbox(int i)
	{
		if (i > numhitboxes) return nullptr;
		return (mstudiobbox_t*)((uint8_t*)this + hitboxindex) + i;
	}
};

class studiohdr_t
{
public:
	__int32 id;                     //0x0000 
	__int32 version;                //0x0004 
	long    checksum;               //0x0008 
	char    szName[64];             //0x000C 
	__int32 length;                 //0x004C 
	Vector  vecEyePos;              //0x0050 
	Vector  vecIllumPos;            //0x005C 
	Vector  vecHullMin;             //0x0068 
	Vector  vecHullMax;             //0x0074 
	Vector  vecBBMin;               //0x0080 
	Vector  vecBBMax;               //0x008C 
	__int32 flags;                  //0x0098 
	__int32 numbones;               //0x009C 
	__int32 boneindex;              //0x00A0 
	__int32 numbonecontrollers;     //0x00A4 
	__int32 bonecontrollerindex;    //0x00A8 
	__int32 numhitboxsets;          //0x00AC 
	__int32 hitboxsetindex;         //0x00B0 
	__int32 numlocalanim;           //0x00B4 
	__int32 localanimindex;         //0x00B8 
	__int32 numlocalseq;            //0x00BC 
	__int32 localseqindex;          //0x00C0 
	__int32 activitylistversion;    //0x00C4 
	__int32 eventsindexed;          //0x00C8 
	__int32 numtextures;            //0x00CC 
	__int32 textureindex;           //0x00D0

	mstudiohitboxset_t* GetHitboxSet(int i)
	{
		if (i > numhitboxsets) return nullptr;
		return (mstudiohitboxset_t*)((uint8_t*)this + hitboxsetindex) + i;
	}
	mstudiobone_t* GetBone(int i)
	{
		if (i > numbones) return nullptr;
		return (mstudiobone_t*)((uint8_t*)this + boneindex) + i;
	}

};//Size=0x00D4

struct DrawModelState_t
{
	studiohdr_t*            m_pStudioHdr;
	studiohwdata_t*         m_pStudioHWData;
	IClientRenderable*      m_pRenderable;
	const matrix3x4_t*      m_pModelToWorld;
	StudioDecalHandle_t     m_decals;
	int                     m_drawFlags;
	int                     m_lod;
};

struct StaticPropRenderInfo_t
{
	const matrix3x4_t*      pModelToWorld;
	const model_t*          pModel;
	IClientRenderable*      pRenderable;
	Vector*                 pLightingOrigin;
	short                   skin;
	ModelInstanceHandle_t   instance;
};

struct ModelRenderInfo_t
{
	Vector                  origin;
	QAngle                  angles;
	IClientRenderable*      pRenderable;
	const model_t*          pModel;
	const matrix3x4_t*      pModelToWorld;
	const matrix3x4_t*      pLightingOffset;
	const Vector*           pLightingOrigin;
	int                     flags;
	int                     entity_index;
	int                     skin;
	int                     body;
	int                     hitboxset;
	ModelInstanceHandle_t   instance;

	ModelRenderInfo_t()
	{
		pModelToWorld = NULL;
		pLightingOffset = NULL;
		pLightingOrigin = NULL;
	}
};

struct LightingQuery_t
{
	Vector                  m_LightingOrigin;
	ModelInstanceHandle_t   m_InstanceHandle;
	bool                    m_bAmbientBoost;
};

struct StaticLightingQuery_t : public LightingQuery_t
{
	IClientRenderable*        m_pRenderable;
};

class IMaterial
{
public:
	virtual const char*             GetName() const = 0;
	virtual const char*             GetTextureGroupName() const = 0;
	virtual PreviewImageRetVal_t    GetPreviewImageProperties(int *width, int *height, ImageFormat *imageFormat, bool* isTranslucent) const = 0;
	virtual PreviewImageRetVal_t    GetPreviewImage(unsigned char *data, int width, int height, ImageFormat imageFormat) const = 0;
	virtual int                     GetMappingWidth() = 0;
	virtual int                     GetMappingHeight() = 0;
	virtual int                     GetNumAnimationFrames() = 0;
	virtual bool                    InMaterialPage(void) = 0;
	virtual    void                 GetMaterialOffset(float *pOffset) = 0;
	virtual void                    GetMaterialScale(float *pScale) = 0;
	virtual IMaterial*              GetMaterialPage(void) = 0;
	virtual IMaterialVar*           FindVar(const char *varName, bool *found, bool complain = true) = 0;
	virtual void                    IncrementReferenceCount(void) = 0;
	virtual void                    DecrementReferenceCount(void) = 0;
	inline void                     AddRef() { IncrementReferenceCount(); }
	inline void                     Release() { DecrementReferenceCount(); }
	virtual int                     GetEnumerationID(void) const = 0;
	virtual void                    GetLowResColorSample(float s, float t, float *color) const = 0;
	virtual void                    RecomputeStateSnapshots() = 0;
	virtual bool                    IsTranslucent() = 0;
	virtual bool                    IsAlphaTested() = 0;
	virtual bool                    IsVertexLit() = 0;
	virtual VertexFormat_t          GetVertexFormat() const = 0;
	virtual bool                    HasProxy(void) const = 0;
	virtual bool                    UsesEnvCubemap(void) = 0;
	virtual bool                    NeedsTangentSpace(void) = 0;
	virtual bool                    NeedsPowerOfTwoFrameBufferTexture(bool bCheckSpecificToThisFrame = true) = 0;
	virtual bool                    NeedsFullFrameBufferTexture(bool bCheckSpecificToThisFrame = true) = 0;
	virtual bool                    NeedsSoftwareSkinning(void) = 0;
	virtual void                    AlphaModulate(float alpha) = 0;
	virtual void                    ColorModulate(float r, float g, float b) = 0;
	virtual void                    SetMaterialVarFlag(MaterialVarFlags_t flag, bool on) = 0;
	virtual bool                    GetMaterialVarFlag(MaterialVarFlags_t flag) const = 0;
	virtual void                    GetReflectivity(Vector& reflect) = 0;
	virtual bool                    GetPropertyFlag(MaterialPropertyTypes_t type) = 0;
	virtual bool                    IsTwoSided() = 0;
	virtual void                    SetShader(const char *pShaderName) = 0;
	virtual int                     GetNumPasses(void) = 0;
	virtual int                     GetTextureMemoryBytes(void) = 0;
	virtual void                    Refresh() = 0;
	virtual bool                    NeedsLightmapBlendAlpha(void) = 0;
	virtual bool                    NeedsSoftwareLighting(void) = 0;
	virtual int                     ShaderParamCount() const = 0;
	virtual IMaterialVar**          GetShaderParams(void) = 0;
	virtual bool                    IsErrorMaterial() const = 0;
	virtual void                    Unused() = 0;
	virtual float                   GetAlphaModulation() = 0;
	virtual void                    GetColorModulation(float *r, float *g, float *b) = 0;
	virtual bool                    IsTranslucentUnderModulation(float fAlphaModulation = 1.0f) const = 0;
	virtual IMaterialVar*           FindVarFast(char const *pVarName, unsigned int *pToken) = 0;
	virtual void                    SetShaderAndParams(KeyValues *pKeyValues) = 0;
	virtual const char*             GetShaderName() const = 0;
	virtual void                    DeleteIfUnreferenced() = 0;
	virtual bool                    IsSpriteCard() = 0;
	virtual void                    CallBindProxy(void *proxyData) = 0;
	virtual void                    RefreshPreservingMaterialVars() = 0;
	virtual bool                    WasReloadedFromWhitelist() = 0;
	virtual bool                    SetTempExcluded(bool bSet, int nExcludedDimensionLimit) = 0;
	virtual int                     GetReferenceCount() const = 0;
};

class IVModelRender
{
public:
	virtual int                     DrawModel(int flags, IClientRenderable *pRenderable, ModelInstanceHandle_t instance, int entity_index, const model_t *model, Vector const& origin, QAngle const& angles, int skin, int body, int hitboxset, const matrix3x4_t *modelToWorld = NULL, const matrix3x4_t *pLightingOffset = NULL) = 0;
	virtual void                    ForcedMaterialOverride(IMaterial *newMaterial, OverrideType_t nOverrideType = 0, int nOverrides = 0) = 0;
	virtual bool                    IsForcedMaterialOverride(void) = 0;
	virtual void                    SetViewTarget(const CStudioHdr *pStudioHdr, int nBodyIndex, const Vector& target) = 0;
	virtual ModelInstanceHandle_t   CreateInstance(IClientRenderable *pRenderable, LightCacheHandle_t *pCache = NULL) = 0;
	virtual void                    DestroyInstance(ModelInstanceHandle_t handle) = 0;
	virtual void                    SetStaticLighting(ModelInstanceHandle_t handle, LightCacheHandle_t* pHandle) = 0;
	virtual LightCacheHandle_t      GetStaticLighting(ModelInstanceHandle_t handle) = 0;
	virtual bool                    ChangeInstance(ModelInstanceHandle_t handle, IClientRenderable *pRenderable) = 0;
	virtual void                    AddDecal(ModelInstanceHandle_t handle, Ray_t const& ray, Vector const& decalUp, int decalIndex, int body, bool noPokeThru, int maxLODToDecal) = 0;
	virtual void                    RemoveAllDecals(ModelInstanceHandle_t handle) = 0;
	virtual bool                    ModelHasDecals(ModelInstanceHandle_t handle) = 0;
	virtual void                    RemoveAllDecalsFromAllModels() = 0;
	virtual matrix3x4_t*            DrawModelShadowSetup(IClientRenderable *pRenderable, int body, int skin, DrawModelInfo_t *pInfo, matrix3x4_t *pCustomBoneToWorld = NULL) = 0;
	virtual void                    DrawModelShadow(IClientRenderable *pRenderable, const DrawModelInfo_t &info, matrix3x4_t *pCustomBoneToWorld = NULL) = 0;
	virtual bool                    RecomputeStaticLighting(ModelInstanceHandle_t handle) = 0;
	virtual void                    ReleaseAllStaticPropColorData(void) = 0;
	virtual void                    RestoreAllStaticPropColorData(void) = 0;
	virtual int                     DrawModelEx(ModelRenderInfo_t &pInfo) = 0;
	virtual int                     DrawModelExStaticProp(ModelRenderInfo_t &pInfo) = 0;
	virtual bool                    DrawModelSetup(ModelRenderInfo_t &pInfo, DrawModelState_t *pState, matrix3x4_t **ppBoneToWorldOut) = 0;
	virtual void                    DrawModelExecute(IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld = NULL) = 0;
	virtual void                    SetupLighting(const Vector &vecCenter) = 0;
	virtual int                     DrawStaticPropArrayFast(StaticPropRenderInfo_t *pProps, int count, bool bShadowDepth) = 0;
	virtual void                    SuppressEngineLighting(bool bSuppress) = 0;
	virtual void                    SetupColorMeshes(int nTotalVerts) = 0;
	virtual void                    SetupLightingEx(const Vector &vecCenter, ModelInstanceHandle_t handle) = 0;
	virtual bool                    GetBrightestShadowingLightSource(const Vector &vecCenter, Vector& lightPos, Vector& lightBrightness, bool bAllowNonTaggedLights) = 0;
	virtual void                    ComputeLightingState(int nCount, const LightingQuery_t *pQuery, MaterialLightingState_t *pState, ITexture **ppEnvCubemapTexture) = 0;
	virtual void                    GetModelDecalHandles(StudioDecalHandle_t *pDecals, int nDecalStride, int nCount, const ModelInstanceHandle_t *pHandles) = 0;
	virtual void                    ComputeStaticLightingState(int nCount, const StaticLightingQuery_t *pQuery, MaterialLightingState_t *pState, MaterialLightingState_t *pDecalState, ColorMeshInfo_t **ppStaticLighting, ITexture **ppEnvCubemapTexture, DataCacheHandle_t *pColorMeshHandles) = 0;
	virtual void                    CleanupStaticLightingState(int nCount, DataCacheHandle_t *pColorMeshHandles) = 0;
};

enum CompiledVtfFlags
{
	TEXTUREFLAGS_POINTSAMPLE = 0x00000001,
	TEXTUREFLAGS_TRILINEAR = 0x00000002,
	TEXTUREFLAGS_CLAMPS = 0x00000004,
	TEXTUREFLAGS_CLAMPT = 0x00000008,
	TEXTUREFLAGS_ANISOTROPIC = 0x00000010,
	TEXTUREFLAGS_HINT_DXT5 = 0x00000020,
	TEXTUREFLAGS_PWL_CORRECTED = 0x00000040,
	TEXTUREFLAGS_NORMAL = 0x00000080,
	TEXTUREFLAGS_NOMIP = 0x00000100,
	TEXTUREFLAGS_NOLOD = 0x00000200,
	TEXTUREFLAGS_ALL_MIPS = 0x00000400,
	TEXTUREFLAGS_PROCEDURAL = 0x00000800,
	TEXTUREFLAGS_ONEBITALPHA = 0x00001000,
	TEXTUREFLAGS_EIGHTBITALPHA = 0x00002000,
	TEXTUREFLAGS_ENVMAP = 0x00004000,
	TEXTUREFLAGS_RENDERTARGET = 0x00008000,
	TEXTUREFLAGS_DEPTHRENDERTARGET = 0x00010000,
	TEXTUREFLAGS_NODEBUGOVERRIDE = 0x00020000,
	TEXTUREFLAGS_SINGLECOPY = 0x00040000,
	TEXTUREFLAGS_PRE_SRGB = 0x00080000,
	TEXTUREFLAGS_UNUSED_00100000 = 0x00100000,
	TEXTUREFLAGS_UNUSED_00200000 = 0x00200000,
	TEXTUREFLAGS_UNUSED_00400000 = 0x00400000,
	TEXTUREFLAGS_NODEPTHBUFFER = 0x00800000,
	TEXTUREFLAGS_UNUSED_01000000 = 0x01000000,
	TEXTUREFLAGS_CLAMPU = 0x02000000,
	TEXTUREFLAGS_VERTEXTEXTURE = 0x04000000,
	TEXTUREFLAGS_SSBUMP = 0x08000000,
	TEXTUREFLAGS_UNUSED_10000000 = 0x10000000,
	TEXTUREFLAGS_BORDER = 0x20000000,
	TEXTUREFLAGS_UNUSED_40000000 = 0x40000000,
	TEXTUREFLAGS_UNUSED_80000000 = 0x80000000
};

enum StandardLightmap_t
{
	MATERIAL_SYSTEM_LIGHTMAP_PAGE_WHITE = -1,
	MATERIAL_SYSTEM_LIGHTMAP_PAGE_WHITE_BUMP = -2,
	MATERIAL_SYSTEM_LIGHTMAP_PAGE_USER_DEFINED = -3
};


struct MaterialSystem_SortInfo_t
{
	IMaterial	*material;
	int			lightmapPageID;
};

enum MaterialThreadMode_t
{
	MATERIAL_SINGLE_THREADED,
	MATERIAL_QUEUED_SINGLE_THREADED,
	MATERIAL_QUEUED_THREADED
};

enum MaterialContextType_t
{
	MATERIAL_HARDWARE_CONTEXT,
	MATERIAL_QUEUED_CONTEXT,
	MATERIAL_NULL_CONTEXT
};

enum
{
	MATERIAL_ADAPTER_NAME_LENGTH = 512
};

struct MaterialTextureInfo_t
{
	int iExcludeInformation;
};

struct ApplicationPerformanceCountersInfo_t
{
	float msMain;
	float msMST;
	float msGPU;
	float msFlip;
	float msTotal;
};

struct ApplicationInstantCountersInfo_t
{
	uint32_t m_nCpuActivityMask;
	uint32_t m_nDeferredWordsAllocated;
};
struct MaterialAdapterInfo_t
{
	char m_pDriverName[MATERIAL_ADAPTER_NAME_LENGTH];
	unsigned int m_VendorID;
	unsigned int m_DeviceID;
	unsigned int m_SubSysID;
	unsigned int m_Revision;
	int m_nDXSupportLevel;			// This is the *preferred* dx support level
	int m_nMinDXSupportLevel;
	int m_nMaxDXSupportLevel;
	unsigned int m_nDriverVersionHigh;
	unsigned int m_nDriverVersionLow;
};

struct MaterialVideoMode_t
{
	int m_Width;			// if width and height are 0 and you select 
	int m_Height;			// windowed mode, it'll use the window size
	ImageFormat m_Format;	// use ImageFormats (ignored for windowed mode)
	int m_RefreshRate;		// 0 == default (ignored for windowed mode)
};
enum HDRType_t
{
	HDR_TYPE_NONE,
	HDR_TYPE_INTEGER,
	HDR_TYPE_FLOAT,
};

enum RestoreChangeFlags_t
{
	MATERIAL_RESTORE_VERTEX_FORMAT_CHANGED = 0x1,
	MATERIAL_RESTORE_RELEASE_MANAGED_RESOURCES = 0x2,
};

enum RenderTargetSizeMode_t
{
	RT_SIZE_NO_CHANGE = 0,
	RT_SIZE_DEFAULT = 1,
	RT_SIZE_PICMIP = 2,
	RT_SIZE_HDR = 3,
	RT_SIZE_FULL_FRAME_BUFFER = 4,
	RT_SIZE_OFFSCREEN = 5,
	RT_SIZE_FULL_FRAME_BUFFER_ROUNDED_UP = 6
};

enum MaterialRenderTargetDepth_t
{
	MATERIAL_RT_DEPTH_SHARED = 0x0,
	MATERIAL_RT_DEPTH_SEPARATE = 0x1,
	MATERIAL_RT_DEPTH_NONE = 0x2,
	MATERIAL_RT_DEPTH_ONLY = 0x3,
};

typedef void(*MaterialBufferReleaseFunc_t)(int nChangeFlags);	// see RestoreChangeFlags_t
typedef void(*MaterialBufferRestoreFunc_t)(int nChangeFlags);	// see RestoreChangeFlags_t
typedef void(*ModeChangeCallbackFunc_t)(void);
typedef void(*EndFrameCleanupFunc_t)(void);
typedef bool(*EndFramePriorToNextContextFunc_t)(void);
typedef void(*OnLevelShutdownFunc_t)(void *pUserData);

typedef unsigned short MaterialHandle_t;
DECLARE_POINTER_HANDLE(MaterialLock_t);

class IMaterialProxyFactory;
class ITexture;
class IMaterialSystemHardwareConfig;
class CShadowMgr;
struct MaterialSystem_Config_t;
struct MaterialSystemHWID_t;
struct AspectRatioInfo_t;
class IShader;

class IMaterialSystem : public IAppSystem
{
public:

	virtual CreateInterfaceFn               Init(char const* pShaderAPIDLL, IMaterialProxyFactory *pMaterialProxyFactory, CreateInterfaceFn fileSystemFactory, CreateInterfaceFn cvarFactory = NULL) = 0;
	virtual void                            SetShaderAPI(char const *pShaderAPIDLL) = 0;
	virtual void                            SetAdapter(int nAdapter, int nFlags) = 0;
	virtual void                            ModInit() = 0;
	virtual void                            ModShutdown() = 0;
	virtual void                            SetThreadMode(MaterialThreadMode_t mode, int nServiceThread = -1) = 0;
	virtual MaterialThreadMode_t            GetThreadMode() = 0;
	virtual void                            ExecuteQueued() = 0;
	virtual void                            OnDebugEvent(const char *pEvent) = 0;
	virtual IMaterialSystemHardwareConfig*  GetHardwareConfig(const char *pVersion, int *returnCode) = 0;
	virtual void                            __unknown() = 0;
	virtual bool                            UpdateConfig(bool bForceUpdate) = 0; //20
	virtual bool                            OverrideConfig(const MaterialSystem_Config_t &config, bool bForceUpdate) = 0;
	virtual const MaterialSystem_Config_t&  GetCurrentConfigForVideoCard() const = 0;
	virtual bool                            GetRecommendedConfigurationInfo(int nDXLevel, KeyValues * pKeyValues) = 0;
	virtual int                             GetDisplayAdapterCount() const = 0;
	virtual int                             GetCurrentAdapter() const = 0;
	virtual void                            GetDisplayAdapterInfo(int adapter, MaterialAdapterInfo_t& info) const = 0;
	virtual int                             GetModeCount(int adapter) const = 0;
	virtual void                            GetModeInfo(int adapter, int mode, MaterialVideoMode_t& info) const = 0;
	virtual void                            AddModeChangeCallBack(ModeChangeCallbackFunc_t func) = 0;
	virtual void                            GetDisplayMode(MaterialVideoMode_t& mode) const = 0; //30
	virtual bool                            SetMode(void* hwnd, const MaterialSystem_Config_t &config) = 0;
	virtual bool                            SupportsMSAAMode(int nMSAAMode) = 0;
	virtual const MaterialSystemHWID_t&     GetVideoCardIdentifier(void) const = 0;
	virtual void                            SpewDriverInfo() const = 0;
	virtual void                            GetBackBufferDimensions(int &width, int &height) const = 0;
	virtual ImageFormat                     GetBackBufferFormat() const = 0;
	virtual const AspectRatioInfo_t&        GetAspectRatioInfo() const = 0;
	virtual bool                            SupportsHDRMode(HDRType_t nHDRModede) = 0;
	virtual bool                            AddView(void* hwnd) = 0;
	virtual void                            RemoveView(void* hwnd) = 0; //40
	virtual void                            SetView(void* hwnd) = 0;
	virtual void                            BeginFrame(float frameTime) = 0;
	virtual void                            EndFrame() = 0;
	virtual void                            Flush(bool flushHardware = false) = 0;
	virtual unsigned int                    GetCurrentFrameCount() = 0;
	virtual void                            SwapBuffers() = 0;
	virtual void                            EvictManagedResources() = 0;
	virtual void                            ReleaseResources(void) = 0;
	virtual void                            ReacquireResources(void) = 0;
	virtual void                            AddReleaseFunc(MaterialBufferReleaseFunc_t func) = 0; //50
	virtual void                            RemoveReleaseFunc(MaterialBufferReleaseFunc_t func) = 0;
	virtual void                            AddRestoreFunc(MaterialBufferRestoreFunc_t func) = 0;
	virtual void                            RemoveRestoreFunc(MaterialBufferRestoreFunc_t func) = 0;
	virtual void                            AddEndFrameCleanupFunc(EndFrameCleanupFunc_t func) = 0;
	virtual void                            RemoveEndFrameCleanupFunc(EndFrameCleanupFunc_t func) = 0;
	virtual void                            OnLevelShutdown() = 0;
	virtual bool                            AddOnLevelShutdownFunc(OnLevelShutdownFunc_t func, void *pUserData) = 0;
	virtual bool                            RemoveOnLevelShutdownFunc(OnLevelShutdownFunc_t func, void *pUserData) = 0;
	virtual void                            OnLevelLoadingComplete() = 0;
	virtual void                            ResetTempHWMemory(bool bExitingLevel = false) = 0; //60
	virtual void                            HandleDeviceLost() = 0;
	virtual int                             ShaderCount() const = 0;
	virtual int                             GetShaders(int nFirstShader, int nMaxCount, IShader **ppShaderList) const = 0;
	virtual int                             ShaderFlagCount() const = 0;
	virtual const char*                     ShaderFlagName(int nIndex) const = 0;
	virtual void                            GetShaderFallback(const char *pShaderName, char *pFallbackShader, int nFallbackLength) = 0;
	virtual IMaterialProxyFactory*          GetMaterialProxyFactory() = 0;
	virtual void                            SetMaterialProxyFactory(IMaterialProxyFactory* pFactory) = 0;
	virtual void                            EnableEditorMaterials() = 0;
	virtual void                            EnableGBuffers() = 0; //70
	virtual void                            SetInStubMode(bool bInStubMode) = 0;
	virtual void                            DebugPrintUsedMaterials(const char *pSearchSubString, bool bVerbose) = 0;
	virtual void                            DebugPrintUsedTextures(void) = 0;
	virtual void                            ToggleSuppressMaterial(char const* pMaterialName) = 0;
	virtual void                            ToggleDebugMaterial(char const* pMaterialName) = 0;
	virtual bool                            UsingFastClipping(void) = 0;
	virtual int                             StencilBufferBits(void) = 0; //number of bits per pixel in the stencil buffer
	virtual void                            UncacheAllMaterials() = 0;
	virtual void                            UncacheUnusedMaterials(bool bRecomputeStateSnapshots = false) = 0;
	virtual void                            CacheUsedMaterials() = 0; //80
	virtual void                            ReloadTextures() = 0;
	virtual void                            ReloadMaterials(const char *pSubString = NULL) = 0;
	virtual IMaterial*                      CreateMaterial(const char *pMaterialName, KeyValues *pVMTKeyValues) = 0;
	virtual IMaterial*                      FindMaterial(char const* pMaterialName, const char *pTextureGroupName, bool complain = true, const char *pComplainPrefix = NULL) = 0;
	virtual MaterialHandle_t                FirstMaterial() const = 0;
	virtual MaterialHandle_t                NextMaterial(MaterialHandle_t h) const = 0;
	virtual MaterialHandle_t                InvalidMaterial() const = 0;
	virtual IMaterial*                      GetMaterial(MaterialHandle_t h) const = 0;
	virtual int                             GetNumMaterials() const = 0;
	virtual ITexture*                       FindTexture(char const* pTextureName, const char *pTextureGroupName, bool complain = true) = 0;
	virtual bool                            IsTextureLoaded(char const* pTextureName) const = 0;
	virtual ITexture*                       CreateProceduralTexture(const char	*pTextureName, const char *pTextureGroupName, int w, int h, ImageFormat fmt, int nFlags) = 0;
	virtual void                            BeginRenderTargetAllocation() = 0;
	virtual void                            EndRenderTargetAllocation() = 0; // Simulate an Alt-Tab in here, which causes a release/restore of all resources
	virtual ITexture*                       CreateRenderTargetTexture(int w, int h, RenderTargetSizeMode_t sizeMode, ImageFormat	format, MaterialRenderTargetDepth_t depth = MATERIAL_RT_DEPTH_SHARED) = 0;
	virtual ITexture*                       CreateNamedRenderTargetTextureEx(const char *pRTName, int w, int h, RenderTargetSizeMode_t sizeMode, ImageFormat format, MaterialRenderTargetDepth_t depth = MATERIAL_RT_DEPTH_SHARED, unsigned int textureFlags = TEXTUREFLAGS_CLAMPS | TEXTUREFLAGS_CLAMPT, unsigned int renderTargetFlags = 0) = 0;
	virtual ITexture*                       CreateNamedRenderTargetTexture(const char *pRTName, int w, int h, RenderTargetSizeMode_t sizeMode, ImageFormat format, MaterialRenderTargetDepth_t depth = MATERIAL_RT_DEPTH_SHARED, bool bClampTexCoords = true, bool bAutoMipMap = false) = 0;
	virtual ITexture*                       CreateNamedRenderTargetTextureEx2(const char *pRTName, int w, int h, RenderTargetSizeMode_t sizeMode, ImageFormat format, MaterialRenderTargetDepth_t depth = MATERIAL_RT_DEPTH_SHARED, unsigned int textureFlags = TEXTUREFLAGS_CLAMPS | TEXTUREFLAGS_CLAMPT, unsigned int renderTargetFlags = 0) = 0;

};

typedef struct InputContextHandle_t__ *InputContextHandle_t;
struct client_textmessage_t;
struct model_t;
class SurfInfo;
class CSentence;
class CAudioSource;
class AudioState_t;
class ISpatialQuery;
class CPhysCollide;
class IAchievementMgr;
class INetChannelInfo;
class ISPSharedMemory;
class CGamestatsData;
class KeyValues;
class CSteamAPIContext;
struct Frustum_t;

typedef void(*pfnDemoCustomDataCallback)(uint8_t *pData, size_t iSize);

typedef struct player_info_s
{
	__int64         unknown;            //0x0000 
	union
	{
		__int64       steamID64;          //0x0008 - SteamID64
		struct
		{
			__int32     xuid_low;
			__int32     xuid_high;
		};
	};
	char            szName[128];        //0x0010 - Player Name
	int             userId;             //0x0090 - Unique Server Identifier
	char            szSteamID[20];      //0x0094 - STEAM_X:Y:Z
	char            pad_0x00A8[0x10];   //0x00A8
	unsigned long   iSteamID;           //0x00B8 - SteamID 
	char            szFriendsName[128];
	bool            fakeplayer;
	bool            ishltv;
	unsigned int    customfiles[4];
	unsigned char   filesdownloaded;
} player_info_t;

class IVEngineClient
{
public:
	virtual int                   GetIntersectingSurfaces(const model_t *model, const Vector &vCenter, const float radius, const bool bOnlyVisibleSurfaces, SurfInfo *pInfos, const int nMaxInfos) = 0;
	virtual Vector                GetLightForPoint(const Vector &pos, bool bClamp) = 0;
	virtual IMaterial*            TraceLineMaterialAndLighting(const Vector &start, const Vector &end, Vector &diffuseLightColor, Vector& baseColor) = 0;
	virtual const char*           ParseFile(const char *data, char *token, int maxlen) = 0;
	virtual bool                  CopyFile(const char *source, const char *destination) = 0;
	virtual void                  GetScreenSize(int& width, int& height) = 0;
	virtual void                  ServerCmd(const char *szCmdString, bool bReliable = true) = 0;
	virtual void                  ClientCmd(const char *szCmdString) = 0;
	virtual bool                  GetPlayerInfo(int ent_num, player_info_t *pinfo) = 0;
	virtual int                   GetPlayerForUserID(int userID) = 0;
	virtual client_textmessage_t* TextMessageGet(const char *pName) = 0; // 10
	virtual bool                  Con_IsVisible(void) = 0;
	virtual int                   GetLocalPlayer(void) = 0;
	virtual const model_t*        LoadModel(const char *pName, bool bProp = false) = 0;
	virtual float                 GetLastTimeStamp(void) = 0;
	virtual CSentence*            GetSentence(CAudioSource *pAudioSource) = 0; // 15
	virtual float                 GetSentenceLength(CAudioSource *pAudioSource) = 0;
	virtual bool                  IsStreaming(CAudioSource *pAudioSource) const = 0;
	virtual void                  GetViewAngles(Vector& va) = 0;
	virtual void                  SetViewAngles(Vector& va) = 0;
	virtual int                   GetMaxClients(void) = 0; // 20
	virtual const char*           Key_LookupBinding(const char *pBinding) = 0;
	virtual const char*           Key_BindingForKey(int &code) = 0;
	virtual void                  Key_SetBinding(int, char const*) = 0;
	virtual void                  StartKeyTrapMode(void) = 0;
	virtual bool                  CheckDoneKeyTrapping(int &code) = 0;
	virtual bool                  IsInGame(void) = 0;
	virtual bool                  IsConnected(void) = 0;
	virtual bool                  IsDrawingLoadingImage(void) = 0;
	virtual void                  HideLoadingPlaque(void) = 0;
	virtual void                  Con_NPrintf(int pos, const char *fmt, ...) = 0; // 30
	virtual void                  Con_NXPrintf(const struct con_nprint_s *info, const char *fmt, ...) = 0;
	virtual int                   IsBoxVisible(const Vector& mins, const Vector& maxs) = 0;
	virtual int                   IsBoxInViewCluster(const Vector& mins, const Vector& maxs) = 0;
	virtual bool                  CullBox(const Vector& mins, const Vector& maxs) = 0;
	virtual void                  Sound_ExtraUpdate(void) = 0;
	virtual const char*           GetGameDirectory(void) = 0;
	virtual const VMatrix&        WorldToScreenMatrix() = 0;
	virtual const VMatrix&        WorldToViewMatrix() = 0;
	virtual int                   GameLumpVersion(int lumpId) const = 0;
	virtual int                   GameLumpSize(int lumpId) const = 0; // 40
	virtual bool                  LoadGameLump(int lumpId, void* pBuffer, int size) = 0;
	virtual int                   LevelLeafCount() const = 0;
	virtual ISpatialQuery*        GetBSPTreeQuery() = 0;
	virtual void                  LinearToGamma(float* linear, float* gamma) = 0;
	virtual float                 LightStyleValue(int style) = 0; // 45
	virtual void                  ComputeDynamicLighting(const Vector& pt, const Vector* pNormal, Vector& color) = 0;
	virtual void                  GetAmbientLightColor(Vector& color) = 0;
	virtual int                   GetDXSupportLevel() = 0;
	virtual bool                  SupportsHDR() = 0;
	virtual void                  Mat_Stub(IMaterialSystem *pMatSys) = 0; // 50
	virtual void                  GetChapterName(char *pchBuff, int iMaxLength) = 0;
	virtual char const*           GetLevelName(void) = 0;
	virtual char const*           GetLevelNameShort(void) = 0;
	virtual char const*           GetMapGroupName(void) = 0;
	virtual struct IVoiceTweak_s* GetVoiceTweakAPI(void) = 0;
	virtual void                  SetVoiceCasterID(unsigned int someint) = 0; // 56
	virtual void                  EngineStats_BeginFrame(void) = 0;
	virtual void                  EngineStats_EndFrame(void) = 0;
	virtual void                  FireEvents() = 0;
	virtual int                   GetLeavesArea(unsigned short *pLeaves, int nLeaves) = 0;
	virtual bool                  DoesBoxTouchAreaFrustum(const Vector &mins, const Vector &maxs, int iArea) = 0; // 60
	virtual int                   GetFrustumList(Frustum_t **pList, int listMax) = 0;
	virtual bool                  ShouldUseAreaFrustum(int i) = 0;
	virtual void                  SetAudioState(const AudioState_t& state) = 0;
	virtual int                   SentenceGroupPick(int groupIndex, char *name, int nameBufLen) = 0;
	virtual int                   SentenceGroupPickSequential(int groupIndex, char *name, int nameBufLen, int sentenceIndex, int reset) = 0;
	virtual int                   SentenceIndexFromName(const char *pSentenceName) = 0;
	virtual const char*           SentenceNameFromIndex(int sentenceIndex) = 0;
	virtual int                   SentenceGroupIndexFromName(const char *pGroupName) = 0;
	virtual const char*           SentenceGroupNameFromIndex(int groupIndex) = 0;
	virtual float                 SentenceLength(int sentenceIndex) = 0;
	virtual void                  ComputeLighting(const Vector& pt, const Vector* pNormal, bool bClamp, Vector& color, Vector *pBoxColors = NULL) = 0;
	virtual void                  ActivateOccluder(int nOccluderIndex, bool bActive) = 0;
	virtual bool                  IsOccluded(const Vector &vecAbsMins, const Vector &vecAbsMaxs) = 0; // 74
	virtual int                   GetOcclusionViewId(void) = 0;
	virtual void*                 SaveAllocMemory(size_t num, size_t size) = 0;
	virtual void                  SaveFreeMemory(void *pSaveMem) = 0;
	virtual INetChannelInfo*      GetNetChannelInfo(void) = 0;
	virtual void                  DebugDrawPhysCollide(const CPhysCollide *pCollide, IMaterial *pMaterial, const matrix3x4_t& transform, const uint8_t* color) = 0; //79
	virtual void                  CheckPoint(const char *pName) = 0; // 80
	virtual void                  DrawPortals() = 0;
	virtual bool                  IsPlayingDemo(void) = 0;
	virtual bool                  IsRecordingDemo(void) = 0;
	virtual bool                  IsPlayingTimeDemo(void) = 0;
	virtual int                   GetDemoRecordingTick(void) = 0;
	virtual int                   GetDemoPlaybackTick(void) = 0;
	virtual int                   GetDemoPlaybackStartTick(void) = 0;
	virtual float                 GetDemoPlaybackTimeScale(void) = 0;
	virtual int                   GetDemoPlaybackTotalTicks(void) = 0;
	virtual bool                  IsPaused(void) = 0; // 90
	virtual float                 GetTimescale(void) const = 0;
	virtual bool                  IsTakingScreenshot(void) = 0;
	virtual bool                  IsHLTV(void) = 0;
	virtual bool                  IsLevelMainMenuBackground(void) = 0;
	virtual void                  GetMainMenuBackgroundName(char *dest, int destlen) = 0;
	virtual void                  SetOcclusionParameters(const int /*OcclusionParams_t*/ &params) = 0; // 96
	virtual void                  GetUILanguage(char *dest, int destlen) = 0;
	virtual int                   IsSkyboxVisibleFromPoint(const Vector &vecPoint) = 0;
	virtual const char*           GetMapEntitiesString() = 0;
	virtual bool                  IsInEditMode(void) = 0; // 100
	virtual float                 GetScreenAspectRatio(int viewportWidth, int viewportHeight) = 0;
	virtual bool                  REMOVED_SteamRefreshLogin(const char *password, bool isSecure) = 0;
	virtual bool                  REMOVED_SteamProcessCall(bool & finished) = 0;
	virtual unsigned int          GetEngineBuildNumber() = 0; // engines build
	virtual const char *          GetProductVersionString() = 0; // mods version number (steam.inf)
	virtual void                  GrabPreColorCorrectedFrame(int x, int y, int width, int height) = 0;
	virtual bool                  IsHammerRunning() const = 0;
	virtual void                  ExecuteClientCmd(const char *szCmdString) = 0; //108
	virtual bool                  MapHasHDRLighting(void) = 0;
	virtual bool                  MapHasLightMapAlphaData(void) = 0;
	virtual int                   GetAppID() = 0;
	virtual Vector                GetLightForPointFast(const Vector &pos, bool bClamp) = 0;
	virtual void                  ClientCmd_Unrestricted(char  const*, int, bool) = 0;
	virtual void                  ClientCmd_Unrestricted(const char *szCmdString) = 0; // 114
	virtual void                  SetRestrictServerCommands(bool bRestrict) = 0;
	virtual void                  SetRestrictClientCommands(bool bRestrict) = 0;
	virtual void                  SetOverlayBindProxy(int iOverlayID, void *pBindProxy) = 0;
	virtual bool                  CopyFrameBufferToMaterial(const char *pMaterialName) = 0;
	virtual void                  ReadConfiguration(const int iController, const bool readDefault) = 0;
	virtual void                  SetAchievementMgr(IAchievementMgr *pAchievementMgr) = 0;
	virtual IAchievementMgr*      GetAchievementMgr() = 0;
	virtual bool                  MapLoadFailed(void) = 0;
	virtual void                  SetMapLoadFailed(bool bState) = 0;
	virtual bool                  IsLowViolence() = 0;
	virtual const char*           GetMostRecentSaveGame(void) = 0;
	virtual void                  SetMostRecentSaveGame(const char *lpszFilename) = 0;
	virtual void                  StartXboxExitingProcess() = 0;
	virtual bool                  IsSaveInProgress() = 0;
	virtual bool                  IsAutoSaveDangerousInProgress(void) = 0;
	virtual unsigned int          OnStorageDeviceAttached(int iController) = 0;
	virtual void                  OnStorageDeviceDetached(int iController) = 0;
	virtual char* const           GetSaveDirName(void) = 0;
	virtual void                  WriteScreenshot(const char *pFilename) = 0;
	virtual void                  ResetDemoInterpolation(void) = 0;
	virtual int                   GetActiveSplitScreenPlayerSlot() = 0;
	virtual int                   SetActiveSplitScreenPlayerSlot(int slot) = 0;
	virtual bool                  SetLocalPlayerIsResolvable(char const *pchContext, int nLine, bool bResolvable) = 0;
	virtual bool                  IsLocalPlayerResolvable() = 0;
	virtual int                   GetSplitScreenPlayer(int nSlot) = 0;
	virtual bool                  IsSplitScreenActive() = 0;
	virtual bool                  IsValidSplitScreenSlot(int nSlot) = 0;
	virtual int                   FirstValidSplitScreenSlot() = 0; // -1 == invalid
	virtual int                   NextValidSplitScreenSlot(int nPreviousSlot) = 0; // -1 == invalid
	virtual ISPSharedMemory*      GetSinglePlayerSharedMemorySpace(const char *szName, int ent_num = (1 << 11)) = 0;
	virtual void                  ComputeLightingCube(const Vector& pt, bool bClamp, Vector *pBoxColors) = 0;
	virtual void                  RegisterDemoCustomDataCallback(const char* szCallbackSaveID, pfnDemoCustomDataCallback pCallback) = 0;
	virtual void                  RecordDemoCustomData(pfnDemoCustomDataCallback pCallback, const void *pData, size_t iDataLength) = 0;
	virtual void                  SetPitchScale(float flPitchScale) = 0;
	virtual float                 GetPitchScale(void) = 0;
	virtual bool                  LoadFilmmaker() = 0;
	virtual void                  UnloadFilmmaker() = 0;
	virtual void                  SetLeafFlag(int nLeafIndex, int nFlagBits) = 0;
	virtual void                  RecalculateBSPLeafFlags(void) = 0;
	virtual bool                  DSPGetCurrentDASRoomNew(void) = 0;
	virtual bool                  DSPGetCurrentDASRoomChanged(void) = 0;
	virtual bool                  DSPGetCurrentDASRoomSkyAbove(void) = 0;
	virtual float                 DSPGetCurrentDASRoomSkyPercent(void) = 0;
	virtual void                  SetMixGroupOfCurrentMixer(const char *szgroupname, const char *szparam, float val, int setMixerType) = 0;
	virtual int                   GetMixLayerIndex(const char *szmixlayername) = 0;
	virtual void                  SetMixLayerLevel(int index, float level) = 0;
	virtual int                   GetMixGroupIndex(char  const* groupname) = 0;
	virtual void                  SetMixLayerTriggerFactor(int i1, int i2, float fl) = 0;
	virtual void                  SetMixLayerTriggerFactor(char  const* char1, char  const* char2, float fl) = 0;
	virtual bool                  IsCreatingReslist() = 0;
	virtual bool                  IsCreatingXboxReslist() = 0;
	virtual void                  SetTimescale(float flTimescale) = 0;
	virtual void                  SetGamestatsData(CGamestatsData *pGamestatsData) = 0;
	virtual CGamestatsData*       GetGamestatsData() = 0;
	virtual void                  GetMouseDelta(int &dx, int &dy, bool b) = 0; // unknown
	virtual   const char*         Key_LookupBindingEx(const char *pBinding, int iUserId = -1, int iStartCount = 0, int iAllowJoystick = -1) = 0;
	virtual int                   Key_CodeForBinding(char  const*, int, int, int) = 0;
	virtual void                  UpdateDAndELights(void) = 0;
	virtual int                   GetBugSubmissionCount() const = 0;
	virtual void                  ClearBugSubmissionCount() = 0;
	virtual bool                  DoesLevelContainWater() const = 0;
	virtual float                 GetServerSimulationFrameTime() const = 0;
	virtual void                  SolidMoved(class IClientEntity *pSolidEnt, class ICollideable *pSolidCollide, const Vector* pPrevAbsOrigin, bool accurateBboxTriggerChecks) = 0;
	virtual void                  TriggerMoved(class IClientEntity *pTriggerEnt, bool accurateBboxTriggerChecks) = 0;
	virtual void                  ComputeLeavesConnected(const Vector &vecOrigin, int nCount, const int *pLeafIndices, bool *pIsConnected) = 0;
	virtual bool                  IsInCommentaryMode(void) = 0;
	virtual void                  SetBlurFade(float amount) = 0;
	virtual bool                  IsTransitioningToLoad() = 0;
	virtual void                  SearchPathsChangedAfterInstall() = 0;
	virtual void                  ConfigureSystemLevel(int nCPULevel, int nGPULevel) = 0;
	virtual void                  SetConnectionPassword(char const *pchCurrentPW) = 0;
	virtual CSteamAPIContext*     GetSteamAPIContext() = 0;
	virtual void                  SubmitStatRecord(char const *szMapName, unsigned int uiBlobVersion, unsigned int uiBlobSize, const void *pvBlob) = 0;
	virtual void                  ServerCmdKeyValues(KeyValues *pKeyValues) = 0; // 203
	virtual void                  SpherePaintSurface(const model_t* model, const Vector& location, unsigned char chr, float fl1, float fl2) = 0;
	virtual bool                  HasPaintmap(void) = 0;
	virtual void                  EnablePaintmapRender() = 0;
	//virtual void                TracePaintSurface( const model_t *model, const Vector& position, float radius, CUtlVector<Color>& surfColors ) = 0;
	virtual void                  SphereTracePaintSurface(const model_t* model, const Vector& position, const Vector &vec2, float radius, /*CUtlVector<unsigned char, CUtlMemory<unsigned char, int>>*/ int& utilVecShit) = 0;
	virtual void                  RemoveAllPaint() = 0;
	virtual void                  PaintAllSurfaces(unsigned char uchr) = 0;
	virtual void                  RemovePaint(const model_t* model) = 0;
	virtual bool                  IsActiveApp() = 0;
	virtual bool                  IsClientLocalToActiveServer() = 0;
	virtual void                  TickProgressBar() = 0;
	virtual InputContextHandle_t  GetInputContext(int /*EngineInputContextId_t*/ id) = 0;
	virtual void                  GetStartupImage(char* filename, int size) = 0;
	virtual bool                  IsUsingLocalNetworkBackdoor(void) = 0;
	virtual void                  SaveGame(const char*, bool, char*, int, char*, int) = 0;
	virtual void                  GetGenericMemoryStats(/* GenericMemoryStat_t */ void **) = 0;
	virtual bool                  GameHasShutdownAndFlushedMemory(void) = 0;
	virtual int                   GetLastAcknowledgedCommand(void) = 0;
	virtual void                  FinishContainerWrites(int i) = 0;
	virtual void                  FinishAsyncSave(void) = 0;
	virtual int                   GetServerTick(void) = 0;
	virtual const char*           GetModDirectory(void) = 0;
	virtual bool                  AudioLanguageChanged(void) = 0;
	virtual bool                  IsAutoSaveInProgress(void) = 0;
	virtual void                  StartLoadingScreenForCommand(const char* command) = 0;
	virtual void                  StartLoadingScreenForKeyValues(KeyValues* values) = 0;
	virtual void                  SOSSetOpvarFloat(const char*, float) = 0;
	virtual void                  SOSGetOpvarFloat(const char*, float &) = 0;
	virtual bool                  IsSubscribedMap(const char*, bool) = 0;
	virtual bool                  IsFeaturedMap(const char*, bool) = 0;
	virtual void                  GetDemoPlaybackParameters(void) = 0;
	virtual int                   GetClientVersion(void) = 0;
	virtual bool                  IsDemoSkipping(void) = 0;
	virtual void                  SetDemoImportantEventData(const KeyValues* values) = 0;
	virtual void                  ClearEvents(void) = 0;
	virtual int                   GetSafeZoneXMin(void) = 0;
	virtual bool                  IsVoiceRecording(void) = 0;
	virtual void                  ForceVoiceRecordOn(void) = 0;
	virtual bool                  IsReplay(void) = 0;
};

class IPanel
{
public:
	unsigned int GetPanel(int type)
	{
		typedef unsigned int(__thiscall *tGetPanel)(void*, int type);
		return VMT.getvfunc<tGetPanel>(this, 1)(this, type);
	}
	const char* GetName(unsigned int vguiPanel)
	{
		typedef const char* (__thiscall* OriginalFn)(void*, unsigned int);
		return VMT.getvfunc<OriginalFn>(this, 36)(this, vguiPanel);
	}
};

namespace vgui
{
	typedef unsigned long HFont;
	typedef unsigned int VPANEL;
};

enum FontFeature
{
	FONT_FEATURE_ANTIALIASED_FONTS = 1,
	FONT_FEATURE_DROPSHADOW_FONTS = 2,
	FONT_FEATURE_OUTLINE_FONTS = 6,
};

enum FontDrawType
{
	FONT_DRAW_DEFAULT = 0,
	FONT_DRAW_NONADDITIVE,
	FONT_DRAW_ADDITIVE,
	FONT_DRAW_TYPE_COUNT = 2,
};

enum FontFlags
{
	FONTFLAG_NONE,
	FONTFLAG_ITALIC = 0x001,
	FONTFLAG_UNDERLINE = 0x002,
	FONTFLAG_STRIKEOUT = 0x004,
	FONTFLAG_SYMBOL = 0x008,
	FONTFLAG_ANTIALIAS = 0x010,
	FONTFLAG_GAUSSIANBLUR = 0x020,
	FONTFLAG_ROTARY = 0x040,
	FONTFLAG_DROPSHADOW = 0x080,
	FONTFLAG_ADDITIVE = 0x100,
	FONTFLAG_OUTLINE = 0x200,
	FONTFLAG_CUSTOM = 0x400,
	FONTFLAG_BITMAP = 0x800,
};

struct IntRect
{
	int x0;
	int y0;
	int x1;
	int y1;
};

typedef float vec_t;

class Vector2D
{
public:
	// Members
	vec_t x, y;

	// Construction/destruction:
	Vector2D(void);
	Vector2D(vec_t X, vec_t Y);
	Vector2D(vec_t* clr);

	Vector2D::Vector2D(const Vector2D &vOther)
	{
		x = vOther.x; y = vOther.y;
	}

	// Initialization
	void Init(vec_t ix = 0.0f, vec_t iy = 0.0f);
	// TODO (Ilya): Should there be an init that takes a single float for consistency?

	// Got any nasty NAN's?
	bool IsValid() const;
	void Invalidate();

	// array access...
	vec_t operator[](int i) const;
	vec_t& operator[](int i);

	// Base address...
	vec_t* Base();
	vec_t const* Base() const;

	// Initialization methods
	void Random(vec_t minVal, vec_t maxVal);
	void Zero(); ///< zero out a vector

				 // equality
	bool operator==(const Vector2D& v) const;
	bool operator!=(const Vector2D& v) const;

	// arithmetic operations
	Vector2D& operator+=(const Vector2D& v)
	{
		x += v.x; y += v.y;
		return *this;
	}

	Vector2D& operator-=(const Vector2D& v)
	{
		x -= v.x; y -= v.y;
		return *this;
	}

	Vector2D& operator*=(float fl)
	{
		x *= fl;
		y *= fl;
		return *this;
	}

	Vector2D& operator*=(const Vector2D& v)
	{
		x *= v.x;
		y *= v.y;
		return *this;
	}

	Vector2D& operator/=(const Vector2D& v)
	{
		x /= v.x;
		y /= v.y;
		return *this;
	}

	// this ought to be an opcode.
	Vector2D& operator+=(float fl)
	{
		x += fl;
		y += fl;
		return *this;
	}

	// this ought to be an opcode.
	Vector2D& operator/=(float fl)
	{
		x /= fl;
		y /= fl;
		return *this;
	}
	Vector2D& operator-=(float fl)
	{
		x -= fl;
		y -= fl;
		return *this;
	}

	// negate the vector components
	void Negate();

	// Get the vector's magnitude.
	vec_t Length() const;

	// Get the vector's magnitude squared.
	vec_t LengthSqr(void) const
	{
		return (x*x + y*y);
	}

	// return true if this vector is (0,0,0) within tolerance
	bool IsZero(float tolerance = 0.01f) const
	{
		return (x > -tolerance && x < tolerance &&
			y > -tolerance && y < tolerance);
	}

	vec_t NormalizeInPlace();
	Vector2D Normalized() const;
	bool IsLengthGreaterThan(float val) const;
	bool IsLengthLessThan(float val) const;

	// check if a vector is within the box defined by two other vectors
	bool WithinAABox(Vector2D const &boxmin, Vector2D const &boxmax);

	// Get the distance from this vector to the other one.
	vec_t DistTo(const Vector2D &vOther) const;

	// Get the distance from this vector to the other one squared.
	// NJS: note, VC wasn't inlining it correctly in several deeply nested inlines due to being an 'out of line' .  
	// may be able to tidy this up after switching to VC7
	vec_t DistToSqr(const Vector2D &vOther) const
	{
		Vector2D delta;

		delta.x = x - vOther.x;
		delta.y = y - vOther.y;

		return delta.LengthSqr();
	}

	// Copy
	void CopyToArray(float* rgfl) const;

	// Multiply, add, and assign to this (ie: *this = a + b * scalar). This
	// is about 12% faster than the actual vector equation (because it's done per-component
	// rather than per-vector).
	void MulAdd(const Vector2D& a, const Vector2D& b, float scalar);

	// Dot product.
	vec_t Dot(const Vector2D& vOther) const;

	// assignment
	Vector2D& operator=(const Vector2D &vOther);

	// 2d
	vec_t Length2D(void) const;
	vec_t Length2DSqr(void) const;

	/// Get the component of this vector parallel to some other given vector
	Vector2D  ProjectOnto(const Vector2D& onto);

	// copy constructors
	// Vector2D(const Vector2D &vOther);

	// arithmetic operations
	Vector2D operator-(void) const;

	Vector2D operator+(const Vector2D& v) const;
	Vector2D operator-(const Vector2D& v) const;
	Vector2D operator*(const Vector2D& v) const;
	Vector2D operator/(const Vector2D& v) const;
	Vector2D operator*(float fl) const;
	Vector2D operator/(float fl) const;

	// Cross product between two vectors.
	Vector2D Cross(const Vector2D &vOther) const;

	// Returns a vector with the min or max in X, Y, and Z.
	Vector2D Min(const Vector2D &vOther) const;
	Vector2D Max(const Vector2D &vOther) const;
};

struct Vertex_t
{
	Vertex_t() {}
	Vertex_t(const Vector2D &pos, const Vector2D &coord = Vector2D(0, 0))
	{
		m_Position = pos;
		m_TexCoord = coord;
	}
	void Init(const Vector2D &pos, const Vector2D &coord = Vector2D(0, 0))
	{
		m_Position = pos;
		m_TexCoord = coord;
	}

	Vector2D m_Position;
	Vector2D m_TexCoord;
};

class ISurface
{
public:

	unsigned long Create_Font()
	{
		typedef unsigned int(__thiscall* OriginalFn)(PVOID);
		return VMT.getvfunc< OriginalFn >(this, 71)(this);
	}
	void DrawSetColor(int r, int g, int b, int a)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, int, int, int, int); // 
		VMT.getvfunc<OriginalFn>(this, 15)(this, r, g, b, a); // r, g, b, a
	}

	void DrawFilledRect(int x0, int y0, int x1, int y1)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, int, int, int, int);
		VMT.getvfunc<OriginalFn>(this, 16)(this, x0, y0, x1, y1);
	}
	void DrawSetTextFont(unsigned long font)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, unsigned long);
		VMT.getvfunc<OriginalFn>(this, 23)(this, font);
	}
	void DrawLine(int x0, int y0, int x1, int y1)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, int, int, int, int);
		VMT.getvfunc<OriginalFn>(this, 19)(this, x0, y0, x1, y1);
	}
	void DrawSetTextColor(int r, int g, int b, int a)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, int, int, int, int);
		VMT.getvfunc<OriginalFn>(this, 25)(this, r, g, b, a);
	}
	void DrawSetTextPos(int x, int y)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, int, int);
		VMT.getvfunc<OriginalFn>(this, 26)(this, x, y);
	}
	void DrawPrintText(const wchar_t *text, int textLen)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, const wchar_t *, int, int);
		return VMT.getvfunc<OriginalFn>(this, 28)(this, text, textLen, 0);
	}
	void SetFontGlyphSet(unsigned long &font, const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, unsigned long, const char*, int, int, int, int, int, int, int);
		VMT.getvfunc<OriginalFn>(this, 72)(this, font, windowsFontName, tall, weight, blur, scanlines, flags, 0, 0);
	}
	void GetTextSize(unsigned long font, const wchar_t *text, int &wide, int &tall)
	{
		typedef void(__thiscall* OriginalFn)(void*, unsigned long font, const wchar_t *text, int &wide, int &tall);
		VMT.getvfunc<OriginalFn>(this, 79)(this, font, text, wide, tall);
	}
	void DrawColoredCircle(int centerx, int centery, float radius, int r, int g, int b, int a)
	{
		typedef void(__thiscall* OriginalFn)(void*, int, int, float, int, int, int, int);
		VMT.getvfunc<OriginalFn>(this, 162)(this, centerx, centery, radius, r, g, b, a);
	}
	void DrawOutlinedCircle(int x, int y, int r, int seg)
	{
		typedef void(__thiscall* oDrawOutlinedCircle)(PVOID, int, int, int, int);
		return VMT.getvfunc< oDrawOutlinedCircle >(this, 103)(this, x, y, r, seg);
	}
	void LockCursor()
	{
		typedef void(__thiscall* OriginalFn)(void*);
		VMT.getvfunc<OriginalFn>(this, 67)(this);
	}
	void DrawOutlinedRect(int X, int Y, int W, int H)
	{
		typedef void(__thiscall* OriginalFn)(void*, int, int, int, int);
		return VMT.getvfunc<OriginalFn>(this, 18)(this, X, Y, W, H);
	}
};

enum ClientFrameStage_t
{
	FRAME_UNDEFINED = -1,
	FRAME_START,
	FRAME_NET_UPDATE_START,
	FRAME_NET_UPDATE_POSTDATAUPDATE_START,
	FRAME_NET_UPDATE_POSTDATAUPDATE_END,
	FRAME_NET_UPDATE_END,
	FRAME_RENDER_START,
	FRAME_RENDER_END
};

// Used by RenderView
enum RenderViewInfo_t
{
	RENDERVIEW_UNSPECIFIED = 0,
	RENDERVIEW_DRAWVIEWMODEL = (1 << 0),
	RENDERVIEW_DRAWHUD = (1 << 1),
	RENDERVIEW_SUPPRESSMONITORRENDERING = (1 << 2),
};

enum ClassId
{
	ClassId_CAI_BaseNPC = 0,
	ClassId_CAK47,
	ClassId_CBaseAnimating,
	ClassId_CBaseAnimatingOverlay,
	ClassId_CBaseAttributableItem,
	ClassId_CBaseButton,
	ClassId_CBaseCombatCharacter,
	ClassId_CBaseCombatWeapon,
	ClassId_CBaseCSGrenade,
	ClassId_CBaseCSGrenadeProjectile,
	ClassId_CBaseDoor,
	ClassId_CBaseEntity,
	ClassId_CBaseFlex,
	ClassId_CBaseGrenade,
	ClassId_CBaseParticleEntity,
	ClassId_CBasePlayer,
	ClassId_CBasePropDoor,
	ClassId_CBaseTeamObjectiveResource,
	ClassId_CBaseTempEntity,
	ClassId_CBaseToggle,
	ClassId_CBaseTrigger,
	ClassId_CBaseViewModel,
	ClassId_CBaseVPhysicsTrigger,
	ClassId_CBaseWeaponWorldModel,
	ClassId_CBeam,
	ClassId_CBeamSpotlight,
	ClassId_CBoneFollower,
	ClassId_CBreakableProp,
	ClassId_CBreakableSurface,
	ClassId_CC4,
	ClassId_CCascadeLight,
	ClassId_CChicken,
	ClassId_CColorCorrection,
	ClassId_CColorCorrectionVolume,
	ClassId_CCSGameRulesProxy,
	ClassId_CCSPlayer,
	ClassId_CCSPlayerResource,
	ClassId_CCSRagdoll,
	ClassId_CCSTeam,
	ClassId_CDEagle,
	ClassId_CDecoyGrenade,
	ClassId_CDecoyProjectile,
	ClassId_CDynamicLight,
	ClassId_CDynamicProp,
	ClassId_CEconEntity,
	ClassId_CEconWearable,
	ClassId_CEmbers,
	ClassId_CEntityDissolve,
	ClassId_CEntityFlame,
	ClassId_CEntityFreezing,
	ClassId_CEntityParticleTrail,
	ClassId_CEnvAmbientLight,
	ClassId_CEnvDetailController,
	ClassId_CEnvDOFController,
	ClassId_CEnvParticleScript,
	ClassId_CEnvProjectedTexture,
	ClassId_CEnvQuadraticBeam,
	ClassId_CEnvScreenEffect,
	ClassId_CEnvScreenOverlay,
	ClassId_CEnvTonemapController,
	ClassId_CEnvWind,
	ClassId_CFEPlayerDecal,
	ClassId_CFireCrackerBlast,
	ClassId_CFireSmoke,
	ClassId_CFireTrail,
	ClassId_CFish,
	ClassId_CFlashbang,
	ClassId_CFogController,
	ClassId_CFootstepControl,
	ClassId_CFunc_Dust,
	ClassId_CFunc_LOD,
	ClassId_CFuncAreaPortalWindow,
	ClassId_CFuncBrush,
	ClassId_CFuncConveyor,
	ClassId_CFuncLadder,
	ClassId_CFuncMonitor,
	ClassId_CFuncMoveLinear,
	ClassId_CFuncOccluder,
	ClassId_CFuncReflectiveGlass,
	ClassId_CFuncRotating,
	ClassId_CFuncSmokeVolume,
	ClassId_CFuncTrackTrain,
	ClassId_CGameRulesProxy,
	ClassId_CHandleTest,
	ClassId_CHEGrenade,
	ClassId_CHostage,
	ClassId_CHostageCarriableProp,
	ClassId_CIncendiaryGrenade,
	ClassId_CInferno,
	ClassId_CInfoLadderDismount,
	ClassId_CInfoOverlayAccessor,
	ClassId_CItem_Healthshot,
	ClassID_CItemDogTags,
	ClassId_CKnife,
	ClassId_CKnifeGG,
	ClassId_CLightGlow,
	ClassId_CMaterialModifyControl,
	ClassId_CMolotovGrenade,
	ClassId_CMolotovProjectile,
	ClassId_CMovieDisplay,
	ClassId_CParticleFire,
	ClassId_CParticlePerformanceMonitor,
	ClassId_CParticleSystem,
	ClassId_CPhysBox,
	ClassId_CPhysBoxMultiplayer,
	ClassId_CPhysicsProp,
	ClassId_CPhysicsPropMultiplayer,
	ClassId_CPhysMagnet,
	ClassId_CPlantedC4,
	ClassId_CPlasma,
	ClassId_CPlayerResource,
	ClassId_CPointCamera,
	ClassId_CPointCommentaryNode,
	ClassId_CPointWorldText,
	ClassId_CPoseController,
	ClassId_CPostProcessController,
	ClassId_CPrecipitation,
	ClassId_CPrecipitationBlocker,
	ClassId_CPredictedViewModel,
	ClassId_CProp_Hallucination,
	ClassId_CPropDoorRotating,
	ClassId_CPropJeep,
	ClassId_CPropVehicleDriveable,
	ClassId_CRagdollManager,
	ClassId_CRagdollProp,
	ClassId_CRagdollPropAttached,
	ClassId_CRopeKeyframe,
	ClassId_CSCAR17,
	ClassId_CSceneEntity,
	ClassId_CSensorGrenade,
	ClassId_CSensorGrenadeProjectile,
	ClassId_CShadowControl,
	ClassId_CSlideshowDisplay,
	ClassId_CSmokeGrenade,
	ClassId_CSmokeGrenadeProjectile,
	ClassId_CSmokeStack,
	ClassId_CSpatialEntity,
	ClassId_CSpotlightEnd,
	ClassId_CSprite,
	ClassId_CSpriteOriented,
	ClassId_CSpriteTrail,
	ClassId_CStatueProp,
	ClassId_CSteamJet,
	ClassId_CSun,
	ClassId_CSunlightShadowControl,
	ClassId_CTeam,
	ClassId_CTeamplayRoundBasedRulesProxy,
	ClassId_CTEArmorRicochet,
	ClassId_CTEBaseBeam,
	ClassId_CTEBeamEntPoint,
	ClassId_CTEBeamEnts,
	ClassId_CTEBeamFollow,
	ClassId_CTEBeamLaser,
	ClassId_CTEBeamPoints,
	ClassId_CTEBeamRing,
	ClassId_CTEBeamRingPoint,
	ClassId_CTEBeamSpline,
	ClassId_CTEBloodSprite,
	ClassId_CTEBloodStream,
	ClassId_CTEBreakModel,
	ClassId_CTEBSPDecal,
	ClassId_CTEBubbles,
	ClassId_CTEBubbleTrail,
	ClassId_CTEClientProjectile,
	ClassId_CTEDecal,
	ClassId_CTEDust,
	ClassId_CTEDynamicLight,
	ClassId_CTEEffectDispatch,
	ClassId_CTEEnergySplash,
	ClassId_CTEExplosion,
	ClassId_CTEFireBullets,
	ClassId_CTEFizz,
	ClassId_CTEFootprintDecal,
	ClassId_CTEFoundryHelpers,
	ClassId_CTEGaussExplosion,
	ClassId_CTEGlowSprite,
	ClassId_CTEImpact,
	ClassId_CTEKillPlayerAttachments,
	ClassId_CTELargeFunnel,
	ClassId_CTEMetalSparks,
	ClassId_CTEMuzzleFlash,
	ClassId_CTEParticleSystem,
	ClassId_CTEPhysicsProp,
	ClassId_CTEPlantBomb,
	ClassId_CTEPlayerAnimEvent,
	ClassId_CTEPlayerDecal,
	ClassId_CTEProjectedDecal,
	ClassId_CTERadioIcon,
	ClassId_CTEShatterSurface,
	ClassId_CTEShowLine,
	ClassId_CTesla,
	ClassId_CTESmoke,
	ClassId_CTESparks,
	ClassId_CTESprite,
	ClassId_CTESpriteSpray,
	ClassId_CTest_ProxyToggle_Networkable,
	ClassId_CTestTraceline,
	ClassId_CTEWorldDecal,
	ClassId_CTriggerPlayerMovement,
	ClassId_CTriggerSoundOperator,
	ClassId_CVGuiScreen,
	ClassId_CVoteController,
	ClassId_CWaterBullet,
	ClassId_CWaterLODControl,
	ClassId_CWeaponAug,
	ClassId_CWeaponAWP,
	ClassId_CWeaponBaseItem,
	ClassId_CWeaponBizon,
	ClassId_CWeaponCSBase,
	ClassId_CWeaponCSBaseGun,
	ClassId_CWeaponCycler,
	ClassId_CWeaponElite,
	ClassId_CWeaponFamas,
	ClassId_CWeaponFiveSeven,
	ClassId_CWeaponG3SG1,
	ClassId_CWeaponGalil,
	ClassId_CWeaponGalilAR,
	ClassId_CWeaponGlock,
	ClassId_CWeaponHKP2000,
	ClassId_CWeaponM249,
	ClassId_CWeaponM3,
	ClassId_CWeaponM4A1,
	ClassId_CWeaponMAC10,
	ClassId_CWeaponMag7,
	ClassId_CWeaponMP5Navy,
	ClassId_CWeaponMP7,
	ClassId_CWeaponMP9,
	ClassId_CWeaponNegev,
	ClassId_CWeaponNOVA,
	ClassId_CWeaponP228,
	ClassId_CWeaponP250,
	ClassId_CWeaponP90,
	ClassId_CWeaponSawedoff,
	ClassId_CWeaponSCAR20,
	ClassId_CWeaponScout,
	ClassId_CWeaponSG550,
	ClassId_CWeaponSG552,
	ClassId_CWeaponSG556,
	ClassId_CWeaponSSG08,
	ClassId_CWeaponTaser,
	ClassId_CWeaponTec9,
	ClassId_CWeaponTMP,
	ClassId_CWeaponUMP45,
	ClassId_CWeaponUSP,
	ClassId_CWeaponXM1014,
	ClassId_CWorld,
	ClassId_DustTrail,
	ClassId_MovieExplosion,
	ClassId_ParticleSmokeGrenade,
	ClassId_RocketTrail,
	ClassId_SmokeTrail,
	ClassId_SporeExplosion,
	ClassId_SporeTrail,
	MAX_CLASSID
};

enum SendPropType
{
	DPT_Int = 0,
	DPT_Float,
	DPT_Vector,
	DPT_VectorXY,
	DPT_String,
	DPT_Array,
	DPT_DataTable,
	DPT_Int64,
	DPT_NUMSendPropTypes
};

class DVariant
{
public:
	union
	{
		float	   m_Float;
		long	   m_Int;
		char*    m_pString;
		void*    m_pData;
		float	   m_Vector[3];
		__int64  m_Int64;
	};
	SendPropType  m_Type;
};

class RecvProp;

class CRecvProxyData
{
public:
	const RecvProp*     m_pRecvProp;        // The property it's receiving.
	DVariant		    m_Value;            // The value given to you to store.
	int				    m_iElement;         // Which array element you're getting.
	int				    m_ObjectID;         // The object being referred to.
};

typedef void(*RecvVarProxyFn)(const CRecvProxyData *pData, void *pStruct, void *pOut);
typedef void(*ArrayLengthRecvProxyFn)(void *pStruct, int objectID, int currentArrayLength);
typedef void(*DataTableRecvVarProxyFn)(const RecvProp *pProp, void **pOut, void *pData, int objectID);

class IClientNetworkable;

typedef IClientNetworkable* (*CreateClientClassFn)(int entnum, int serialNum);
typedef IClientNetworkable* (*CreateEventFn)();

class RecvTable;

class RecvProp
{
public:
	char*                   m_pVarName;
	SendPropType            m_RecvType;
	int                     m_Flags;
	int                     m_StringBufferSize;
	int                     m_bInsideArray;
	const void*             m_pExtraData;
	RecvProp*               m_pArrayProp;
	ArrayLengthRecvProxyFn  m_ArrayLengthProxy;
	RecvVarProxyFn          m_ProxyFn;
	DataTableRecvVarProxyFn m_DataTableProxyFn;
	RecvTable*              m_pDataTable;
	int                     m_Offset;
	int                     m_ElementStride;
	int                     m_nElements;
	const char*             m_pParentArrayPropName;

	RecvVarProxyFn			GetProxyFn() const;
	void					SetProxyFn(RecvVarProxyFn fn);
	DataTableRecvVarProxyFn	GetDataTableProxyFn() const;
	void					SetDataTableProxyFn(DataTableRecvVarProxyFn fn);

};

class RecvTable
{
public:
	RecvProp*               m_pProps;
	int                     m_nProps;
	void*                   m_pDecoder;
	char*                   m_pNetTableName;
	bool                    m_bInitialized;
	bool                    m_bInMainList;
};

class ClientClass
{
public:
	CreateClientClassFn      m_pCreateFn;
	CreateEventFn            m_pCreateEventFn;
	char*                    m_pNetworkName;
	RecvTable*               m_pRecvTable;
	ClientClass*             m_pNext;
	ClassId                  m_ClassID;
};

class IClientUnknown;

class IClientNetworkable
{
public:
	virtual IClientUnknown*  GetIClientUnknown() = 0;
	virtual void             Release() = 0;
	virtual ClientClass*     GetClientClass() = 0;
	virtual void             NotifyShouldTransmit(int state) = 0;
	virtual void             OnPreDataChanged(int updateType) = 0;
	virtual void             OnDataChanged(int updateType) = 0;
	virtual void             PreDataUpdate(int updateType) = 0;
	virtual void             PostDataUpdate(int updateType) = 0;
	virtual void             __unkn(void) = 0;
	virtual bool             IsDormant(void) = 0;
	virtual int              EntIndex(void) const = 0;
	virtual void             ReceiveMessage(int classID, bf_read& msg) = 0;
	virtual void*            GetDataTableBasePtr() = 0;
	virtual void             SetDestroyedOnRecreateEntities(void) = 0;
};

class CGlobalVarsBase
{
public:
	float     realtime;                     // 0x0000
	int       framecount;                   // 0x0004
	float     absoluteframetime;            // 0x0008
	float     absoluteframestarttimestddev; // 0x000C
	float     curtime;                      // 0x0010
	float     frametime;                    // 0x0014
	int       maxClients;                   // 0x0018
	int       tickcount;                    // 0x001C
	float     interval_per_tick;            // 0x0020
	float     interpolation_amount;         // 0x0024
	int       simTicksThisFrame;            // 0x0028
	int       network_protocol;             // 0x002C
	void*     pSaveData;                    // 0x0030
	bool      m_bClient;                    // 0x0031
	bool      m_bRemoteClient;              // 0x0032

private:
	// 100 (i.e., tickcount is rounded down to this base and then the "delta" from this base is networked
	int       nTimestampNetworkingBase;
	// 32 (entindex() % nTimestampRandomizeWindow ) is subtracted from gpGlobals->tickcount to Set the networking basis, prevents
	//  all of the entities from forcing a new PackedEntity on the same tick (i.e., prevents them from getting lockstepped on this)
	int       nTimestampRandomizeWindow;
};

class IBaseClientDLL
{
public:
	virtual int              Connect(CreateInterfaceFn appSystemFactory, CGlobalVarsBase *pGlobals) = 0;
	virtual int              Disconnect(void) = 0;
	virtual int              Init(CreateInterfaceFn appSystemFactory, CGlobalVarsBase *pGlobals) = 0;
	virtual void             PostInit() = 0;
	virtual void             Shutdown(void) = 0;
	virtual void             LevelInitPreEntity(char const* pMapName) = 0;
	virtual void             LevelInitPostEntity() = 0;
	virtual void             LevelShutdown(void) = 0;
	virtual ClientClass*     GetAllClasses(void) = 0;
};

#define IN_ATTACK					(1 << 0)
#define IN_JUMP						(1 << 1)
#define IN_DUCK						(1 << 2)
#define IN_FORWARD					(1 << 3)
#define IN_BACK						(1 << 4)
#define IN_USE						(1 << 5)
#define IN_CANCEL					(1 << 6)
#define IN_LEFT						(1 << 7)
#define IN_RIGHT					(1 << 8)
#define IN_MOVELEFT					(1 << 9)
#define IN_MOVERIGHT				(1 << 10)
#define IN_ATTACK2					(1 << 11)
#define IN_RUN						(1 << 12)
#define IN_RELOAD					(1 << 13)
#define IN_ALT1						(1 << 14)
#define IN_ALT2						(1 << 15)
#define IN_SCORE					(1 << 16)   
#define IN_SPEED					(1 << 17)	
#define IN_WALK						(1 << 18)	
#define IN_ZOOM						(1 << 19)	
#define IN_WEAPON1					(1 << 20)	
#define IN_WEAPON2					(1 << 21)
#define IN_BULLRUSH					(1 << 22)
#define IN_GRENADE1					(1 << 23)
#define IN_GRENADE2					(1 << 24)	
#define	IN_ATTACK3					(1 << 25)
#define	FL_ONGROUND					(1 << 0)	
#define FL_DUCKING					(1 << 1)	
#define	FL_WATERJUMP				(1 << 2)	
#define FL_ONTRAIN					(1 << 3)
#define FL_INRAIN					(1 << 4)	
#define FL_FROZEN					(1 << 5)
#define FL_ATCONTROLS				(1 << 6)
#define	FL_CLIENT					(1 << 7)	
#define FL_FAKECLIENT				(1 << 8)	
#define	FL_INWATER					(1 << 9)										
#define	FL_FLY						(1 << 10)	
#define	FL_SWIM						(1 << 11)
#define	FL_CONVEYOR					(1 << 12)
#define	FL_NPC						(1 << 13)
#define	FL_GODMODE					(1 << 14)
#define	FL_NOTARGET					(1 << 15)
#define	FL_AIMTARGET				(1 << 16)	
#define	FL_PARTIALGROUND			(1 << 17)	
#define FL_STATICPROP				(1 << 18)
#define FL_GRAPHED					(1 << 19) 
#define FL_GRENADE					(1 << 20)
#define FL_STEPMOVEMENT				(1 << 21)	
#define FL_DONTTOUCH				(1 << 22)
#define FL_BASEVELOCITY				(1 << 23)
#define FL_WORLDBRUSH				(1 << 24)	
#define FL_OBJECT					(1 << 25) 
#define FL_KILLME					(1 << 26)
#define FL_ONFIRE					(1 << 27)	
#define FL_DISSOLVING				(1 << 28)
#define FL_TRANSRAGDOLL				(1 << 29)
#define FL_UNBLOCKABLE_BY_PLAYER	(1 << 30) 

enum CSWeaponType
{
	WEAPONTYPE_KNIFE = 0,
	WEAPONTYPE_PISTOL,
	WEAPONTYPE_SUBMACHINEGUN,
	WEAPONTYPE_RIFLE,
	WEAPONTYPE_SHOTGUN,
	WEAPONTYPE_SNIPER_RIFLE,
	WEAPONTYPE_MACHINEGUN,
	WEAPONTYPE_C4,
	WEAPONTYPE_PLACEHOLDER,
	WEAPONTYPE_GRENADE,
	WEAPONTYPE_UNKNOWN
};

class CHudTexture
{
public:
	char szShortName[64];    //0x0000
	char szTextureFile[64];  //0x0040
	bool bRenderUsingFont;   //0x0080
	bool bPrecached;         //0x0081
	int8_t cCharacterInFont; //0x0082
	uint8_t pad_0083[1];     //0x0083
	uint32_t hFont;          //0x0084
	int32_t iTextureId;      //0x0088
	float afTexCoords[4];    //0x008C
	uint8_t pad_009C[16];    //0x009C
};

#pragma pack(push, 1)
class CCSWeaponInfo
{
public:
	void*         vftbl;
	bool          bParsedScript;
	bool          bLoadedHudElements;
	char          szClassName[80];
	char          szPrintName[80];
	char          szViewModel[80];
	char          szWorldModel[80];
	char          szBulletType[32];
	char          szWorldDroppedModel[80];
	char          szAnimationPrefix[16];
	uint8_t       pad_01C6[2];
	int32_t       iSlot;
	int32_t       iPosition;
	int32_t       iMaxClip1;
	int32_t       iMaxClip2;
	int32_t       iDefaultClip1;
	int32_t       iDefaultClip2;
	int32_t       iWeight;
	int32_t       iRumbleEffect;
	bool          bAutoSwitchTo;
	bool          bAutoSwitchFrom;
	uint8_t       pad_01EA[2];
	uint32_t      iFlags;
	char          szAmmo1[16];
	char          szAmmo2[16];
	char          szAIAddOn[80];
	char          aShootSounds[18][80];
	int           iAmmoType;
	int           iAmmo2Type;
	bool          bMeleeWeapon;
	bool          bBuiltRightHanded;
	bool          bAllowFlipping;
	BYTE          pad_07BB;
	int           iSpriteCount;
	CHudTexture*  iconWeaponS;
	CHudTexture*  iconWeapon;
	CHudTexture*  iconAmmo;
	CHudTexture*  iconAmmo2;
	CHudTexture*  iconCrosshair;
	CHudTexture*  iconAutoaim;
	CHudTexture*  iconZoomedCrosshair;
	CHudTexture*  iconZoomedAutoaim;
	uint8_t       pad_07E0[8];
	CSWeaponType  m_WeaponType;
	int32_t       m_iTeam;
	int32_t       iWeaponID;
	bool          bFullAuto;
	uint8_t       pad_07F5[7];
	int32_t       m_iWeaponPrice;
	float         m_flArmorRatio;
	float         m_flMaxSpeed;
	float         m_flMaxSpeedAlt;
	int32_t       m_iCrosshairMinDistance;
	int32_t       m_iCrosshairDeltaDistance;
	float         m_flPenetration;
	int32_t       m_iDamage;
	float         m_flRange;
	float         m_flRangeModifier;
	int32_t       m_iBullets;
}; //Size: 0x0828
#pragma pack(pop)

class C_BasePlayer;

class C_BaseWeapon
{
public:
	
	CHandle<C_BasePlayer>& m_hOwner()
	{
		return *(CHandle<C_BasePlayer>*)((DWORD)this + 0x31C0);
	}

	int& m_nModelIndex() const
	{
		return *(int*)((uintptr_t)this + 0x254);
	}

	ClientClass* GetClientClass()
	{
		ClientClass* Networkable = (ClientClass*)(this + 0x8);
		typedef ClientClass* (__thiscall* OriginalFn)(ClientClass*);
		return (VMT.getvfunc<OriginalFn>(Networkable, 2))(Networkable);
	}

	int& m_iItemDefinitionIndex() const
	{
		return *(int*)((uintptr_t)this + 0x2F88);
	}

	int& m_iItemIDHigh() const
	{
		return *(int*)((uintptr_t)this + 0x2FA0);
	}

	float& m_flNextPrimaryAttack() const
	{
		return *(float*)((uintptr_t)this + 0x31D8);
	}

	int& m_iClip1() const
	{
		return *(int*)((uintptr_t)this + 0x3204);
	}

	float& m_flFallbackWear() const
	{
		return *(float*)((uintptr_t)this + 0x3178);
	}

	int& m_nFallbackPaintKit() const
	{
		return *(int*)((uintptr_t)this + 0x3170);
	}

	int& m_nFallbackSeed() const
	{
		return *(int*)((uintptr_t)this + 0x3174);
	}

	int& m_nFallbackStatTrak() const
	{
		return *(int*)((uintptr_t)this + 0x317C);
	}

	bool IsWeapon()
	{
		return VMT.getvfunc<bool(__thiscall*)(C_BaseWeapon*)>(this, 160)(this);
	}

	bool IsPlantedC4()
	{
		return GetClientClass()->m_ClassID == ClassId_CPlantedC4;
	}

	bool IsDefuseKit()
	{
		return GetClientClass()->m_ClassID == ClassId_CBaseAnimating;
	}

	CCSWeaponInfo* GetCSWeaponData()
	{
		static auto fnGetWpnData = (CCSWeaponInfo*(__thiscall*)(void*))(PatternScan(GetModuleHandleW(L"client.dll"), "55 8B EC 81 EC ? ? ? ? 53 8B D9 56 57 8D 8B"));
		return fnGetWpnData(this);
	}

	bool HasBullets()
	{
		return !IsReloading() && m_iClip1() > 0;
	}

	bool IsGrenade()
	{
		return GetCSWeaponData()->m_WeaponType == WEAPONTYPE_GRENADE;
	}

	bool IsKnife()
	{
		return GetCSWeaponData()->m_WeaponType == WEAPONTYPE_KNIFE;
	}

	bool IsShotgun()
	{
		switch (GetCSWeaponData()->m_WeaponType)
		{
		case WEAPONTYPE_SHOTGUN:
			return true;
		default:
			return false;
		}
	}

	bool IsSMG()
	{
		switch (GetCSWeaponData()->m_WeaponType)
		{
		case WEAPONTYPE_SUBMACHINEGUN:
			return true;
		default:
			return false;
		}
	}

	bool IsRifle()
	{
		switch (GetCSWeaponData()->m_WeaponType)
		{
		case WEAPONTYPE_RIFLE:
			return true;
		case WEAPONTYPE_MACHINEGUN:
			return true;
		default:
			return false;
		}
	}

	bool IsPistol()
	{
		switch (GetCSWeaponData()->m_WeaponType)
		{
		case WEAPONTYPE_PISTOL:
			return true;
		default:
			return false;
		}
	}

	bool IsSniper()
	{
		switch (GetCSWeaponData()->m_WeaponType)
		{
		case WEAPONTYPE_SNIPER_RIFLE:
			return true;
		default:
			return false;
		}
	}

	bool IsReloading()
	{
		static auto inReload = *(uint32_t*)(PatternScan(GetModuleHandleW(L"client.dll"), "C6 87 ? ? ? ? ? 8B 06 8B CE FF 90") + 2);
		return *(bool*)((uintptr_t)this + inReload);
	}
};

class C_BaseViewModel;

class C_BasePlayer
{
public:

	Vector& m_aimPunchAngle() const
	{
		return *reinterpret_cast<Vector*>((DWORD)this + 0x301C);
	}

	bool& m_flSimulationTime() const
	{
		return *reinterpret_cast<bool*>((DWORD)this + 0x264);
	}

	bool& m_flLowerBodyYawTarget() const
	{
		return *reinterpret_cast<bool*>((DWORD)this + 0x39DC);
	}

	CHandle<C_BaseViewModel>& m_hViewModel()
	{
		return *(CHandle<C_BaseViewModel>*)((DWORD)this + 0x32F0);
	}

	bool m_bIsScoped()
	{
		return *reinterpret_cast<bool*>((DWORD)this + 0x387C);
	}

	CHandle<C_BaseWeapon>& m_hActiveWeapon() const
	{
		return *(CHandle<C_BaseWeapon>*)((DWORD)this + 0x2EE8);
	}

	CHandle<C_BaseWeapon>* m_hMyWeapons()
	{
		return (CHandle<C_BaseWeapon>*)((DWORD)this + 0x2DE8);
	}

	float Friction()
	{
		return *reinterpret_cast<float*>((DWORD)this + 0x140);
	}

	int m_iTeamNum()
	{
		return *reinterpret_cast<int*>((DWORD)this + 0xF0);
	}

	Vector m_vecVelocity()
	{
		return *reinterpret_cast<Vector*>((DWORD)this + 0x110);
	}

	int Health()
	{
		return *reinterpret_cast<int*>((DWORD)this + 0xFC);
	}

	int Index()
	{
		void* networkable = (void*)(this + 0x8);
		typedef int(__thiscall* OriginalFn)(void*);
		return ((OriginalFn)VMT.GetFunction(networkable, 9))(networkable);
		//return *(int*)(this + 64);
	}

	int m_fFlags()
	{
		return *reinterpret_cast<int*>((DWORD)this + 0x100);
	}

	Vector GetEyePos()
	{
		Vector vecViewOffset = *reinterpret_cast<Vector*>((DWORD)this + 0x104);
		return m_vecAbsOrigin() + vecViewOffset;
	}

	Vector& m_vecAbsOrigin()
	{
		typedef Vector& (__thiscall* OriginalFn)(void*);
		return ((OriginalFn)VMT.GetFunction(this, 10))(this);
	}

	QAngle& GetAbsAngles()
	{
		typedef QAngle& (__thiscall* OriginalFn)(void*);
		return ((OriginalFn)VMT.GetFunction(this, 11))(this);
	}

	void* GetClientClass()
	{
		void* Networkable = (void*)(this + 0x8);
		typedef void* (__thiscall* OriginalFn)(void*);
		return ((OriginalFn)VMT.GetFunction(Networkable, 2))(Networkable);
	}

	bool IsDormant()
	{
		return *reinterpret_cast<bool*>((DWORD)this + 0xE9);
	}

	int m_nTickBase()
	{
		return *reinterpret_cast<int*>((DWORD)this + 0x3404);
	}

	float MaxSpeed()
	{
		return *reinterpret_cast<float*>((DWORD)this + 0x160C);
	}

	bool m_bGunGameImmunity()
	{
		return *reinterpret_cast<bool*>((DWORD)this + 0x3890);
	}

	bool IsAlive()
	{
		BYTE lifestate = *(BYTE*)((DWORD)this + 0x025B);
		return (lifestate == 0);
	}

	int GetIndex()
	{
		void* networkable = (void*)(this + 0x8);
		typedef int(__thiscall* OriginalFn)(PVOID);
		return ((OriginalFn)VMT.GetFunction(networkable, 9))(networkable);
	}

	bool SetupBones(VMatrix *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime)
	{
		void *pRenderable = (void*)(this + 0x4);
		typedef bool(__thiscall* OriginalFn)(PVOID, VMatrix*, int, int, float);
		return ((OriginalFn)VMT.GetFunction(pRenderable, 16))(pRenderable, pBoneToWorldOut, nMaxBones, boneMask, currentTime);
	}

	model_t* GetModel()
	{
		model_t* pRenderable = (model_t*)(this + 0x4);
		typedef model_t*(__thiscall* OriginalFn)(model_t*);
		return VMT.getvfunc<OriginalFn>(pRenderable, 9)(pRenderable);
	}

	bool SetupBones(matrix3x4_t* pBoneMatrix, int nMaxBones, int nBoneMask, float flCurTime = 0)
	{
		void* pRenderable = (void*)(this + 0x4);
		typedef bool(__thiscall* OriginalFn)(void*, matrix3x4_t*, int, int, float);
		return (VMT.getvfunc<OriginalFn>(pRenderable, 13))(pRenderable, pBoneMatrix, nMaxBones, nBoneMask, flCurTime);
	}

	Vector GetBonePos(int boneIndex)
	{
		matrix3x4_t BoneMatrix[MAXSTUDIOBONES];

		if (!this->SetupBones(BoneMatrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, 0))
			return this->m_vecAbsOrigin();

		matrix3x4_t hitbox = BoneMatrix[boneIndex];

		return Vector(hitbox[0][3], hitbox[1][3], hitbox[2][3]);
	}
};

class IClientEntityList
{
public:
	virtual IClientNetworkable*   GetClientNetworkable(int entnum) = 0;
	virtual void*                 vtablepad0x1(void) = 0;
	virtual void*                 vtablepad0x2(void) = 0;
	virtual IHandleEntity*         GetClientEntity(int entNum) = 0;
	virtual IHandleEntity*         GetClientEntityFromHandle(CBaseHandle hEnt) = 0;
	virtual int                   NumberOfEntities(bool bIncludeNonNetworkable) = 0;
	virtual int                   GetHighestEntityIndex(void) = 0;
	virtual void                  SetMaxEntities(int maxEnts) = 0;
	virtual int                   GetMaxEntities() = 0;
};

class C_LocalPlayer
{
	friend bool operator==(const C_LocalPlayer& lhs, void* rhs);
public:
	C_LocalPlayer() : m_local(nullptr) {}

	operator bool() const { return *m_local != nullptr; }
	operator C_BasePlayer*() const { return *m_local; }

	C_BasePlayer* operator->() { return *m_local; }

private:
	C_BasePlayer** m_local;
};

class CUserCmd
{
public:
	CUserCmd()
	{
		memset(this, 0, sizeof(*this));
	};
	virtual ~CUserCmd() {};

	CRC32_t GetChecksum(void) const
	{
		CRC32_t crc;
		CRC32_Init(&crc);

		CRC32_ProcessBuffer(&crc, &command_number, sizeof(command_number));
		CRC32_ProcessBuffer(&crc, &tick_count, sizeof(tick_count));
		CRC32_ProcessBuffer(&crc, &viewangles, sizeof(viewangles));
		CRC32_ProcessBuffer(&crc, &aimdirection, sizeof(aimdirection));
		CRC32_ProcessBuffer(&crc, &forwardmove, sizeof(forwardmove));
		CRC32_ProcessBuffer(&crc, &sidemove, sizeof(sidemove));
		CRC32_ProcessBuffer(&crc, &upmove, sizeof(upmove));
		CRC32_ProcessBuffer(&crc, &buttons, sizeof(buttons));
		CRC32_ProcessBuffer(&crc, &impulse, sizeof(impulse));
		CRC32_ProcessBuffer(&crc, &weaponselect, sizeof(weaponselect));
		CRC32_ProcessBuffer(&crc, &weaponsubtype, sizeof(weaponsubtype));
		CRC32_ProcessBuffer(&crc, &random_seed, sizeof(random_seed));
		CRC32_ProcessBuffer(&crc, &mousedx, sizeof(mousedx));
		CRC32_ProcessBuffer(&crc, &mousedy, sizeof(mousedy));

		CRC32_Final(&crc);
		return crc;
	}

	int     command_number;     // 0x04 For matching server and client commands for debugging
	int     tick_count;         // 0x08 the tick the client created this command
	Vector  viewangles;         // 0x0C Player instantaneous view angles.
	Vector  aimdirection;       // 0x18
	float   forwardmove;        // 0x24
	float   sidemove;           // 0x28
	float   upmove;             // 0x2C
	int     buttons;            // 0x30 Attack button states
	char    impulse;            // 0x34
	int     weaponselect;       // 0x38 Current weapon id
	int     weaponsubtype;      // 0x3C
	int     random_seed;        // 0x40 For shared random functions
	short   mousedx;            // 0x44 mouse accum in x from create move
	short   mousedy;            // 0x46 mouse accum in y from create move
	bool    hasbeenpredicted;   // 0x48 Client only, tracks whether we've predicted this command at least once
	char    pad_0x4C[0x18];     // 0x4C Current sizeof( usercmd ) =  100  = 0x64
};

class CVerifiedUserCmd
{
public:
	CUserCmd m_cmd;
	CRC32_t  m_crc;
};

class CInput
{
public:
	virtual void  Init_All(void);
	virtual void  Shutdown_All(void);
	virtual int   GetButtonBits(int);
	virtual void  CreateMove(int sequence_number, float input_sample_frametime, bool active);
	virtual void  ExtraMouseSample(float frametime, bool active);
	virtual bool  WriteUsercmdDeltaToBuffer(bf_write *buf, int from, int to, bool isnewcommand);
	virtual void  EncodeUserCmdToBuffer(bf_write& buf, int slot);
	virtual void  DecodeUserCmdFromBuffer(bf_read& buf, int slot);


	inline CUserCmd* GetUserCmd(int sequence_number);
	inline CVerifiedUserCmd* GetVerifiedCmd(int sequence_number);

	bool                m_fTrackIRAvailable;            //0x04
	bool                m_fMouseInitialized;            //0x05
	bool                m_fMouseActive;                 //0x06
	bool                m_fJoystickAdvancedInit;        //0x07
	char                pad_0x08[0x2C];                 //0x08
	void*               m_pKeys;                        //0x34
	char                pad_0x38[0x64];                 //0x38
	bool                m_fCameraInterceptingMouse;     //0x9C
	bool                m_fCameraInThirdPerson;         //0x9D
	bool                m_fCameraMovingWithMouse;       //0x9E
	Vector              m_vecCameraOffset;              //0xA0
	bool                m_fCameraDistanceMove;          //0xAC
	int                 m_nCameraOldX;                  //0xB0
	int                 m_nCameraOldY;                  //0xB4
	int                 m_nCameraX;                     //0xB8
	int                 m_nCameraY;                     //0xBC
	bool                m_CameraIsOrthographic;         //0xC0
	QAngle              m_angPreviousViewAngles;        //0xC4
	QAngle              m_angPreviousViewAnglesTilt;    //0xD0
	float               m_flLastForwardMove;            //0xDC
	int                 m_nClearInputState;             //0xE0
	char                pad_0xE4[0x8];                  //0xE4
	CUserCmd*           m_pCommands;                    //0xEC
	CVerifiedUserCmd*   m_pVerifiedCommands;            //0xF0
};

#define MULTIPLAYER_BACKUP 150

CUserCmd* CInput::GetUserCmd(int sequence_number)
{
	return &m_pCommands[sequence_number % MULTIPLAYER_BACKUP];
}

CVerifiedUserCmd* CInput::GetVerifiedCmd(int sequence_number)
{
	return &m_pVerifiedCommands[sequence_number % MULTIPLAYER_BACKUP];
}

enum MDLCacheDataType_t
{
	// Callbacks to Get called when data is loaded or unloaded for these:
	MDLCACHE_STUDIOHDR = 0,
	MDLCACHE_STUDIOHWDATA,
	MDLCACHE_VCOLLIDE,

	// Callbacks NOT called when data is loaded or unloaded for these:
	MDLCACHE_ANIMBLOCK,
	MDLCACHE_VIRTUALMODEL,
	MDLCACHE_VERTEXES,
	MDLCACHE_DECODEDANIMBLOCK
};

enum MDLCacheFlush_t
{
	MDLCACHE_FLUSH_STUDIOHDR = 0x01,
	MDLCACHE_FLUSH_STUDIOHWDATA = 0x02,
	MDLCACHE_FLUSH_VCOLLIDE = 0x04,
	MDLCACHE_FLUSH_ANIMBLOCK = 0x08,
	MDLCACHE_FLUSH_VIRTUALMODEL = 0x10,
	MDLCACHE_FLUSH_AUTOPLAY = 0x20,
	MDLCACHE_FLUSH_VERTEXES = 0x40,

	MDLCACHE_FLUSH_IGNORELOCK = 0x80000000,
	MDLCACHE_FLUSH_ALL = 0xFFFFFFFF
};

class IMDLCacheNotify
{
public:
	virtual void OnDataLoaded(MDLCacheDataType_t type, MDLHandle_t handle) = 0;
	virtual void OnDataUnloaded(MDLCacheDataType_t type, MDLHandle_t handle) = 0;
};

struct vcollide_t;
struct virtualmodel_t;
struct vertexFileHeader_t;

class IMDLCache : public IAppSystem
{
public:
	virtual void                SetCacheNotify(IMDLCacheNotify *pNotify) = 0;
	virtual MDLHandle_t         FindMDL(const char *pMDLRelativePath) = 0;
	virtual int                 AddRef(MDLHandle_t handle) = 0;
	virtual int                 Release(MDLHandle_t handle) = 0;
	virtual int                 GetRef(MDLHandle_t handle) = 0;
	virtual studiohdr_t*        GetStudioHdr(MDLHandle_t handle) = 0;
	virtual studiohwdata_t*     GetHardwareData(MDLHandle_t handle) = 0;
	virtual vcollide_t*         GetVCollide(MDLHandle_t handle) = 0;
	virtual unsigned char*      GetAnimBlock(MDLHandle_t handle, int nBlock) = 0;
	virtual virtualmodel_t*     GetVirtualModel(MDLHandle_t handle) = 0;
	virtual int                 GetAutoplayList(MDLHandle_t handle, unsigned short **pOut) = 0;
	virtual vertexFileHeader_t* GetVertexData(MDLHandle_t handle) = 0;
	virtual void                TouchAllData(MDLHandle_t handle) = 0;
	virtual void                SetUserData(MDLHandle_t handle, void* pData) = 0;
	virtual void*               GetUserData(MDLHandle_t handle) = 0;
	virtual bool                IsErrorModel(MDLHandle_t handle) = 0;
	virtual void                Flush(MDLCacheFlush_t nFlushFlags = MDLCACHE_FLUSH_ALL) = 0;
	virtual void                Flush(MDLHandle_t handle, int nFlushFlags = MDLCACHE_FLUSH_ALL) = 0;
	virtual const char*         GetModelName(MDLHandle_t handle) = 0;
	virtual virtualmodel_t*     GetVirtualModelFast(const studiohdr_t *pStudioHdr, MDLHandle_t handle) = 0;
	virtual void                BeginLock() = 0;
	virtual void                EndLock() = 0;
	virtual int*                GetFrameUnlockCounterPtrOLD() = 0;
	virtual void                FinishPendingLoads() = 0;
	virtual vcollide_t*         GetVCollideEx(MDLHandle_t handle, bool synchronousLoad = true) = 0;
	virtual bool                GetVCollideSize(MDLHandle_t handle, int *pVCollideSize) = 0;
	virtual bool                GetAsyncLoad(MDLCacheDataType_t type) = 0;
	virtual bool                SetAsyncLoad(MDLCacheDataType_t type, bool bAsync) = 0;
	virtual void                BeginMapLoad() = 0;
	virtual void                EndMapLoad() = 0;
	virtual void                MarkAsLoaded(MDLHandle_t handle) = 0;
	virtual void                InitPreloadData(bool rebuild) = 0;
	virtual void                ShutdownPreloadData() = 0;
	virtual bool                IsDataLoaded(MDLHandle_t handle, MDLCacheDataType_t type) = 0;
	virtual int*                GetFrameUnlockCounterPtr(MDLCacheDataType_t type) = 0;
	virtual studiohdr_t*        LockStudioHdr(MDLHandle_t handle) = 0;
	virtual void                UnlockStudioHdr(MDLHandle_t handle) = 0;
	virtual bool                PreloadModel(MDLHandle_t handle) = 0;
	virtual void                ResetErrorModelStatus(MDLHandle_t handle) = 0;
	virtual void                MarkFrame() = 0;
	virtual void                BeginCoarseLock() = 0;
	virtual void                EndCoarseLock() = 0;
	virtual void                ReloadVCollide(MDLHandle_t handle) = 0;
};

struct Ray_t
{
	VectorAligned  m_Start;  // starting point, centered within the extents
	VectorAligned  m_Delta;  // direction + length of the ray
	VectorAligned  m_StartOffset; // Add this to m_Start to get the actual ray start
	VectorAligned  m_Extents;     // Describes an axis aligned box extruded along a ray
	const matrix3x4_t *m_pWorldAxisTransform;
	bool m_IsRay;  // are the extents zero?
	bool m_IsSwept;     // is delta != 0?

	Ray_t() : m_pWorldAxisTransform(NULL) {}

	void Init(Vector const& start, Vector const& end)
	{
		m_Delta = end - start;

		m_IsSwept = (m_Delta.LengthSqr() != 0);

		m_Extents.Init();

		m_pWorldAxisTransform = NULL;
		m_IsRay = true;

		// Offset m_Start to be in the center of the box...
		m_StartOffset.Init();
		m_Start = start;
	}

	void Init(Vector const& start, Vector const& end, Vector const& mins, Vector const& maxs)
	{
		m_Delta = end - start;

		m_pWorldAxisTransform = NULL;
		m_IsSwept = (m_Delta.LengthSqr() != 0);

		m_Extents = maxs - mins;
		m_Extents *= 0.5f;
		m_IsRay = (m_Extents.LengthSqr() < 1e-6);

		// Offset m_Start to be in the center of the box...
		m_StartOffset = maxs + mins;
		m_StartOffset *= 0.5f;
		m_Start = start + m_StartOffset;
		m_StartOffset *= -1.0f;
	}
	Vector InvDelta() const
	{
		Vector vecInvDelta;
		for (int iAxis = 0; iAxis < 3; ++iAxis) {
			if (m_Delta[iAxis] != 0.0f) {
				vecInvDelta[iAxis] = 1.0f / m_Delta[iAxis];
			}
			else {
				vecInvDelta[iAxis] = FLT_MAX;
			}
		}
		return vecInvDelta;
	}


private:
};

struct csurface_t
{
	const char *name;
	short surfaceProps;
	unsigned short flags;
};

struct trace_t
{
	Vector start;
	Vector end;
	cplane_t plane;
	float fraction;
	int contents;
	WORD dispFlags;
	bool allsolid;
	bool startSolid;
	float fractionLeftSolid;
	csurface_t surface;
	int hitGroup;
	short physicsBone;
	IHandleEntity* m_pEnt;
	int hitbox;
};
enum TraceType_t
{
	TRACE_EVERYTHING = 0,
	TRACE_WORLD_ONLY,				// NOTE: This does *not* test static props!!!
	TRACE_ENTITIES_ONLY,			// NOTE: This version will *not* test static props
	TRACE_EVERYTHING_FILTER_PROPS,	// NOTE: This version will pass the IHandleEntity for props through the filter, unlike all other filters
};
class ITraceFilter
{
public:
	virtual bool ShouldHitEntity(void* pEntity, int mask) = 0;
	virtual TraceType_t GetTraceType() const = 0;
};


class CTraceFilter : public ITraceFilter
{
public:
	bool ShouldHitEntity(void* pEntityHandle, int contentsMask)
	{
		return !(pEntityHandle == pSkip);
	}

	TraceType_t GetTraceType() const
	{
		return TRACE_EVERYTHING;
	}

	void* pSkip;
};

class CTraceFilterWorldAndPropsOnly : public ITraceFilter
{
public:
	bool ShouldHitEntity(void* /*pServerEntity*/, int /*contentsMask*/)
	{
		return false;
	}
	virtual TraceType_t GetTraceType() const
	{
		return TRACE_EVERYTHING;
	}
};

class IVEngineTrace
{
public:
	void TraceRay(const Ray_t &ray, unsigned int fMask, ITraceFilter* filter, trace_t *trace)
	{
		typedef void(__thiscall* fn)(void*, const Ray_t&, unsigned int, ITraceFilter*, trace_t*);
		VMT.getvfunc<fn>(this, 5)(this, ray, fMask, filter, trace);
	}
};

struct CViewSetup
{
	char _0x0000[16];
	__int32 x;
	__int32 x_old;
	__int32 y;
	__int32 y_old;
	__int32 width;
	__int32    width_old;
	__int32 height;
	__int32    height_old;
	char _0x0030[128];
	float fov;
	float fovViewmodel;
	Vector origin;
	Vector angles;
	float zNear;
	float zFar;
	float zNearViewmodel;
	float zFarViewmodel;
	float m_flAspectRatio;
	float m_flNearBlurDepth;
	float m_flNearFocusDepth;
	float m_flFarFocusDepth;
	float m_flFarBlurDepth;
	float m_flNearBlurRadius;
	float m_flFarBlurRadius;
	float m_nDoFQuality;
	__int32 m_nMotionBlurMode;
	char _0x0104[68];
	__int32 m_EdgeBlur;
};

enum ACT
{
	ACT_NONE,
	ACT_THROW,
	ACT_LOB,
	ACT_DROP,
};

class CBaseViewport;
class CBaseHudWeaponSelection;
typedef unsigned long HCursor;
class CHudChat
{
public:
	void ChatPrintf(int iPlayerIndex, int iFilter, const char* fmt, ...)
	{
		char msg[1024];

		va_list args;
		va_start(args, fmt);
		vsprintf(msg, fmt, args);
		VMT.getvfunc<void(__cdecl*)(void*, int, int, const char*, ...)>(this, 26)(this, iPlayerIndex, iFilter, fmt);
		va_end(args);
	}
};

class IClientMode
{
public:
	virtual             ~IClientMode() {}
	virtual int         ClientModeCSNormal(void *) = 0;
	virtual void        Init() = 0;
	virtual void        InitViewport() = 0;
	virtual void        Shutdown() = 0;
	virtual void        Enable() = 0;
	virtual void        Disable() = 0;
	virtual void        Layout() = 0;
	virtual IPanel*     GetViewport() = 0;
	virtual void*       GetViewportAnimationController() = 0;
	virtual void        ProcessInput(bool bActive) = 0;
	virtual bool        ShouldDrawDetailObjects() = 0;
	virtual bool        ShouldDrawEntity(IHandleEntity *pEnt) = 0;
	virtual bool        ShouldDrawLocalPlayer(C_BasePlayer *pPlayer) = 0;
	virtual bool        ShouldDrawParticles() = 0;
	virtual bool        ShouldDrawFog(void) = 0;
	virtual void        OverrideView(CViewSetup *pSetup) = 0;
	virtual int         KeyInput(int down, int keynum, const char *pszCurrentBinding) = 0;
	virtual void        StartMessageMode(int iMessageModeType) = 0;
	virtual IPanel*     GetMessagePanel() = 0;
	virtual void        OverrideMouseInput(float *x, float *y) = 0;
	virtual bool        CreateMove(float flInputSampleTime, void* usercmd) = 0;
	virtual void        LevelInit(const char *newmap) = 0;
	virtual void        LevelShutdown(void) = 0;
public:
	void* ptr1;
	void* ptr2;
	CBaseViewport			*m_pViewport;
	// Message mode handling
	// All modes share a common chat interface
	CHudChat			*m_pChatElement;
	HCursor			m_CursorNone;
	CBaseHudWeaponSelection *m_pWeaponSelection;
	int						m_nRootSize[2];
};

enum RenderableTranslucencyType_t
{
	RENDERABLE_IS_OPAQUE = 0,
	RENDERABLE_IS_TRANSLUCENT,
	RENDERABLE_IS_TWO_PASS,    // has both translucent and opaque sub-partsa
};

class IVModelInfo
{
public:
	virtual                                 ~IVModelInfo(void) {}
	virtual const model_t*                  GetModel(int modelindex) const = 0;
	virtual int                             GetModelIndex(const char *name) const = 0;
	virtual const char*                     GetModelName(const model_t *model) const = 0;
	virtual vcollide_t*                     GetVCollide(const model_t *model) const = 0;
	virtual vcollide_t*                     GetVCollide(int modelindex) const = 0;
	virtual void                            GetModelBounds(const model_t *model, Vector& mins, Vector& maxs) const = 0;
	virtual void                            GetModelRenderBounds(const model_t *model, Vector& mins, Vector& maxs) const = 0;
	virtual int                             GetModelFrameCount(const model_t *model) const = 0;
	virtual int                             GetModelType(const model_t *model) const = 0;
	virtual void*                           GetModelExtraData(const model_t *model) = 0;
	virtual bool                            ModelHasMaterialProxy(const model_t *model) const = 0;
	virtual bool                            IsTranslucent(model_t const* model) const = 0;
	virtual bool                            IsTranslucentTwoPass(const model_t *model) const = 0;
	virtual void                            Unused0() {};
	virtual RenderableTranslucencyType_t    ComputeTranslucencyType(const model_t *model, int nSkin, int nBody) = 0;
	virtual int                             GetModelMaterialCount(const model_t* model) const = 0;
	virtual void                            GetModelMaterials(const model_t *model, int count, IMaterial** ppMaterial) = 0;
	virtual bool                            IsModelVertexLit(const model_t *model) const = 0;
	virtual const char*                     GetModelKeyValueText(const model_t *model) = 0;
	virtual bool                            function() = 0;
	virtual float                           GetModelRadius(const model_t *model) = 0;
	virtual CStudioHdr*                     GetStudioHdr(MDLHandle_t handle) = 0;
	virtual const studiohdr_t*              FindModel(const studiohdr_t *pStudioHdr, void **cache, const char *modelname) const = 0;
	virtual const studiohdr_t*              FindModel(void *cache) const = 0;
	virtual virtualmodel_t*                 GetVirtualModel(const studiohdr_t *pStudioHdr) const = 0;
	virtual uint8_t*                        GetAnimBlock(const studiohdr_t *pStudioHdr, int iBlock) const = 0;
	virtual void                            GetModelMaterialColorAndLighting(const model_t *model, Vector const& origin, QAngle const& angles, trace_t* pTrace, Vector& lighting, Vector& matColor) = 0;
	virtual void                            GetIlluminationPoint(const model_t *model, IClientRenderable *pRenderable, Vector const& origin, QAngle const& angles, Vector* pLightingCenter) = 0;
	virtual int                             GetModelContents(int modelIndex) const = 0;
	virtual studiohdr_t*                    GetStudioModel(const model_t *mod) = 0;
	virtual int                             GetModelSpriteWidth(const model_t *model) const = 0;
	virtual int                             GetModelSpriteHeight(const model_t *model) const = 0;
	virtual void                            SetLevelScreenFadeRange(float flMinSize, float flMaxSize) = 0;
	virtual void                            GetLevelScreenFadeRange(float *pMinArea, float *pMaxArea) const = 0;
	virtual void                            SetViewScreenFadeRange(float flMinSize, float flMaxSize) = 0;
	virtual unsigned char                   ComputeLevelScreenFade(const Vector &vecAbsOrigin, float flRadius, float flFadeScale) const = 0;
	virtual unsigned char                   ComputeViewScreenFade(const Vector &vecAbsOrigin, float flRadius, float flFadeScale) const = 0;
	virtual int                             GetAutoplayList(const studiohdr_t *pStudioHdr, unsigned short **pAutoplayList) const = 0;
	virtual CPhysCollide*                   GetCollideForVirtualTerrain(int index) = 0;
	virtual bool                            IsUsingFBTexture(const model_t *model, int nSkin, int nBody, IClientRenderable* *pClientRenderable) const = 0;
	virtual const model_t*                  FindOrLoadModel(const char *name) const = 0;
	virtual MDLHandle_t                     GetCacheHandle(const model_t *model) const = 0;
	virtual int                             GetBrushModelPlaneCount(const model_t *model) const = 0;
	virtual void                            GetBrushModelPlane(const model_t *model, int nIndex, cplane_t &plane, Vector *pOrigin) const = 0;
	virtual int                             GetSurfacepropsForVirtualTerrain(int index) = 0;
	virtual bool                            UsesEnvCubemap(const model_t *model) const = 0;
	virtual bool                            UsesStaticLighting(const model_t *model) const = 0;
};

class IVModelInfoClient : public IVModelInfo
{
public:

};

class C_BaseViewModel : public C_BaseWeapon
{
public:
	int& m_nModelIndex() const
	{
		return *(int*)((uintptr_t)this + 0x254);
	}

	CHandle<C_BaseWeapon>& m_hWeapon()
	{
		return *(CHandle<C_BaseWeapon>*)((DWORD)this + 0x29B8);
	}

	CHandle<C_BasePlayer>& m_hOwner()
	{
		return *(CHandle<C_BasePlayer>*)((DWORD)this + 0x29BC);
	}

	void SetWeaponModel(const char* Filename, C_BaseWeapon* Weapon)
	{
		typedef void(__thiscall* tSetWeaponModel)(void*, const char*, C_BaseWeapon*);
		return VMT.getvfunc<tSetWeaponModel>(this, 242)(this, Filename, Weapon);
	}
};

class OverlayText_t;

class IVDebugOverlay
{
public:
	virtual void            __unkn() = 0;
	virtual void            AddEntityTextOverlay(int ent_index, int line_offset, float duration, int r, int g, int b, int a, const char *format, ...) = 0;
	virtual void            AddBoxOverlay(const Vector& origin, const Vector& mins, const Vector& max, QAngle const& orientation, int r, int g, int b, int a, float duration) = 0;
	virtual void            AddSphereOverlay(const Vector& vOrigin, float flRadius, int nTheta, int nPhi, int r, int g, int b, int a, float flDuration) = 0;
	virtual void            AddTriangleOverlay(const Vector& p1, const Vector& p2, const Vector& p3, int r, int g, int b, int a, bool noDepthTest, float duration) = 0;
	virtual void            AddLineOverlay(const Vector& origin, const Vector& dest, int r, int g, int b, bool noDepthTest, float duration) = 0;
	virtual void            AddTextOverlay(const Vector& origin, float duration, const char *format, ...) = 0;
	virtual void            AddTextOverlay(const Vector& origin, int line_offset, float duration, const char *format, ...) = 0;
	virtual void            AddScreenTextOverlay(float flXPos, float flYPos, float flDuration, int r, int g, int b, int a, const char *text) = 0;
	virtual void            AddSweptBoxOverlay(const Vector& start, const Vector& end, const Vector& mins, const Vector& max, const QAngle & angles, int r, int g, int b, int a, float flDuration) = 0;
	virtual void            AddGridOverlay(const Vector& origin) = 0;
	virtual void            AddCoordFrameOverlay(const matrix3x4_t& frame, float flScale, int vColorTable[3][3] = NULL) = 0;
	virtual int             ScreenPosition(const Vector& point, Vector& screen) = 0;
	virtual int             ScreenPosition(float flXPos, float flYPos, Vector& screen) = 0;
	virtual OverlayText_t*  GetFirst(void) = 0;
	virtual OverlayText_t*  GetNext(OverlayText_t *current) = 0;
	virtual void            ClearDeadOverlays(void) = 0;
	virtual void            ClearAllOverlays() = 0;
	virtual void            AddTextOverlayRGB(const Vector& origin, int line_offset, float duration, float r, float g, float b, float alpha, const char *format, ...) = 0;
	virtual void            AddTextOverlayRGB(const Vector& origin, int line_offset, float duration, int r, int g, int b, int a, const char *format, ...) = 0;
	virtual void            AddLineOverlayAlpha(const Vector& origin, const Vector& dest, int r, int g, int b, int a, bool noDepthTest, float duration) = 0;
	virtual void            AddBoxOverlay2(const Vector& origin, const Vector& mins, const Vector& max, QAngle const& orientation, const uint8_t* faceColor, const uint8_t* edgeColor, float duration) = 0;
	virtual void            PurgeTextOverlays() = 0;
	virtual void            DrawPill(const Vector& mins, const Vector& max, float& diameter, int r, int g, int b, int a, float duration) = 0;
};

#define FORCEINLINE_CVAR
#pragma region CVAR Flags
#define FCVAR_NONE                      0 
#define FCVAR_UNREGISTERED              (1<<0)    // If this is set, don't add to linked list, etc.
#define FCVAR_DEVELOPMENTONLY           (1<<1)    // Hidden in released products. Flag is removed automatically if ALLOW_DEVELOPMENT_CVARS is defined.
#define FCVAR_GAMEDLL                   (1<<2)    // defined by the game DLL
#define FCVAR_CLIENTDLL                 (1<<3)  // defined by the client DLL
#define FCVAR_HIDDEN                    (1<<4)    // Hidden. Doesn't appear in find or autocomplete. Like DEVELOPMENTONLY, but can't be compiled out.

// ConVar only
#define FCVAR_PROTECTED                 (1<<5)  // It's a server cvar, but we don't send the data since it's a password, etc.  Sends 1 if it's not bland/zero, 0 otherwise as value
#define FCVAR_SPONLY                    (1<<6)  // This cvar cannot be changed by clients connected to a multiplayer server.
#define   FCVAR_ARCHIVE                 (1<<7)    // set to cause it to be saved to vars.rc
#define   FCVAR_NOTIFY                  (1<<8)    // notifies players when changed
#define   FCVAR_USERINFO                (1<<9)    // changes the client's info string
#define FCVAR_CHEAT                     (1<<14) // Only useable in singleplayer / debug / multiplayer & sv_cheats

#define FCVAR_PRINTABLEONLY             (1<<10)  // This cvar's string cannot contain unprintable characters ( e.g., used for player name etc ).
#define FCVAR_UNLOGGED                  (1<<11)  // If this is a FCVAR_SERVER, don't log changes to the log file / console if we are creating a log
#define FCVAR_NEVER_AS_STRING           (1<<12)  // never try to print that cvar

// It's a ConVar that's shared between the client and the server.
// At signon, the values of all such ConVars are sent from the server to the client (skipped for local
//  client, of course )
// If a change is requested it must come from the console (i.e., no remote client changes)
// If a value is changed while a server is active, it's replicated to all connected clients
#define FCVAR_REPLICATED                (1<<13)   // server setting enforced on clients, TODO rename to FCAR_SERVER at some time
#define FCVAR_DEMO                      (1<<16)  // record this cvar when starting a demo file
#define FCVAR_DONTRECORD                (1<<17)  // don't record these command in demofiles

#define FCVAR_NOT_CONNECTED             (1<<22)   // cvar cannot be changed by a client that is connected to a server

#define FCVAR_ARCHIVE_XBOX              (1<<24) // cvar written to config.cfg on the Xbox

#define FCVAR_SERVER_CAN_EXECUTE        (1<<28)// the server is allowed to execute this command on clients via ClientCommand/NET_StringCmd/CBaseClientState::ProcessStringCmd.
#define FCVAR_SERVER_CANNOT_QUERY       (1<<29)// If this is set, then the server is not allowed to query this cvar's value (via IServerPluginHelpers::StartQueryCvarValue).
#define FCVAR_CLIENTCMD_CAN_EXECUTE     (1<<30)   // IVEngineClient::ClientCmd is allowed to execute this command. 
#pragma endregion

enum EConVar
{
	ConVar_GetName = 5,
	ConVar_GetBaseName = 6,
	ConVar_SetString = 14,
	ConVar_SetFloat,
	ConVar_SetInt
};

enum EICVar
{
	ICVar_FindVar = 15
};


class ConVar
{
public:

	float GetFloat()
	{
		typedef float(*oGetFloat)(void*);
		return VMT.getvfunc<oGetFloat>(this, 15)(this);
	}

	int GetInt()
	{
		typedef int(*oGetInt)(void*);
		return VMT.getvfunc<oGetInt>(this, 16)(this);
	}

	void SetValue(const char* value)
	{
		typedef void(*oSetValue)(void*, const char*);
		return VMT.getvfunc<oSetValue>(this, 17)(this, value);
	}

	void SetValue(float value)
	{
		typedef void(*oSetValue)(void*, float);
		return VMT.getvfunc<oSetValue>(this, 18)(this, value);
	}

	void SetValue(int value)
	{
		typedef void(*oSetValue)(void*, int);
		return VMT.getvfunc<oSetValue>(this, 19)(this, value);
	}

	void function();

	char pad_0x0000[0x4]; //0x0000
	ConVar* pNext; //0x0004
	int32_t bRegistered; //0x0008
	char* pszName; //0x000C
	char* pszHelpString; //0x0010
	int32_t nFlags; //0x0014
	char pad_0x0018[0x4]; //0x0018
	ConVar* pParent; //0x001C
	char* pszDefaultValue; //0x0020
	char* strString; //0x0024
	int32_t StringLength; //0x0028
	float fValue; //0x002C
	int32_t nValue; //0x0030
	int32_t bHasMin; //0x0034
	float fMinVal; //0x0038
	int32_t bHasMax; //0x003C
	float fMaxVal; //0x0040
	void* fnChangeCallback; //0x0044

};//Size=0x0048

class ICVar
{
public:
	ConVar* FindVar(const char *var_name)
	{
		typedef ConVar*(__thiscall* FindVarFn)(void*, const char*);
		return VMT.getvfunc<FindVarFn>(this, 15)(this, var_name);
	}
};

static const unsigned int Font_compressed_size = 30511;
static const unsigned int Font_compressed_data[30512 / 4] =
{
	0x0000bc57, 0x00000000, 0xb08a0000, 0x00000400, 0x544f5308, 0x0b004f54, 0x03008000, 0x46433000, 0x2c782046, 0x000060ec, 0x00005c48, 0x50475442,
	0x929d534f, 0x00004ac7, 0x0000dc2e, 0x534f7e19, 0xd87f322f, 0x0000b61c, 0x00002001, 0x6d636000, 0x3eaa7061, 0x0000f420, 0x00008c05, 0x65683a03,
	0xa2ed6461, 0x1b822bec, 0x0382bc20, 0x82683621, 0x32072110, 0x00201b83, 0x003c2382, 0x746d6824, 0x3347b178, 0x08000010, 0x020000c8, 0x72656bf4,
	0x16e6166e, 0x0b0000f7, 0x22285f82, 0x78616dfe, 0x50bd0070, 0x01213a82, 0x28048218, 0x6d616e06, 0x68286b65, 0x2c6f82fe, 0x04000080, 0x736f7009,
	0x00b8ff74, 0x832f8232, 0x0020235f, 0x1a820001, 0x6300002d, 0x5fbb3437, 0x00f53c0f, 0x82e80303, 0xc5002411, 0x8743541b, 0xe8ff2707, 0x6d042aff,
	0x0f829b03, 0x02000322, 0x02832f82, 0x7d820120, 0x38ffbc26, 0xa3051e00, 0xf3221f82, 0x49846d04, 0x03871983, 0x0000bd26, 0xbd000050, 0x02260e82,
	0x2c014a02, 0x09820500, 0x64006422, 0x8c200582, 0x07850582, 0xfa003228, 0x07020000, 0x15820303, 0x00000626, 0xaf000080, 0x08210982, 0x23008600,
	0x73727970, 0x21220a83, 0x6b840222, 0xd6208382, 0x11200f82, 0x00230382, 0x8502f401, 0x000228c1, 0x011a0000, 0x8201003e, 0x20028307, 0x8504823b,
	0x0001240b, 0x863b0009, 0x82022017, 0x8644207f, 0x0003240b, 0x86490011, 0x0004240b, 0x865a0017, 0x0005240b, 0x86710007, 0x0006240b, 0x86780016,
	0x0007240b, 0x868e0031, 0x8208200b, 0x86bf203b, 0x8a09200b, 0x000c240b, 0x86d0001f, 0x8a102017, 0x84112077, 0x00032477, 0x82090401, 0x00762397,
	0x0b8500ef, 0x1e000124, 0x0b866501, 0x0e000224, 0x0b868301, 0x22000324, 0x0b869101, 0x2c000424, 0x0b86b301, 0x23820520, 0x0b86df20, 0x178a0620,
	0x62000724, 0x1786ed01, 0x22000824, 0x0b864f02, 0x0b8a0920, 0x3e000c24, 0x17867102, 0x12001024, 0x0b86af02, 0x00112f08, 0x43c1020a, 0x7279706f,
	0x74686769, 0x29632820, 0x646e4120, 0x20776572, 0x6c676150, 0x77616e69, 0x202c6e61, 0x38303032, 0x6c41202e, 0x2684206c, 0x72207334, 0x72657365,
	0x2e646576, 0x63697551, 0x6e61736b, 0x3f834c64, 0x4e4f4630, 0x42414c54, 0x46544f3a, 0x4f505845, 0x1e885452, 0x1f842020, 0x6552202e, 0x616c7567,
	0x31303072, 0x3030302e, 0x2d203c8d, 0x44231c86, 0x82687361, 0x20673679, 0x61207369, 0x61727420, 0x616d6564, 0x6f206b72, 0x68742066, 0x209e9165,
	0x2ab0902e, 0x70747468, 0x772f2f3a, 0x822e7777, 0x20cc8299, 0x3dcb8870, 0x6d6f632e, 0x6f004300, 0x79007000, 0x69007200, 0x68006700, 0x20007400,
	0x63002800, 0x07822900, 0x6e004124, 0x19826400, 0x77006522, 0x50220d82, 0x21826100, 0x27826c20, 0x09826e20, 0x03827720, 0x2c006e22, 0x32221782,
	0x01823000, 0x2e003822, 0x6c223184, 0x07826c00, 0x73204d89, 0x65240d84, 0x65007300, 0x76204982, 0x64200582, 0x51222982, 0x47827500, 0x6b006322,
	0x61201782, 0x20226584, 0x81884c00, 0x25825220, 0x21826720, 0x63826c20, 0x46007228, 0x4e004f00, 0x1f825400, 0x42004124, 0x0d823a00, 0x13825420,
	0x58004524, 0x0b825000, 0x54005223, 0x894d9100, 0x002d214b, 0x30204d8d, 0x3120ab82, 0x07838182, 0x44003024, 0x81826100, 0xb3826820, 0x85826920,
	0xab826720, 0x0f826920, 0x15822020, 0x74002024, 0x07827200, 0x8b826420, 0x87846d20, 0x1d826b20, 0x66006f22, 0x74220582, 0x15826800, 0x41002021,
	0x2e20211f, 0x682023a2, 0x74226982, 0xd9827000, 0x2f002f24, 0x01847700, 0x71822e20, 0x20097b41, 0x12794170, 0x63002e26, 0x6d006f00, 0x0020fb9c,
	0x03200083, 0x03820482, 0x43220121, 0x1c22068d, 0x09830300, 0x00220f82, 0x1b820601, 0x1b200287, 0x03830882, 0x028b0682, 0x401b8308, 0x00985a50,
	0x42414f1a, 0x4a4e4352, 0x1d1c4b47, 0x21201f1e, 0x25242322, 0x49444d4c, 0x46534845, 0x04030201, 0x08070605, 0x0c0b0a09, 0x100f0e0d, 0x14131211,
	0x18171615, 0x95549419, 0x265e51a2, 0x2a292827, 0x2e2d2c2b, 0x3231302f, 0x36353433, 0x3a393837, 0x3e3d3c3b, 0x9799963f, 0xbaad005c, 0xb06661a9,
	0x716f70b1, 0xaa9072bb, 0xb9757374, 0xb5b6b8b7, 0x79777876, 0x7b7c7ab4, 0x8800b37d, 0x8f829ba6, 0x8a89ab28, 0x008b5f87, 0x0b829d91, 0x839c0021,
	0x3f038404, 0x84859e8e, 0x00ac0000, 0x8c00a700, 0x6a827f1b, 0xa5a48fbc, 0x59585655, 0x86b20083, 0xa1a30000, 0x57352483, 0x6281008d, 0x6460ae80,
	0x6863af65, 0x6c670069, 0x5d006b6d, 0x2019835b, 0x220483a8, 0x82180204, 0x00542606, 0x00050040, 0x08098214, 0x2f002450, 0x41003900, 0x60005a00,
	0x7e007a00, 0xa500a300, 0xab00a900, 0xb000ae00, 0xb800b400, 0xc300bf00, 0xd500d000, 0xdf00dd00, 0xef00e300, 0xfd00f500, 0x5301ff00, 0x78016101,
	0xc6029201, 0x7e03dc02, 0x1a201420, 0x26201e20, 0x22213a20, 0x5382ffff, 0x20000032, 0x30002600, 0x42003a00, 0x61005b00, 0xa0007b00, 0xa8205582,
	0xad205582, 0xc0265588, 0xd100c400, 0x5582d600, 0xe400e026, 0xf600f100, 0x52225582, 0x558a6001, 0x18201324, 0x55821c20, 0x55883920, 0xecff0026,
	0xbfff0000, 0xc5200382, 0xff256583, 0xff0000f7, 0x260382fc, 0xffabffd8, 0x84c6fff0, 0x82952019, 0xffcc220f, 0x2005828f, 0x20038285, 0x2c0382b3,
	0xff0eff32, 0xfd97fd1a, 0xe0cffc7f, 0x27378391, 0x000066e0, 0x010065df, 0x52200b82, 0x0022f782, 0x09826a00, 0x03827620, 0x84007e22, 0x88200582,
	0x06820386, 0x80200284, 0x96200582, 0x00200382, 0x0022c582, 0x0982b400, 0x0382c020, 0xb422028b, 0x0e82b800, 0x0382ba20, 0x1b000026, 0x50004000,
	0x38085582, 0x001a0098, 0x0041004f, 0x00520042, 0x004e0043, 0x0047004a, 0x004c004b, 0x0044004d, 0x00450049, 0x00530048, 0x00940046, 0x00950054,
	0x005100a2, 0x0096005e, 0x00970099, 0x2841825c, 0x00a60084, 0x008b009b, 0x3c35828a, 0x00ad0089, 0x009100ba, 0x006000a9, 0x00620061, 0x006300ae,
	0x00650064, 0x009f00af, 0x3a9182b0, 0x006b009d, 0x006d006c, 0x006e00b1, 0x009000bb, 0x00aa008e, 0x00740073, 0x82b90075, 0x00b72495, 0x82b500b6,
	0x833408b3, 0x7b009e00, 0x7d007c00, 0x7e00b300, 0x8f00bc00, 0x59005800, 0x55005700, 0x8d005600, 0xa100a300, 0xf4010000, 0x52020000, 0x03036800,
	0xcf024900, 0x51026800, 0x5d200382, 0x2e200f82, 0xc5260f82, 0x31016800, 0x1f826f00, 0xcb022b22, 0x33201782, 0x4c201782, 0x68222b84, 0x1f825203,
	0x0f844720, 0x02422008, 0x025a0071, 0x0242009b, 0x03320088, 0x0269002e, 0x043900ee, 0x023e004e, 0x024a00d2, 0x8238005c, 0x0247261b, 0x013100b8,
	0x3c6b822c, 0x014100c6, 0x021f004f, 0x022d004e, 0x02480062, 0x02300098, 0x023c005d, 0x02400082, 0x2203824b, 0x82370076, 0x02422c0b, 0x0228006b,
	0x025d0094, 0x823a003b, 0x023c2807, 0x013a0061, 0x8441006a, 0x0065340b, 0x0019015d, 0xfff9005e, 0x003002fb, 0x00f9005d, 0x82b70364, 0x8217822b,
	0x8446208b, 0x00942337, 0x1382013c, 0xec012108, 0x90013400, 0x65022a00, 0x46025300, 0x5d033600, 0x3a023b00, 0x4c024400, 0x2a023400, 0xfc003e00,
	0xae244b82, 0xae014700, 0x6c208382, 0x6c207f82, 0x5a3c0784, 0x38008602, 0x5200e600, 0x27001602, 0x4f006c02, 0x42009801, 0x4a00a601, 0x5700f000,
	0x3b200382, 0x36201b82, 0x45280382, 0x45009a01, 0xe8ff5802, 0x4c2c3f82, 0x3500d403, 0x3e00c001, 0x49009501, 0x23831782, 0x23820020, 0x03824d20,
	0xbc023b2c, 0xf4012d00, 0xcc016900, 0x07825500, 0x03828d20, 0x0382a520, 0x5102c923, 0x05734100, 0x01270782, 0x0196005c, 0x82530012, 0x03012803,
	0x03680016, 0x8b490052, 0xf8022103, 0x0387e382, 0x3800cc26, 0x3f003e02, 0x6123038c, 0x87023a00, 0x00652403, 0x8280025e, 0x23038b0f, 0x02530065,
	0xff830387, 0x43828920, 0x0223e782, 0x84390098, 0x21ff8203, 0x6382fc00, 0x36001623, 0x2c5f8302, 0x011600b2, 0x014a00c2, 0x032200a0, 0x26db823e,
	0x028500f4, 0x825200a8, 0x033626db, 0x043300ba, 0x205f821f, 0x2207823e, 0x82280015, 0x0142286b, 0x013400f1, 0x826200ae, 0x01372303, 0x038300e0,
	0x78023534, 0x1e013e00, 0x38037900, 0x84023700, 0xae024200, 0xd3833700, 0x3a222f82, 0x13822c03, 0x64006c28, 0x48003701, 0x5782f401, 0x0f823720,
	0x42001024, 0x27828203, 0x3e004530, 0x00000302, 0xaa00f401, 0x49000303, 0x6b823b02, 0x5e006c24, 0xb382fa01, 0xaf829720, 0x68009e26, 0xf8ff1201,
	0xf8204b84, 0xcf83b382, 0xef82db83, 0xff190127, 0x001901fa, 0x20038209, 0x31038232, 0x00480234, 0x0086023d, 0x003e022c, 0x00a3053f, 0x57820049,
	0x00820020, 0x00b5ff23, 0x93068232, 0x00013402, 0x00fa2200, 0x18d20501, 0x0a0a0000, 0x000100ec, 0x82baff01, 0xff082205, 0x210582f6, 0x1183ff13,
	0x05841520, 0xf1ff1622, 0x17201182, 0x18220b84, 0x0b82e2ff, 0xd8ff2e22, 0x3b200582, 0x3c200584, 0x3e202f84, 0x46200584, 0x48262384, 0x0200fbff,
	0x0b820200, 0x06000222, 0x0e200584, 0x10200584, 0x17200584, 0x02222982, 0x0b841800, 0x0b842820, 0x05842920, 0x05842a20, 0x05842c20, 0xe0ff3122,
	0x34204182, 0x36200b84, 0x3b200584, 0x6b830584, 0x3d000222, 0x71830b84, 0x46000222, 0x02229b82, 0x11844a00, 0x05848f20, 0x0584a420, 0x0582a520,
	0x03000322, 0x0328ad82, 0xceff0400, 0x07000300, 0x08200584, 0x09200584, 0x0324b982, 0xc4ff0d00, 0x0e201782, 0x03224182, 0x8f821100, 0x12000322,
	0x13223584, 0x1782c9ff, 0xd3ff1522, 0x16220582, 0x0582ddff, 0x11841720, 0xbfff1822, 0x19200b82, 0x26221784, 0x0b82a6ff, 0xebff3c22, 0x8f830582,
	0x42000322, 0x46206b84, 0x47202384, 0x48205f84, 0x4b205384, 0x4e200b84, 0x54200584, 0x8c200584, 0x91200584, 0x95204784, 0x97202384, 0x04280582,
	0x14001100, 0x28000400, 0x04228382, 0x05842900, 0x05842a20, 0x05843420, 0x05843b20, 0x05843c20, 0x0421fb83, 0x22ef8300, 0x82020005, 0x0005223b,
	0x26058406, 0x0092ff09, 0x840e0005, 0x8410200b, 0x82192005, 0x0005222f, 0x249b821a, 0xff260005, 0x201d82e7, 0x20ef8228, 0x225f8405, 0x842a0005,
	0x842c200b, 0xff322223, 0x201d82b7, 0x20058434, 0x20118436, 0x20058438, 0x2011843a, 0x8329843b, 0x0005227d, 0x200b843e, 0x2205843f, 0x82b0ff46,
	0xff472235, 0x2205829c, 0x820a0048, 0xff4b2205, 0x200582ba, 0x2011844e, 0x22d18256, 0x84590005, 0x848c2005, 0x848f2011, 0x84912035, 0x82a12035,
	0x00062a0b, 0x00cfff05, 0xff070006, 0x220582d3, 0x82ceff0b, 0xff132205, 0x200582ec, 0x20058415, 0x20238416, 0x22898217, 0x82180006, 0x000624c5,
	0x820a0026, 0x8434201d, 0xff3b221d, 0x220b82fb, 0x82ccff3c, 0x843e2005, 0x8448200b, 0x82542029, 0x00072235, 0x224d8208, 0x820e0007, 0x00072235,
	0x200b8416, 0x28658218, 0xff460007, 0x000900bf, 0x20418209, 0x204d8209, 0x220b8234, 0x8229002a, 0x002d2205, 0x22058215, 0x821f002e, 0xff462205,
	0x200582e7, 0x22478247, 0x844e0009, 0x848c2005, 0x82912005, 0x000a2217, 0x22478201, 0x8202000a, 0x848b8253, 0x840e2005, 0x84102005, 0x82122005,
	0x000a2283, 0x20058413, 0x22358214, 0x8215000a, 0x820a2083, 0x82e22083, 0xff1822bb, 0x200582d8, 0x201d841a, 0x20058426, 0x201d8428, 0x20598429,
	0x200b842a, 0x229b822b, 0x842c000a, 0x84342011, 0x84362011, 0x8439200b, 0x843a2005, 0xff3b221d, 0x204782c4, 0x2071843c, 0x2005843e, 0x2005844a,
	0x202f848f, 0x202384a3, 0x201184a4, 0x200584a5, 0x201182a7, 0x24b3820b, 0x000b00d8, 0x24838206, 0x0008000b, 0x200b8205, 0x200b840e, 0x22058410,
	0x829cff13, 0x84142011, 0xff152229, 0x200b828d, 0x22118416, 0x827eff18, 0x22b3830b, 0x8228000b, 0x000b24b3, 0x82fbff29, 0x842a2011, 0x842b200b,
	0x842c202f, 0x84342011, 0x84362011, 0x8439200b, 0x21a78317, 0xa783000b, 0x48000b22, 0x0b22b382, 0x6b844a00, 0xb0ff5222, 0x54224182, 0x058288ff,
	0x11845520, 0x05845620, 0x05845820, 0x05845920, 0xa6ff8722, 0x8f201d82, 0xa4205384, 0xa5201184, 0x0c220582, 0xd1820400, 0x0c000c2a, 0x0c00e0ff,
	0xd3ff0e00, 0x0f200582, 0x46261184, 0x0d00bfff, 0x0b820300, 0x23820d20, 0x06200b83, 0x08220b84, 0x1782ddff, 0xdfff0d22, 0x0e200582, 0x13201184,
	0x0d225382, 0x0b841400, 0xd3ff1826, 0x01000e00, 0x0e242382, 0xe2ff0200, 0x03200b82, 0x07200584, 0x0e226b82, 0x23820800, 0x09000e22, 0x0a201184,
	0x0b200584, 0x0e20dd82, 0xd8207d82, 0x0d222982, 0x0582e6ff, 0x4d820e20, 0x12000e22, 0x13221784, 0x1182c9ff, 0x53841420, 0x41841520, 0x59841620,
	0x47841720, 0xc4ff1822, 0x19201d82, 0x3d201784, 0x42202f84, 0x46205384, 0x47201184, 0x48220584, 0x2382ecff, 0x0b844b20, 0x05844e20, 0x05845420,
	0x05848c20, 0x05849120, 0x23849520, 0x05829720, 0x9b820f20, 0x0f009c24, 0x83821500, 0x16000f22, 0x0f225382, 0x53821700, 0x18000f22, 0x19221184,
	0x1d82f1ff, 0x29841a20, 0x1d842820, 0xebff2922, 0x2a201182, 0x2b221d84, 0x0b82e1ff, 0x11842c20, 0x29843420, 0x0f823920, 0x3a000f24, 0x07820500,
	0x0a003b22, 0x3c200582, 0x3e200584, 0x46220584, 0x1182baff, 0x83ff4721, 0x824b2071, 0x000f22cb, 0x220b844e, 0x82140056, 0x84592017, 0x848c2005,
	0x848f2011, 0x8491206b, 0x84a3202f, 0x82a7203b, 0x00102605, 0x00c9ff13, 0x20ad8210, 0x220582d8, 0x82ddff16, 0xff182205, 0x200582bf, 0x228f8242,
	0x82480010, 0x821120a7, 0x00fb26d1, 0xff0e0011, 0x200582ef, 0x20118413, 0x206b8214, 0x20358211, 0x201182ec, 0x20a78216, 0x20dd8211, 0x200b82e7,
	0x22358228, 0x82290011, 0x0011216b, 0x1122d183, 0x65822b00, 0x2c001122, 0x34201184, 0x36201d84, 0x39200b84, 0x8f201784, 0x12221182, 0x05841200,
	0x47821320, 0x53821220, 0x1200e226, 0xe7ff1600, 0x17200582, 0x18210584, 0x201183ff, 0x20238419, 0x223b822b, 0x84390012, 0x843b2005, 0x843c202f,
	0x843d2017, 0x843e200b, 0x843f2005, 0x8446201d, 0x8448200b, 0x8254201d, 0x001222b3, 0x22118291, 0x82020013, 0x001322ef, 0x200b8403, 0x240b8406,
	0x0092ff09, 0x20dd8213, 0x200582c9, 0x20118410, 0x201d8412, 0x24358219, 0xff1a0013, 0x221782ba, 0x8279ff26, 0x84282005, 0xff292205, 0x200b8283,
	0x220b842a, 0x82ceff2b, 0x842c200b, 0x842d2011, 0xff2e222f, 0x201182d8, 0x2005842f, 0x22478431, 0x829cff32, 0x84332011, 0x84342005, 0x84352035,
	0x8436200b, 0x84372035, 0xff38220b, 0x2023828d, 0x204d8439, 0x2011843a, 0x2005843b, 0x2005843c, 0x2005843d, 0x2005843e, 0x22ad843f, 0x82a1ff46,
	0x8447202f, 0xff4a2211, 0x200b82a6, 0x2005844b, 0x2041844c, 0x2005844d, 0x201d844e, 0x2005848c, 0x2077848f, 0x20238491, 0x20d184a1, 0x200b84a3,
	0x200584a4, 0x200584a5, 0x280582a7, 0xff030014, 0x001400e2, 0x22418204, 0x84050014, 0x84072005, 0x84082005, 0x82092005, 0x001422e3, 0x220b840a,
	0x82d2ff0b, 0x840c2029, 0x820d200b, 0x001422e9, 0x24f5820e, 0xff110014, 0x201782e7, 0x20478413, 0x221d8416, 0x82f6ff17, 0xff192211, 0x220582d3,
	0x82fbff3d, 0x84462005, 0x84472017, 0x844b2035, 0x844e2005, 0x848c2005, 0x82912005, 0x00152241, 0x204d8202, 0x24898215, 0x001500e2, 0x830b8406,
	0x00152483, 0x8288ff09, 0x226b8311, 0x84100015, 0x84122017, 0x8215202f, 0x00152265, 0x20058416, 0x22a78217, 0x84180015, 0x84192005, 0xff1a2211,
	0x203582c9, 0x22fb8226, 0x84280015, 0xff292205, 0x201182bf, 0x200b842a, 0x2041842b, 0x2011842c, 0x2035842e, 0x2005842f, 0x203b8431, 0x20658432,
	0x20058433, 0x202f8434, 0x200b8435, 0x202f8436, 0x220b8437, 0x82c4ff38, 0x8439204d, 0x843a2035, 0x843b2011, 0xff3c2205, 0x201782dd, 0x22ef823d,
	0x843e0015, 0x843f2011, 0xff462289, 0x20178279, 0x20cb8447, 0x2017844a, 0x200b844b, 0x2041844c, 0x2005844d, 0x2011844e, 0x2005848c, 0x2277848f,
	0x829cff91, 0x84a12035, 0x84a3202f, 0x84a42071, 0x84a5200b, 0x82a72005, 0x00162211, 0x22718202, 0x82040016, 0x00162241, 0x200b8406, 0x287d8208,
	0xff090016, 0x00160097, 0x2011840e, 0x20058410, 0x20238412, 0x22e38215, 0x84160016, 0xff172205, 0x202382f1, 0x20178418, 0x22118419, 0x82d3ff1a,
	0x82262011, 0x0016227d, 0x22ef8228, 0x84290016, 0x842a2065, 0xff2b220b, 0x201d82e2, 0x2011842c, 0x203b842e, 0x2005842f, 0x203b8431, 0x20658432,
	0x20058433, 0x202f8434, 0x200b8435, 0x202f8436, 0x200b8437, 0x220b8438, 0x82e7ff39, 0x843a2047, 0x843b2011, 0x843c2005, 0x823d2005, 0x001622d7,
	0x220b843e, 0x82c9ff3f, 0xff462223, 0x22058288, 0x829cff47, 0x844a2005, 0x844b2017, 0x844c200b, 0x844d2077, 0x844e2005, 0x848c2011, 0x848f2005,
	0xff912277, 0x202f82a6, 0x222f84a1, 0x82ceffa3, 0x84a4200b, 0x84a5200b, 0x82a72005, 0x00172211, 0x20058402, 0x22ef8204, 0x84060017, 0x8408200b,
	0x82092005, 0x001722cb, 0x200b840e, 0x20058410, 0x22f58212, 0x84140017, 0xff15260b, 0x001700ec, 0x22778216, 0x84180017, 0x841a200b, 0x8426202f,
	0x84282005, 0x8229204d, 0x001722c5, 0x200b842a, 0x2023842b, 0x2011842c, 0x2023842e, 0x2005842f, 0x20058431, 0x20238434, 0x201d8436, 0x20298439,
	0x2005843a, 0x206b843b, 0x2017843c, 0x2005843e, 0x20118446, 0x20778448, 0x200b844a, 0x203b848f, 0x202f84a1, 0x201184a3, 0x200584a4, 0x200584a5,
	0x200582a7, 0x22e38218, 0x821800c4, 0x220583dd, 0x82320008, 0xff09220b, 0x2005827e, 0x210b840b, 0x1783ff0e, 0x05841020, 0x1e001122, 0x12221782,
	0x0582ddff, 0x1820e383, 0xec20e382, 0x17200b82, 0x18225f82, 0xad821900, 0x1a001824, 0x1182baff, 0x9cff2622, 0x28220582, 0x058292ff, 0x97ff2922,
	0x2a200582, 0x2b200b84, 0x1822a782, 0x11842c00, 0x35842e20, 0x05842f20, 0x1824e983, 0xb5ff3200, 0x33202982, 0x34200584, 0x35202f84, 0x36200b84,
	0x37202f84, 0x38200b84, 0x39225984, 0x2982e2ff, 0x11843a20, 0x9b843b20, 0xc9ff3c22, 0x3d201182, 0x3e207d84, 0x3f221184, 0x1182b0ff, 0xd0ff4622,
	0x47200582, 0x4a20d184, 0x4b201184, 0x4c205984, 0x4d208984, 0x4e200584, 0x8c201d84, 0x8f200584, 0x91201d84, 0xa1200584, 0xa3205f84, 0xa4207184,
	0xa5203b84, 0xa7200584, 0x19221182, 0x3b820200, 0x06001922, 0x0e200584, 0x10200584, 0x12280584, 0x1900f6ff, 0x23001300, 0x19200582, 0x1a220b84,
	0x0b82f1ff, 0xe7ff2822, 0x29200582, 0x1922ef82, 0x0b842a00, 0x1d842b20, 0x11842c20, 0x11843420, 0x0b843620, 0x05843b20, 0x35843c20, 0x05843e20,
	0xef824a20, 0x8f001922, 0xa3202384, 0xa4201d84, 0xa5201184, 0xa7200584, 0x1a211182, 0x2a778300, 0xff13001a, 0x001a00a6, 0x82c4ff15, 0xff162205,
	0x280582ce, 0x00baff18, 0xff1d001c, 0x200582fb, 0x2271821e, 0x841f001c, 0x82232005, 0x001c223b, 0x243b8247, 0xff4b001c, 0x201d82dd, 0x220b824e,
	0x8220001e, 0x001e2217, 0x22778223, 0x8221001f, 0x821f2035, 0x00e7240b, 0x8425001f, 0x824b200b, 0x8220203b, 0x00ec224d, 0x20178220, 0x200582d8,
	0x20118425, 0x213b824b, 0x5f830021, 0x5f822120, 0x2100fb22, 0xe2201d82, 0x35830582, 0x5f822120, 0x2200ec22, 0x22203584, 0xf6201d82, 0x23200b82,
	0x2220cb82, 0x0b835382, 0x23225383, 0x41821c00, 0x1d002326, 0x23000a00, 0xf1204782, 0x1f200582, 0x20221184, 0x0b82abff, 0x2f822120, 0x22002322,
	0x24201184, 0x23207182, 0x23833b82, 0x9cff4722, 0x4a201d82, 0x2320b382, 0x74206582, 0x4e200b82, 0xa4201184, 0xa5201184, 0x24200582, 0xf6228982,
	0xbf842400, 0xa1842520, 0x83842520, 0xdd842520, 0xdd842520, 0x47002522, 0x25205382, 0xe7244182, 0x4e002500, 0x26220b82, 0x4b822c00, 0x2f00262a,
	0x26007800, 0x0e003100, 0x33200582, 0x3b201784, 0x26228382, 0x05843c00, 0x1f003e22, 0x48221782, 0x0582ddff, 0xf1ff5222, 0x54260582, 0x2700b5ff,
	0xad823b00, 0x23842720, 0x3d002722, 0x27228382, 0x11843e00, 0x23823f20, 0x42002722, 0x71831184, 0x83002721, 0x8427203b, 0x00272171, 0x27244183,
	0xbaff5400, 0x55204182, 0x27226b82, 0x05845800, 0x05848c20, 0x71829520, 0x97002722, 0xa1264184, 0x2800fbff, 0x0b822800, 0x29002822, 0x28221d82,
	0x0b842a00, 0x0b842c20, 0x0b843420, 0x0b843620, 0x29843b20, 0x05843c20, 0x11843d20, 0x0b843e20, 0x23844220, 0x05844820, 0xd8ff5422, 0x55224d82,
	0x05820a00, 0x0f005622, 0x58200582, 0x59200b84, 0x8f200b84, 0xa1202384, 0xa3201184, 0xa7204184, 0x29200582, 0x2a287184, 0xebff3700, 0x39002a00,
	0x2a204182, 0x2a20f584, 0xe720f582, 0xf5831182, 0x3e002a24, 0x0b821f00, 0x2a20f583, 0x2a21f584, 0x20f58300, 0x20f5822a, 0x221782d8, 0x82f6ff4e,
	0x82522005, 0x822a20d7, 0x82b020f5, 0x21e9830b, 0xe983002a, 0xe9822a20, 0x2b00f624, 0x7d822600, 0xe9822b20, 0xe9830b83, 0x77822b20, 0x2c200b83,
	0x2b243b82, 0xf4ff3400, 0xe9832382, 0x3f002b22, 0x42221184, 0x11821e00, 0xd3ff4722, 0x48220582, 0x05822300, 0x0b844b20, 0x05844e20, 0x1d845220,
	0x05845420, 0x05845520, 0x23845620, 0x0b845820, 0x0b845920, 0x37008722, 0x8c203582, 0x8f202f84, 0x95225384, 0x11821400, 0x23849720, 0x8f84a320,
	0x0582a720, 0xb3822c20, 0x2d00ce26, 0x28002600, 0x3b200582, 0x2d20c582, 0xf120fb82, 0x3e200b82, 0x2d201d82, 0xe220e382, 0x52200b82, 0x542a0b84,
	0x2e00b5ff, 0xebff3100, 0xd1822f00, 0x3000ec22, 0xf6200582, 0x28220582, 0x0582e7ff, 0x05842920, 0x05842a20, 0x05842c20, 0x05843420, 0x05843620,
	0x83823920, 0x3a003022, 0x65830584, 0x3c003022, 0x30216b82, 0x22658300, 0x844a0030, 0xff54220b, 0x204782d8, 0x202f848f, 0x202984a1, 0x201784a3,
	0x200584a4, 0x200584a5, 0x220582a7, 0x822a0031, 0x00312793, 0x0041003e, 0x4d830032, 0x4d823220, 0x0b82f120, 0x32204d83, 0x3221b384, 0x20b38300,
	0x24dd8232, 0x003300b5, 0x20358229, 0x20298433, 0x20298433, 0x20778433, 0x20298433, 0x20298433, 0x28298433, 0xff2b0034, 0x003400dd, 0x242f8237,
	0xff390034, 0x220b82ed, 0x82f0ff3a, 0xff3b2205, 0x200582e2, 0x22a1823c, 0x843d0034, 0x823e2023, 0x00342283, 0x2029843f, 0x201d8442, 0x220b8447,
	0x82ceff48, 0x844e2029, 0x8452200b, 0xff542205, 0x201182b0, 0x200b8455, 0x20058458, 0x2005848c, 0x22058495, 0x82f1ff97, 0x22ef831d, 0x822a0035,
	0x003528f5, 0x00e1ff2e, 0x82310035, 0x8235201d, 0x82e720b3, 0x843c200b, 0x823d200b, 0x82352065, 0x201183b9, 0x2135823f, 0x77830035, 0x47003522,
	0x48223584, 0x2982ddff, 0x0b844e20, 0x3520fb83, 0xba20d182, 0x55201182, 0x58201184, 0x8c200584, 0x77830584, 0x83003521, 0x00352677, 0x00fbffa1,
	0x26298236, 0x003700ce, 0x82e7ff26, 0x84282005, 0x84292005, 0x822a2005, 0x003722cb, 0x200b8434, 0x20058436, 0x2041823f, 0x20718237, 0x222982a6,
	0x82b5ff4b, 0xff4e2105, 0x52220b83, 0x0b821400, 0xa1825420, 0x55003722, 0x56220b84, 0x11822300, 0x0b845820, 0x0b845920, 0x29848c20, 0x47848f20,
	0x4784a320, 0x0582a720, 0x38003822, 0x39240584, 0x38000f00, 0xec20ef82, 0x3c200582, 0x3822dd82, 0x29823d00, 0x3e003822, 0x38213b82, 0x20778300,
	0x83ef8238, 0x20e9831d, 0x20b38238, 0x202982b5, 0x833b8455, 0x003822dd, 0x20358495, 0x83118497, 0x0038215f, 0x39205f83, 0xe224d782, 0x28003900,
	0x39221d82, 0x05842900, 0xd8ff2a26, 0x2c003900, 0x2d220b84, 0x0b82ecff, 0x0b843420, 0x05843620, 0xbaff3722, 0x39221182, 0x0582c1ff, 0x29845420,
	0x0a005622, 0x59200b82, 0x8f200584, 0x5f832384, 0x5f843920, 0x31003a2a, 0x3a00e1ff, 0xe8ff3300, 0x54240582, 0x3b00ceff, 0xe7207182, 0x28220582,
	0x0582e2ff, 0xc5822920, 0x2a003b22, 0x2c200b84, 0x34200b84, 0x36200b84, 0x38200b84, 0x3b207d82, 0xf120f582, 0x3c202982, 0x3b225982, 0xbf823d00,
	0x3e003b22, 0x3f220b84, 0x1782fbff, 0xabff4722, 0x48200582, 0x4a201784, 0x4b201784, 0x3b22a182, 0x17844e00, 0x8c207185, 0x8f200b84, 0x95205984,
	0x97205384, 0xa1202f84, 0xa3202f84, 0xa4201784, 0xa5200b84, 0xa7200584, 0x3c201182, 0xec24a782, 0x28003c00, 0x3c228982, 0x35822900, 0xa7823c20,
	0x1182c420, 0x0b842c20, 0xc2ff2e22, 0x31220b82, 0x0582c7ff, 0x23843420, 0x17843620, 0xf1ff3822, 0x3b201182, 0x3c200584, 0x3c206582, 0x3c20b384,
	0xf620b382, 0x3f201782, 0x3c22b382, 0xa1824700, 0x48003c22, 0x4a201d84, 0x4b210584, 0x205983ff, 0x8317844e, 0x003c22b3, 0x200b848c, 0x8359848f,
	0x003c21b3, 0x3c22b383, 0x2f84a100, 0x6b84a320, 0x0b84a420, 0x0584a520, 0x1182a720, 0xb3823d20, 0x3d00d322, 0xdd20b382, 0x29200582, 0x3d20c582,
	0x0b83b382, 0x0b842c20, 0x83ff2e21, 0xff34211d, 0x36201183, 0x38201184, 0x3b203584, 0x3d214182, 0x20ad8300, 0x20a7843d, 0x209b823d, 0x204182f1,
	0x8323844a, 0x003d228f, 0x2035848f, 0x83298495, 0x823d2089, 0x20238389, 0x205384a3, 0x202984a4, 0x200584a5, 0x201182a7, 0x2289823e, 0x823e00e7,
	0x82e22089, 0xff292105, 0x2a200b83, 0x3e221d82, 0x0b842c00, 0x0b843120, 0x05843420, 0x11843620, 0x3e248983, 0xf1ff3b00, 0x89832f82, 0x3d003e22,
	0x3e206b82, 0x3e248f84, 0xfbff3f00, 0x47221782, 0x0582abff, 0x17844820, 0x29844a20, 0xbaff4b22, 0x4e201182, 0xa7831784, 0x8c003e22, 0x8f200b84,
	0x95225984, 0x1d82ecff, 0x3e20ad83, 0x3e22ad84, 0x1784a300, 0x3b84a420, 0x0584a520, 0x1182a720, 0xa7823f20, 0x3f00f124, 0x11822900, 0x2a003f22,
	0x2c200584, 0x34200584, 0x36200584, 0x54200584, 0x3f22d782, 0x0b848f00, 0x0584a320, 0x0582a720, 0x02004122, 0x41224182, 0x05840600, 0x11840920,
	0x0b840e20, 0x05841020, 0x05842820, 0x05842920, 0x05842a20, 0x95822c20, 0x2f004128, 0x41001e00, 0x11843400, 0x05843620, 0x3b843820, 0x4620b983,
	0xd8225382, 0x53824600, 0x0582e020, 0xedff0722, 0x0e210582, 0x221183ff, 0x82efff12, 0xff13220b, 0x2205829c, 0x82e7ff14, 0xff152205, 0x220582a6,
	0x82afff16, 0xff182205, 0x220582b5, 0x82230019, 0x84282005, 0x8429201d, 0x842a2005, 0x822b2005, 0x00462277, 0x200b842c, 0x20058434, 0x20058436,
	0x247d8239, 0xff3a0046, 0x223582f6, 0x82c4ff3b, 0x823c2005, 0x004624e9, 0x82f5ff3e, 0x8448200b, 0x844a2011, 0x8452207d, 0xff542277, 0x22178288,
	0x82b0ff55, 0x84562005, 0x84582005, 0x84592005, 0x84872005, 0x848f2023, 0x84a32059, 0x84a42071, 0x84a5203b, 0x82a72005, 0x84472011, 0x824720dd,
	0x00d823dd, 0xd7830047, 0x10004722, 0x47221d82, 0x3b821300, 0x14004724, 0x1782f1ff, 0x65821520, 0x16004722, 0x18221184, 0x11827eff, 0x3b841c20,
	0xceff1d22, 0x23200b82, 0x28200b84, 0x29220584, 0x1182f6ff, 0x0b842a20, 0x3b842b20, 0x11842c20, 0x11843420, 0x0b843620, 0x8f823920, 0x3b004724,
	0x2982abff, 0xbaff3c22, 0x3e200582, 0x4722d782, 0x77845600, 0x05845920, 0x2f828f20, 0x83824a20, 0x4a00a624, 0x11821500, 0x7d824a20, 0x0b82dd20,
	0x71821720, 0x18004a22, 0x4a24dd82, 0xe2ff1900, 0x1d201182, 0x1f200584, 0x4a225982, 0x29842300, 0x77823b20, 0x3c004a22, 0x3d201184, 0x3e201d84,
	0x3f201184, 0x46201184, 0x91202384, 0x4b200582, 0x4b20fb84, 0x4b22fb84, 0xd7820900, 0x0e004b22, 0x10201784, 0x12200584, 0x4b203b82, 0xec266582,
	0x1c004b00, 0x0582ddff, 0x0a001d22, 0x1e200582, 0x4b219b82, 0x24718300, 0xff20004b, 0x201182a1, 0x20118421, 0x20238422, 0x22658223, 0x8224004b,
	0x004b2271, 0x22178425, 0x82bfff26, 0x82282023, 0x004b22b3, 0x22ef8229, 0x842a004b, 0xff2b220b, 0x201782e7, 0x2011842c, 0x22d78232, 0x8433004b,
	0x84342005, 0x8435201d, 0x8436200b, 0x8437201d, 0xff38220b, 0x202f82ab, 0x20538439, 0x2011843a, 0x2005843b, 0x2005843c, 0x2005843d, 0x2205843e,
	0x82c4ff3f, 0xff462229, 0x22058288, 0x8256ff4b, 0x848f2005, 0x82912053, 0x004c2211, 0x22238213, 0x8215004c, 0x004c2247, 0x22a78216, 0x8218004c,
	0x844d20ef, 0x844d2017, 0x844d2017, 0x844d2017, 0x004e2217, 0x221d8202, 0x8406004e, 0x840e2005, 0x84102005, 0xff132605, 0x004e009c, 0x20418214,
	0x2035824e, 0x200b8288, 0x22118416, 0x827eff18, 0x821c200b, 0x004e225f, 0x226b821d, 0x8423004e, 0x8428200b, 0xff292205, 0x201d82f6, 0x200b842a,
	0x203b842b, 0x2211842c, 0x820f002f, 0x84342017, 0x84362017, 0xff392211, 0x201182e7, 0x24f5823b, 0xff3c004e, 0x220b82ba, 0x82d3ff3e, 0x84562005,
	0x8459207d, 0x828f2005, 0x0052222f, 0x22dd8209, 0x82260052, 0x00522135, 0x52226583, 0x59822900, 0x83005221, 0x00522265, 0x830b842c, 0x00522259,
	0x200b8436, 0x26298438, 0x000a0039, 0x82460052, 0x005221b3, 0x91204785, 0x54200b82, 0x5420ef84, 0x5422ef84, 0x65820e00, 0x10005422, 0x13240584,
	0x5400a6ff, 0x5420ef84, 0x5421ef84, 0x24ef8300, 0xff180054, 0x22178292, 0x82f6ff2b, 0x002f2205, 0x2205821e, 0x82e2ff39, 0x823b2005, 0x825420b3,
	0x82c420b9, 0xff3e240b, 0x845500c4, 0x845520a7, 0x825520a7, 0x82f120a7, 0xff292111, 0x2a200583, 0x2c200584, 0x34200584, 0xa7830584, 0x83005521,
	0x005524a7, 0x820f0039, 0x823f2029, 0x8255205f, 0x82a620ad, 0xff85220b, 0x200582dd, 0x2029848f, 0x20958291, 0x20b38457, 0x20b38457, 0x20b38457,
	0x22b38457, 0x82130057, 0x845720d1, 0x845720b3, 0x845720b3, 0x825720b3, 0x007e22b3, 0x20838257, 0x220582ec, 0x82f6ff29, 0xff2a2105, 0x2b200b83,
	0x57225382, 0x11842c00, 0x77822f20, 0x34005722, 0x36201784, 0x39221184, 0x2982e7ff, 0xabff3b22, 0x3c200582, 0x5724dd82, 0xd3ff3e00, 0x8f200b82,
	0x58202382, 0x5820dd84, 0x5820dd84, 0x5821dd84, 0x22dd8300, 0x822a0058, 0x82582053, 0x00f12253, 0x834d8258, 0x20dd8305, 0x20dd8458, 0x21dd8458,
	0xdd830058, 0xdd845820, 0x83005821, 0x005821dd, 0x5821dd83, 0x20dd8300, 0x22598259, 0x8259009c, 0x82ec2059, 0x82282005, 0x82592071, 0x82d82059,
	0x842a200b, 0xff2c210b, 0x34200b83, 0x36200b84, 0x38220b84, 0x1d82e2ff, 0xfb824620, 0x47825920, 0x0b82d320, 0x0b829120, 0x02008522, 0x8522a782,
	0x05840600, 0x05840e20, 0x05841020, 0xc4ff1326, 0x14008500, 0x8522a782, 0x0b841500, 0xceff1622, 0x17221182, 0x0582f6ff, 0xe9821820, 0x26008524,
	0x0b820a00, 0x11842b20, 0x29843920, 0x71823b20, 0x3c008524, 0x1782ddff, 0x05823e20, 0x5f828c20, 0x8c00d822, 0x05835f82, 0x89820e20, 0x10008c22,
	0x13200584, 0x8c207d82, 0x8c205f84, 0x88205f82, 0x16202382, 0x18221184, 0x0b827eff, 0xecff2822, 0x29220582, 0x0582f6ff, 0x0b842a20, 0x5f822b20,
	0x2c008c22, 0x34201184, 0x36201184, 0x39220b84, 0x2382e7ff, 0xabff3b22, 0x3c200582, 0x8c229582, 0xad823e00, 0xe3828c20, 0x8d00ec22, 0x8d207d84,
	0x8d227d84, 0x77820e00, 0x7d848d20, 0x83008d21, 0x848d207d, 0x848d207d, 0x008d217d, 0x8d217d83, 0x217d8300, 0x7d83008d, 0x83008d21, 0x008d217d,
	0x8d217d83, 0x207d8300, 0x247d848d, 0x002f008d, 0x8359820f, 0x008d2183, 0x8d218383, 0x21838300, 0x8383008d, 0xfb828d20, 0x1d82ba20, 0xd3ff3e22,
	0x8f200582, 0x8f20a782, 0xe7221782, 0x17828f00, 0x3d220583, 0x0b82e2ff, 0xb3823e20, 0x3f008f22, 0x8f22d182, 0x11844200, 0xf6ff4722, 0x48201782,
	0x8f22a182, 0x0b844e00, 0x3b845220, 0xb0ff5422, 0x8c201782, 0x95201184, 0x97201184, 0x91200b82, 0xf622a182, 0xa1849100, 0xa1829120, 0x34200b83,
	0x3b201784, 0x3c200584, 0x3e200584, 0x77200584, 0x78200584, 0x79200584, 0x7a200584, 0x7e200584, 0x8f200584, 0x9e200584, 0xb2200584, 0xb4200584,
	0xb9200584, 0x94220582, 0x71820200, 0x06009422, 0x09200584, 0x0e201184, 0x10200b84, 0x26200584, 0x28201184, 0x29200b84, 0x2a200584, 0x2f260584,
	0x94001e00, 0x0b843400, 0x05843620, 0xe9823820, 0x3b009422, 0x3c200b84, 0x3d200584, 0xa1833b84, 0x8f009422, 0x96201182, 0x96206b84, 0x96216b84,
	0x226b8300, 0x840e0096, 0x84102017, 0x82282005, 0x82962041, 0x00f122ef, 0x83ef8296, 0x822c2005, 0x00962441, 0x8223002f, 0x84342011, 0x8436201d,
	0x84382005, 0x843b2017, 0x843c2005, 0x843d2005, 0x226b8305, 0x843f0096, 0x828f200b, 0x00982229, 0x220b8223, 0x8220009b, 0x829c200b, 0x00ec2405,
	0x821200a1, 0x00a12411, 0x82a6ff13, 0xff15220b, 0x220582c4, 0x82ceff16, 0x84172005, 0xff182205, 0x200b829c, 0x222f8219, 0x842b00a1, 0x226b8329,
	0x843900a1, 0xff3b220b, 0x201d82e2, 0x24ad823c, 0xff3d00a1, 0x200b82d3, 0x2011843e, 0x2011843f, 0x20058446, 0x20058291, 0x265f82a3, 0x00a300ba,
	0x82d8ff15, 0xff162205, 0x200582dd, 0x22178417, 0x82b5ff18, 0x8228200b, 0x82a3204d, 0x82fb20e9, 0x842a200b, 0xff2c210b, 0x34200b83, 0x36200b84,
	0x3b200b84, 0xa3218382, 0x20dd8300, 0x206b82a3, 0x202982f1, 0x2011843e, 0x2023828f, 0x20bf84a4, 0x215f84a4, 0x5f8300a4, 0x8300a421, 0x00a428bf,
	0x00b0ff18, 0x821900a4, 0x00a42295, 0x2005841d, 0x202f841f, 0x21898223, 0x538300a4, 0x5384a420, 0x5382a420, 0x2982e220, 0xa4225383, 0x23843f00,
	0x23844620, 0x05829120, 0x5f84a520, 0x5f84a520, 0x5f84a520, 0x5f84a520, 0x5f84a520, 0x5f84a520, 0x5f84a520, 0x8300a521, 0x00a5215f, 0xa5205f83,
	0xa5205f84, 0xa5205f84, 0xa5215f84, 0x205f8300, 0x215f84a5, 0x5f8300a5, 0x8300a521, 0x82a7205f, 0x00ba225f, 0x205f84a7, 0x205f84a7, 0x205f82a7,
	0x221182ec, 0x82b5ff18, 0x82282005, 0x00a7248f, 0x82fbff29, 0x842a200b, 0x842c200b, 0x8434200b, 0x8436200b, 0xff3b220b, 0x201d82f1, 0x2011843c,
	0x200b843d, 0x2005843e, 0x2011828f, 0x228982ab, 0x82ab00f6, 0x82fb2089, 0x843d2005, 0x823e2011, 0x00ac2a05, 0x001e001d, 0xff2000ac, 0x200582e2,
	0x26118221, 0x00010000, 0x820a0000, 0x002c2a15, 0x74616c01, 0x0008006e, 0x24118204, 0x00ffff00, 0x23078201, 0x72656b01, 0x00201383, 0x1b820d85,
	0x83000221, 0x0008240b, 0x82be1801, 0x00c8080d, 0xc0005b00, 0x5001f600, 0xec01c601, 0xac026e02, 0xec02c202, 0xf0036a03, 0x2c040604, 0x2005aa04,
	0x7c053a05, 0x8806c605, 0xb807e606, 0x24098a08, 0x580af609, 0x8c0a6e0a, 0xa80a960a, 0xd00aba0a, 0x240be60a, 0x4c0b2e0b, 0xbc0b760b, 0x180c120c,
	0xc40c5a0c, 0xe80cca0c, 0xf40cee0c, 0x500d460d, 0x880d6a0d, 0x300ede0d, 0x880e360e, 0x0c0fca0e, 0x8c0f1a0f, 0x64100610, 0x0411da10, 0xd4113e11,
	0x80123e12, 0x38132613, 0xb8134a13, 0x2c14ee13, 0xc4146a14, 0x34150215, 0xcc157615, 0x60162616, 0xf016a616, 0x44173e17, 0x50174a17, 0xd8179617,
	0x5c181a18, 0xb0189e18, 0x01000d00, 0x0800baff, 0x1300f6ff, 0x1500ecff, 0x16240382, 0x1700f1ff, 0x18280782, 0x2e00e2ff, 0x3b00d8ff, 0x3c200382,
	0x3e201f82, 0x46200382, 0x48261782, 0x1600fbff, 0x09820200, 0x03820620, 0x03820e20, 0x03821020, 0x1d821720, 0x07821820, 0x07822820, 0x03822920,
	0x03822a20, 0x03822c20, 0xe0ff3124, 0x07823400, 0x03823620, 0x03823b20, 0x3d204983, 0xf1224d88, 0x0b824a00, 0x03828f20, 0x0382a420, 0x0382a520,
	0x03001d22, 0x04247782, 0x0700ceff, 0x08200382, 0x09200382, 0x0d247f82, 0x0e00c4ff, 0x11209782, 0x12206182, 0xc920ab84, 0xd324ab82, 0xddff1600,
	0xc931ab82, 0xbfff1800, 0xd3ff1900, 0xa6ff2600, 0xebff3c00, 0x20618300, 0x20478242, 0x20178246, 0x203f8247, 0x20378248, 0x2007824b, 0x2003824e,
	0x20038254, 0x2003828c, 0x202f8291, 0x20178295, 0x25038297, 0x00110009, 0xbb8b0014, 0xaf87b383, 0xa383f983, 0xf5882020, 0x92ff0922, 0xec218f82,
	0x20f98300, 0x208f8219, 0x226b821a, 0x82e7ff26, 0x00f12141, 0xf121fd85, 0x22fd8300, 0x82b7ff32, 0x00b721fd, 0x3820fd83, 0x3a202782, 0x3b200f82,
	0x4f41c782, 0x00f12205, 0x300b823f, 0x00b0ff46, 0x009cff47, 0x000a0048, 0x00baff4b, 0x200b824e, 0x208d8256, 0x20038259, 0x200b828c, 0x2023828f,
	0x20238291, 0x260782a1, 0xff05000f, 0x820700cf, 0xff0b22b9, 0x0eb141ce, 0x1800f622, 0x45828583, 0xd1823420, 0xfbff3b28, 0xccff3c00, 0x07823e00,
	0x79824820, 0x3d825420, 0x08000522, 0x0e203582, 0x16202582, 0x18200782, 0x46224582, 0x2f82bfff, 0x2f820920, 0x34002630, 0x29002a00, 0x15002d00,
	0x1f002e00, 0xcd824600, 0x33824720, 0x03824e20, 0x81848c20, 0x1582e720, 0x33820120, 0x3b820220, 0x47840620, 0xff82ce20, 0x4100ce21, 0xf622058f,
	0x25821400, 0x5b841520, 0x8982e220, 0x1a00d826, 0x2600f6ff, 0x28200382, 0x29201382, 0x2a204d82, 0x2b200782, 0x2c206982, 0x34200b82, 0x36200b82,
	0x39200782, 0x3a200382, 0xc420ad84, 0xce20ad82, 0xce22ad82, 0x5b824a00, 0x1f828f20, 0x1782a320, 0x0b82a420, 0x0382a520, 0x0b82a720, 0x02002126,
	0x0600d8ff, 0x08240382, 0x0e000500, 0x10200782, 0x13220382, 0x79829cff, 0x1500ec23, 0x05d747ff, 0x7eff1822, 0xcb417984, 0x00fb2205, 0x8581822a,
	0x42fb2075, 0xfb220687, 0x718a3900, 0xc4ff4822, 0xd82a7182, 0xb0ff5200, 0x88ff5400, 0x53825500, 0x03825620, 0x03825820, 0x03825920, 0xa6ff8723,
	0x059f4200, 0xa500d822, 0x05220f82, 0x8f820400, 0xe0ff0c22, 0xd3228182, 0x0b820f00, 0xbfff4626, 0x03000900, 0x04200982, 0x06200982, 0x08260782,
	0x0d00ddff, 0x2182dfff, 0x1300ce22, 0x14207f82, 0x22066d41, 0x8201001f, 0x82022019, 0x820320db, 0x82072003, 0x05df424d, 0x0a00e222, 0x0b240f82,
	0x0c00fbff, 0x0d221382, 0x3982e6ff, 0x1200f622, 0x13220f82, 0xdd82c9ff, 0xdd82d320, 0xeb42d820, 0x00ce2206, 0x24b18218, 0x00d8ff19, 0x201f823d,
	0x20378242, 0x420b8246, 0xd82119e3, 0x07e34200, 0x09001d24, 0x49829cff, 0x4982f620, 0x1700fb22, 0x18203982, 0x19289b82, 0x1a00f1ff, 0x2800ecff,
	0x29224d82, 0x2f41ebff, 0x00e12206, 0x200b822c, 0x2c1b8234, 0x000f0039, 0x0005003a, 0x000a003b, 0x2003823c, 0x2403823e, 0x00baff46, 0x204b8247,
	0x4289824b, 0xfb2211ad, 0x1f829100, 0x2782a320, 0x0382a720, 0x83000621, 0x821520c3, 0x821620a3, 0xff1828fb, 0xff4200bf, 0x824800f6, 0x82102071,
	0x82fb208f, 0x82ef20e9, 0x82ec2021, 0x44c420e5, 0x1822087d, 0x8b43e7ff, 0x06bf4106, 0x2c204782, 0x34209f82, 0x36203982, 0x39200782, 0xb7610f82,
	0x42132009, 0xe7200877, 0xe722d582, 0xd9821800, 0x29821920, 0x29832b20, 0x3b200382, 0x20066f43, 0x2007823d, 0x2003823e, 0x2013823f, 0x43078246,
	0xec200515, 0xf128ad82, 0x02003000, 0x0300c9ff, 0x06201582, 0xdd430782, 0x00c92205, 0x200b8210, 0x20138212, 0x20ad8219, 0x22d3821a, 0x8279ff26,
	0x00792697, 0x0083ff29, 0x240b822a, 0x00ceff2b, 0x200b822c, 0x201f822d, 0x20e1822e, 0x2003822f, 0x242f8231, 0x009cff32, 0x20038233, 0x20238234,
	0x20078235, 0x20238236, 0x24078237, 0x008dff38, 0x20338239, 0x200b823a, 0x2003823b, 0x2003823c, 0x2003823d, 0x2803823e, 0x0092ff3f, 0x00a1ff46,
	0x240b8247, 0x00a6ff4a, 0x2003824b, 0x202b824c, 0x2003824d, 0x4413824e, 0x79200509, 0xa622ad82, 0x8b82a100, 0x1f82a320, 0x0382a420, 0x0382a520,
	0x0382a720, 0x37451720, 0x45052008, 0xec220c3b, 0x0f820a00, 0xd2ff0b24, 0x07820c00, 0x9d820d20, 0xa5820e20, 0xe7ff1128, 0xe2ff1300, 0x13821600,
	0xf6ff172a, 0xd3ff1900, 0xfbff3d00, 0xce208182, 0xf1208182, 0xf1217d82, 0x0bf94300, 0x02003422, 0x04203582, 0x06203182, 0x2008b542, 0x08834388,
	0x4d821220, 0x45821520, 0x03821620, 0x20053946, 0x265182ec, 0xff1a00f6, 0x822600c9, 0x822820ab, 0xff292403, 0x822a00bf, 0x822b2007, 0x822c202b,
	0xff2e240b, 0x822f00ec, 0x82312003, 0x82322033, 0x82332053, 0x82342003, 0x8235201f, 0x82362007, 0x8237201f, 0xff382407, 0x823900c4, 0x463a2023,
	0xdd200885, 0xce22a582, 0x0f823e00, 0x5b823f20, 0x79ff4622, 0x8822ad82, 0x0f824a00, 0x88ff4b24, 0x2b824c00, 0x03824d20, 0x0b824e20, 0x03828c20,
	0x4f828f20, 0x9cff9124, 0x1f82a100, 0x4b82a320, 0x0782a420, 0x0382a520, 0x0b82a720, 0xdd22d183, 0x2d820400, 0xddff0623, 0x05874300, 0xd1829720,
	0x1000dd22, 0x12200f82, 0xd1891782, 0x1800f122, 0xe7450f82, 0x86d32005, 0x00bf22d1, 0x20458229, 0x24a9822a, 0x00e2ff2b, 0x220b822c, 0x82f1ff2e,
	0x00f121d1, 0xdd22d185, 0x47823300, 0x1f823420, 0x07823520, 0x1f823620, 0x07823720, 0xe722d185, 0x0b823a00, 0x03823b20, 0x03823c20, 0x91823d20,
	0x07823e20, 0x8820d185, 0x22060142, 0x824b00dd, 0x824c20b9, 0x824d2053, 0x09014203, 0x0142bf20, 0x00dd2606, 0x00ceffa3, 0x203782a4, 0x200382a5,
	0x220b82a7, 0x82020026, 0xff042405, 0x470600d3, 0xf6200839, 0xa545d182, 0x00e22207, 0x47178214, 0xd18307e1, 0x2009a145, 0x25cd82d3, 0xff2a00d8,
	0x2b4500d3, 0x00d82405, 0x82f6ff2e, 0x84f620cd, 0x823420cd, 0x82362017, 0x058b41a5, 0x3b00ec22, 0x3c204782, 0xd822b584, 0x0b824600, 0xa1824820,
	0x2405b545, 0xffa100d3, 0x219986ec, 0xb94500ce, 0x00ce2205, 0x20998234, 0x2a9582c4, 0x000800c4, 0xff090032, 0x820b007e, 0xff0e2407, 0x821000c4,
	0x00112403, 0x8212001e, 0x229985c3, 0x411700ec, 0xba28086b, 0x9cff2600, 0x92ff2800, 0x97209d82, 0x92209d82, 0xd8249d82, 0x97ff2c00, 0x3d429d82,
	0x00b5260b, 0x00b5ff33, 0x20278234, 0x20078235, 0x201f8236, 0x28078237, 0x009cff38, 0x00e2ff39, 0x200b823a, 0x2867823b, 0x00c9ff3c, 0x00baff3d,
	0x220b823e, 0x82b0ff3f, 0x00d022b9, 0x208b8247, 0x200b824a, 0x203b824b, 0x20d1824c, 0x2003824d, 0x2013824e, 0x2003828c, 0x2013828f, 0x20038291,
	0x203f82a1, 0x204b82a3, 0x202782a4, 0x200382a5, 0x200b82a7, 0x088b4618, 0x22078746, 0x5ff6ff12, 0x07820585, 0xf1ff1a22, 0xe720b582, 0xec20b582,
	0xe720b582, 0xf620b582, 0xec20b582, 0xe722a182, 0xdd823600, 0x03823b20, 0x8d443c20, 0x824a2006, 0xff8f23a1, 0xed4600e7, 0x00e22205, 0x200f82a5,
	0x201f82a7, 0x06574705, 0x1500a622, 0x1624af82, 0x1800ceff, 0x0726bb82, 0xfbff1d00, 0x63821e00, 0x03821f20, 0x2b822320, 0x2b824720, 0xddff4b24,
	0x07824e00, 0x20000222, 0x23201182, 0x04225982, 0x27822100, 0x57822320, 0x07822520, 0x2b824b20, 0x39820420, 0x2300ec22, 0x2520df82, 0x4b200d82,
	0x47856184, 0x1582fb20, 0x8500e221, 0x00ec2227, 0x85278405, 0x82e7205d, 0x00f62115, 0x0f223d83, 0x31821c00, 0x0a001d22, 0xf1227b82, 0x0b821f00,
	0xabff2022, 0xe7226382, 0x0b822200, 0x51822420, 0x75822520, 0x2005bb44, 0x203182e2, 0x228f8274, 0x82a4009c, 0x20cf838d, 0x20618202, 0x206184f6,
	0x87bb8807, 0x8447209f, 0x82e7208d, 0x82f6202f, 0x002c2c5d, 0x002f0023, 0x00310078, 0x8233000e, 0x05294119, 0x3e00ec26, 0x48001f00, 0x5220e582,
	0x54266582, 0x1100b5ff, 0xdb823b00, 0x7b823c20, 0x63823d20, 0x0b823e20, 0x19823f20, 0x0b824220, 0x29834f83, 0x3d824e20, 0xba222d85, 0x0b825500,
	0x03825820, 0x03828c20, 0x26050d47, 0xffa100f1, 0x491500fb, 0x342010af, 0x36204582, 0xfb257784, 0xfbff3c00, 0x05a94a00, 0x4200fb22, 0x48201782,
	0xd8227b84, 0xa7825500, 0x0f005624, 0x07825800, 0x07825920, 0x17828f20, 0x0b82a120, 0x3782a320, 0x0382a720, 0x2a000122, 0x10281182, 0xebff3700,
	0xd8ff3900, 0xe720a986, 0xe2204b82, 0xa98dc784, 0xf984d820, 0xc9825220, 0xb0ff5423, 0x24a18900, 0x001a00f6, 0x4b478226, 0x34240f4f, 0x3600f4ff,
	0x3f206582, 0x422c0382, 0x47001e00, 0x4800d3ff, 0x4b002300, 0x4e200782, 0x52200382, 0x54201382, 0x55200382, 0x56200382, 0x58201782, 0x59200782,
	0x87220782, 0x5d823700, 0x8f00d32a, 0x9500f6ff, 0x97001400, 0xa3201782, 0xa7205f82, 0x012a0382, 0xceff5400, 0x26000700, 0xad822800, 0xf982ec20,
	0x3e00f122, 0xe221f184, 0x07714100, 0x31000122, 0x0120cf82, 0xec209382, 0x26203d82, 0x28234582, 0x4900e7ff, 0xe72105d7, 0x05d34900, 0x9982e720,
	0xf382e720, 0x3a00f622, 0x820bbb41, 0x844a2049, 0x82d820df, 0x00e7227d, 0x421b82a1, 0xec2205ed, 0x1782a500, 0x0382a720, 0x2a00022a, 0x3e002d00,
	0x06004100, 0x07208198, 0xec206d82, 0x15261d98, 0xddff2b00, 0x4b823700, 0xedff3922, 0xf0207f82, 0xe2202982, 0xd822c982, 0x17823d00, 0x5d823e20,
	0x1b823f20, 0x22052142, 0x824800ec, 0x824e20ef, 0x0777410f, 0x0b825520, 0x03825820, 0x03828c20, 0x20092142, 0x20e982f6, 0x22ad822a, 0x82e1ff2e,
	0x207b83fd, 0x205182e7, 0x215182ec, 0x734200e2, 0x056b4137, 0x26001425, 0x4100e7ff, 0xce200941, 0x22061b44, 0x823f00e7, 0xff47286f, 0xff4b00a6,
	0x824e00b5, 0x82522007, 0xff542483, 0x825500e2, 0x05cf4107, 0x59001426, 0x8c002300, 0x37441b82, 0x00f62205, 0x223382a7, 0x82380010, 0x00392205,
	0x0821410f, 0x61823d20, 0x29823e20, 0x42205183, 0x4824d782, 0x5400ddff, 0x0d435582, 0xff952407, 0x829700f1, 0x07b74233, 0x93821020, 0x9382e220,
	0x2900f122, 0x2a241982, 0x2c00d8ff, 0x2d200782, 0x15433982, 0x00f12805, 0x00baff37, 0x82c1ff39, 0x00d82445, 0x820a0056, 0x000a2287, 0x8723828f,
	0x00032841, 0x00e1ff31, 0x82e8ff33, 0x00ce2221, 0x20e3861c, 0x224f82e2, 0x822a00e7, 0x0bf94bc3, 0x57823820, 0x20059949, 0x066543f1, 0xfb82f120,
	0x4700fb26, 0x4800abff, 0x4a209582, 0x4b205782, 0x4e206f82, 0x54220f82, 0xef82ceff, 0x6b82ab20, 0xcd4ae220, 0x00f62206, 0x227784a1, 0x82a400e2,
	0x82a52007, 0x82a72003, 0x821e205f, 0x45ec2071, 0xc4200a79, 0xec24c182, 0xc2ff2e00, 0xc7209782, 0x23087d45, 0x00f1ff38, 0x2209134a, 0x823e00f6,
	0xff3f216d, 0xba207983, 0xf6217986, 0x05d94a00, 0xc784ba20, 0x85828c20, 0xe7ff8f22, 0xf620798a, 0x2206ad45, 0x82a500f6, 0x82a7203b, 0x821720c9,
	0x82d32079, 0x82dd2079, 0x82e220eb, 0x82dd20eb, 0x82e22079, 0x82d32079, 0x00dd2275, 0x859b8236, 0x00f622ef, 0x8331823c, 0x05334771, 0x6184e220,
	0xddff8f22, 0xf6225d82, 0x1b829700, 0xd320d785, 0x24083b45, 0x00d3ffa7, 0x1449411d, 0x55823120, 0x474d3420, 0x094d4106, 0x4d41f620, 0x41f62006,
	0x0a20404d, 0x200a0b42, 0x20cf82f1, 0x20cb82f1, 0x20cb82f1, 0x21b382f1, 0xf74100d3, 0x00f12505, 0x00f1ffa7, 0x2005256f, 0x208b8206, 0x820d8309,
	0x82102007, 0x82282003, 0x82292003, 0x05f94103, 0x2f00ec24, 0x41821e00, 0x0d41e220, 0x82f12006, 0x00e22241, 0x06474725, 0x0700e02d, 0x0e00edff,
	0x1200d8ff, 0x4d00efff, 0xe73005d3, 0xa6ff1500, 0xafff1600, 0xb5ff1800, 0x23001900, 0x210a2f43, 0x494e00e7, 0x07754409, 0x7544e220, 0x00c42a06,
	0x00d3ff3c, 0x00f5ff3e, 0x08db4d48, 0xdb4d9c20, 0x00b02606, 0x00b0ff56, 0x20038258, 0x22038259, 0x829cff87, 0x077b4785, 0x2405df4d, 0x00ecffa7,
	0x10dd471a, 0xf1209185, 0x88209182, 0x9c209182, 0x7e209182, 0x23051147, 0x2300ceff, 0x28203182, 0x29240382, 0x2a00f6ff, 0x2b240782, 0x2c00f1ff,
	0x34200b82, 0x36200b82, 0x39260782, 0x3b00e7ff, 0x9582abff, 0x9582ba20, 0x8182c420, 0x5900d822, 0x8f20e982, 0x10201f82, 0xa620eb82, 0x260edd4d,
	0xff1900b0, 0x821d00e2, 0x05854703, 0xad42d820, 0x82e2200a, 0x00f12241, 0x2055823f, 0x20418246, 0x20038291, 0x23ab8829, 0x007eff09, 0x20098d48,
	0x204582e2, 0x21a382ec, 0xb54700dd, 0x00ec2105, 0x20244d83, 0x2100a1ff, 0x22227782, 0xb782ddff, 0x2400f622, 0x2520ab82, 0x262a0f82, 0x2800bfff,
	0x2900b0ff, 0xc382baff, 0x4b00b021, 0xba2205a5, 0xdf823200, 0x03823320, 0x1b823420, 0x07823520, 0x1f823620, 0x07823720, 0xcf823820, 0x37823920,
	0x0b823a20, 0x03823b20, 0x03823c20, 0x03823d20, 0x03823e20, 0xc4ff3f2c, 0x88ff4600, 0x56ff4b00, 0x37828f00, 0x0b829120, 0xe7820420, 0xe782ce20,
	0x1600ec22, 0x18207182, 0x1191b782, 0x75411b20, 0x002f2344, 0x7941000f, 0x41d32015, 0x0d220c79, 0x9b820900, 0x2005c746, 0x06f144ec, 0xe542ec20,
	0x00ec2206, 0x26a58236, 0x00f6ff38, 0x820a0039, 0x829c20cb, 0x00ec24c7, 0x829cff91, 0x8202205b, 0x820620bb, 0x097f5003, 0x1400a622, 0x15202d82,
	0x1620e582, 0x18382182, 0x2b0092ff, 0x2f00f6ff, 0x39001e00, 0x3b00e2ff, 0x3c00baff, 0x3e00c4ff, 0x0f200382, 0x6545738a, 0x0d594307, 0x0f207385,
	0x2406e141, 0xff8500a6, 0x207b82dd, 0x227b82f1, 0x411600a6, 0x8942241f, 0x1b134113, 0xecff8f22, 0x0b41d582, 0x2097b109, 0x20d5820c, 0x0679459c,
	0x2809474c, 0x00d8ff2c, 0x00d3ff34, 0x22078236, 0x41e2ff38, 0xd3200645, 0x9c22c982, 0x3d541000, 0xff132210, 0x064541c4, 0x1600c422, 0x3006934e,
	0x00baff18, 0x000a0026, 0x00f6ff2b, 0x00f1ff39, 0x244d823b, 0x00ddff3c, 0x2003823e, 0x380b4115, 0x24151b42, 0xff8f00c4, 0x5a6141ec, 0x054a0e20,
	0x10af4a0d, 0x2019054a, 0x1c955411, 0xf6ff7724, 0x03827800, 0x03827920, 0x03827a20, 0x4d557e20, 0x829e2006, 0x82b22007, 0x82b42003, 0x82b92003,
	0x41122003, 0x09200871, 0x0e200d82, 0x4206b355, 0xec2209fd, 0x13822a00, 0x2205a145, 0x823600ec, 0xff38220b, 0x064948f1, 0x5146ec20, 0x828f2008,
	0x94132017, 0x82282049, 0x0535482d, 0x2205fb44, 0x4623002f, 0x3820082d, 0x2012f955, 0x24a7843f, 0x000100f1, 0x22098223, 0x82200001, 0x2205833f,
	0x4c1100ec, 0x172810f7, 0x1800ceff, 0x19009cff, 0x2b202382, 0x4f832d82, 0x53843920, 0x4c00e221, 0xd3260501, 0xe2ff3e00, 0x9d823f00, 0x03824620,
	0x57459120, 0x45ba2006, 0xec200a57, 0xb5204182, 0x520c5352, 0x21520b4f, 0x828f200f, 0x4199455d, 0xc5bd41c5, 0x3b000422, 0x3c20cb82, 0x2c08c94c,
	0x000300f6, 0x001e001d, 0x00e2ff20, 0x2c198221, 0x00160002, 0x00070001, 0x00090000, 0x2407821a, 0x001c001c, 0x201f8219, 0x320d823f, 0x00410041,
	0x0046003c, 0x003d0047, 0x004e004a, 0x8252003f, 0x00442a01, 0x00550054, 0x00570045, 0x22198259, 0x82850085, 0x008c261b, 0x004b008d, 0x2201828f,
	0x8291004d, 0x004e2201, 0x22018294, 0x8296004f, 0x00502201, 0x24018298, 0x009b0051, 0x223d829c, 0x82a100a1, 0x00a3223f, 0x284382a5, 0x00a700a7,
	0x00ab0058, 0x2a4982ac, 0x04000100, 0x01010002, 0x18511701, 0x180c6b45, 0x83072e45, 0x262c081a, 0xf80010f8, 0x1cf8011b, 0x031df802, 0x730416f8,
	0x041c6afb, 0x052ffa6d, 0x0f87071c, 0x11ea081c, 0x5d3b1ca0, 0x01030012, 0x62533c01, 0x43ed4518, 0x0dce4518, 0x0ee54518, 0x82790021, 0x390025af,
	0x76003e00, 0xfd09f182, 0x00e200de, 0x012e01f1, 0x0178013f, 0x01ab01a1, 0x01ba01af, 0x01cd01c7, 0x02e501da, 0x022f0218, 0x02480236, 0x028c0267,
	0x02c202a8, 0x03f902ca, 0x03220315, 0x0345033c, 0x0355034a, 0x03770371, 0x0381037b, 0x039d0393, 0x03a803a3, 0x03b903ae, 0x03ca03c4, 0x03f203e4,
	0x040c04f6, 0x041d0413, 0x042b0421, 0x04400436, 0x044f0449, 0x04590454, 0x0470045d, 0x0482047b, 0x048c0488, 0x04a20490, 0x04ae04aa, 0x04bd04b8,
	0x04c504c1, 0x04cd04c9, 0x04df04d1, 0x04ef04eb, 0x050305f6, 0x0514050c, 0x051c0518, 0x05280520, 0x0532052c, 0x05410536, 0x0556054b, 0x0562055d,
	0x056b0567, 0x0573056f, 0x057e0577, 0x058a0586, 0x0592058e, 0x059f0596, 0x05ae05a8, 0x05ba05b4, 0x05c305bf, 0x05cc05c8, 0x05d405d0, 0x05dc05d8,
	0x40d115e0, 0x95918505, 0x941b967e, 0x8f90888f, 0x90878e8a, 0x05e3431f, 0x92848296, 0x06831b7f, 0x8084847f, 0x33431f82, 0x8a868705, 0x910a7588,
	0x9595938e, 0x1e909098, 0x207ef80e, 0xa6150b0a, 0xa1979997, 0xaf64c3af, 0xa7a6b21b, 0x8c1f98b3, 0x8e918c8e, 0x9283951a, 0x808a8281, 0x701e8885,
	0x757b7b7f, 0x65b15369, 0x6e64701b, 0x8a1f816c, 0x88838888, 0x88900b1a, 0x1b78958f, 0x7f85837f, 0x2e4d1f83, 0x89888905, 0x8c0a7587, 0x1e8e918a,
	0xf71d920e, 0x2bf78c5c, 0x0a3f35f7, 0x2c8d0a66, 0x895dfb1d, 0x1d3f0a92, 0x5df735f7, 0xa61d2c8a, 0xf74dfb16, 0xf728fb22, 0x0a2a8940, 0xf78d3ff7,
	0x8c28f722, 0x0a414df7, 0xfb8a4ef7, 0xfb29f722, 0xfb0a3d3f, 0xfb22fb40, 0x2c4efb29, 0x1d280b1d, 0x0a330a24, 0x9e9e9a9a, 0x78789a7c, 0x1e787c7c,
	0xfb1d500b, 0x03fbf21a, 0x03f70a57, 0x1d2c1af7, 0xf7231bf7, 0x2a14fb02, 0x2414fb0a, 0x16a41d6a, 0x0ef7078b, 0x5b821d5a, 0x83e90523, 0x247d820a,
	0xfb282d0d, 0x081d8205, 0x7a2e053a, 0x150b1f0a, 0x8c8c8891, 0x898f1a90, 0x1e8e898f, 0x0b05e84d, 0x89160af7, 0x7e878306, 0x918f911d, 0x8c1e8c93,
	0xf8068c06, 0x851d8407, 0x86868484, 0xfcfb1f84, 0xd1f7ccfb, 0x5f080e82, 0xfb1d8e85, 0x90cdfbd1, 0x6a84861d, 0x06fc1f0a, 0x898a0b06, 0x851b898c,
	0x8e92888e, 0xf990f792, 0x918d1834, 0x8a918e90, 0x88909008, 0xf71f8e86, 0x8e34fd90, 0x85848784, 0xd90b1988, 0x1fb1dbc2, 0x0a390a87, 0x078b1f0b,
	0xca01770b, 0xf8030a91, 0x0b1d2195, 0x7c9d0a2c, 0x7c79799a, 0x0a29797c, 0x1334e882, 0x130a24b8, 0x130a55be, 0x131d2a7e, 0x0b0a38be, 0x78130a3b,
	0xb8200a82, 0x27080a82, 0xabf71d71, 0x1e930a94, 0x4391078b, 0x3ed82b0a, 0xea0a2ae9, 0x2cebd8d8, 0x6c1d681d, 0x920a2a0a, 0x02fb0a43, 0x01fb3332,
	0x002ae782, 0x02f7e333, 0x920b1d2c, 0xbf831d60, 0xf7cdf725, 0x825206d1, 0xf72b08bc, 0x4afcf7cc, 0x0a290b0a, 0x2d1648f7, 0x131d220a, 0x88918170,
	0xb0131e95, 0x91918e95, 0xf70e1f8e, 0x06f8160b, 0xfc1d3306, 0x828a0607, 0x83880801, 0x8585878a, 0x840a761a, 0x1e93858f, 0x150b068d, 0x8792897c,
	0x8f941b94, 0x1f8f9890, 0x055df7c4, 0x948e948d, 0x84971a91, 0x781e8290, 0x83827b06, 0x0b1f8879, 0x868a1d4b, 0xfb81fb86, 0x82fb18db, 0x9087dbf7,
	0x87868c83, 0x8a878419, 0x0b869083, 0xa4c4f701, 0x98c4f703, 0xf71d4815, 0x8bf707dd, 0x1d38e5f7, 0xe5fb89f7, 0x1afb0b18, 0xf703fbf2, 0x838a0b14,
	0x06891b1d, 0x8a8a8988, 0x88881f89, 0x8a8b8a05, 0x898a1f8b, 0x4a080182, 0xfb07891a, 0x873007b2, 0x1b304241, 0xe9d83e2c, 0x4babf71f, 0x614a0b0a,
	0x1b395d42, 0xfb0464f8, 0xfb383101, 0xf81f7e01, 0x01f7063a, 0x20de317e, 0x1d440b1b, 0x78997d9e, 0x787d7d78, 0xc50b0a29, 0xb6ac9fc2, 0x8b90901f,
	0x84918792, 0x88500893, 0x5a646d08, 0x0b1b5778, 0x0a3d51fb, 0x2bfb51fb, 0xe81d490b, 0x76a20b29, 0x4ff776a1, 0x7778f8a5, 0xfb1d570b, 0x1834f990,
	0x8e868890, 0x8c851b86, 0x85898886, 0x34fd90fb, 0x080a6918, 0x0a2a850b, 0x150b8683, 0x8b0b0a2c, 0xa44cf8a4, 0x1d50980b, 0x7d08df82, 0x93900a60,
	0x0736f91f, 0x0b0a448b, 0x43fb0a45, 0x681564f8, 0x1d6b0b0a, 0x0b1e1d5c, 0x05f71d5a, 0x131d4f0b, 0x8f1d56b0, 0x85938990, 0x8f86198f, 0x40fb0b83,
	0xf722fb8d, 0x1a4df728, 0xf976a00b, 0x060b7750, 0x053cf7a6, 0x8d999013, 0x1b7a9a8e, 0x7e828b7e, 0xa0131f89, 0x0543fb6f, 0x0660150b, 0x0505fb3e,
	0x878a8487, 0x150b1a87, 0xf70b078b, 0xfb24f748, 0x8c46fb26, 0x48fb8a1f, 0x018224fb, 0x89484808, 0x61920e08, 0x0a93910a, 0xf73ff70b, 0xfb29fb22,
	0x840b8c4e, 0x640b8591, 0x8d078c1d, 0x0a518c07, 0x7e9d800b, 0x93909a9b, 0x0e1e9197, 0x911b0a67, 0x88928f8e, 0x8d8d0b92, 0x901b8c8a, 0x7e0b8f90,
	0xa532f9a6, 0x82ede80b, 0x86833cab, 0x920b8584, 0x7d0b0a5c, 0x9c7c8889, 0x948b981b, 0x131f8d98, 0x47f7a860, 0x41070b05, 0x54080759, 0x8db00b1e,
	0x0177a1af, 0x920a5c0b, 0x820b9086, 0xfb0b841d, 0x720b07ab, 0xa4a476a0, 0xa4a4a0a0, 0x7272a076, 0x1e727676, 0x9091070b, 0x1f8c9191, 0x1d75080b,
	0xa48b900b, 0xf7a373f7, 0xf80ba554, 0x0b92072a, 0x0b920a62, 0x0b011d45, 0x0b1f0a4f, 0x0b921d54, 0x08da821a, 0xb8078c68, 0x6060ac65, 0x1e5b6d65,
	0x060b078a, 0x7d857f7d, 0x9f7f9184, 0x920e1f8d, 0x590b9186, 0x0a78011d, 0x01770b03, 0xebf7a4de, 0xf7de03a5, 0x690b155a, 0xf7a5ce1d, 0x0b03a4ba,
	0x8c969806, 0x0b809898, 0x0b0a8ac5, 0x0b1d898b, 0x0b8c8b8c, 0x92910a5f, 0x0b1e9291, 0x0b0a7490, 0xe59801f7, 0x86910bde, 0x6fb00b92, 0xbc82be76,
	0xb0131222, 0x32089e82, 0x84859185, 0x0b1e0a97, 0x1596c8f8, 0x87908e86, 0x910b1b90, 0x36f91f92, 0xa4c50b07, 0xfb0b31f8, 0x0b76a0d7, 0x0b1d8792,
	0x0b858584, 0x0b8a8c8a, 0x82909206, 0x949131ad, 0x980b1e1d, 0x910a6c1d, 0x910b1e91, 0x180b9291, 0x0720bc82, 0xaf090382, 0x0b918592, 0x47f725f7,
	0x25f748f7, 0xb0acf70b, 0xacf7e701, 0x8a840b15, 0x0b1d941d, 0x84868683, 0x76a00b1f, 0x0ba465f8, 0x1d84fcf7, 0x1d8a070b, 0xf1f7d40b, 0x621f0b15,
	0x5c930b0a, 0x6f070b0a, 0xccf70b1d, 0x86930ba4, 0x90850b90, 0x00010b85, 0x07001823, 0x00010000, 0x00091100, 0x02001942, 0x01090000, 0x00000c00,
	0x1f00001d, 0x00220000, 0x00000f00, 0x1e000020, 0x000e0000, 0x00001000, 0x0d00011b, 0x00680000, 0x00000300, 0x0b000040, 0x00210000, 0x00003d00,
	0x77000069, 0x00750000, 0x00004100, 0x04000008, 0x007f0000, 0x00005f00, 0x7c00007e, 0x00b50001, 0x0001b200, 0xb60000b9, 0x00ba0001, 0x0000be00,
	0xbf0001bb, 0x00c40000, 0x0001c100, 0xcb0000c5, 0x01c80000, 0x0000cd00, 0xcf0000d2, 0x00d70001, 0x0000db00, 0xdc0001d8, 0x00e10000, 0x0001de00,
	0xae0000e2, 0x01ab0000, 0x0000b000, 0x6000009f, 0x007b0000, 0x0000c600, 0xa1000099, 0x00a50000, 0x0000aa00, 0x79000083, 0x00760000, 0x00009000,
	0xcc000094, 0x008a0000, 0x0000c000, 0x3c0000dd, 0x003e0000, 0x00005c00, 0x0500005e, 0x005d0000, 0x00000600, 0x64000062, 0x008d0000, 0x00009300,
	0xa800009a, 0x006c0000, 0x00003f00, 0x6f00006b, 0x00890000, 0x00006100, 0x8500006a, 0x00b10000, 0x0000ce00, 0x65000095, 0x00ad0000, 0x0000b400,
	0xbd0000b8, 0x00c30000, 0x0000e300, 0xda0000e0, 0x00d50000, 0x0000d400, 0xd60000d3, 0x00d10000, 0x0000af00, 0x8e0000ca, 0x02242682, 0x03000100,
	0x190a4b82, 0x01f600d5, 0x01300100, 0x01ed01b3, 0x023e02fd, 0x03b40296, 0x036a031e, 0x04e203a8, 0x054205a8, 0x061e06f4, 0x078f0630, 0x0796072a,
	0x08e8079f, 0x09e508e2, 0x0ab00957, 0x0bae0a24, 0x0ca70b10, 0x0c6c0c2a, 0x0d680de9, 0x0dd40d75, 0x0e420edd, 0x0fa40e4b, 0x0f780f1b, 0x10e00f9a,
	0x104d102a, 0x114e11e7, 0x12b9115d, 0x1379121f, 0x13711328, 0x14f013a6, 0x14e1147e, 0x155b15fe, 0x16eb1589, 0x169c164f, 0x174a17ef, 0x1769175b,
	0x180918ca, 0x18681833, 0x18841876, 0x18a01891, 0x19d818b6, 0x1a421a6e, 0x1a961a7c, 0x1ac61ac3, 0x1be61ad7, 0x1b6a1b5f, 0x1b811b76, 0x1b971b8c,
	0x1bc51bae, 0x1cec1bd7, 0x1c141c01, 0x1c7a1c68, 0x1cd61cc5, 0x1df21ce6, 0x1d131d07, 0x1d361d21, 0x1d571d4b, 0x1d7f1d67, 0x1ea81d94, 0x1e2b1e17,
	0x1e4f1e42, 0x1e6b1e5e, 0x1eb01ea3, 0x1edc1eca, 0x1f071ff5, 0x1f511f1a, 0x20e61f81, 0x21d8203e, 0x22ea2119, 0x22b422a4, 0x23fd22d2, 0x24e72377,
	0x25982421, 0x26082652, 0x266f263a, 0x284027d8, 0x2842281a, 0x295a29c7, 0x2a402ad0, 0x2bc52a7e, 0x2b742b32, 0x2cf82bb6, 0x2c0f2c03, 0x2c9e2c9c,
	0x2d2b2dad, 0x2ec72d42, 0x2e4d2e3a, 0x2e762e61, 0x2fa32e8d, 0x2f872f3d, 0x2fb12f9e, 0x2fd22fc2, 0x30ff2fe3, 0x3086306e, 0x810e23f5, 0xe6f7a48b,
	0xa4b1f7a6, 0xf7a4f301, 0xa5b8a5b7, 0x0a5ff303, 0xf7068d1e, 0x00f70660, 0x00f7e2e1, 0x31da47e9, 0xa8bf1f9c, 0x1acbc1ae, 0x2ed73fe7, 0x0632fb1e,
	0xf70a7b89, 0x0756f7e6, 0x2f3fd5e9, 0x2d3f412d, 0xf856fb1f, 0xb1f71501, 0xda0728f7, 0x8c4cca89, 0x8a3c083d, 0x1b3c4c4c, 0x8b3bf70e, 0xa526f9a5,
	0x03a5d401, 0x50f8f7d4, 0xf758fb1d, 0xf733fb37, 0x0a2a8a5c, 0xbaa9d9e1, 0x911d2cca, 0x1d978d90, 0x91860a41, 0x87858b83, 0x515e1d65, 0x3d3b7142,
	0xfb4dfb0a, 0xf727f72c, 0xf71d2c4b, 0x0983f749, 0x0a2a4d3e, 0x5e71d4dc, 0x901d2cc5, 0x918c9386, 0x8f0a4191, 0x85938990, 0xba0a4190, 0x35a93b4e,
	0x5c313182, 0x34fb37fb, 0x0e1f56fb, 0x0a7d07f7, 0x53f8a4f3, 0x35c984a4, 0x19f7068c, 0x19fb0a5d, 0x0a7b8a06, 0x0ef71ef9, 0x801d5107, 0xec820a4c,
	0x36033a08, 0xa08c0e1d, 0xa4e6f776, 0xf3011d96, 0x65f303a3, 0x1d94910a, 0xf7d9f71e, 0x1d5207d2, 0xccf7d2fb, 0x0a4afdf7, 0x8a0607fc, 0x85068906,
	0x761f0a56, 0x66f70e0a, 0xc6f7a58b, 0x823382a5, 0xf84508d1, 0xf903a5bd, 0x15e0f720, 0x600781fb, 0x3d714251, 0xfb52fb1b, 0xf72af72e, 0xf74bf74e,
	0xf72bf72e, 0x6fd6dc52, 0x901fc55d, 0x8f8c9586, 0x8a929090, 0x08908493, 0xa83a4dbc, 0x60fb1b34, 0x35fb3afb, 0x5cfb5afb, 0x080782f7, 0x8a60f72d,
	0xa9dce21f, 0x8d1fc9bb, 0x8c8d8c8d, 0x078d088e, 0x8c1d768c, 0x0792f71a, 0x84918693, 0xfb06891e, 0x0a890677, 0x1d8a8583, 0x84f40e1f, 0xe5f722b2,
	0x08c28277, 0xa422f823, 0x65a3f803, 0x1d81920a, 0x9136f91e, 0x0a6a841d, 0xfcd8fb1e, 0x4bd8f722, 0x840a760a, 0x91938590, 0x32d18387, 0xd9fb22f8,
	0xbffb0e07, 0xf7011d4d, 0xf703a403, 0x821d4603, 0x59813dcc, 0x6ff8010a, 0xf7b803a3, 0xa92d1536, 0x1bf347e2, 0xf4f315f7, 0xf81f15f7, 0x831d9559,
	0x2508f182, 0x0759fc1e, 0x2e07fb8a, 0x8907fb2f, 0xc83c2f08, 0x881f71df, 0x838e8592, 0x87898589, 0x0e080a95, 0x76a003f7, 0x88824ff9, 0xf303a527,
	0x4ef71d77, 0x08b48207, 0xf71f8c44, 0xf740f73f, 0x0504fcb2, 0x888e8e89, 0x8d8e1b8f, 0x901f0a96, 0x86928c8f, 0xf8b3fb92, 0xb2f71805, 0x9090b2f7,
	0x191d978b, 0x8b849086, 0x5ffc8685, 0xf8185ffc, 0x84910756, 0x1d5b8592, 0x0735fd1e, 0x9882620e, 0xd982f320, 0x93167e2c, 0x1f831d60, 0x2af9fdfb,
	0xd0828606, 0x1e1d6b27, 0xf70e068d, 0x08758284, 0xa0774e38, 0xa4f31277, 0x13a4abf8, 0x982cf9b8, 0xf91d8515, 0x830a5b35, 0x8ad8131e, 0x8a8a8b8a,
	0x8988881f, 0xfb1f8989, 0xfb08fca0, 0x0508f8a0, 0x8d88888d, 0xb8131b88, 0x1d828a8c, 0x89841b24, 0x67821f0a, 0x1d482508, 0xf70711f9, 0x05f8fb94,
	0x898f8d88, 0x068d1b90, 0x8e8d8f8f, 0x93f71f8e, 0xfd05f7f7, 0xf70e0710, 0x0176a03b, 0x60206482, 0xaa088b82, 0x8a1594e1, 0x861e0a6e, 0x9088908d,
	0x1f1d5c1b, 0x1d9531f9, 0x1e0a8184, 0xfc070afd, 0x8716f962, 0x848c8490, 0x88881987, 0x1a878789, 0x31fd0788, 0x1d8a8407, 0xf91e1d5c, 0x60f8070f,
	0x0e0513fd, 0x1d921d70, 0x5dfb078b, 0xfb2bf78c, 0xf70a3f35, 0x8d5df735, 0x5cf71d2c, 0xf72bfb89, 0x661d3f35, 0xa61f8a0a, 0x58078b16, 0x530a2a0a,
	0x8a1d2c1d, 0x22fb4dfb, 0x3ffb28fb, 0x4c0a4189, 0x8d760e1d, 0xa4f3010a, 0x03a5fef7, 0x481598f3, 0xf7bef71d, 0xe3f60755, 0xf700f7e1, 0x20e23300,
	0x065ffb1f, 0x1d610689, 0xf2fba41f, 0x82e6f715, 0xe899081b, 0x2d2d3fd7, 0x1f2e413f, 0xfb84f70e, 0xe7c4a428, 0x25f9a472, 0xa6cd12a6, 0x13a4fef8,
	0x44bbf9bc, 0x82908715, 0x0887868c, 0x7d5d5963, 0x55541b60, 0x1f56aaa2, 0x916668a1, 0x23f71b6b, 0x0ff705ce, 0x14f7e1c1, 0xf71a26f7, 0xf72afb58,
	0xfb4efb34, 0xfb2afb4b, 0x8a58fb34, 0x8c57fb1e, 0x32fb2af7, 0x8e8e4bf7, 0x131b8e8b, 0x4823fbdc, 0x95898883, 0x888e190a, 0x8a8d888c, 0x8b91888e,
	0x0a418d91, 0x8f8c8d8b, 0x9b921e8e, 0x1bb899b0, 0x7885aba8, 0xc16b1faa, 0x1bcb72c3, 0x0800828b, 0x9cbfbd4b, 0x911ec0b4, 0x1d6f8c91, 0xf85efd08,
	0x4cf7153d, 0x25f720f7, 0x40f73bf7, 0x25fb1ff7, 0x4afb4cfb, 0x26fb1ffb, 0x3bfb40fb, 0x26f720fb, 0x0e1e4af7, 0x120a8da0, 0xf2f7a4e5, 0x13a57ea4,
	0xb178f8f8, 0x9b957715, 0x901b9584, 0x0867828e, 0x8c921f3a, 0x9189938e, 0x8f83918a, 0x89088a85, 0x89868a06, 0x841f8299, 0xc4aa8399, 0xbb5cd81a,
	0x7f1ea45f, 0x7f917f92, 0xf4130890, 0xd5c6a2dc, 0x00f71ae4, 0x1e20e233, 0x8a065ffb, 0x2a080182, 0x761f1d61, 0xf71d480a, 0x0752f7be, 0x9b8a958b,
	0x83a11e86, 0x72a77ead, 0xa7f81308, 0x5a69a172, 0x6a924f1a, 0xfc1e7797, 0x41bff705, 0x5d08055a, 0xd7e8f413, 0x402d2d3f, 0x1f8a2f42, 0xca0e0689,
	0xf1d20a6f, 0xc45d1d50, 0x3df454ea, 0xf71ef70a, 0x00f7dd08, 0xc11d2c8c, 0xa95eb56e, 0xa95d0a2a, 0x99439e4e, 0x068b8908, 0x9d54974b, 0x0a41a565,
	0xaa76a565, 0xb41d6cb4, 0xa7b1b1a2, 0xb2a70a2a, 0x3dc79dc1, 0x62cadf0a, 0x1d2cc466, 0xcb828890, 0x4190bf08, 0x89918f0a, 0x658e8692, 0x4653b01d,
	0x0a3d30b8, 0x00fb14fb, 0x2c8a273f, 0x64a6591d, 0x0a2a6fb5, 0x78c56fb5, 0x8a8d7fcd, 0x7fd11d88, 0x6fb478c6, 0x6fb40a41, 0x6c5e69a2, 0x62725e1d,
	0x0a2a6c61, 0x7851616c, 0x2a0a3d4a, 0x54b7bf30, 0x8f861d2c, 0x87878a84, 0x86870a41, 0x8790838c, 0xa0b70e08, 0xa437f976, 0xa5baf701, 0x77baf703,
	0xf72af91d, 0x9093077a, 0x830a6b90, 0x067bfc1f, 0x7a841d61, 0x7bf71f1d, 0x0e062afd, 0x0a5966f7, 0xf8a4f401, 0xf403a592, 0x2afc0a37, 0x26f70e07,
	0xf1f70a72, 0x8013158c, 0x8a1d768b, 0x8c068d1e, 0x8c401306, 0x8c8c8b8d, 0x8e068c1f, 0x8c8d8d8c, 0x1982088e, 0xf71a3d08, 0x8d35f9af, 0x84938991,
	0x8e85198d, 0x85898883, 0x19fda4fb, 0xf9a3fb18, 0x84918719, 0x1988848e, 0x95888985, 0xfdaff70a, 0x898c1835, 0x8a8d898c, 0x8c0a7308, 0x86f80e1b,
	0x98f90a72, 0x8a255e83, 0x1b1d768c, 0x245e828c, 0x0a828d8d, 0x215f821f, 0x5d821e8c, 0x32828220, 0x1835f92d, 0x9388918e, 0x8e848d84, 0x83878885,
	0x181a3658, 0x65f857fb, 0x8f859188, 0x86198a86, 0x89858885, 0xfc58fb1f, 0x246d8265, 0x9591881a, 0x286c830a, 0x858f8387, 0x33fdaef7, 0x256c8318,
	0x1e0a7989, 0x70834013, 0x8025bd82, 0x8b8c8c8a, 0x826a828d, 0x1d7621c7, 0x3a08c684, 0x8e8c0a96, 0x6af85af7, 0xfc58f718, 0x8d878c69, 0x198a8f88,
	0xa00af70e, 0x8eda0176, 0x8d8e8915, 0x8f1b8e8a, 0x8e8d8e8e, 0xf77ff71f, 0xfb80f7d7, 0x8e8905d7, 0x1b8e888f, 0x828c8d8e, 0x90903c14, 0x9088928c,
	0xddf785fb, 0xf785f718, 0x8a908ede, 0x198f8693, 0x8a838f85, 0x82fb8687, 0xfb18212c, 0x7d433582, 0x19630805, 0x838b8785, 0x84f7868f, 0xfb18defb,
	0x87ddfb84, 0x91848b86, 0x8b0e1986, 0x1d3976a0, 0x0e07ddfb, 0xf9a48bca, 0xf801a51c, 0x1535f9b9, 0x1efd6dfc, 0x8a898705, 0x841a8787, 0x1e938590,
	0x7ef8068c, 0x1d609306, 0x67fc1f83, 0xf96ff806, 0x768c8c1f, 0x8b198d1d, 0x0a678c8c, 0x1d7a1a8d, 0x7ffc1e83, 0x831d5b06, 0x1fb2082b, 0xa48be70e,
	0xf8a495f7, 0xbc01a404, 0xf7a5aca4, 0x16f7a508, 0x03a4b5a5, 0x1574f7bc, 0x27ef10fb, 0x11f710f7, 0x10f7efef, 0x869e899f, 0x8d981e9e, 0x94969198,
	0xa398999e, 0x9608aa8a, 0x9688978a, 0x84938a1e, 0x848a848e, 0x0a95868a, 0x8d808d08, 0x891a8282, 0x7e7d8473, 0x7e820881, 0x1b79867b, 0x7e8c0685,
	0x1b7f8b80, 0x72795f43, 0x71701f70, 0x1a746c86, 0x8b7d8f78, 0x908e851e, 0x8f1b9088, 0x8f8e9106, 0x08938891, 0x908a0a6e, 0x918a901e, 0x909e1a93,
	0x1e9e9ea3, 0x9cb09f9e, 0x95971bcf, 0x1f98898b, 0x79900691, 0x1a78788e, 0x323202fb, 0x058203fb, 0xf7e4bc08, 0xcfb7d802, 0x8d1eadcc, 0x8d8d8c8d,
	0x8d8e901f, 0x08918991, 0x8c8b8a8b, 0x8d1d831a, 0x67088c8a, 0xb9b476a4, 0xc8c8d71a, 0x4ec8d7d5, 0xe51d8d3f, 0x3331d342, 0x5b314342, 0x6dab619f,
	0x5f64491e, 0x0e1a3942, 0xf50ec4fb, 0xa4cc0a7d, 0x03a43ff8, 0x50f3f7cc, 0xf252fb1d, 0x1bf734fb, 0xf70a2a8a, 0xf7f18c1d, 0x6c52f734, 0x2550f71d,
	0x1dfb34f7, 0xfb0a2a8c, 0xfb248a1b, 0x1a50fb34, 0x507af7e5, 0xc0b2c71d, 0x0a3dc3ad, 0x4f69c0c4, 0xb31d2cb2, 0x3139a451, 0x722f1d6c, 0x2a516339,
	0x56644f0a, 0x0a3d5269, 0xc7ad5653, 0x641d2c64, 0xe7dd71c5, 0xa5e51d6c, 0x1ec5b2dd, 0x47a1fb0e, 0xf6080518, 0x03a440f7, 0x509840f7, 0x911d541d,
	0x1d940a2a, 0x0735f91f, 0x830a5b8b, 0x89850a2a, 0x1ffb1f0a, 0x861d5031, 0x8f8a908d, 0x8d8d0a3d, 0x1f8d8c8b, 0x05df20f7, 0x8f92078b, 0x9188928d,
	0x92870a41, 0x87858c83, 0x883820fb, 0x8a87841d, 0x08868e83, 0x0a7d7d0e, 0x03a459f8, 0x871590bb, 0x8f8a8f8d, 0x8e068c1b, 0x0628f806, 0x921d8791,
	0xfc1f1d98, 0xcef7060d, 0xbd05b1f7, 0xc8c8a4b7, 0xc277c01a, 0xbb1eb665, 0x4ea44e5f, 0x7856551b, 0x681f5f64, 0x8a87866a, 0x19869082, 0x8b928690,
	0xabae9091, 0xbbb1ad18, 0xc01bbb9d, 0xb26074c2, 0x9d65ad1f, 0x551a5c5b, 0x63605574, 0xfbe6fb1e, 0x8b8686c5, 0x19868f83, 0xa58c910e, 0x01a51cf9,
	0x03a554f8, 0x66f89cf7, 0xf762f715, 0x8c0a6767, 0x8d8c198d, 0x1a8d8d8b, 0x1e841d8a, 0x6106e7fb, 0x1f1d6b1d, 0xfb06c9f7, 0x8264fb5e, 0x08658339,
	0x8b8c8b9a, 0x8e871a8a, 0x1b90898f, 0x6377bfc0, 0x62b31fb4, 0x1a57569f, 0x62567755, 0x63621e63, 0x1b557756, 0x719f5756, 0x8690830a, 0xc7b85d08,
	0x8c1bc675, 0x1f8b8b8b, 0xb8a2c7c6, 0xb9b81fb9, 0x1ac7c7a2, 0x5ec774c6, 0xb3641eb8, 0x9057a158, 0xa0c70e08, 0xa429f776, 0x0177a1f8, 0x03a56af8,
	0x15986af8, 0xf71e1d8d, 0x9307c81c, 0x86920a61, 0x4e1f8390, 0x8d0693f8, 0x8c8a8c07, 0x768a1a8c, 0x088c8a1d, 0x8e87898e, 0x89881b87, 0x1f88898a,
	0x3afc8989, 0x8687a1fc, 0x190a988c, 0x3905c543, 0x2bf8068d, 0xa413fc06, 0xf813f815, 0x6efc056e, 0x8b8c0e07, 0xa532f8a5, 0xee8264f7, 0x03a5492d,
	0xc71679f7, 0xb8b9a2c7, 0x83b8b91f, 0xc723088c, 0xb85fc674, 0x4f5db91e, 0x5a1b4fa1, 0x626c7c5a, 0x92f7a91f, 0x06ddf705, 0x93909192, 0x84928591,
	0x51e6fb1f, 0x9a080642, 0x87878a87, 0xfb68858a, 0x878a18bb, 0x888d888c, 0x8a8b8b08, 0x1d831e8c, 0x87901e8c, 0x908e8b94, 0xbfb5b308, 0xc11bc09f,
	0xb36377bf, 0x9f63b41f, 0x561a5755, 0x62625677, 0x5563631e, 0x571b5676, 0x0a71a056, 0x08879082, 0x74c7b95d, 0xb10e1bc5, 0x27f8a48b, 0xa472f7a4,
	0xf8a7cb01, 0xce03a428, 0x8a1561f7, 0x8a8c898b, 0x9706fb1e, 0x09f734eb, 0xf111f71b, 0x8c12f7f0, 0x8a11f71f, 0x11fbf025, 0x5a3f331b, 0x921f6642,
	0xafd49ed3, 0xd0d9bac8, 0x0890f6c2, 0x91919091, 0x82908593, 0x074708a5, 0x5b4d3b86, 0x385b0837, 0x1a2d2277, 0x8d768c76, 0xa1a41e77, 0xe503f715,
	0xf703f7e5, 0xfb31e404, 0xfb1e8d03, 0x31328904, 0xfb1b04fb, 0xf7e53103, 0x7a0e1f04, 0x76a176a0, 0x12a534f9, 0x46f76013, 0xa013158c, 0x828f1d58,
	0xae2b086e, 0x8d0531f9, 0x8e8e8d8e, 0x9284911a, 0x1afc1e85, 0x84858406, 0x1f1d6b85, 0xfb0605f8, 0x0523fda7, 0x84896013, 0x8892848d, 0x82a50e08,
	0xa71f29c4, 0x01a477f7, 0xa4c6a4c2, 0x99080782, 0xf703a4c7, 0x15c5f81f, 0xbb5bab54, 0x712d1e75, 0x1a253545, 0x28f00ffb, 0x0ff70ef7, 0x0ff7eeef,
	0x2de145f1, 0xa1b91ea5, 0x1ac2bbac, 0x3fc94dd8, 0x3e4d4d3e, 0xe7fb501e, 0xe401f715, 0xf701f7e4, 0xfb32e302, 0x3301fb01, 0xfb02fb33, 0xf7e33201,
	0xf7df1e01, 0xbeca15e7, 0x1e8ccabd, 0x59be8ac9, 0x584d1a4c, 0x584c4d58, 0x0e1ec9be, 0xf7a48bb1, 0x27f8a472, 0xa4cd01a4, 0x03a728f8, 0x156df8cd,
	0x26f211fb, 0xd7e211f7, 0x1eb0d3bc, 0x41764484, 0x3e5d4f69, 0x86205346, 0x00828508, 0x1f8cc108, 0xf71e0a49, 0xc9d99007, 0xbb08dfbd, 0xe9f39fdf,
	0x9f8a9f1a, 0xf71e9e88, 0xe62a8109, 0xfb1b0afb, 0xfb262411, 0x16a41f12, 0xe5e604f7, 0xe1f304f7, 0x1e95243c, 0x0787078a, 0x868b868c, 0x03fb1a86,
	0x03fb3131, 0xe53004fb, 0x0e1e03f7, 0x010a539a, 0x030a91b3, 0x1d217ef8, 0x1d45c30e, 0x017766f7, 0x3cf8a4e8, 0x42e803a4, 0x070df70a, 0x54dbb23c,
	0x18f71be7, 0xf703f7f5, 0x211af71b, 0x18fb02f7, 0x3b543b2f, 0xdef71f64, 0x93859207, 0x83868385, 0x41fc1e84, 0x7f078107, 0x15a0a407, 0xea8f09f7,
	0x1b04f7e8, 0x29eb09f7, 0x0efb0dfb, 0x09fb282b, 0xee2b07fb, 0x0e1f0ef7, 0xc51d696a, 0x0a3103a4, 0xc7206787, 0xf8336784, 0xf71598aa, 0x07970771,
	0x41f80795, 0x93869207, 0x82858583, 0xfb250852, 0x64db07de, 0x1b2fc23b, 0xfb2118fb, 0xfb1afb02, 0x03fbf51b, 0xdbe718f7, 0x1fb2dac2, 0x85070dfb,
	0x7df7721d, 0x22608415, 0x8309fb07, 0x0df73160, 0x09f7edeb, 0x2eea04f7, 0x1e8f09fb, 0x011d660e, 0x75086d82, 0xfb0e0a2b, 0xf876a186, 0xf771a567,
	0x32f7127d, 0xf7b013a5, 0x1551f9ac, 0x73836c64, 0x73761f75, 0x1a596882, 0x3b49d013, 0x6b1d6107, 0xfcdb1f1d, 0x1d6b065a, 0xf81e1d81, 0x9207df5a,
	0x92929190, 0xcd370a93, 0xa893ba06, 0x9b1e9b9a, 0xaf93a299, 0x1f1d811b, 0x8592b013, 0x0e1e8490, 0xa460fbc3, 0xf8a547f7, 0xc701a44b, 0xf8030a91,
	0x157ef891, 0x84858585, 0x9f821d2c, 0x66d94308, 0x5a31c33c, 0x3a0a4f0a, 0xc3dae51d, 0x201fb0da, 0x2f06fb07, 0x8906fb30, 0x936e6a1e, 0x841f6f99,
	0x8889848d, 0x8d858885, 0x08879184, 0x82acab7c, 0x14f71bb0, 0x14f7f3f2, 0x9555f81f, 0x7f1e831d, 0xde8289fb, 0xfb2a2e38, 0x2e05fb05, 0xf70ef7ec,
	0xfb1d400e, 0x0e0a2a0e, 0xf876a094, 0xa841a564, 0xf74c0805, 0xf803a4ef, 0xf70a4265, 0x02f707aa, 0x02fbe433, 0x4e614a40, 0xc0f71e69, 0xfc0a8607,
	0x07830718, 0x8907aafb, 0x888d8807, 0x891e898d, 0x8e8a8e8e, 0x8c068d1b, 0x1f0a828d, 0x8f8d8d8e, 0xb1f71a90, 0xd78fe707, 0xeb1be9d5, 0x1f2b3ed8,
	0x0e332b82, 0x1cf91d80, 0xa4e90177, 0x0a50e903, 0x980419f7, 0x8291931d, 0x1e460800, 0x989307a3, 0x8686831d, 0xfb0e1e83, 0x764afbf7, 0x0177e9f9,
	0x8603a5c5, 0x861554fb, 0xb6928590, 0x1eb6adad, 0x9107f0f8, 0x84849184, 0xfc1e0a97, 0x746d07f0, 0x86846f75, 0xcc1e8286, 0x9815b9f9, 0x9391930a,
	0x46829290, 0x9183922a, 0x1e0a4d85, 0x0a5e5f0e, 0x0c22b582, 0x00838c07, 0x07f71f22, 0x5f230182, 0x460589fb, 0x8d34053c, 0x8d8d8c8e, 0x8d90901f,
	0xfb928692, 0x188bf75f, 0x5ff75ff7, 0x30059a4d, 0x8b849085, 0xd6fb8686, 0xf818d6fb, 0x760a4b9b, 0x3b8f820a, 0xef011d4d, 0x98ef03a4, 0x90841d50,
	0x8b1e9385, 0x7e909207, 0x1d958b1d, 0x4d0a2a84, 0x30051c4f, 0x011d8fef, 0xebf7a5e8, 0xa4eaf7a4, 0x98d1f903, 0x05034d15, 0xf70a623a, 0xfbe43401,
	0x56433600, 0xd61e6f40, 0x37c0436f, 0x624c411b, 0xe41f694e, 0x8720ef84, 0xfb31ef82, 0x838a0735, 0x1a83838a, 0x0a981d62, 0x8e1e8a91, 0x3d9d8506,
	0x1b8c1d76, 0x8c0a6e8c, 0x8c8d8c1e, 0xf71a8e8d, 0x8ee707b2, 0x1be7d5d7, 0x2b3ed8ea, 0x88820a80, 0x8e8f9225, 0x851e0a96, 0x27e48222, 0x1d768c1a,
	0x1d681a8d, 0xe8222684, 0x2684e81b, 0x83940e21, 0xf7a42698, 0xf803a4ed, 0x25958263, 0x8193858f, 0x96821e1d, 0xe3320226, 0x4a4100fb, 0x1e3f8a82,
	0x869307e5, 0x8584848f, 0xfb1e8387, 0x07830746, 0x07891d62, 0x8c898c89, 0x8a0a798a, 0x820a731e, 0x8a640885, 0x088a1d76, 0x068d068d, 0x8e8d8e8f,
	0x8d8d1f8d, 0x621a0a96, 0xd7daec0a, 0x3fd9e9e9, 0x1d621e2a, 0x1d69a00e, 0x31f8a4d1, 0xf7d103a4, 0x0e1d2789, 0xa4e80a77, 0x03a537f8, 0x155dfbe8,
	0xe7f70a39, 0xdbb13d07, 0xf71be654, 0x01f7f318, 0x1af71af7, 0xfb00f723, 0x553b3018, 0xf71f653b, 0x1d97070d, 0x86868384, 0x6f227d82, 0x01828107,
	0x159ea43d, 0xe98f07f7, 0x1b03f7e8, 0x29ea07f7, 0x0cfb0bfb, 0x07fb2a2c, 0xec2c06fb, 0x821f0cf7, 0xa5c7235b, 0xcf8237f8, 0x5c82a620, 0x074bf823,
	0x21018295, 0x44826ff7, 0x84848321, 0x0d2a0844, 0x3b65db07, 0xfb1b30c1, 0x00fb2318, 0x1afb1afb, 0xf701fbf3, 0xc2dbe618, 0xfb1fb1d9, 0x908307e7,
	0x61939286, 0xf8721e0a, 0x58841555, 0x07fb0622, 0x64085883, 0xedea0bf7, 0x03f707f7, 0x07fb2ee9, 0xfb0e1e8f, 0xf876a039, 0xe901a564, 0x98e903a3,
	0x86918315, 0x908f9391, 0x79f71e93, 0x978f8b07, 0x981ea193, 0xaebda3aa, 0xacb008b3, 0x1bc9a9ba, 0x6b919093, 0x4c2d830a, 0x1f65524b, 0x7781777e,
	0xf7087983, 0x8792073a, 0x85858391, 0xfb1e8485, 0x888a0779, 0x898c8a8b, 0x08098208, 0x04fb0ed2, 0xd1c41d72, 0xb26b1d50, 0x3dd565cc, 0xc4dcea0a,
	0x1d2c8cd8, 0x6caa76b1, 0x6b0a2aa0, 0x5a9661a0, 0x8b8a0895, 0x66945f06, 0x419c7297, 0x7f9c720a, 0x1d6ca59e, 0xa4a299a5, 0x9e0a2a9e, 0xb497afa4,
	0xb5c10a3d, 0x2cb1726f, 0x9387911d, 0x41918f8d, 0x8a908e0a, 0x658f8593, 0x5b65a51d, 0x0a3d4daa, 0x44563f31, 0x681d2c8a, 0x78a86e9f, 0x76a80a2a,
	0x84b87fb3, 0x068b8e08, 0x7eb182ba, 0x0a4179a7, 0x759878a6, 0x0a416f8c, 0x6f707b6e, 0x770a2a77, 0x607e666f, 0x4e4a0a3d, 0x2c67a8ad, 0x8391851d,
	0x4184878a, 0x8b86870a, 0x08879184, 0x9f60fb0e, 0xa466f876, 0x017765f7, 0x03a41ff7, 0x7ef888f7, 0x58f73b15, 0x841d8a06, 0x0a2a0a6a, 0x073758fb,
	0x3a05384d, 0x061afcdf, 0xb569ad61, 0x85921d81, 0x746f8490, 0xf81ea7a2, 0x9207db1a, 0x82929091, 0x1f6f082b, 0xa48b940e, 0x1d7165f8, 0x02fb078b,
	0x00f733e3, 0x01f70a2a, 0x02f7e3e4, 0x2a840a52, 0x800a810a, 0x2b078b0a, 0x2a2c3e3e, 0xd83e2d0a, 0x430a52eb, 0x8084851d, 0xa0750e0a, 0x8ff70176,
	0x878d1593, 0x8a8e888f, 0x8c068e08, 0x8d8e8e06, 0x671f8e8e, 0xf71a8c0a, 0x8e62f857, 0x84928791, 0x8e85198e, 0x84898884, 0x47fc4cfb, 0xf84cfb18,
	0x83928847, 0x19220810, 0x84878885, 0x57f7858e, 0x0e1863fc, 0x0a7295f7, 0x158cbbf8, 0x8d8a8013, 0x1b8e8b8c, 0x8d8c8d8d, 0x04821f8e, 0x838d8d21,
	0x82632048, 0x85932448, 0x8384198d, 0x82852048, 0x18483b48, 0xccf717fb, 0x87888f05, 0x851b868e, 0x8988858c, 0xfb17fb86, 0x4bfb18cc, 0x5b4d48f8,
	0x843c0805, 0x0a958889, 0x61fc56f7, 0x888d8818, 0x8a1e898e, 0x8c8a8d8d, 0x8f068d1b, 0x0a968f06, 0x18f78f8d, 0xf718cef7, 0x05cefb18, 0x8c8a078a,
	0x8a1e8b8b, 0x1e1d838c, 0x0a734013, 0x690e1b8d, 0xd425d782, 0x8e88158e, 0x088a828d, 0x8c8e8f2f, 0xf71f8e8f, 0xf76ef734, 0x056efb31, 0x8a8f8e87,
	0x8e8d1b90, 0x1f8c8e8b, 0x938d8e91, 0x37fb9187, 0xf71875f7, 0x4b74f737, 0x8686891d, 0x212482fb, 0x2d82fb18, 0x90875108, 0x87848d84, 0x8a878619,
	0xf7868f83, 0x1874fb38, 0x75fb38fb, 0x838c8587, 0x0e198890, 0xa45efb7b, 0xf8a447f7, 0x77a07762, 0xf7a4bf12, 0xec13a4e9, 0xdc130a34, 0x1d621d3b,
	0x0a53590e, 0xf81af801, 0xd8fb1564, 0x868650fc, 0x878f848c, 0x8d8a8c19, 0x08390182, 0x8b8d8c8a, 0xe7f71b8d, 0x0a619306, 0x83918691, 0x06cffb1f,
	0x4cf8d6f7, 0x2300828d, 0x8c198e8c, 0x38080082, 0x8d078d1a, 0x8a8e8a07, 0x088d888e, 0x8c89888d, 0xe4fb1b85, 0x84868406, 0x85908485, 0xfb0e1f92,
	0xf70a5ef4, 0x981d503b, 0x900a600a, 0x9cf81f92, 0x0a448b07, 0x8b0436fd, 0x07b15a07, 0x1d89a33d, 0x43918591, 0x1f85851d, 0xd20a8c0e, 0x0bf703a8,
	0x421d50a4, 0xd95ac3ac, 0x828b0a3d, 0x2a280800, 0x9292930a, 0x84930a74, 0x751e8391, 0xb15f4a1d, 0x1d2c6cce, 0xea7ccd6c, 0xf41d6cf4, 0xceaaea9a,
	0xaace0a2a, 0x3dccb0b7, 0x2a3d2789, 0x538c3d0a, 0x41426a5a, 0x7c426a0a, 0x6c00fb29, 0x9a00fb1d, 0x1e43ac29, 0xf70a8c0e, 0x27628299, 0xac15a486,
	0xf7ed9ad3, 0x38081982, 0xed7c00f7, 0x0a2ad46a, 0xbc53d46a, 0x0a418a3d, 0x83858483, 0x92831d2c, 0x0a2a9384, 0x4866b7cc, 0xaa1d2caa, 0x222c9a48,
	0x7c221d6c, 0x2a496c2c, 0x5f6c480a, 0x751b4a65, 0x85288b1d, 0xd92808a7, 0xacd4bcc3, 0xf79b0e1f, 0x79f7760b, 0x7779f7ae, 0xaeb9f701, 0xf7b9f703,
	0x92811508, 0x94939683, 0xf71e9593, 0x0768f767, 0x95290983, 0x1f839282, 0x68f768fb, 0x29098306, 0x81848480, 0xfb68fb1e, 0x09830767, 0x28082d83,
	0xfb67f71f, 0x9b0e0667, 0xf80176a0, 0x88158f20, 0x8e8a8f8e, 0x96908f1b, 0x918f1f0a, 0x9187948b, 0xd7f7d6fb, 0xf7d6f718, 0x330e82d8, 0x19918795,
	0x8b819185, 0xdefb8585, 0x8a18e2fb, 0x8a89898a, 0x88290183, 0x898b8919, 0x891e898c, 0x2701828d, 0xf71e0a79, 0x05e1fbde, 0xf7395284, 0xf7158f07,
	0x8ce1f7de, 0x8c0a678c, 0x198c8c8d, 0x8d8c078d, 0x1a8d8d8a, 0x2705828a, 0x8c088c8a, 0x1a8c8b8a, 0x8a250382, 0xf7ddfb1b, 0x2f5b85e2, 0x8b858719,
	0xf7858f81, 0x18d8fbd5, 0xd7fbd5fb, 0x822b0e82, 0x8819858f, 0x8f8a908e, 0x828f8e1b, 0x0ea60899, 0x121d41b5, 0x1d7db013, 0x1d427013, 0x1d47b013,
	0x830afc0e, 0xcfdd01cf, 0x15a5dd03, 0x450e1d26, 0xa9f776a0, 0xa30bf8a3, 0xa368f701, 0x03a442f7, 0x2ff768f7, 0xf71d8615, 0x01f7070e, 0xf7e8e591,
	0x09f71a04, 0x07fbea2b, 0x4b61473c, 0x86871e67, 0x8791848d, 0x8d928890, 0xc408908f, 0xd1b0c7ac, 0x37dff11b, 0x36372523, 0x86868425, 0x078a1f84,
	0xa6fb0789, 0x6c0a9804, 0x1e92900a, 0x0e1d7ca2, 0xa87bf79b, 0x01a647f7, 0x157bf7e9, 0x8beaf78b, 0x78929307, 0x9384931d, 0x83830a8b, 0x91831d2c,
	0xf71e9486, 0x19850464, 0x2c0a6c22, 0x92231a82, 0x82840a8b, 0x91852e1a, 0x0e1e9485, 0xe8f758fb, 0xf7d7019f, 0x3b3c82e8, 0x91078b78, 0x2c918e8e,
	0x9088911d, 0x78fb1e85, 0x8785068b, 0x1d2c8586, 0x91888f85, 0x40082982, 0x0176a04a, 0x1d508cdd, 0x8b8e8c8a, 0x900a3d8c, 0x8c908e90, 0xf9b1f71f,
	0x078b0535, 0x9387918d, 0x0a418d85, 0x87838e85, 0xaffb8589, 0x1d8834fd, 0x0e080a69, 0xe88700fc, 0x0a47e0f7, 0x40043dfc, 0x350d820a, 0xa2f87637,
	0xfc6f0a47, 0x0e1d3dcf, 0x8e370afc, 0x03cddd0a, 0x0c8431c1, 0x7663f82e, 0x017798f7, 0x3761f8d1, 0x56fb0e1d, 0xf7210e86, 0x230f838e, 0x6bfb66fb,
	0x24091582, 0xad34fb87, 0x34fb8901, 0x06e2f715, 0x97909597, 0x7f908197, 0x06e2fb1f, 0x7f86817f, 0x9786957f, 0x42fb0e1f, 0xf7763bf8, 0xd71277c4,
	0xab6b2ff7, 0x132ff76b, 0xf857f7d0, 0x8a7f153c, 0x1b978194, 0x97959497, 0xf7831f8a, 0x054de601, 0x898f9187, 0xc8131b91, 0x96959496, 0x84908496,
	0xbc241f8e, 0x9205bcf2, 0x9690928e, 0x9582961a, 0x89878580, 0x301e8587, 0x01f7934d, 0x828c9705, 0x7f1b7f95, 0x8c7f8182, 0x01fb931f, 0x8f05c930,
	0x848d8885, 0x80e0131b, 0x8082828a, 0x8591821a, 0xf21e8793, 0x055a245a, 0x84848884, 0x94801a82, 0x8f919681, 0x1e918f8d, 0x0e05c9e6, 0xa58b0cf8,
	0xa571d8e6, 0xf7a3cdf7, 0xc012a524, 0xa52cf7a5, 0xf7a3b9f7, 0xbf13a52e, 0xf2f7c080, 0xf755fb15, 0xf731fb31, 0x8fa9a955, 0x921ea691, 0x8a918f8e,
	0x84918893, 0x088a848f, 0x88707284, 0x47fb1b6f, 0x1d8b25fb, 0x25fb1d8b, 0x7e5648fb, 0x1f607558, 0x8886898b, 0x7d811e87, 0x866d6e73, 0x87878808,
	0x7e1b868b, 0x84939084, 0x0889821f, 0x1a93973f, 0x8e0730f7, 0xef079207, 0x2d2cdc3f, 0x28273a3d, 0xcae938d9, 0xa9c0afc1, 0x7d073f1e, 0x7f957d8f,
	0x80df131e, 0x819c947d, 0x8c9191a0, 0x9ac21b92, 0x8d8cc2a8, 0x99bba408, 0xf71ac4c2, 0x258a8256, 0xfb56fb31, 0x9482fb55, 0x56fb6f08, 0x1660f71e,
	0xdbd1cde2, 0x3445cedb, 0x3b454835, 0xe1d1493b, 0x30fb0e1e, 0xf80176a1, 0x91158d00, 0x88928e8e, 0xfb0a4192, 0x8934f9af, 0x858f8391, 0x078b1988,
	0x83878985, 0x0a41858d, 0x35fdb1f7, 0x908c8605, 0x0a3d9088, 0x8c8b8e8c, 0x0e1d2c8c, 0x91f85bfb, 0xd40156f7, 0xcd06f7cd, 0xf9eff703, 0x0a2e1544,
	0x2e1648fb, 0x56fb0e0a, 0x198292f8, 0x1984e220, 0xa1f8c635, 0x86938115, 0x99a49492, 0x9d1e9d9d, 0xc3a9949d, 0x6107a41a, 0x342b07bd, 0x0e0afc1d,
	0xa6f80afc, 0x82d80a8e, 0x843f2045, 0x25108345, 0xe20a8ea7, 0x3c8303cd, 0xeb0a2d3f, 0x7078f78a, 0xa694f7a6, 0x1274f770, 0x2cf7a013, 0x1d5d15a7,
	0x6f0677f7, 0x130540fb, 0x2e0a82a0, 0x8b1d7313, 0x0bfb1f7e, 0x94f7b406, 0x850af705, 0x4e02240d, 0x8277fb1d, 0x06143103, 0x7e8a807e, 0x988b967e,
	0x060cf71f, 0x0594fb62, 0x12832882, 0x60131f22, 0x1e221583, 0x498403f7, 0x5bf7cb23, 0x203f8415, 0x31248577, 0x230e0677, 0x1d7beef8, 0xeef81df7,
	0x24fb1d35, 0x0b8291f7, 0x91f7092e, 0xf9231d35, 0xf7017764, 0x204af98d, 0x67200a82, 0xa3230a82, 0x8625eaf8, 0xf8643a0a, 0x800a23ea, 0xf78b0a4c,
	0xa4f31273, 0x1d29e813, 0xb2f964f7, 0xd8131d28, 0x22168324, 0x848cf772, 0x1d362b16, 0xb2f920f7, 0xd8130a26, 0x2d821d23, 0x7770f726, 0x03a4f301,
	0x4b2a2c82, 0x1d2012fa, 0x1d4d94fb, 0x588287f7, 0x03a4552d, 0x0a3655f7, 0x25a5f9ca, 0x86defb1d, 0x82142014, 0x46142714, 0x03f7651d, 0x14840a23,
	0x01778435, 0xed03a4ed, 0xfa960a36, 0xf71d2005, 0xf976a04e, 0x581d5fb6, 0x480809f8, 0x13f960fc, 0x070ffd05, 0x0a818684, 0xf91e9291, 0x078e0731,
	0x8e8f8d8f, 0x8f921e8e, 0x868f8a92, 0x16fd62f8, 0x070af918, 0x901d8192, 0xfd1e8485, 0x0a6a0731, 0x908e8686, 0x1f0a7989, 0xadf9e9fb, 0x1d590a22,
	0x1269f78b, 0x2cae8278, 0x01f81d24, 0x1d2f55f8, 0xf7711d59, 0x20118483, 0x39175992, 0x829df721, 0x2d5c824a, 0x017766f7, 0x24030a78, 0xf8cef71d,
	0x5b8220b5, 0x1d5fe722, 0x5f300f85, 0x0a2259f8, 0x87f70a64, 0xa8f70a32, 0x1d2599f8, 0x772f0b83, 0xf8a4e801, 0xe803a592, 0x69f70a37, 0x832303f7,
	0x82842020, 0xf8892220, 0x35fe82f9, 0xfa76a004, 0x1d39772f, 0x235cf87c, 0x0a536d0a, 0x127ef771, 0xba8291ca, 0x2195f828, 0xb8f8be1d, 0x148f1d2f,
	0x14825a20, 0xf7272983, 0xa61d2d5f, 0x832016f9, 0xe7260820, 0xa1af8db0, 0x03fb1d2d, 0x0a22c1f8, 0x8b0a7090, 0xc51264f7, 0xf76a13a4, 0x130a28bc,
	0xbc1d3c96, 0x1d2501f7, 0x5c831d66, 0xe8251682, 0xf7570a2b, 0x05ce4101, 0xf71d6628, 0xc501775f, 0x2a8303a4, 0x891d3c29, 0x1d205ff7, 0x821d8f94,
	0x4ce920d2, 0x640805c2, 0x0a909864, 0x2dd73dec, 0x2a3f3c2d, 0x881d621e, 0x8989888a, 0x8889881e, 0x891b8789, 0x83068906, 0x8c8a8b1d, 0x0a738a08,
	0x6e8c8a1f, 0x1d831a0a, 0x8d1a8d8d, 0x930a6207, 0x0746f707, 0x928f9193, 0x83879092, 0xc807311e, 0xd5b4ccad, 0xe400f71b, 0x8002fb33, 0x831d610a,
	0x1e929187, 0xcdf8b1fb, 0x45af0a22, 0x2caf831d, 0x13a41d7f, 0xf70a25d8, 0x2fdcf795, 0x2313821d, 0x127df772, 0xc5261384, 0x1d2789f7, 0x16822ff7,
	0xf7282a83, 0xf70a305f, 0x203af860, 0xf7842383, 0xe90a3033, 0x0a22e5f7, 0xf9a48b94, 0xf71d3235, 0x250bf86b, 0x4a0c841d, 0x1f242cf9, 0x23eb2cf7,
	0x30204484, 0x4c244482, 0x1d2069f8, 0x2f06ce49, 0x017733f9, 0xe9f7a4bf, 0x0a3403a4, 0x4cf71d3b, 0xb8322682, 0x87f71d41, 0xb8131277, 0x1d31c8f8,
	0x2524f7af, 0x1187b51d, 0x131d7d30, 0x131d4278, 0xf71d47b8, 0x23daf81e, 0x1882c70a, 0x2a848420, 0x1d31c927, 0x2084f791, 0x0811831d, 0x8db00e4e,
	0x1277a1af, 0x03fb1d30, 0x0a2228f7, 0xe704f79b, 0xbb6cf72f, 0xe72f6df7, 0xe3601312, 0x7e15dcf7, 0x98989680, 0x1f989696, 0x980650f8, 0x7e7e8096,
	0x1f7e8080, 0x6ff796fb, 0x63901315, 0x0419fc1d, 0xfb0e1d63, 0xf80a5ef4, 0x9cfc15a9, 0x0805e549, 0xf81d78f7, 0x8592079c, 0x861d4390, 0xf71d2c84,
	0x0773042e, 0x0a601d98, 0x1d2c9191, 0x8b0a44a3, 0x8b450e07, 0xa30bf8a3, 0x0177a9f7, 0xf7a377f7, 0xf703a442, 0x15b5f877, 0x890717fb, 0x98078a07,
	0xdff1920a, 0x37232536, 0x4f452537, 0x1e6ac4b0, 0x8d849087, 0x87858886, 0x868f8489, 0xcfaf4b08, 0xf71bda61, 0xf7eaeb07, 0x3104f709, 0x9101fbe8,
	0x7c0ef71f, 0x0423f71d, 0x1d860774, 0x989207a2, 0x8686841d, 0xf70e1e84, 0xf976a004, 0xf701cf7b, 0x05f7cf58, 0x58f703cf, 0x26159df9, 0x1649f71d,
	0xfd421d26, 0xddf71590, 0xf789fb07, 0x8c9086e5, 0x198f9293, 0x8a938f90, 0x82f7868f, 0xf718dbfb, 0x61dbf781, 0x87908c0a, 0x8d879119, 0xfb868783,
	0x18e5fb8b, 0x6a07ddfb, 0x9185840a, 0xe10e1e92, 0xf77638f8, 0x77a077c0, 0xf714f712, 0xf7b01348, 0x1528f814, 0x8e898d88, 0x3401828d, 0xeeb2f71e,
	0x8d8d8f07, 0x8d898e8e, 0x66fb1f87, 0x89898806, 0x261c8388, 0xf806f01f, 0x84b2fb30, 0x828e2028, 0xf71e311f, 0x898e07b6, 0x131e888e, 0x8b8b8ad0,
	0x8a1f8a8a, 0x1b300082, 0x35fb07fb, 0x35f709fb, 0x1d838a05, 0x8cb0131b, 0x1b201a83, 0x88254082, 0x07b6fb1f, 0x85538288, 0xa691083a, 0xfb03f707,
	0x0a73052d, 0x068d1b8d, 0x1f0a678d, 0x2df702f7, 0x2efb0e05, 0xf77645f8, 0xd50177ba, 0xa192f7a1, 0xf873f703, 0xc9e21530, 0x0a84d9cf, 0x3cca45de,
	0x3252453c, 0xcc3b0a88, 0x9e1edf49, 0xc8554204, 0xcd0a84cd, 0xd4d4c9c1, 0x88494dc1, 0x4e55490a, 0xfb0e1e42, 0xf7fcf750, 0x00f79436, 0x7782c994,
	0x94f4ad12, 0xf79209f7, 0xf7ee1302, 0x15fcf764, 0xeadbd8ec, 0xea078c1f, 0x2a29da3f, 0x1e2c3b3e, 0xd72c078a, 0x941eed3c, 0xd5452d04, 0x291882e6,
	0xd2e6de13, 0xd1e9e8d6, 0x1a823041, 0x44302308, 0xc61e2e40, 0x928715cb, 0x1aa09588, 0x8297849a, 0x94831e92, 0x8d848f7f, 0x888c8608, 0x4e1a8b8c,
	0xd782062b, 0x8989892b, 0xf71e8d8e, 0xee13076e, 0x0800828d, 0x8c8b1e7d, 0xad06cd07, 0x6e686fa7, 0x83707379, 0x908a8f1f, 0x08898e89, 0x7b9b8399,
	0x8d771a73, 0x1e868d82, 0x8b8d8e87, 0x8d0a3d8c, 0x8c898a8d, 0x888a8a1f, 0x8a8a1e89, 0x86878a8b, 0x1e88918d, 0x1d50ed52, 0xa9a3a4a9, 0x6da272aa,
	0x00fb4d1f, 0x76f70e06, 0x2af7a77f, 0xb3bff7b3, 0x01a71ff7, 0x2ef7a9c0, 0xa966f8b7, 0x7f32f803, 0xf75ff715, 0xf739f734, 0xf70a8459, 0xf732fb59,
	0x825ffb37, 0xfb343901, 0x8859fb39, 0xf759fb0a, 0xf737fb32, 0x04a71e5f, 0x22fb51fb, 0x4bf729f7, 0x4b2e2682, 0x2bf723f7, 0x51f752f7, 0x29fb22f7,
	0x26824bfb, 0xfb4b7208, 0xfb2bfb23, 0xf7901e52, 0xb0c7152a, 0x1fb0aba1, 0x918e8e8f, 0x82961a91, 0x86858094, 0x1e888988, 0x7b6d6c6f, 0x4d3e1b5e,
	0x0a84dccf, 0xdacec6dc, 0x7478abb5, 0x90871ea6, 0x1b918890, 0x97949597, 0x86928792, 0x6ba71f8f, 0x1b50a165, 0x25343b24, 0x2407891f, 0x1ef236da,
	0xeff8230e, 0x19f701cf, 0xf7030a7f, 0x2111f919, 0xe17ed70a, 0xf7e1dd01, 0x29028311, 0xa9dd7014, 0xf70a4615, 0x04861667, 0x56fb0e2e, 0x0156f722,
	0x06f7cddd, 0x31c103cd, 0x67123948, 0x7d080742, 0xf2f71d34, 0x72f7a48b, 0xa556f7a3, 0xf8a4be01, 0xf903a431, 0x157df870, 0x324d3729, 0x20f71f67,
	0x5b840a5b, 0x0a871e1d, 0xc33d65da, 0xfb0a5a30, 0x3a1bfb03, 0x1d2be61d, 0xaf071ef7, 0xed4cdf34, 0x0a35088a, 0x2c8c09fb, 0x1a0bf7ee, 0x5547f88e,
	0xfc56fc1d, 0x05fb1564, 0x0ef7ed2e, 0x1d5a0ef7, 0x83e806f7, 0x2e0efb0a, 0x1f06fb29, 0x8af783f7, 0xf71d7915, 0x0a631b00, 0x0e0639fc, 0x798657f8,
	0x778ca32b, 0xa81d7f12, 0x75f9ec13, 0x314e8c16, 0x518f1d64, 0x4231210a, 0x131f6d27, 0x326eefdc, 0xe45423d5, 0xe4f33705, 0x1fa8efd5, 0x41e528a9,
	0xfd088af5, 0x130a8a22, 0x490ef7ec, 0x6a82e91d, 0x282d0d24, 0x7f8205fb, 0x1e0a7a27, 0x6ef722f9, 0x846a8415, 0x6d0e2874, 0xa1db0a53, 0x82a107f7,
	0xf76f08e7, 0x06f7a312, 0xa410f7a3, 0xf8aff703, 0xadba15ce, 0x6d1fb6b0, 0x67af5f1d, 0x04a11eb8, 0xa8a7736a, 0x6e0a6d1f, 0x1e6a6f73, 0x20256ef7,
	0x4df80e0a, 0x45f7a88b, 0xf7abf3a6, 0xf801a7c3, 0xb503a989, 0x8e8c159e, 0x89997683, 0x938f8998, 0x0cf78e8d, 0xf7185af7, 0x0648fbcd, 0x997f977d,
	0x064ef81e, 0x988d9698, 0x7e8c8098, 0xf74afc1f, 0x8218f8c8, 0x828e200f, 0x828e200f, 0xf718270f, 0x7345f8c3, 0x0a828c1d, 0x06852c08, 0x8185847e,
	0x21fc1f85, 0x868b24fd, 0xf7088789, 0x156af737, 0x4cf89cf7, 0x4cfcab05, 0x6faf0e06, 0xf9a0f70a, 0xfb0a3af9, 0x5f47fd9e, 0x2026ac5d, 0x48f71d72,
	0xb98218f9, 0x86fc5422, 0x3ea8df52, 0xa4ed0a85, 0x1523ed03, 0x997f977d, 0x067cf71e, 0x958c9395, 0x818e8395, 0xf97dfb1f, 0x827df7a2, 0x828b200f,
	0x828f200f, 0x7c29080f, 0x7f7f7d06, 0x850e1f7d, 0xa4c6f70a, 0xf9dff703, 0x7f991536, 0xfb1e7d97, 0x8381067c, 0x93818187, 0xf71f958b, 0xfba2fd7d,
	0x200f827d, 0x200f8288, 0x080f828a, 0x99067c36, 0x1f999797, 0xf710fb0e, 0xf701b1a0, 0xf803a861, 0x151ffb2a, 0x89958996, 0x85931a97, 0x1e8d858f,
	0x67b125fb, 0xd61aeebb, 0xac66e407, 0xca1ea148, 0xe4afb49e, 0x35080d82, 0xf7bbafee, 0x911eb125, 0x938e918d, 0x8a81971a, 0xfb1e8980, 0x4b6b693d,
	0x073c1a24, 0x245a743d, 0x8083837b, 0x9b839380, 0x3d5aa2f2, 0x24073c1e, 0x3df74bab, 0x68871e69, 0x03a88a23, 0x086782d5, 0xad3df742, 0x1af2cbab,
	0xa2d907da, 0x939bf2bc, 0x83969693, 0x74247b93, 0xda1ed9bc, 0xcb6bf207, 0x1ead3dfb, 0x8c818d80, 0x91831a7f, 0x1e899188, 0xaf6525f7, 0x401a285b,
	0x67b43207, 0x481e78ca, 0x326a6675, 0x42080d82, 0xfb5b6728, 0x851e6525, 0x83878589, 0x8d957f1a, 0x0e1e8d96, 0xb9766fa7, 0xa22bf9a2, 0xe00177bc,
	0xa15cf7a2, 0x03a276f7, 0x4ff7d7f8, 0xb56ec115, 0x5e1ea95e, 0x459e4fa8, 0xc8f70899, 0xc888db07, 0x6167c264, 0xdd6105df, 0x554a0805, 0x34b648af,
	0x07a6088e, 0x7f958b97, 0x7f818d7f, 0xfb07701e, 0x4127850e, 0x59082b8a, 0x6fb564a6, 0xc171b21e, 0x087fc878, 0x3007e4fb, 0x57bb3791, 0x848f86b5,
	0x8787878a, 0x90838c86, 0xe35fc187, 0x0885ee58, 0x897f0773, 0x41829781, 0xa31e972f, 0x8d1bf707, 0x8cdd05f7, 0x30fc08f5, 0x0566628a, 0xb41a4e08,
	0xa7b1b1a2, 0xbca5af1e, 0x088ec19c, 0x5207c4fb, 0x689c5a97, 0xd6f708a3, 0x6215fefb, 0x4c78546d, 0xe0f7088a, 0xc47ecd07, 0x0870b279, 0x69a26fb4,
	0x725e1a5e, 0x1e6c6162, 0xfbd2fb0e, 0x3afa7603, 0x0df70177, 0x0df703a8, 0x7f1502fb, 0x2869828c, 0x1e97958f, 0x97070efa, 0x20b38287, 0x08b3828a,
	0x70f70eb6, 0x58f97698, 0x1277a777, 0x98f7a3c2, 0xf7a2eda3, 0xbe13a29a, 0xf2f764f7, 0xfb570a4e, 0x848b15e2, 0x8d909a7e, 0x1e8c8c8f, 0x2af97bf8,
	0x9895918f, 0x13198f8a, 0x928a8bde, 0x8b848986, 0xfc1e8684, 0x052bfd7b, 0x8a7e828b, 0x735ef81a, 0x28fc0a4e, 0x461506f8, 0x84ecd14d, 0xd6c3e50a,
	0x44c9d0d4, 0x2e0a882b, 0x1e424353, 0xf9fb2af8, 0xd14b4715, 0xe40a84eb, 0xcfd3d6c5, 0x882c44cb, 0x43512f0a, 0xb30e1e43, 0xbaf7a384, 0xa2ecf7a4,
	0x07f7cd12, 0xf013a472, 0x84158acd, 0x1e928c91, 0x980676f8, 0x98988a96, 0x1f7e8a80, 0x058c10fc, 0xc4f7b9f7, 0x89969807, 0x0805a144, 0xf7c4fb40,
	0xa5cb0615, 0x1eacaccf, 0xa1c2a7a7, 0xb9da1bbb, 0x1fb35d6f, 0x78959086, 0x92961b94, 0x88929986, 0x1f908792, 0xb4515fbf, 0x574e1b29, 0x1f666676,
	0x6161e813, 0x1a404b74, 0x074914fb, 0x184ef013, 0x988d2105, 0x43081682, 0x06b9fbcd, 0x1305823d, 0x878a83f0, 0x0e1a8489, 0xf776a0dd, 0x10f7a845,
	0x16f712a6, 0xa4725af7, 0x135bf772, 0x98c3f7f0, 0xf71d4815, 0x0733f738, 0x9293e413, 0x84939390, 0x131f8393, 0xf733fbf0, 0x0633f710, 0x21081382,
	0x84930a6c, 0xfb1f8392, 0x80f70628, 0x1d38d6f7, 0xd6fb7ef7, 0x0627fb18, 0x8582f013, 0x91858384, 0x6b829485, 0xfb32f725, 0x8432fb10, 0x83832413,
	0x86869183, 0x383b0813, 0x1d700e06, 0x1d5099f0, 0x8c8a8e89, 0x89900a2a, 0x8f8e8c90, 0x07f9f3f8, 0x908f1d88, 0x8f86948a, 0x90860a41, 0x86878a82,
	0x06fdf0fc, 0x85861d88, 0x8790838b, 0xe3f76f08, 0x66fb1d50, 0xaf2f3697, 0x1d7f1d69, 0x80db03a4, 0x8d8c1d50, 0x823d8c89, 0x8c912774, 0x3af88f8d,
	0x748462f8, 0x74849320, 0x8982912b, 0x37fc8788, 0x1d8861fc, 0x08748385, 0x75088687, 0x1d2794f7, 0x8b64f70e, 0x9fd2f7a4, 0xf7011d96, 0x53f8a439,
	0x39f703a4, 0xf71e0a5f, 0xfb0a5d1a, 0x0a4d061a, 0x27d8fb1f, 0x97878507, 0x91888f0a, 0xa406ef1f, 0xf715d2fb, 0x9107f2d2, 0x91918e8e, 0x1f859088,
	0xf7ccf724, 0x1d51060e, 0x7637f79b, 0x017738f8, 0x9cf894f8, 0x8e909015, 0x981a9291, 0x847e9680, 0x86868885, 0xfb3bfb1e, 0xf73afb3d, 0x8690053c,
	0x1b838f84, 0x7d80807d, 0x0a988f83, 0xfb3cf71f, 0x0819823a, 0x86053b3c, 0x84858886, 0x80967e1a, 0x8e919298, 0xf71e9090, 0xf73df73b, 0x053cfb3a,
	0x87929086, 0x96991b93, 0x87939996, 0x1f908692, 0x3af73cfb, 0xb9fb0e05, 0xd70176ce, 0x8a1556f8, 0x84828b84, 0xfb21f73c, 0x21fb1848, 0x800546fb,
	0x9c788782, 0x8d93931b, 0xf71f9092, 0x0546f71d, 0x89839492, 0x88924108, 0x1e948491, 0x47f71dfb, 0x8a749286, 0x0e197f8a, 0x77f1f723, 0x41f7e801,
	0x8e8a9215, 0xf790918b, 0x1821f748, 0x21fb46f7, 0x9e829605, 0x931a9c87, 0x90849389, 0xf746fb1e, 0x8292051d, 0x1b848e85, 0x842dc782, 0x47fb1f82,
	0x86841dfb, 0x8a97748c, 0x21838519, 0x84837bf7, 0x8c74972b, 0x1dfb8486, 0x841847fb, 0x27c78382, 0x92858e84, 0x1df71e82, 0x84219082, 0x244a8290,
	0x9e879c1b, 0x083d8296, 0x46f72170, 0x48f721f7, 0x8e8b9190, 0x0e19928a, 0xf71d8c3f, 0xfb0a7cec, 0xf71d6eec, 0xf81d8cba, 0xfc0a7c41, 0x741d6e41,
	0x767ca48b, 0x12a46ff8, 0x7013a4c9, 0x15812cf7, 0x8b8e8c8a, 0x90901b8c, 0x9e1f908e, 0xb01305b0, 0x7eb6b076, 0x1d3e1bba, 0x9f966460, 0x5bf71f69,
	0x839f1cf8, 0x7e9d809e, 0x780a5419, 0x74977799, 0x18b29f94, 0x9386918d, 0x01548c85, 0x775c0805, 0x75911864, 0x1b738f74, 0x05fb1efb, 0xb43b0a4f,
	0x1f5eca43, 0x84886276, 0x8991848f, 0xf835f719, 0x9e9f156e, 0x1f9e8688, 0x17fc56fb, 0x68b35405, 0xf71ad2cb, 0xf7eeef0d, 0x320e1e12, 0x0dfb230e,
	0x772cf776, 0x95bbf701, 0x1d561d4f, 0x22fb3bf7, 0xa5713cf7, 0x12a526f9, 0x7013a5d4, 0x2667506c, 0xeefb12f8, 0x826a1d4a, 0x3b30097c, 0x4cf8a472,
	0xa4c512a4, 0x0a317013, 0x7ffbaaf7, 0x899b1d4a, 0xf97679b0, 0xe912a85c, 0xabf7f7ab, 0xbc13af94, 0x1599b2f7, 0x9687957f, 0x9024f71e, 0xf78dd5f4,
	0x01f78d04, 0x22c107fb, 0xa7d9089d, 0x1aedcbea, 0x3cec078d, 0x8f0cfbd5, 0x8f18fb1e, 0x1ffb2533, 0x0774fc1a, 0x8a7d7c13, 0x90999981, 0xf81e9995,
	0x09f70774, 0xe3ece6e5, 0x313f45db, 0x6d355532, 0x8287801e, 0x7d1a7e83, 0x899a8395, 0xf37df61e, 0x131a305e, 0x58252abc, 0x1e8506fb, 0x83828a80,
	0x290e1a7f, 0x1ef8769f, 0x77b1f7a6, 0x158ae901, 0xdeafaac7, 0xf7bd1f9c, 0x24f705a7, 0x8c969706, 0x8d809797, 0x1cfb1f7f, 0x22f7a406, 0xc0b4d298,
	0x8d1982c8, 0x6983f07c, 0x8995971b, 0x8e849797, 0x901f8e7e, 0x748c7b77, 0x625f3b1b, 0x711f7828, 0x290525fb, 0x8a807f06, 0x8a967f7e, 0x06e51f97,
	0x81a4fb5a, 0x576e7b53, 0x89781986, 0x1a7c8d7b, 0xa587977e, 0x41c60e1e, 0xcf0ff71d, 0x7f5af712, 0xfb1d300a, 0x214bf711, 0x0a4ccd0a, 0xf301cff2,
	0xf706f7a4, 0x1d290307, 0x21d9f9c4, 0x08055f52, 0x01cf0f28, 0x030a7f83, 0xfb0a36f4, 0x21ccf905, 0xe81d590a, 0xa6d401cf, 0x07f794f7, 0x03a795f7,
	0x51f71d24, 0x0a217cf8, 0x51820a64, 0xa4e80124, 0x178259f7, 0x0a275a39, 0xc0f82ff7, 0xfb7b0a21, 0xf78ba45e, 0xf8a48b47, 0xbf01cfb3, 0x8203f7a4,
	0x0786081c, 0xf7bf03a4, 0xabf7155a, 0x91930a94, 0x1e848591, 0xd82d1d62, 0xd5e6ea3e, 0x1e8fe6d4, 0x8d07b2f7, 0x8c0a8207, 0x0a6e1e8d, 0x8e8e1e8c,
	0x8d8d8c05, 0x8d1b8e8c, 0x8a8c8c06, 0x888e1b8c, 0x8a8b8c05, 0x898d1f8b, 0x1a88888c, 0xfb0755fc, 0xfb232512, 0x94696714, 0x841e6d9a, 0x8f92898e,
	0x93918d92, 0x0889908d, 0x83a8a77d, 0x06f71bac, 0x05f7e7e6, 0x4f07d51f, 0x42634b6b, 0x3301fb1b, 0x1f00f7e3, 0x212af8d0, 0x9382940a, 0x01cfb524,
	0x9183a4de, 0xa505f724, 0x9387de03, 0x2b761e20, 0x83d42029, 0x45af2949, 0x01cfdb1d, 0x28f7a4c5, 0x2a3a4a82, 0x0a2503a4, 0x21fbf7dc, 0xf81d800a,
	0x8501cfce, 0xf6030a7f, 0x05fb0a3c, 0x1283e3f8, 0x7749f93d, 0x03a4f501, 0x960a3cf5, 0x1d2022f9, 0x4ef91d80, 0xa4ea0177, 0x0a50ea03, 0x8223ea65,
	0x080f8733, 0xca0a3c24, 0x1d25c4f8, 0xdb0a7077, 0xa4c812cf, 0x07f72df7, 0xbff76f13, 0x97130a28, 0x13fb1d3c, 0x0a2120f7, 0x7675a1b5, 0xda9b0806,
	0x78f712a1, 0xa306f7a3, 0xd4f87c13, 0x86fb159e, 0xad0519f9, 0xaeaaa195, 0x661d6d1a, 0x82ad6ca4, 0xfd85fb1e, 0x190a6918, 0x0a45bc13, 0x0547fbce,
	0x87908e86, 0x7c131b90, 0xfb081d57, 0x1564f9d8, 0x7b730a6d, 0x1e847374, 0x8a858689, 0x87861b85, 0x1f868c8c, 0xa3799172, 0xfc201aa4, 0x39f715a0,
	0x0a684af8, 0x0a536d0e, 0xca01cfe4, 0x0a7fdca4, 0xf803a4dc, 0xfb1d2195, 0x21e0f811, 0x7edbf90a, 0xf7a472b1, 0xe5f7a4cd, 0x1277a977, 0x10f9a6d4,
	0xfab613a4, 0x1d3316f5, 0x770609fc, 0xfb35050d, 0xae13076e, 0xfb5b21f7, 0x29fbef11, 0xfb51fb1b, 0x8a0a662b, 0x06b7461f, 0x1b51f73f, 0x11f729f7,
	0xbb21f7ef, 0x076bfb1f, 0x8f847613, 0xfd1e9385, 0x15f1f71c, 0x1d530a58, 0x094d6d1e, 0x1d4c0838, 0xf88b8b0e, 0x5ef78b86, 0x6afb068c, 0xe6f9078b,
	0x15f0f814, 0xc07a13a0, 0x36b50805, 0x57004a00, 0x77005b00, 0xc800b700, 0x4a010301, 0x65016101, 0xab019801, 0xdf01ae01, 0xee01e501, 0x26021d02,
	0x75022c02, 0xa5028f02, 0xf902e002, 0x39030003, 0x67034d03, 0x6f036b03, 0x87037803, 0x92038b03, 0xa4039a03, 0xc303b103, 0xd603cf03, 0xe303db03,
	0xf203ea03, 0x0d04f603, 0x21041204, 0x31042b04, 0x43043704, 0x4c044804, 0x5d045404, 0x68046304, 0x70046c04, 0x8b048304, 0x95048f04, 0x9d049904,
	0xaa04a504, 0xb504b004, 0xc204b904, 0xcd04c904, 0xd504d104, 0xe704e304, 0x0105f404, 0x16050e05, 0x1f051b05, 0x27052305, 0x39053305, 0x42053e05,
	0x05be7a05, 0x05645408, 0x056d0569, 0x05750571, 0x057d0579, 0x058e0587, 0x05960592, 0x059e059a, 0x05b005a7, 0x05bf05b9, 0x05c805c4, 0x05d205cd,
	0x05dc05d7, 0x05e405e0, 0x15ec05e8, 0x871f1d5b, 0x3d65da0a, 0x0a5a31c3, 0x1afb03fb, 0x2be51d3a, 0x5b64f81d, 0xfb1e840a, 0x4e65fc6f, 0x0d3307d0,
	0x0dfb1d40, 0x292e0efb, 0x0b1f05fb, 0x9a990a3e, 0x7a7d9e9e, 0x42080614, 0x26164af7, 0x1d220e1d, 0x95889181, 0x91918e95, 0x260e1e8e, 0x971d230a,
	0x7f918383, 0x818f781b, 0x881f8886, 0x1d549286, 0x96183ed2, 0x958c8e7f, 0x740e0887, 0xf21bf71d, 0x0a5702f7, 0x078b1d6a, 0x82231afb, 0x14200878,
	0x14fb0a2a, 0xf703f724, 0xa41d2c1a, 0xe80dfb16, 0x2a05f728, 0xe905f70a, 0x1d2c0a7a, 0xed2d0ef7, 0x2006487a, 0x080e8283, 0x95150b38, 0x968a8e8f,
	0x18d8d297, 0x88921d87, 0x7e0b0890, 0x942af80a, 0x9092930a, 0x22fb0a48, 0x04fb06f7, 0x1e8921f7, 0xf78d20f7, 0x8c04f706, 0xf80822f7, 0x1d81072a,
	0x1d829291, 0xfb304d08, 0xfb11fb13, 0xfb2ffb2e, 0xf711f711, 0x160b1e30, 0x228d17fb, 0xf78901f7, 0x1cf7081a, 0x01f7f48d, 0xf71b17f7, 0xfb28f011,
	0x8a1f9413, 0x8a078907, 0x978a8507, 0x48fc1b0a, 0x0cfb0688, 0x0af728ea, 0xccd51e8a, 0x1fb0c7b3, 0x8b92918f, 0x41087c82, 0x87838d88, 0x1e0b0885,
	0x97770776, 0x1e879a80, 0x757d688e, 0x86087767, 0x86868888, 0x8b1e0b1a, 0xbcf70b07, 0xfb0a3516, 0xee2c8c0a, 0x8e1a0cf7, 0x1d5548f8, 0xfb2217fb,
	0x891cfb01, 0x1afb8d1f, 0x7182fbf4, 0xf808892f, 0x0a630464, 0x79063afc, 0x1b01f71d, 0x11f0580b, 0x2e0ba608, 0x83950e1d, 0x72828490, 0x1e79797d,
	0x6d827979, 0x07721a53, 0x9d7c9a79, 0x9d9a9a9d, 0x9f07a01e, 0x8f7c967f, 0x99ae881e, 0x089fafa1, 0x908e8e90, 0x260b1a90, 0x23b8130a, 0x7f01771d,
	0x2503a41d, 0x1d740b0a, 0x05f71afb, 0x1ff702fb, 0x1d3e1e8a, 0x7a2713fb, 0xef0df70a, 0xbf13f7ee, 0xb26d79bc, 0x080a541f, 0x9e5460ad, 0x1ffb1b51,
	0x1d6a05fb, 0xe801770b, 0x2792f8a4, 0x9a780b0a, 0x0b9e9e7c, 0x155af7bf, 0x33e300fb, 0xcbd401f7, 0x1eabc7b3, 0x05fb0741, 0x06fb2f30, 0x99936e6a,
	0x8d861e6f, 0x85898983, 0x05eb6787, 0xa97ca508, 0x1baf82ad, 0xf3f114f7, 0xf81f12f7, 0x8a8e0755, 0x1e8d898e, 0x1e8a0a6e, 0x0b058e88, 0xccb9d4dd,
	0x918f1fb5, 0x8e869389, 0x8b848e84, 0x4f088587, 0x41634a66, 0x15980b1b, 0x930736f9, 0x0a609090, 0x0a760a8f, 0x43868583, 0x0b1d781d, 0xfb15adf7,
	0xfb11f730, 0xf72ff711, 0xf713f72e, 0x1e30f711, 0x91841d67, 0x48858485, 0x22fb8a0a, 0x04fb06fb, 0x088920fb, 0xfb8d21fb, 0xf704f706, 0x2af81a22,
	0x4d841d95, 0x890b1e0a, 0x1b8a8a8a, 0x908f8686, 0x2bfc1f88, 0xf71561f7, 0x39fb06de, 0x0b054af8, 0x61920a49, 0x4a081f82, 0x81869015, 0x831b8198,
	0x87868e85, 0x868f888c, 0x0533d31f, 0x84929480, 0x06931b97, 0x96929297, 0xe3d31f94, 0x8c908f05, 0x901a8f8e, 0x80828887, 0x85857e81, 0x0540451e,
	0x4815960b, 0xfb0547f7, 0xfb4806f2, 0x8a860547, 0x83868786, 0x65f821bd, 0x3308bd82, 0x9191938f, 0xfc1e8387, 0x1d820765, 0x91868385, 0x1b0b1e92,
	0x150b078b, 0xf70b0a33, 0xf70a3d51, 0x0b2bf751, 0x99780770, 0x999e9e7d, 0xa61e9e99, 0x080b1d5e, 0x5f261f82, 0x1d87910a, 0xd5820b1e, 0x621d2c34,
	0x93070b1d, 0x1d439085, 0x0b1f8386, 0x8c901d58, 0x6d82ce1f, 0xf2f73908, 0x9e730b06, 0x9ea3a378, 0x78a3a39e, 0x7873739e, 0x0b1e7378, 0xcde201e8,
	0x75f8e203, 0x0b0a4015, 0xfc1e8485, 0x830b072a, 0x0b938690, 0x92901d84, 0x0b0a9392, 0x4d851d91, 0x2908e982, 0xcdf7a48b, 0x0b1d96a4, 0x0b0a5683,
	0xe0c9e815, 0xe70a84e9, 0x2e2fe04e, 0x882d364d, 0x37c82e0a, 0xfb0b1ee7, 0x0b1bfb02, 0xc4830a42, 0x828f8521, 0x08c482b9, 0xf7070b3b, 0xee268213,
	0x0b1b11fb, 0x918b1d93, 0xa48b0b1d, 0x0ba54bf8, 0x8b928791, 0x918f908f, 0x0b1d978b, 0x0a3bc9f8, 0x8485860b, 0x2a14f70b, 0xf314f70a, 0xf74ef70b,
	0xf729f722, 0x372b8240, 0x0b7737f9, 0x2414fb1b, 0x1d7a070b, 0x9291900b, 0x56f7060b, 0x31f730f7, 0x54210582, 0x080782fb, 0x1f56fb4a, 0x011d4d0b,
	0xe803a4e8, 0x930a650b, 0x0a2a930b, 0x900b9191, 0xf70b9390, 0xf60b07ab, 0x01fb38e5, 0xf70b1f98, 0x0b0a5930, 0x90841598, 0x35fb0b85, 0x8c0b5cfb,
	0xf70b0a82, 0x054afc39, 0x0b06defb, 0x848e8488, 0x840b8891, 0x9224d682, 0x920b1d6f, 0x8c3d3f82, 0xa7a3a807, 0x6fa3acac, 0x078a1e6e, 0x8c8b8b0b,
	0xf9a28c0b, 0xe401a22b, 0xa254f8a2, 0x32968203, 0x8b73f78b, 0x54f78ba3, 0xb30ba58b, 0x90861f62, 0x82868b83, 0x8b4d0800, 0xa176a00b, 0x40131276,
	0x8c8c8a0b, 0x2c930b8a, 0x1a870b1d, 0x36fd0b86, 0xfbc30b07, 0x6ff77655, 0xa446f8a5, 0xa6d40b01, 0x0ba708f9, 0x0a6e8a1e, 0x0df7ee0b, 0x0a4d060b,
	0x2afda41f, 0x99060b15, 0x99998e97, 0x1f7d917f, 0xf9a48b0b, 0x0829821e, 0xe803a547, 0x0b15adf7, 0xcf07f7ce, 0x1d621f0b, 0x840a890b, 0x8d8c8c0b,
	0x0efb290b, 0x078d1f0b, 0xfb42fb0b, 0xa2f9a316, 0x6f0b01a3, 0x1d61841d, 0x0efb0b1e, 0x891e0b07, 0x84850b07, 0x89f70b85, 0x1e830b15, 0x068beafb,
	0x26f68282, 0xf9a82a42, 0x8201a87a, 0xcbf72ea1, 0xa4e6f7a5, 0x56f7760b, 0x860b0177, 0x08a48283, 0x0a621527, 0x30f8a40b, 0x2bfb0ba4, 0x860b35fb,
	0x0b1f8491, 0x91909207, 0x858e830b, 0x8e8e8c0b, 0x8585860b, 0x8690840b, 0xe6fa050b, 0x008fd323,
};
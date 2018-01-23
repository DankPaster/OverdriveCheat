#pragma once
#include <limits>
#include <sstream>
#include "VMT.h"

typedef unsigned short ModelInstanceHandle_t;

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

class IClientRenderable
{
public:
	virtual void function(void) = 0;
	virtual Vector const&              GetRenderOrigin(void) = 0;
	virtual void function1(void) = 0;
	virtual bool                       ShouldDraw(void) = 0;
	virtual int                        GetRenderFlags(void) = 0; // ERENDERFLAGS_xxx
	virtual void                       Unused(void) const {}
	virtual void function2(void) = 0;
	virtual void function3(void) = 0;
	virtual const model_t*             GetModel() const = 0;
	virtual int                        DrawModel(int flags, const int /*RenderableInstance_t*/ &instance) = 0;
	virtual int                        GetBody() = 0;
	virtual void                       GetColorModulation(float* color) = 0;
	virtual bool                       LODTest() = 0;
	virtual bool                       SetupBones(matrix3x4_t *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime) = 0;
	virtual void                       SetupWeights(const matrix3x4_t *pBoneToWorld, int nFlexWeightCount, float *pFlexWeights, float *pFlexDelayedWeights) = 0;
	virtual void                       DoAnimationEvents(void) = 0;
	virtual void* /*IPVSNotify*/       GetPVSNotifyInterface() = 0;
	virtual void                       GetRenderBounds(Vector& mins, Vector& maxs) = 0;
	virtual void                       GetRenderBoundsWorldspace(Vector& mins, Vector& maxs) = 0;
	virtual void                       GetShadowRenderBounds(Vector &mins, Vector &maxs, int /*ShadowType_t*/ shadowType) = 0;
	virtual bool                       ShouldReceiveProjectedTextures(int flags) = 0;
	virtual bool                       GetShadowCastDistance(float *pDist, int /*ShadowType_t*/ shadowType) const = 0;
	virtual bool                       GetShadowCastDirection(Vector *pDirection, int /*ShadowType_t*/ shadowType) const = 0;
	virtual bool                       IsShadowDirty() = 0;
	virtual void                       MarkShadowDirty(bool bDirty) = 0;
	virtual IClientRenderable*         GetShadowParent() = 0;
	virtual IClientRenderable*         FirstShadowChild() = 0;
	virtual IClientRenderable*         NextShadowPeer() = 0;
	virtual int /*ShadowType_t*/       ShadowCastType() = 0;
	virtual void                       CreateModelInstance() = 0;
	virtual ModelInstanceHandle_t      GetModelInstance() = 0;
	virtual const matrix3x4_t&         RenderableToWorldTransform() = 0;
	virtual int                        LookupAttachment(const char *pAttachmentName) = 0;
	virtual void function4(void) = 0;
	virtual bool                       GetAttachment(int number, matrix3x4_t &matrix) = 0;
	virtual float*                     GetRenderClipPlane(void) = 0;
	virtual int                        GetSkin() = 0;
	virtual void                       OnThreadedDrawSetup() = 0;
	virtual bool                       UsesFlexDelayedWeights() = 0;
	virtual void                       RecordToolMessage() = 0;
	virtual bool                       ShouldDrawForSplitScreenUser(int nSlot) = 0;
	virtual uint8_t                      OverrideAlphaModulation(uint8_t nAlpha) = 0;
	virtual uint8_t                      OverrideShadowAlphaModulation(uint8_t nAlpha) = 0;
};

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

struct studiohwdata_t;

typedef void* StudioDecalHandle_t;
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

class IMatRenderContext;

enum PreviewImageRetVal_t
{
	MATERIAL_PREVIEW_IMAGE_BAD = 0,
	MATERIAL_PREVIEW_IMAGE_OK,
	MATERIAL_NO_PREVIEW_IMAGE,
};

enum ImageFormat 
{
	IMAGE_FORMAT_UNKNOWN = -1,
	IMAGE_FORMAT_RGBA8888 = 0,
	IMAGE_FORMAT_ABGR8888,
	IMAGE_FORMAT_RGB888,
	IMAGE_FORMAT_BGR888,
	IMAGE_FORMAT_RGB565,
	IMAGE_FORMAT_I8,
	IMAGE_FORMAT_IA88,
	IMAGE_FORMAT_P8,
	IMAGE_FORMAT_A8,
	IMAGE_FORMAT_RGB888_BLUESCREEN,
	IMAGE_FORMAT_BGR888_BLUESCREEN,
	IMAGE_FORMAT_ARGB8888,
	IMAGE_FORMAT_BGRA8888,
	IMAGE_FORMAT_DXT1,
	IMAGE_FORMAT_DXT3,
	IMAGE_FORMAT_DXT5,
	IMAGE_FORMAT_BGRX8888,
	IMAGE_FORMAT_BGR565,
	IMAGE_FORMAT_BGRX5551,
	IMAGE_FORMAT_BGRA4444,
	IMAGE_FORMAT_DXT1_ONEBITALPHA,
	IMAGE_FORMAT_BGRA5551,
	IMAGE_FORMAT_UV88,
	IMAGE_FORMAT_UVWQ8888,
	IMAGE_FORMAT_RGBA16161616F,
	IMAGE_FORMAT_RGBA16161616,
	IMAGE_FORMAT_UVLX8888,
	IMAGE_FORMAT_R32F,            // Single-channel 32-bit floating point
	IMAGE_FORMAT_RGB323232F,    // NOTE: D3D9 does not have this format
	IMAGE_FORMAT_RGBA32323232F,
	IMAGE_FORMAT_RG1616F,
	IMAGE_FORMAT_RG3232F,
	IMAGE_FORMAT_RGBX8888,

	IMAGE_FORMAT_NULL,            // Dummy format which takes no video memory

								  // Compressed normal map formats
								  IMAGE_FORMAT_ATI2N,            // One-surface ATI2N / DXN format
								  IMAGE_FORMAT_ATI1N,            // Two-surface ATI1N format

								  IMAGE_FORMAT_RGBA1010102,    // 10 bit-per component render targets
								  IMAGE_FORMAT_BGRA1010102,
								  IMAGE_FORMAT_R16F,            // 16 bit FP format

																// Depth-stencil texture formats
																IMAGE_FORMAT_D16,
																IMAGE_FORMAT_D15S1,
																IMAGE_FORMAT_D32,
																IMAGE_FORMAT_D24S8,
																IMAGE_FORMAT_LINEAR_D24S8,
																IMAGE_FORMAT_D24X8,
																IMAGE_FORMAT_D24X4S4,
																IMAGE_FORMAT_D24FS8,
																IMAGE_FORMAT_D16_SHADOW,    // Specific formats for shadow mapping
																IMAGE_FORMAT_D24X8_SHADOW,    // Specific formats for shadow mapping

																							  // supporting these specific formats as non-tiled for procedural cpu access (360-specific)
																							  IMAGE_FORMAT_LINEAR_BGRX8888,
																							  IMAGE_FORMAT_LINEAR_RGBA8888,
																							  IMAGE_FORMAT_LINEAR_ABGR8888,
																							  IMAGE_FORMAT_LINEAR_ARGB8888,
																							  IMAGE_FORMAT_LINEAR_BGRA8888,
																							  IMAGE_FORMAT_LINEAR_RGB888,
																							  IMAGE_FORMAT_LINEAR_BGR888,
																							  IMAGE_FORMAT_LINEAR_BGRX5551,
																							  IMAGE_FORMAT_LINEAR_I8,
																							  IMAGE_FORMAT_LINEAR_RGBA16161616,

																							  IMAGE_FORMAT_LE_BGRX8888,
																							  IMAGE_FORMAT_LE_BGRA8888,

																							  NUM_IMAGE_FORMATS
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
	MATERIAL_VAR_MULTIPASS = (1 << 9),
	MATERIAL_VAR_ZNEARER = (1 << 10),
	MATERIAL_VAR_MODEL = (1 << 11),
	MATERIAL_VAR_FLAT = (1 << 12),
	MATERIAL_VAR_NOCULL = (1 << 13),
	MATERIAL_VAR_NOFOG = (1 << 14),
	MATERIAL_VAR_IGNOREZ = (1 << 15),
	MATERIAL_VAR_DECAL = (1 << 16),
	MATERIAL_VAR_ENVMAPSPHERE = (1 << 17),
	MATERIAL_VAR_NOALPHAMOD = (1 << 18),
	MATERIAL_VAR_ENVMAPCAMERASPACE = (1 << 19),
	MATERIAL_VAR_BASEALPHAENVMAPMASK = (1 << 20),
	MATERIAL_VAR_TRANSLUCENT = (1 << 21),
	MATERIAL_VAR_NORMALMAPALPHAENVMAPMASK = (1 << 22),
	MATERIAL_VAR_NEEDS_SOFTWARE_SKINNING = (1 << 23),
	MATERIAL_VAR_OPAQUETEXTURE = (1 << 24),
	MATERIAL_VAR_ENVMAPMODE = (1 << 25),
	MATERIAL_VAR_SUPPRESS_DECALS = (1 << 26),
	MATERIAL_VAR_HALFLAMBERT = (1 << 27),
	MATERIAL_VAR_WIREFRAME = (1 << 28),
	MATERIAL_VAR_ALLOWALPHATOCOVERAGE = (1 << 29),
};

enum MaterialPropertyTypes_t
{
	MATERIAL_PROPERTY_NEEDS_LIGHTMAP = 0,					// bool
	MATERIAL_PROPERTY_OPACITY,								// int (enum MaterialPropertyOpacityTypes_t)
	MATERIAL_PROPERTY_REFLECTIVITY,							// vec3_t
	MATERIAL_PROPERTY_NEEDS_BUMPED_LIGHTMAPS				// bool
};

class IMaterial
{
public:
	virtual const char *    GetName() const = 0;
	virtual const char *    GetTextureGroupName() const = 0;

	virtual PreviewImageRetVal_t GetPreviewImageProperties(int *width, int *height, ImageFormat *imageFormat, bool* isTranslucent) const = 0;

	virtual PreviewImageRetVal_t GetPreviewImage(unsigned char *data, int width, int height, ImageFormat imageFormat) const = 0;

	virtual int             GetMappingWidth() = 0;
	virtual int             GetMappingHeight() = 0;

	virtual int             GetNumAnimationFrames() = 0;

	virtual bool            InMaterialPage(void) = 0;
	virtual void            GetMaterialOffset(float *pOffset) = 0;
	virtual void            GetMaterialScale(float *pScale) = 0;
	virtual IMaterial       *GetMaterialPage(void) = 0;

	virtual void function(void) = 0;

	virtual void            IncrementReferenceCount(void) = 0;
	virtual void            DecrementReferenceCount(void) = 0;

	inline void AddRef() { IncrementReferenceCount(); }
	inline void Release() { DecrementReferenceCount(); }

	virtual int             GetEnumerationID(void) const = 0;

	virtual void            GetLowResColorSample(float s, float t, float *color) const = 0;

	virtual void            RecomputeStateSnapshots() = 0;

	virtual bool            IsTranslucent() = 0;

	virtual bool            IsAlphaTested() = 0;

	virtual bool            IsVertexLit() = 0;

	virtual void function1(void) = 0;

	virtual bool            HasProxy(void) const = 0;

	virtual bool            UsesEnvCubemap(void) = 0;

	virtual bool            NeedsTangentSpace(void) = 0;

	virtual bool            NeedsPowerOfTwoFrameBufferTexture(bool bCheckSpecificToThisFrame = true) = 0;
	virtual bool            NeedsFullFrameBufferTexture(bool bCheckSpecificToThisFrame = true) = 0;

	virtual bool            NeedsSoftwareSkinning(void) = 0;

	virtual void            AlphaModulate(float alpha) = 0;
	virtual void            ColorModulate(float r, float g, float b) = 0;

	virtual void            SetMaterialVarFlag(MaterialVarFlags_t flag, bool on) = 0;
	virtual bool            GetMaterialVarFlag(MaterialVarFlags_t flag) = 0;

	virtual void            GetReflectivity(Vector& reflect) = 0;

	virtual bool            GetPropertyFlag(MaterialPropertyTypes_t type) = 0;

	virtual bool            IsTwoSided() = 0;

	virtual void            SetShader(const char *pShaderName) = 0;

	virtual int             GetNumPasses(void) = 0;

	virtual int             GetTextureMemoryBytes(void) = 0;

	virtual void            Refresh() = 0;

	virtual bool            NeedsLightmapBlendAlpha(void) = 0;

	virtual bool            NeedsSoftwareLighting(void) = 0;

	virtual int             ShaderParamCount() const = 0;
	virtual void function2(void) = 0;

	virtual bool            IsErrorMaterial() const = 0;

	virtual void            Unused() = 0;

	virtual float           GetAlphaModulation() = 0;
	virtual void            GetColorModulation(float *r, float *g, float *b) = 0;

	virtual bool            IsTranslucentUnderModulation(float fAlphaModulation = 1.0f) const = 0;

	virtual void function3(void) = 0;

	virtual void            SetShaderAndParams(void *pKeyValues) = 0;
	virtual const char *    GetShaderName() const = 0;

	virtual void            DeleteIfUnreferenced() = 0;

	virtual bool            IsSpriteCard() = 0;

	virtual void            CallBindProxy(void *proxyData) = 0;

	virtual void            RefreshPreservingMaterialVars() = 0;

	virtual bool            WasReloadedFromWhitelist() = 0;

	virtual bool            SetTempExcluded(bool bSet, int nExcludedDimensionLimit) = 0;

	virtual int             GetReferenceCount() const = 0;
};

enum OverrideType_t
{
	OVERRIDE_NORMAL = 0,
	OVERRIDE_BUILD_SHADOWS,
	OVERRIDE_DEPTH_WRITE,
};

class IVModelRender
{
public:
	virtual int                     DrawModel(int flags, IClientRenderable *pRenderable, ModelInstanceHandle_t instance, int entity_index, const model_t *model, Vector const& origin, QAngle const& angles, int skin, int body, int hitboxset, const matrix3x4_t *modelToWorld = NULL, const matrix3x4_t *pLightingOffset = NULL) = 0;
	virtual void                    ForcedMaterialOverride(IMaterial *newMaterial, int nOverrideType = 0, int nOverrides = 0) = 0;
	virtual bool                    IsForcedMaterialOverride(void) = 0;
	virtual void function1(void) = 0;
	virtual void function2(void) = 0;
	virtual void                    DestroyInstance(ModelInstanceHandle_t handle) = 0;
	virtual void function3(void) = 0;
	virtual void function4(void) = 0;
	virtual bool                    ChangeInstance(ModelInstanceHandle_t handle, IClientRenderable *pRenderable) = 0;
	virtual void function5(void) = 0;
	virtual void                    RemoveAllDecals(ModelInstanceHandle_t handle) = 0;
	virtual bool                    ModelHasDecals(ModelInstanceHandle_t handle) = 0;
	virtual void                    RemoveAllDecalsFromAllModels() = 0;
	virtual void function6(void) = 0;
	virtual void function7(void) = 0;
	virtual bool                    RecomputeStaticLighting(ModelInstanceHandle_t handle) = 0;
	virtual void                    ReleaseAllStaticPropColorData(void) = 0;
	virtual void                    RestoreAllStaticPropColorData(void) = 0;
	virtual int                     DrawModelEx(ModelRenderInfo_t &pInfo) = 0;
	virtual int                     DrawModelExStaticProp(ModelRenderInfo_t &pInfo) = 0;
	virtual bool                    DrawModelSetup(ModelRenderInfo_t &pInfo, DrawModelState_t *pState, matrix3x4_t **ppBoneToWorldOut) = 0;
	virtual void                    DrawModelExecute(IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld = NULL) = 0;
	virtual void                    SetupLighting(const Vector &vecCenter) = 0;
	virtual void function8(void) = 0;
	virtual void                    SuppressEngineLighting(bool bSuppress) = 0;
	virtual void                    SetupColorMeshes(int nTotalVerts) = 0;
	virtual void                    SetupLightingEx(const Vector &vecCenter, ModelInstanceHandle_t handle) = 0;
	virtual bool                    GetBrightestShadowingLightSource(const Vector &vecCenter, Vector& lightPos, Vector& lightBrightness, bool bAllowNonTaggedLights) = 0;
	virtual void function9(void) = 0;
	virtual void                    GetModelDecalHandles(StudioDecalHandle_t *pDecals, int nDecalStride, int nCount, const ModelInstanceHandle_t *pHandles) = 0;
	virtual void function10(void) = 0;
	virtual void function11(void) = 0;
};

typedef unsigned short MaterialHandle_t;

class KeyValues
{
public:
	char _pad[0x20];//csgo, for css its a diff size
};

class IMaterialSystem
{
public:
	IMaterial* FindMaterial(char const* pMaterialName, const char *pTextureGroupName, bool complain = true, const char *pComplainPrefix = NULL)
	{
		typedef IMaterial*(__thiscall* oFindMaterial)(PVOID, char const*, char const*, bool, char const*);
		return VMT.getvfunc< oFindMaterial >(this, 84)(this, pMaterialName, pTextureGroupName, complain, pComplainPrefix);
	}

	IMaterial*	CreateMaterial(const char *pMaterialName, KeyValues *pVMTKeyValues)
	{
		typedef IMaterial* (__thiscall* oCreateMaterial)(PVOID, const char *, KeyValues*);
		return VMT.getvfunc<oCreateMaterial>(this, 83)(this, pMaterialName, pVMTKeyValues);
	}

	MaterialHandle_t FirstMaterial()
	{
		typedef MaterialHandle_t(__thiscall* FirstMaterialFn)(void*);
		return VMT.getvfunc<FirstMaterialFn>(this, 86)(this);
	}

	MaterialHandle_t NextMaterial(MaterialHandle_t h)
	{
		typedef MaterialHandle_t(__thiscall* NextMaterialFn)(void*, MaterialHandle_t);
		return VMT.getvfunc<NextMaterialFn>(this, 87)(this, h);
	}

	MaterialHandle_t InvalidMaterial()
	{
		typedef MaterialHandle_t(__thiscall* InvalidMaterialFn)(void*);
		return VMT.getvfunc<InvalidMaterialFn>(this, 88)(this);
	}

	IMaterial* GetMaterial(MaterialHandle_t h)
	{
		typedef IMaterial*(__thiscall* GetMaterialFn)(void*, MaterialHandle_t);
		return VMT.getvfunc<GetMaterialFn>(this, 89)(this, h);
	}
};

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

class ConVar
{
public:
	void SetValue(const char* value)
	{
		typedef void(__thiscall* OriginalFn)(void*, const char*);
		return VMT.getvfunc<OriginalFn>(this, 14)(this, value);
	}
	void SetValue(float value)
	{
		typedef void(__thiscall* OriginalFn)(void*, float);
		return VMT.getvfunc<OriginalFn>(this, 15)(this, value);
	}	
	void SetValue(int value)
	{
		typedef void(__thiscall* OriginalFn)(void*, int);
		return VMT.getvfunc<OriginalFn>(this, 16)(this, value);
	}
	void InternalSetString(const char* str);	
	char* GetName()
	{
		typedef char*(__thiscall* OriginalFn)(void*);
		return VMT.getvfunc<OriginalFn>(this, 5)(this);
	}	
	char* GetDefault()
	{
		return pszDefaultValue;
	}

	float GetFloat(void) const;
	int GetInt(void) const;
	const char* GetString(void) const;

	char pad_0x0000[0x4]; //0x0000
	ConVar* pNext; //0x0004 
	__int32 bRegistered; //0x0008 
	char* pszName; //0x000C 
	char* pszHelpString; //0x0010 
	__int32 nFlags; //0x0014 
	char pad_0x0018[0x4]; //0x0018
	ConVar* pParent; //0x001C 
	char* pszDefaultValue; //0x0020 
	char* strString; //0x0024 
	__int32 StringLength; //0x0028 
	float fValue; //0x002C 
	__int32 nValue; //0x0030 
	__int32 bHasMin; //0x0034 
	float fMinVal; //0x0038 
	__int32 bHasMax; //0x003C 
	float fMaxVal; //0x0040 
	void* fnChangeCallback; //0x0044 

};//Size=0x0048

class ICVar : public IAppSystem
{
public:
	virtual void                       function11() = 0;
	virtual void                       function() = 0;
	virtual void                       function1() = 0;
	virtual void                       function10() = 0;
	virtual const char*                GetCommandLineValue(const char *pVariableName) = 0;
	virtual void                       function2() = 0;
	virtual void                       function3() = 0;
	virtual ConVar*                    FindVar(const char *var_name) = 0; //16
	virtual const ConVar*              FindVar(const char *var_name) const = 0;
	virtual void                       function4() = 0;
	virtual void                       function5() = 0;
	virtual void                       function6() = 0;
	virtual void                       function7() = 0;
	virtual void                       CallGlobalChangeCallbacks(ConVar *var, const char *pOldString, float flOldValue) = 0;
	virtual void                       function8() = 0;
	virtual void                       function9() = 0;
	virtual void                       ConsoleColorPrintf(const uint8_t* clr, const char *pFormat, ...) const = 0;
	virtual void                       ConsolePrintf(const char *pFormat, ...) const = 0;
	virtual void                       ConsoleDPrintf(const char *pFormat, ...) const = 0;
	virtual void                       RevertFlaggedConVars(int nFlag) = 0;
};

#define FCVAR_NONE 0 

#define FCVAR_UNREGISTERED (1<<0)	// If this is set, don't add to linked list, etc.
#define FCVAR_DEVELOPMENTONLY (1<<1)	// Hidden in released products. Flag is removed automatically if ALLOW_DEVELOPMENT_CVARS is defined.
#define FCVAR_GAMEDLL (1<<2)	// defined by the game DLL
#define FCVAR_CLIENTDLL (1<<3)  // defined by the client DLL
#define FCVAR_HIDDEN (1<<4)	// Hidden. Doesn't appear in find or autocomplete. Like DEVELOPMENTONLY, but can't be compiled out.

#define FCVAR_PROTECTED (1<<5)  // It's a server cvar, but we don't send the data since it's a password, etc.  Sends 1 if it's not bland/zero, 0 otherwise as value
#define FCVAR_SPONLY (1<<6)  // This cvar cannot be changed by clients connected to a multiplayer server.
#define	FCVAR_ARCHIVE (1<<7)	// set to cause it to be saved to vars.rc
#define	FCVAR_NOTIFY (1<<8)	// notifies players when changed
#define	FCVAR_USERINFO (1<<9)	// changes the client's info string
#define FCVAR_CHEAT (1<<14) // Only useable in singleplayer / debug / multiplayer & sv_cheats

#define FCVAR_PRINTABLEONLY (1<<10)  // This cvar's string cannot contain unprintable characters ( e.g., used for player name etc ).
#define FCVAR_UNLOGGED (1<<11)  // If this is a FCVAR_SERVER, don't log changes to the log file / console if we are creating a log
#define FCVAR_NEVER_AS_STRING (1<<12)  // never try to print that cvar

#define FCVAR_REPLICATED (1<<13)	// server setting enforced on clients, TODO rename to FCAR_SERVER at some time
#define FCVAR_DEMO (1<<16)  // record this cvar when starting a demo file
#define FCVAR_DONTRECORD (1<<17)  // don't record these command in demofiles
#define FCVAR_RELOAD_MATERIALS (1<<20)	// If this cvar changes, it forces a material reload
#define FCVAR_RELOAD_TEXTURES (1<<21)	// If this cvar changes, if forces a texture reload

#define FCVAR_NOT_CONNECTED (1<<22)	// cvar cannot be changed by a client that is connected to a server
#define FCVAR_MATERIAL_SYSTEM_THREAD (1<<23)	// Indicates this cvar is read from the material system thread
#define FCVAR_ARCHIVE_XBOX (1<<24) // cvar written to config.cfg on the Xbox

#define FCVAR_ACCESSIBLE_FROM_THREADS	(1<<25)	// used as a debugging tool necessary to check material system thread convars

#define FCVAR_SERVER_CAN_EXECUTE (1<<28)// the server is allowed to execute this command on clients via ClientCommand/NET_StringCmd/CBaseClientState::ProcessStringCmd.
#define FCVAR_SERVER_CANNOT_QUERY (1<<29)// If this is set, then the server is not allowed to query this cvar's value (via IServerPluginHelpers::StartQueryCvarValue).
#define FCVAR_CLIENTCMD_CAN_EXECUTE	(1<<30)	// IVEngineClient::ClientCmd is allowed to execute this command. 

#define FCVAR_MATERIAL_THREAD_MASK ( FCVAR_RELOAD_MATERIALS | FCVAR_RELOAD_TEXTURES | FCVAR_MATERIAL_SYSTEM_THREAD )	
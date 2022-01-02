
#include <math.h>

#define M_PI                       3.14159265358979323846f

#define URotationToRadians(URotation)		((URotation)* (M_PI / 32768.0f))
#define URotationToDegree( URotation )		( ( URotation ) * ( 360.0f / 65536.0f ) ) 

#define DegreeToURotation( Degree )			( ( Degree ) * ( 65536.0f / 360.0f ) )
#define DegreeToRadian( Degree )			( ( Degree ) * ( M_PI / 180.0f ) )

#define RadianToURotation( URotation )		( ( URotation ) * ( 32768.0f / M_PI ) ) 
#define RadianToDegree( Radian )			( ( Radian ) * ( 180.0f / M_PI ) )

#define RAD2DEG( x  )  ( (float)(x) * (float)(180.f / M_PI_F) )

#define SilverWhite2       D3DCOLOR_RGBA(220, 220, 220, 255)
#define ASilverWhite2       D3DCOLOR_RGBA(220, 220, 220, 50)

#define SilverWhite       D3DCOLOR_RGBA(200, 200, 200, 255)
#define ASilverWhite       D3DCOLOR_RGBA(200, 200, 200, 50)
#define orange2     D3DCOLOR_ARGB(255, 255, 130, 000)

#define Gold      D3DCOLOR_ARGB(255, 255, 200, 000)
#define AGold      D3DCOLOR_ARGB(50, 255, 200, 000)

#define aRed       D3DCOLOR_ARGB(30, 150, 150, 150/*80, 255, 000, 000*/)
#define Red       D3DCOLOR_ARGB(255, 255, 30, 30)
#define ARed       D3DCOLOR_ARGB(50, 255, 30, 30)

#define Green     D3DCOLOR_ARGB(255, 0, 240, 0)
#define Orange    D3DCOLOR_ARGB(255, 255, 73, 000)
#define hOrange    D3DCOLOR_ARGB(100, 255, 73, 000)
#define Blue      D3DCOLOR_ARGB(255, 000, 000, 255)
#define Yellow    D3DCOLOR_ARGB(255, 255, 255,  51)
#define AYellow    D3DCOLOR_ARGB(50, 255, 255,  51)
#define TBlack2     D3DCOLOR_ARGB(100, 000, 000, 000)
#define Black     D3DCOLOR_ARGB(255, 030, 030, 030)
#define Grey      D3DCOLOR_ARGB(255, 112, 112, 112)

#define Pink      D3DCOLOR_ARGB(255, 255, 192, 203)
#define Purple    D3DCOLOR_ARGB(255, 128, 000, 128)
#define White     D3DCOLOR_ARGB(255, 255, 255, 249)
#define Cyan      D3DCOLOR_ARGB(255, 000, 255, 255)
#define ACyan      D3DCOLOR_ARGB(50, 000, 255, 255)

#define Cyan2      D3DCOLOR_ARGB(255, 000, 220, 255)
#define ACyan2      D3DCOLOR_ARGB(50, 000, 220, 255)
#define Magenta   D3DCOLOR_ARGB(255, 255, 000, 255)
#define TBlack    D3DCOLOR_ARGB(200, 000, 000, 000)
int Width = GetSystemMetrics(SM_CXSCREEN);
int Height = GetSystemMetrics(SM_CYSCREEN);
float ScreenCenterX;
float ScreenCenterY;

namespace Offsets
{
    uintptr_t oLevel = 0x38;//ok
    uintptr_t oGameInstance = 0x1A8;//ok
    uintptr_t oLocalPlayers = 0x40;
    uintptr_t oPlayerController = 0x38;//ok
    uintptr_t oLocalPawn = 0x440;//ok
    uintptr_t RootComp = 0x210;//ok
    uintptr_t oCameraManager = 0x458;//ok
    uintptr_t oCameraCache = 0x1220;//ok
    uintptr_t ObjID = 0x18;

    uintptr_t Dormant = 0xD8;
    uintptr_t RelativeLocation = 0x164;//ok
    uintptr_t RelativeRotation = 0x170;//ok
    uintptr_t PlayerState = 0x3D0;//ok
    uintptr_t Ping = 0x3B8;
    uintptr_t TeamComponent = 0x580;//ok
    uintptr_t Team = 0xF8;//ok
    uintptr_t DamageHandler = 0x928;//ok
    uintptr_t Health = 0x1B0;
     uintptr_t Mesh = 0x410;//ok
    uintptr_t StaticMesh = 0x558; //0x518;
    uintptr_t StaticMesh_Cached = 0x568;//0x528;
    uintptr_t ComponentToWorld = 0x250;//ok
    uintptr_t UniqueID = 0x38;
    uintptr_t ControlRotation = 0x420;//ok
    uintptr_t SpikeTimer = 0x4B4;
    uintptr_t PlayerName = 0x3A8;
};

class Vector3
{
public:
	Vector3() : x(0.f), y(0.f), z(0.f)
	{

	}

	Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z)
	{

	}
	~Vector3()
	{

	}

	float x;
	float y;
	float z;
	//Vector3 toVector3;

    __forceinline float Dot(const Vector3& vOther) const {
        const Vector3& a = *this;
        return (a.x * vOther.x + a.y * vOther.y + a.z * vOther.z);
    }
	inline float Distance(Vector3 v)
	{

		return float(sqrtf(powf(v.x - x, 2.0) + powf(v.y - y, 2.0) + powf(v.z - z, 2.0)));
	}

	__forceinline Vector3 operator-(Vector3 V) {

		return Vector3(x - V.x, y - V.y, z - V.z);
	}

	__forceinline Vector3 operator+(Vector3 V) {

		return Vector3(x + V.x, y + V.y, z + V.z);
	}

	__forceinline Vector3 operator*(float Scale) {

		return Vector3(x * Scale, y * Scale, z * Scale);
	}

	__forceinline Vector3 operator/(float Scale) {
		float RScale = 1.f / Scale;
		return Vector3(x * RScale, y * RScale, z * RScale);
	}

	__forceinline Vector3 operator+(float A) {

		return Vector3(x + A, y + A, z + A);
	}

	__forceinline Vector3 operator-(float A) {

		return Vector3(x - A, y - A, z - A);
	}

	__forceinline Vector3 operator*(Vector3 V) {

		return Vector3(x * V.x, y * V.y, z * V.z);
	}

	__forceinline Vector3 operator/(Vector3 V) {

		return Vector3(x / V.x, y / V.y, z / V.z);
	}

	__forceinline float operator|(Vector3 V) {

		return x * V.x + y * V.y + z * V.z;
	}

	__forceinline float operator^(Vector3 V) {

		return x * V.y - y * V.x - z * V.z;
	}

	__forceinline Vector3 operator+=(Vector3 v) {

		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	__forceinline Vector3 operator-=(Vector3 v) {

		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	__forceinline Vector3 operator*=(Vector3 v) {

		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}

	__forceinline Vector3 operator/=(Vector3 v) {

		x /= v.x;
		y /= v.y;
		z /= v.z;
		return *this;
	}

	__forceinline bool operator==(Vector3 src) {

		return (src.x == x) && (src.y == y) && (src.z == z);
	}

	__forceinline bool operator!=(Vector3 src) {

		return (src.x != x) || (src.y != y) || (src.z != z);
	}

	__forceinline float Size() {

		return sqrt(x * x + y * y + z * z);
	}

	float Size2D() const
	{
		return sqrt(x * x + y * y);
	}

	__forceinline float SizeSquared() {

		return x * x + y * y + z * z;
	}

	__forceinline float SizeSquared2D() {

		return x * x + y * y;
	}
};

struct alignas(16) FQuat
{
    float                                              X;                                                        // 0x0000(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
    float                                              Y;                                                        // 0x0004(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
    float                                              Z;                                                        // 0x0008(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
    float                                              W;                                                        // 0x000C(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
};

struct alignas(16) FPlane : public Vector3
{
    float                                              W;                                                        // 0x000C(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
};

struct FMatrix
{
    struct FPlane                                      XPlane;                                                   // 0x0000(0x0010) (Edit, BlueprintVisible, SaveGame, IsPlainOldData)
    struct FPlane                                      YPlane;                                                   // 0x0010(0x0010) (Edit, BlueprintVisible, SaveGame, IsPlainOldData)
    struct FPlane                                      ZPlane;                                                   // 0x0020(0x0010) (Edit, BlueprintVisible, SaveGame, IsPlainOldData)
    struct FPlane                                      WPlane;                                                   // 0x0030(0x0010) (Edit, BlueprintVisible, SaveGame, IsPlainOldData)

    FMatrix operator*(const FMatrix& pM2)
    {
        FMatrix pOut;
        pOut.XPlane.x = XPlane.x * pM2.XPlane.x + XPlane.y * pM2.YPlane.x + XPlane.z * pM2.ZPlane.x + XPlane.W * pM2.WPlane.x;
        pOut.XPlane.y = XPlane.x * pM2.XPlane.y + XPlane.y * pM2.YPlane.y + XPlane.z * pM2.ZPlane.y + XPlane.W * pM2.WPlane.y;
        pOut.XPlane.z = XPlane.x * pM2.XPlane.z + XPlane.y * pM2.YPlane.z + XPlane.z * pM2.ZPlane.z + XPlane.W * pM2.WPlane.z;
        pOut.XPlane.W = XPlane.x * pM2.XPlane.W + XPlane.y * pM2.YPlane.W + XPlane.z * pM2.ZPlane.W + XPlane.W * pM2.WPlane.W;
        pOut.YPlane.x = YPlane.x * pM2.XPlane.x + YPlane.y * pM2.YPlane.x + YPlane.z * pM2.ZPlane.x + YPlane.W * pM2.WPlane.x;
        pOut.YPlane.y = YPlane.x * pM2.XPlane.y + YPlane.y * pM2.YPlane.y + YPlane.z * pM2.ZPlane.y + YPlane.W * pM2.WPlane.y;
        pOut.YPlane.z = YPlane.x * pM2.XPlane.z + YPlane.y * pM2.YPlane.z + YPlane.z * pM2.ZPlane.z + YPlane.W * pM2.WPlane.z;
        pOut.YPlane.W = YPlane.x * pM2.XPlane.W + YPlane.y * pM2.YPlane.W + YPlane.z * pM2.ZPlane.W + YPlane.W * pM2.WPlane.W;
        pOut.ZPlane.x = ZPlane.x * pM2.XPlane.x + ZPlane.y * pM2.YPlane.x + ZPlane.z * pM2.ZPlane.x + ZPlane.W * pM2.WPlane.x;
        pOut.ZPlane.y = ZPlane.x * pM2.XPlane.y + ZPlane.y * pM2.YPlane.y + ZPlane.z * pM2.ZPlane.y + ZPlane.W * pM2.WPlane.y;
        pOut.ZPlane.z = ZPlane.x * pM2.XPlane.z + ZPlane.y * pM2.YPlane.z + ZPlane.z * pM2.ZPlane.z + ZPlane.W * pM2.WPlane.z;
        pOut.ZPlane.W = ZPlane.x * pM2.XPlane.W + ZPlane.y * pM2.YPlane.W + ZPlane.z * pM2.ZPlane.W + ZPlane.W * pM2.WPlane.W;
        pOut.WPlane.x = WPlane.x * pM2.XPlane.x + WPlane.y * pM2.YPlane.x + WPlane.z * pM2.ZPlane.x + WPlane.W * pM2.WPlane.x;
        pOut.WPlane.y = WPlane.x * pM2.XPlane.y + WPlane.y * pM2.YPlane.y + WPlane.z * pM2.ZPlane.y + WPlane.W * pM2.WPlane.y;
        pOut.WPlane.z = WPlane.x * pM2.XPlane.z + WPlane.y * pM2.YPlane.z + WPlane.z * pM2.ZPlane.z + WPlane.W * pM2.WPlane.z;
        pOut.WPlane.W = WPlane.x * pM2.XPlane.W + WPlane.y * pM2.YPlane.W + WPlane.z * pM2.ZPlane.W + WPlane.W * pM2.WPlane.W;
        return pOut;
    }
};
struct alignas(16) FTransform
{
    struct FQuat                                       Rotation;                                                 // 0x0000(0x0010) (Edit, BlueprintVisible, SaveGame, IsPlainOldData)
    struct Vector3                                     Translation;                                              // 0x0010(0x000C) (Edit, BlueprintVisible, SaveGame, IsPlainOldData)
    unsigned char                                      UnknownData00[0x4];                                       // 0x001C(0x0004) MISSED OFFSET
    struct Vector3                                     Scale3D;                                                  // 0x0020(0x000C) (Edit, BlueprintVisible, SaveGame, IsPlainOldData)
    unsigned char                                      UnknownData01[0x4];                                       // 0x002C(0x0004) MISSED OFFSET

    FMatrix ToMatrixWithScale()
    {
        FMatrix m;
        m.WPlane.x = Translation.x;
        m.WPlane.y = Translation.y;
        m.WPlane.z = Translation.z;

        float x2 = Rotation.X + Rotation.X;
        float y2 = Rotation.Y + Rotation.Y;
        float z2 = Rotation.Z + Rotation.Z;

        float xx2 = Rotation.X * x2;
        float yy2 = Rotation.Y * y2;
        float zz2 = Rotation.Z * z2;
        m.XPlane.x = (1.0f - (yy2 + zz2)) * Scale3D.x;
        m.YPlane.y = (1.0f - (xx2 + zz2)) * Scale3D.x;
        m.ZPlane.z = (1.0f - (xx2 + yy2)) * Scale3D.z;

        float yz2 = Rotation.Y * z2;
        float wx2 = Rotation.W * x2;
        m.ZPlane.y = (yz2 - wx2) * Scale3D.z;
        m.YPlane.z = (yz2 + wx2) * Scale3D.y;

        float xy2 = Rotation.X * y2;
        float wz2 = Rotation.W * z2;
        m.YPlane.x = (xy2 - wz2) * Scale3D.y;
        m.XPlane.y = (xy2 + wz2) * Scale3D.x;

        float xz2 = Rotation.X * z2;
        float wy2 = Rotation.W * y2;
        m.ZPlane.x = (xz2 + wy2) * Scale3D.z;
        m.XPlane.z = (xz2 - wy2) * Scale3D.x;

        m.XPlane.W = 0.0f;
        m.YPlane.W = 0.0f;
        m.ZPlane.W = 0.0f;
        m.WPlane.W = 1.0f;

        return m;
    }
};

static FMatrix ToMatrix(Vector3 rot)
{
    Vector3 origin = Vector3{};
    float radPitch = (rot.x * float(M_PI) / 180.f);
    float radYaw = (rot.y * float(M_PI) / 180.f);
    float radRoll = (rot.z * float(M_PI) / 180.f);

    float SP = sinf(radPitch);
    float CP = cosf(radPitch);
    float SY = sinf(radYaw);
    float CY = cosf(radYaw);
    float SR = sinf(radRoll);
    float CR = cosf(radRoll);

    FMatrix matrix;
    matrix.XPlane.x = CP * CY;
    matrix.XPlane.y = CP * SY;
    matrix.XPlane.z = SP;
    matrix.XPlane.W = 0.f;

    matrix.YPlane.x = SR * SP * CY - CR * SY;
    matrix.YPlane.y = SR * SP * SY + CR * CY;
    matrix.YPlane.z = -SR * CP;
    matrix.YPlane.W = 0.f;

    matrix.ZPlane.x = -(CR * SP * CY + SR * SY);
    matrix.ZPlane.y = CY * SR - CR * SP * SY;
    matrix.ZPlane.z = CR * CP;
    matrix.ZPlane.W = 0.f;

    matrix.WPlane.x = origin.x;
    matrix.WPlane.y = origin.y;
    matrix.WPlane.z = origin.z;
    matrix.WPlane.W = 1.f;

    return matrix;
}

typedef struct _ValEntity
{
    uintptr_t Addr;
    uintptr_t Mesh;
    int Type;
   // int ObjectID;

}ValEntity;
vector<ValEntity> entityList;


uintptr_t ULevel;
uintptr_t GameInstance;
uintptr_t LocalPlayer;
uintptr_t AcknowledgedPawn;
uintptr_t LocalPlayerState;
uint64_t PlayerController = 0;
uint64_t CameraCache = 0;

class CameraStruct
{
public:
    Vector3 Position; //0x11B0 
    Vector3 Rotation; //0x11BC 
    float Fov; //0x11C8 
}; //Size=0x11CC
//CameraStruct GetCameraCache()
//{
//    CameraStruct Camera = read<CameraStruct>(CameraCache + Offsets::oCameraCache);
//    return Camera;
//}


template <typename T>
T read(uintptr_t address)
{
    T buffer{ };
    if(ReadProcessMemory(GetCurrentProcess(), (LPVOID)address, &buffer, sizeof(T), 0))   
        return buffer;
}

template<typename T>
bool write(uintptr_t address, T* buffer, SIZE_T sizeh)
{
    if (address > 0x7FFFFFFFFFFF || address < 1) return 0;
    WriteProcessMemory(GetCurrentProcess(), (LPVOID)address, buffer, sizeh, 0);
}

CameraStruct GetCameraCache()
{
    auto Camera = read<CameraStruct>(CameraCache + Offsets::oCameraCache);
    return Camera;
}
D3DMATRIX to_matrix(const Vector3& rotation, const Vector3& origin) {
    const auto pitch = rotation.x * float(3.14159265358979323846f) / 180.f;
    const auto yaw = rotation.y * float(3.14159265358979323846f) / 180.f;
    const auto roll = rotation.z * float(3.14159265358979323846f) / 180.f;

    const auto SP = sinf(pitch);
    const auto CP = cosf(pitch);
    const auto SY = sinf(yaw);
    const auto CY = cosf(yaw);
    const auto SR = sinf(roll);
    const auto CR = cosf(roll);

    D3DMATRIX matrix;
    matrix._11 = CP * CY;
    matrix._12 = CP * SY;
    matrix._13 = SP;
    matrix._14 = 0.f;

    matrix._21 = SR * SP * CY - CR * SY;
    matrix._22 = SR * SP * SY + CR * CY;
    matrix._23 = -SR * CP;
    matrix._24 = 0.f;

    matrix._31 = -(CR * SP * CY + SR * SY);
    matrix._32 = CY * SR - CR * SP * SY;
    matrix._33 = CR * CP;
    matrix._34 = 0.f;

    matrix._41 = origin.x;
    matrix._42 = origin.y;
    matrix._43 = origin.z;
    matrix._44 = 1.f;

    return matrix;
}



__forceinline __int64 decrypt_uworld(const uint32_t key, const uintptr_t* state)
{

    unsigned __int64 v19; // r11
    unsigned __int64 v20; // r8
    unsigned __int64 v21; // r9
    unsigned int v22; // er10
    unsigned __int64 v23; // rcx
    unsigned __int64 v24; // rdx
    unsigned __int64 v25; // rcx
    int v26; // ebx
    unsigned int v27; // ecx
    __int64 v28; // rax
    unsigned __int64 v29; // r8
    unsigned __int64 v30; // r8
    unsigned __int64 v31; // rcx
    unsigned __int64 v32; // rdx
    unsigned __int64 v33; // rcx

    v19 = 2685821657736338717i64
        * ((unsigned int)key ^ (unsigned int)(key << 25) ^ (((unsigned int)key ^ ((unsigned __int64)(unsigned int)key >> 15)) >> 12))
        % 7;
    v20 = state[v19];
    v21 = (2685821657736338717i64
        * ((unsigned int)key ^ (unsigned int)(key << 25) ^ (((unsigned int)key ^ ((unsigned __int64)(unsigned int)key >> 15)) >> 12))) >> 32;
    v22 = (unsigned int)v19 % 7;
    if (!((unsigned int)v19 % 7))
    {
        v23 = (2 * (v20 - (unsigned int)(v21 - 1))) ^ ((2 * (v20 - (unsigned int)(v21 - 1))) ^ ((v20
            - (unsigned int)(v21 - 1)) >> 1)) & 0x5555555555555555i64;
        v24 = (4 * v23) ^ ((4 * v23) ^ (v23 >> 2)) & 0x3333333333333333i64;
        v25 = (16 * v24) ^ ((16 * v24) ^ (v24 >> 4)) & 0xF0F0F0F0F0F0F0Fi64;
        v20 = rol8((v25 << 8) ^ ((v25 << 8) ^ (v25 >> 8)) & 0xFF00FF00FF00FFi64, 32);
    LABEL_26:
        v26 = 2 * v19;
        goto LABEL_27;
    }
    if (v22 != 1)
        goto LABEL_26;
    v26 = 2 * v19;
    v20 = rol8(v20 - (unsigned int)(2 * v19 + v21), (unsigned __int8)(((int)v21 + (int)v19) % 0x3Fu) + 1);
LABEL_27:
    v27 = v26 + v21;
    if (v22 == 2)
        v20 = ~(v20 - v27);
    switch (v22)
    {
    case 3u:
        v28 = 2 * ((2 * v20) ^ ((2 * v20) ^ (v20 >> 1)) & 0x5555555555555555i64);
        v20 = v28 ^ (v28 ^ (((2 * v20) ^ ((2 * v20) ^ (v20 >> 1)) & 0x5555555555555555i64) >> 1)) & 0x5555555555555555i64;
        break;
    case 4u:
        v29 = ror8(v20, (unsigned __int8)(v27 % 0x3F) + 1);
        v20 = (2 * v29) ^ ((2 * v29) ^ (v29 >> 1)) & 0x5555555555555555i64;
        break;
    case 5u:
        v30 = ror8(v20, (unsigned __int8)(v27 % 0x3F) + 1);
        v31 = (2 * v30) ^ ((2 * v30) ^ (v30 >> 1)) & 0x5555555555555555i64;
        v32 = (4 * v31) ^ ((4 * v31) ^ (v31 >> 2)) & 0x3333333333333333i64;
        v33 = (16 * v32) ^ ((16 * v32) ^ (v32 >> 4)) & 0xF0F0F0F0F0F0F0Fi64;
        v20 = rol8((v33 << 8) ^ ((v33 << 8) ^ (v33 >> 8)) & 0xFF00FF00FF00FFi64, 32);
        break;
    case 6u:
        v20 = ~v20 - (unsigned int)(v21 + v19);
        break;
    }
    return v20 ^ (unsigned int)key;
}

uint64_t valBase = (uintptr_t)GetModuleHandleA(0);
uint64_t UWorld = 0;


uint64_t ReadWorld()
{
    __try
    {
       // std::printf("valBase %p \n", valBase);
        uint64_t key = read<uint64_t>(valBase + 0x86DA038);
        //std::printf("key %p \n", key);

#pragma pack(push, 1)
        struct State
        {
            uint64_t Keys[7];
        };
#pragma pack(pop)
        const auto state = read<State>(valBase + 0x86DA000);
       // std::printf("state %p \n", state.Keys);

        if (key != 0)
            return read<uint64_t>(decrypt_uworld(key, (uint64_t*)&state));
    }
    __except (1) {}
}
//
//Vector3 WorldToScreen(Vector3& world_loc) {
//    auto CamCache = GetCameraCache();
//    const auto matrix = to_matrix(CamCache.Rotation, Vector3());
//    const auto axisx = Vector3(matrix.m[0][0], matrix.m[0][1], matrix.m[0][2]);
//    const auto axisy = Vector3(matrix.m[1][0], matrix.m[1][1], matrix.m[1][2]);
//    const auto axisz = Vector3(matrix.m[2][0], matrix.m[2][1], matrix.m[2][2]);
//
//    const auto delta = world_loc - CamCache.Position;
//    auto transformed = Vector3(delta.Dot(axisy), delta.Dot(axisz), delta.Dot(axisx));
//
//    if (transformed.z < 0.001f)
//        transformed.z = 0.001f;
//
//    const auto fov_angle = CamCache.Fov;
//    const float center = Width / 2.f;
//    const float centery = Height / 2.f;
//
//    return Vector3(center + transformed.x * (center / static_cast<float>(tan(fov_angle * M_PI / 360))) / transformed.z, centery - transformed.y * (center / static_cast<float>(tan(fov_angle * M_PI / 360))) / transformed.z, 0.f);
//}


FTransform GetBoneIndex(DWORD_PTR mesh, int index)
{
    DWORD_PTR bonearray = read<DWORD_PTR>(mesh + Offsets::StaticMesh);// 5A0 / 590
    if (!bonearray || bonearray == NULL)
        bonearray = read<DWORD_PTR>(mesh + Offsets::StaticMesh_Cached);
    return read<FTransform>(bonearray + (index * 0x30));
}

Vector3 GetBoneWithRotation(DWORD_PTR mesh, int id)
{
    FTransform Bone = GetBoneIndex(mesh, id);
    FTransform ComponentToWorld = read<FTransform>(mesh + Offsets::ComponentToWorld);
    FMatrix Matrix = Bone.ToMatrixWithScale() * ComponentToWorld.ToMatrixWithScale();
    return Vector3{ Matrix.WPlane.x, Matrix.WPlane.y, Matrix.WPlane.z };
}

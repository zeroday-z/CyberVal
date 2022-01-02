#define _CRT_SECURE_NO_WARNINGS
#include "draw.h"
#include "input.h"
#include "menu.h"
#include "xor.hpp"
#include <time.h>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi")

#include <process.h>
#include <vector>
#include "defs.h"
#include "vectors.h"

#include "sha_imp.h"
#include <algorithm>
HWND MyWnd = NULL;
MSG Message = { NULL };
const MARGINS Margin = { -1 ,-1, -1, -1 };
RECT GameRect = { NULL };
HWND GameWnd = NULL;

string tSelect[] = { xorstr_("Enemy"), xorstr_("All"), xorstr_("Bot") };


string lineStyle[] = { ("Off"), xorstr_("Top"), xorstr_("Middle"),xorstr_("Bottom") };
string boxStyle[] = { ("Off"), xorstr_("Corner"),xorstr_("2D"), xorstr_("3D") };
string onOff[] = { ("Off"), "On" };
string aimBone[] = { xorstr_("Head"),xorstr_("Neck"), xorstr_("Body") };
string aimFov[] = { "5", "10", "15", "20", "25", "30", "45", "60", "90", "120", "180" };
string multiplier[] = { "Off", "x1", "x2", "x3", "x4", "x5", "x6", "x7", "x8", "x9", "x10" };
string aimTime[] = { ("0"), "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20" };
string keys[] = { ("Capslock"), ("Alt"), ("LShift"), ("Fire"), "E", "F", "T", "V", "C", "X", "Right Click" };


//#define Lala(s) LI_FN(OutputDebugStringA)(##s);
struct color {
    int a, r, g, b;
    color() = default;

    color(const int r, const int g, const int b, const int a = 255) : _color{ } {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }

    explicit color(const uint32_t color) : _color{ } {
        this->a = color >> 24 & 0xff;
        this->r = color >> 16 & 0xff;
        this->g = color >> 8 & 0xff;
        this->b = color & 0xff;
    }

    static color from_uint(const uint32_t uint) {
        return color(uint);
    }

    unsigned char& operator[ ](const int index) {
        return _color[index];
    }

    template <typename T>
    color hsv_to_rgb(T h, T s, T v, const float alpha) const {
        int _r, _g, _b;

        if (s != 0) {

            h == 360 ? h = 0 : h = h / 60;
            auto i = static_cast<int>(trunc(h));
            int f = h - i;

            const int p = v * (1 - s);
            const int q = v * (1 - s * f);
            const int t = v * (1 - s * (1 - f));

            switch (i) {
            case 0:
                _r = v;
                _g = t;
                _b = p;
                break;

            case 1:
                _r = q;
                _g = v;
                _b = p;
                break;

            case 2:
                _r = p;
                _g = v;
                _b = t;
                break;

            case 3:
                _r = p;
                _g = q;
                _b = v;
                break;

            case 4:
                _r = t;
                _g = p;
                _b = v;
                break;

            default:
                _r = v;
                _g = p;
                _b = q;
                break;
            }
        }
        else {
            _r = v;
            _g = v;
            _b = v;
        }

        return color(static_cast<uint8_t>(_r * 255), static_cast<uint8_t>(_g * 255),
            static_cast<uint8_t>(_b * 255), alpha);
    }

    unsigned char _color[4];
};
void DrawString(int x, int y, DWORD color, const char* fmt, DWORD TRPE = DT_NOCLIP)
{
    RECT FontPos = { x, y, x + 120, y + 20 };
    draw.font->DrawTextA(NULL, fmt, -1, &FontPos, TRPE, color);
}


void DrawRect(float x, float y, float w, float h, D3DCOLOR color, IDirect3DDevice9Ex* pDevice)
{
    D3DRECT rec = { x, y, x + w, y + h };
    pDevice->Clear(1, &rec, D3DCLEAR_TARGET, color, 0, 0);
}

void DrawBorder(float x, float y, float w, float h, float px, D3DCOLOR BorderColor, IDirect3DDevice9Ex* pDevice)
{
    DrawRect(x, (y + h - px), w, px, BorderColor, pDevice);
    DrawRect(x, y, px, h, BorderColor, pDevice);
    DrawRect(x, y, w, px, BorderColor, pDevice);
    DrawRect((x + w - px), y, px, h, BorderColor, pDevice);
}
void DrawCornerBox(float x, float y, float w, float h, float borderPx, D3DCOLOR color, IDirect3DDevice9Ex* pDev)
{
    // draw.GetDevice
    DrawRect(x + borderPx, y, w / 3, borderPx, color, pDev); //top 
    DrawRect(x + w - w / 3 + borderPx, y, w / 3, borderPx, color, pDev); //top 
    DrawRect(x, y, borderPx, h / 3, color, pDev); //left 
    DrawRect(x, y + h - h / 3 + borderPx * 2, borderPx, h / 3, color, pDev); //left 
    DrawRect(x + borderPx, y + h + borderPx, w / 3, borderPx, color, pDev); //bottom 
    DrawRect(x + w - w / 3 + borderPx, y + h + borderPx, w / 3, borderPx, color, pDev); //bottom 
    DrawRect(x + w + borderPx, y, borderPx, h / 3, color, pDev);//right 
    DrawRect(x + w + borderPx, y + h - h / 3 + borderPx * 2, borderPx, h / 3, color, pDev);//right 
}

void DrawBox(int x, int y, int w, int h, D3DCOLOR BoxColor, D3DCOLOR BorderColor, IDirect3DDevice9Ex* pDevice)
{
    DrawRect(x, y, w, h, BoxColor, pDevice);
    DrawBorder(x, y, w, h, 1, BorderColor, pDevice);
}
void DrawESPBox(Vector3 Head, Vector3 Foot, DWORD dwColor, int recAlpha)
{
    Vector3 Box = Head - Foot;

    if (Box.y < 0)
        Box.y *= -1;

    int BoxWidth = (int)Box.y / 2.4f;
    int DrawX = (int)Head.x - (BoxWidth / 2);
    int DrawY = (int)Head.y;

    DrawBox(DrawX, DrawY , BoxWidth + 1, (int)Box.y + 5, D3DCOLOR_ARGB(10, 10, 0, 0), dwColor, draw.GetDevice());
    // DrawRect(DrawX, DrawY, BoxWidth, (int)Box.y, D3DCOLOR_RGBA(255, 0, 0, 5), draw.GetDevice());
    // DrawBorder(DrawX - 1, DrawY - 1, BoxWidth + 1, (int)Box.y + 1, 1, dwColor, draw.GetDevice());
}

LPD3DXLINE xwwwwwwqq = NULL;

void DrawLine(float StartX, float StartY, float EndX, float EndY, D3DCOLOR dColor, float thicc)
{
    xwwwwwwqq[0].SetWidth(thicc);
    xwwwwwwqq[0].SetGLLines(0);
    xwwwwwwqq[0].SetAntialias(1);
    D3DXVECTOR2 v2Line[2];
    v2Line[0].x = StartX;
    v2Line[0].y = StartY;
    v2Line[1].x = EndX;
    v2Line[1].y = EndY;

    xwwwwwwqq[0].Begin();
    xwwwwwwqq[0].Draw(v2Line, 2, dColor);
    xwwwwwwqq[0].End();
}

//===========================================================================
typedef LONG(WINAPI* RealNtFlushInstructionCache)(HANDLE ProcessHandle, PVOID BaseAddress, ULONG NumberOfBytesToFlush);

RealNtFlushInstructionCache NewNtFlushInstructionCache = NULL;
//===========================================================================
typedef struct _PEB_LDR_DATA {
    UINT8 _PADDING_[12];
    LIST_ENTRY InLoadOrderModuleList;
    LIST_ENTRY InMemoryOrderModuleList;
    LIST_ENTRY InInitializationOrderModuleList;
} PEB_LDR_DATA, * PPEB_LDR_DATA;

typedef struct _PEB {
#ifdef _WIN64
    UINT8 _PADDING_[24];
#else
    UINT8 _PADDING_[12];
#endif
    PEB_LDR_DATA* Ldr;
} PEB, * PPEB;

typedef struct _LDR_DATA_TABLE_ENTRY {
    LIST_ENTRY InLoadOrderLinks;
    LIST_ENTRY InMemoryOrderLinks;
    LIST_ENTRY InInitializationOrderLinks;
    VOID* DllBase;
} LDR_DATA_TABLE_ENTRY, * PLDR_DATA_TABLE_ENTRY;

typedef struct _UNLINKED_MODULE
{
    HMODULE hModule;
    PLIST_ENTRY RealInLoadOrderLinks;
    PLIST_ENTRY RealInMemoryOrderLinks;
    PLIST_ENTRY RealInInitializationOrderLinks;
    PLDR_DATA_TABLE_ENTRY Entry;
} UNLINKED_MODULE;

#define UNLINK(x)					\
	(x).Flink->Blink = (x).Blink;	\
	(x).Blink->Flink = (x).Flink;

#define RELINK(x, real)			\
	(x).Flink->Blink = (real);	\
	(x).Blink->Flink = (real);	\
	(real)->Blink = (x).Blink;	\
	(real)->Flink = (x).Flink;

std::vector<UNLINKED_MODULE> UnlinkedModules;

struct FindModuleHandle
{
    HMODULE m_hModule;
    FindModuleHandle(HMODULE hModule) : m_hModule(hModule)
    {
    }
    bool operator() (UNLINKED_MODULE const& Module) const
    {
        return (Module.hModule == m_hModule);
    }
};

void RelinkModuleToPEB(HMODULE hModule)
{
    std::vector<UNLINKED_MODULE>::iterator it = std::find_if(UnlinkedModules.begin(), UnlinkedModules.end(), FindModuleHandle(hModule));

    if (it == UnlinkedModules.end())
    {
        //DBGOUT(TEXT("Module Not Unlinked Yet!"));
        return;
    }

    RELINK((*it).Entry->InLoadOrderLinks, (*it).RealInLoadOrderLinks);
    RELINK((*it).Entry->InInitializationOrderLinks, (*it).RealInInitializationOrderLinks);
    RELINK((*it).Entry->InMemoryOrderLinks, (*it).RealInMemoryOrderLinks);
    UnlinkedModules.erase(it);
}

void UnlinkModuleFromPEB(HMODULE hModule)
{
    std::vector<UNLINKED_MODULE>::iterator it = std::find_if(UnlinkedModules.begin(), UnlinkedModules.end(), FindModuleHandle(hModule));
    if (it != UnlinkedModules.end())
    {
        //DBGOUT(TEXT("Module Already Unlinked!"));
        return;
    }

#ifdef _WIN64
    PPEB pPEB = (PPEB)__readgsqword(0x60);
#else
    PPEB pPEB = (PPEB)__readfsdword(0x30);
#endif

    PLIST_ENTRY CurrentEntry = pPEB->Ldr->InLoadOrderModuleList.Flink;
    PLDR_DATA_TABLE_ENTRY Current = NULL;

    while (CurrentEntry != &pPEB->Ldr->InLoadOrderModuleList && CurrentEntry != NULL)
    {
        Current = CONTAINING_RECORD(CurrentEntry, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
        if (Current->DllBase == hModule)
        {
            UNLINKED_MODULE CurrentModule = { 0 };
            CurrentModule.hModule = hModule;
            CurrentModule.RealInLoadOrderLinks = Current->InLoadOrderLinks.Blink->Flink;
            CurrentModule.RealInInitializationOrderLinks = Current->InInitializationOrderLinks.Blink->Flink;
            CurrentModule.RealInMemoryOrderLinks = Current->InMemoryOrderLinks.Blink->Flink;
            CurrentModule.Entry = Current;
            UnlinkedModules.push_back(CurrentModule);

            UNLINK(Current->InLoadOrderLinks);
            UNLINK(Current->InInitializationOrderLinks);
            UNLINK(Current->InMemoryOrderLinks);

            break;
        }

        CurrentEntry = CurrentEntry->Flink;
    }
}

bool HideThread(HANDLE hThread)
{
    typedef NTSTATUS(NTAPI* pNtSetInformationThread)
        (HANDLE, UINT, PVOID, ULONG);
    NTSTATUS Status;

    // Get NtSetInformationThread
    pNtSetInformationThread NtSIT = (pNtSetInformationThread)
        GetProcAddress(GetModuleHandle(TEXT("ntdll.dll")),
            "NtSetInformationThread");

    // Shouldn't fail
    if (NtSIT == NULL)
        return false;

    // Set the thread info
    if (hThread == NULL)
        Status = NtSIT(GetCurrentThread(),
            0x11, // HideThreadFromDebugger
            0, 0);
    else
        Status = NtSIT(hThread, 0x11, 0, 0);

    if (Status != 0x00000000)
        return false;
    else
        return true;
}

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
//    if (transformed.Z < 0.001f)
//        transformed.Z = 0.001f;
//
//    const auto fov_angle = CamCache.Fov;
//    const float center = Width / 2.f;
//    const float centery = Height / 2.f;
//
//    return Vector3(center + transformed.X * (center / static_cast<float>(tan(fov_angle * M_PI / 360))) / transformed.Z, centery - transformed.Y * (center / static_cast<float>(tan(fov_angle * M_PI / 360))) / transformed.Z, 0.f);
//}


string GetAbilities(int id)
{
    switch (id)
    {
    case 11942984: return xorstr_("Skye Bird");
        break;
    case 11952969: return xorstr_("YoruTP");
        break;
    case 11946521: return xorstr_("Killjoy Bomb Trap");//ok
        break;
    case 11943389: return xorstr_("Skye Wolf");
        break;
    case 11952667: return xorstr_("YoruFAKE");
        break;
    case 11945559: return xorstr_("Sova Drone");//ok
        break;
    case 11947180: return xorstr_("Killjoy AlarmBot");//ok
        break;
    case 11943639: return xorstr_("Skye Ulti");
        break;
    case 11946788: return xorstr_("Killjoy Turret");//ok
        break;
    case 11940250: return xorstr_("RazeBot");//ok
        break;
    case 11947424: return xorstr_("Killjoy Ulti");//ok
        break;

    default:
        return xorstr_("NULL");
        break;
    }
}

#define Player 0
#define Bot 1
#define Abs 2
#define sSpike 3
#define isMale (ObjID == 11939200 || ObjID == 11950957 || ObjID == 11943761 || ObjID == 11955057 || ObjID == 11948640 || ObjID == 11945003 || ObjID == 11952530)
#define isFemale (ObjID == 11949605 || ObjID == 11955916 || ObjID == 11946209 || ObjID == 11939775 || ObjID == 11954266 || ObjID == 11953399 || ObjID == 11942554 || ObjID == 11947546)
#define isBot (ObjID == 11954219)

string GetCharacterName(int id)
{
    switch (id)
    {
        /*Female's*/

    case 11949605: return xorstr_("AESTRA");///ok
        break;
    case 11955916: return xorstr_("JETT");
        break;
    case 11946209: return xorstr_("KILLJOY");
        break;
    case 11939775: return xorstr_("RAZE");
        break;
    case 11954266: return xorstr_("REYNA");
        break;
    case 11953399: return xorstr_("SAGE");
        break;
    case 11942554: return xorstr_("SKYE");
        break;
    case 11947546: return xorstr_("VIPER");
        break;

        /*Male's*/
    case 11939200: return xorstr_("BREACH");
        break;
    case 11950957: return xorstr_("BRIMSTONE");
        break;
    case 11943761: return xorstr_("CYPHER");
        break;
    case 11955057: return xorstr_("OMEN");
        break;
    case 11948640: return xorstr_("PHOENIX");
        break;
    case 11945003: return xorstr_("SOVA");
        break;
    case 11952530: return xorstr_("YORU");
        break;
        
        /*Bot*/
    case 11954219: return  xorstr_("BOT");
        break;
    default:
        return ("");
        break;
    }
}

//
//bool is_visible2(uintptr_t entity)
//{
//    if (entity == 0 || PlayerController == 0) return false;
//    auto LineOfSightAddr = read<uintptr_t>(PlayerController) + 0x6C0;
//    if (!LineOfSightAddr) return false;
//    Vector3 tmp = Vector3(0, 0, 0);
//    auto fLineOfSight = *((bool(__fastcall**)(uintptr_t, uintptr_t, Vector3*, bool))(LineOfSightAddr));
//    if (!fLineOfSight) return false;
//    return fLineOfSight((uintptr_t)PlayerController, entity, &tmp, false);
//}

float HealthLocal;
unsigned __stdcall cache(LPVOID lp)
{
    // Lala(xorstr_("qwerwefv"));
     HideThread(GetCurrentThread());
     //Lala(xorstr_("qwerwefv"));
    while (true)
    {
        vector<ValEntity> TempList;
        ULevel = read<uintptr_t>(UWorld + Offsets::oLevel);//page guard
        if (ULevel == NULL || GameInstance == NULL || LocalPlayer == NULL || PlayerController == NULL)
            UWorld = ReadWorld(); //page guard

        GameInstance = read<uintptr_t>(UWorld + Offsets::oGameInstance);
        if (GameInstance == 0) continue;
        LocalPlayer = read<uintptr_t>(read<uintptr_t>(GameInstance + Offsets::oLocalPlayers));
        if (LocalPlayer == 0) continue;
        PlayerController = read<uintptr_t>(LocalPlayer + Offsets::oPlayerController);
        if (PlayerController == 0) continue;
        CameraCache = read<uint64_t>(PlayerController + Offsets::oCameraManager);
        if (CameraCache == 0) continue;
        AcknowledgedPawn = read<uintptr_t>(PlayerController + Offsets::oLocalPawn);
        if (AcknowledgedPawn == 0) continue;
        uintptr_t DmgHandlerLocal = read<uintptr_t>(AcknowledgedPawn + Offsets::DamageHandler);
        if (DmgHandlerLocal == 0) continue;
        LocalPlayerState = read<uintptr_t>(AcknowledgedPawn + Offsets::PlayerState);
        if (LocalPlayerState == 0) continue;
        auto ActorArray = read<uintptr_t>(ULevel + 0xA0);
        if (ActorArray == 0) continue;

        HealthLocal = read<float>(DmgHandlerLocal + Offsets::Health);

        int ActorCount = read<int>(ULevel + 0xA8);
        for (int i = 0; i < ActorCount; i++)
        {
            uintptr_t Actor = read<uintptr_t>(ActorArray + (i * 0x8));
            if (Actor != 0)
            {
                //int L_ObjID = read<int>(AcknowledgedPawn + Offsets::ObjID);
               // printf("%d\n", L_ObjID);
                uintptr_t Mesh = read<uintptr_t>(Actor + Offsets::Mesh);
                int UniqueId = read<int>(Actor + Offsets::UniqueID);//18743553

                ValEntity valEntity{ };
                valEntity.Addr = Actor;

                int ObjID = read<int>(Actor + Offsets::ObjID);
                std::string get_Ab = GetAbilities(ObjID);


             
                      //  valEntity.ObjectID = ObjID;
                        valEntity.Mesh = Mesh;
                        valEntity.Type = Player;
                        TempList.push_back(valEntity);
             /*      
                else if (isBot)
                {
                    if (Mesh != NULL)
                    {
                        valEntity.ObjectID = ObjID;
                        valEntity.Mesh = Mesh;
                        valEntity.Type = Bot;
                        TempList.push_back(valEntity);
                    }
                }
                else if (get_Ab != xorstr_("NULL"))
                {
                    valEntity.ObjectID = ObjID;
                    valEntity.Type = Abs;
                    TempList.push_back(valEntity);
                }
                else if (ObjID == 12062019)
                {
                    valEntity.ObjectID = ObjID;
                    valEntity.Type = sSpike;
                    TempList.push_back(valEntity);
                }*/
            }
        }
        entityList.clear();
        entityList = TempList;
    }
}

Vector3 WorldToScreen(Vector3& world_loc) {
    auto CamCache = GetCameraCache();
    const auto matrix = to_matrix(CamCache.Rotation, Vector3());
    const auto axisx = Vector3(matrix.m[0][0], matrix.m[0][1], matrix.m[0][2]);
    const auto axisy = Vector3(matrix.m[1][0], matrix.m[1][1], matrix.m[1][2]);
    const auto axisz = Vector3(matrix.m[2][0], matrix.m[2][1], matrix.m[2][2]);

    const auto delta = world_loc - CamCache.Position;
    auto transformed = Vector3(delta.Dot(axisy), delta.Dot(axisz), delta.Dot(axisx));

    if (transformed.z < 0.001f)
        transformed.z = 0.001f;

    const auto fov_angle = CamCache.Fov;
    const float center = Width / 2.f;
    const float centery = Height / 2.f;

    return Vector3(center + transformed.x * (center / static_cast<float>(tan(fov_angle * M_PI / 360))) / transformed.z, centery - transformed.y * (center / static_cast<float>(tan(fov_angle * M_PI / 360))) / transformed.z, 0.f);
}
static void box3d_lines(Vector3 center, float x1, float y1, float z1, float x2, float y2, float z2, DWORD col)
{
    Vector3 pointPos1 = { center.x + x1, center.y + y1, center.z + z1 };
    Vector3 pointPos2 = { center.x + x2, center.y + y2, center.z + z2 };

    Vector3 sxy1 = WorldToScreen(pointPos1);
    Vector3 sxy2 = WorldToScreen(pointPos2);

    if (sxy1.x != 0 || sxy1.y != 0 || sxy2.x != 0 || sxy2.y != 0)
        DrawLine(sxy1.x, sxy1.y, sxy2.x, sxy2.y, col, 1.3);
}

static void box3d(Vector3 center, float w, float h, DWORD col)
{
#define padding 10
    box3d_lines(center, -w, -w, 0, w, -w, 0, col);
    box3d_lines(center, -w, -w, h, w, -w, h, col);

    box3d_lines(center, -w, -w, 0, -w, -w, h, col);
    box3d_lines(center, w, -w, 0, w, -w, h, col);

    box3d_lines(center, w, w + padding, 0, w, w + padding, h, col);
    box3d_lines(center, -w, w + padding, 0, -w, w + padding, h, col);
    box3d_lines(center, w, w + padding, 0, -w, w + padding, 0, col);
    box3d_lines(center, w, w + padding, h, -w, w + padding, h, col);

    box3d_lines(center, -w, -w, 0, -w, w + padding, 0, col);
    box3d_lines(center, w, w + padding, 0, w, -w, 0, col);
    box3d_lines(center, -w, -w, h, -w, w + padding, h, col);
    box3d_lines(center, w, w + padding, h, w, -w, h, col);
}

void DrawSkeleton(uintptr_t Mesh, bool IsMale, DWORD col)
{
    int skeleton[][4] = {
        { 7, 8, 0, 0},
        { 7, IsMale ? 47 : 46, IsMale ? 48 : 47, IsMale ? 49 : 48 },
        { 7, 21, 22, 23 },
        { 7, 5, 4, 3 },
        { 3, IsMale ? 82 : 80, IsMale ? 83 : 81, IsMale ? 85 : 83 },
        { 3, IsMale ? 75 : 73, IsMale ? 76 : 74, IsMale ? 78 : 76 }
    };

    for (auto part : skeleton)
    {
        Vector3 previous{};
        for (int i = 0; i < 4; i++)
        {
            if (!part[i]) break;

            Vector3 current = GetBoneWithRotation(Mesh, part[i]);
            if (previous.x == 0.f)
            {
                previous = current;
                continue;
            }
            Vector3 p1{}, c1{};
            p1 = WorldToScreen(previous);
            c1 = WorldToScreen(current);
            DrawLine(p1.x, p1.y, c1.x, c1.y, col, 1.5f);
            previous = current;
        }
    }

}

Vector3 FindSkeleton(DWORD_PTR mesh, int i)
{
    return WorldToScreen(GetBoneWithRotation(mesh, i));
}

void DrawSkeletonBot(DWORD_PTR mesh, DWORD cColor, float kalinlik = 2.0f)
{

    Vector3 vHipOut = FindSkeleton(mesh, 3);

    Vector3 vHeadBoneOut = FindSkeleton(mesh, 8);
    Vector3 vNeckOut = FindSkeleton(mesh, 7);

    Vector3 vUpperArmLeftOut = FindSkeleton(mesh, 11);
    Vector3 vLeftHandOut = FindSkeleton(mesh, 12);
    Vector3 vLeftHandOut1 = FindSkeleton(mesh, 13);

    Vector3 vUpperArmRightOut = FindSkeleton(mesh, 36);
    Vector3 vRightHandOut = FindSkeleton(mesh, 37);
    Vector3 vRightHandOut1 = FindSkeleton(mesh, 38);



    Vector3 vLeftThighOut = FindSkeleton(mesh, 63);
    Vector3 vLeftCalfOut = FindSkeleton(mesh, 65);
    Vector3 vLeftFootOut = FindSkeleton(mesh, 69);

    Vector3 vRightThighOut = FindSkeleton(mesh, 77);
    Vector3 vRightCalfOut = FindSkeleton(mesh, 79);
    Vector3 vRightFootOut = FindSkeleton(mesh, 83);
    // if (distance > 0)
    {
        DrawLine(vNeckOut.x, vNeckOut.y, vHeadBoneOut.x, vHeadBoneOut.y, cColor, kalinlik);
        DrawLine(vHipOut.x, vHipOut.y, vNeckOut.x, vNeckOut.y, cColor, kalinlik);
        DrawLine(vUpperArmLeftOut.x, vUpperArmLeftOut.y, vNeckOut.x, vNeckOut.y, cColor, kalinlik);
        DrawLine(vUpperArmRightOut.x, vUpperArmRightOut.y, vNeckOut.x, vNeckOut.y, cColor, kalinlik);
        DrawLine(vLeftHandOut.x, vLeftHandOut.y, vUpperArmLeftOut.x, vUpperArmLeftOut.y, cColor, kalinlik);
        DrawLine(vRightHandOut.x, vRightHandOut.y, vUpperArmRightOut.x, vUpperArmRightOut.y, cColor, kalinlik);
        DrawLine(vLeftHandOut.x, vLeftHandOut.y, vLeftHandOut1.x, vLeftHandOut1.y, cColor, kalinlik);
        DrawLine(vRightHandOut.x, vRightHandOut.y, vRightHandOut1.x, vRightHandOut1.y, cColor, kalinlik);
        DrawLine(vLeftThighOut.x, vLeftThighOut.y, vHipOut.x, vHipOut.y, cColor, kalinlik);
        DrawLine(vRightThighOut.x, vRightThighOut.y, vHipOut.x, vHipOut.y, cColor, kalinlik);
        DrawLine(vLeftCalfOut.x, vLeftCalfOut.y, vLeftThighOut.x, vLeftThighOut.y, cColor, kalinlik);
        DrawLine(vRightCalfOut.x, vRightCalfOut.y, vRightThighOut.x, vRightThighOut.y, cColor, kalinlik);
        DrawLine(vLeftFootOut.x, vLeftFootOut.y, vLeftCalfOut.x, vLeftCalfOut.y, cColor, kalinlik);
        DrawLine(vRightFootOut.x, vRightFootOut.y, vRightCalfOut.x, vRightCalfOut.y, cColor, kalinlik);
    }
}

bool is_visible(uint64_t mesh)
{
    bool bVisible;

    float fLastSubmitTime = read<float>(mesh + 0x350);
    float fLastRenderTimeOnScreen = read<float>(mesh + 0x350 + 8);
    const float fVisionTick = 0.06f;
    bVisible = fLastRenderTimeOnScreen + fVisionTick >= fLastSubmitTime;
    return bVisible;
}


D3DXVECTOR2 WorldRadar(Vector3 srcPos, Vector3 distPos, float yaw, float radarX, float radarY, float size)
{
    auto cosYaw = cos(DegreeToRadian(yaw));
    auto sinYaw = sin(DegreeToRadian(yaw));

    auto deltaX = srcPos.x - distPos.x;
    auto deltaY = srcPos.y - distPos.y;

    auto locationX = (float)(deltaY * cosYaw - deltaX * sinYaw) / 45.f;
    auto locationY = (float)(deltaX * cosYaw + deltaY * sinYaw) / 45.f;

    if (locationX > (size - 2.f))
        locationX = (size - 2.f);
    else if (locationX < -(size - 2.f))
        locationX = -(size - 2.f);

    if (locationY > (size - 6.f))
        locationY = (size - 6.f);
    else if (locationY < -(size - 6.f))
        locationY = -(size - 6.f);

    return D3DXVECTOR2((int)(-locationX + radarX), (int)(locationY + radarY));
}
static Vector3 pRadar;

static int enemy_size = 3;



struct vertex
{
    FLOAT x, y, z, rhw;
    DWORD color;
};

void DrawCircleFilled(float x, float y, float r, unsigned int p, DWORD color, IDirect3DDevice9* pDevice)
{
    static bool once = false;
    if (!once)  // nvm this,  tried it without once as well 
    {
        pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
        once = true;
    }


    vertex* pVertex = new vertex[p + 1];

    for (unsigned int i = 0; i <= p; ++i)
    {
        pVertex[i].x = x + r * cos(D3DX_PI * (i / (p / 2.f)));
        pVertex[i].y = y + r * sin(D3DX_PI * (i / (p / 2.f)));
        pVertex[i].z = 0;
        pVertex[i].rhw = 0;
        pVertex[i].color = color;

    }

    pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, p - 1, pVertex, sizeof(vertex));

    delete[] pVertex;
}

void DrawRadar(Vector3 EntityPos, DWORD EntityColor)
{
    auto radar_posX = pRadar.x + 135;
    auto radar_posY = pRadar.y + 135;
    if (AcknowledgedPawn == 0) return;
    uint64_t LocalRootComp = read<uint64_t>(AcknowledgedPawn + Offsets::RootComp);
    Vector3 LocalPos = read<Vector3>(LocalRootComp + Offsets::RelativeLocation);
    auto Radar2D = WorldRadar(LocalPos, EntityPos,GetCameraCache().Rotation.y/* ViewAngles(AcknowledgedPawn).Y*/, radar_posX, radar_posY, 135.f);// radar pos


    DrawCircleFilled(Radar2D.x, Radar2D.y, 4, 10, EntityColor, draw.GetDevice());

   // DrawRect(Radar2D.x, Radar2D.y, 4, 4, EntityColor, draw.GetDevice());
}
D3DXVECTOR4 Rect;
#define red4 D3DCOLOR_RGBA(255, 37, 59, 255)
#define ared4 D3DCOLOR_RGBA(255, 37, 59, 50)

#define glassblack D3DCOLOR_RGBA(0, 0, 0, 180)
#define aglassblack D3DCOLOR_RGBA(0, 0, 0, 50)


float AimFOV = 30.f;


uintptr_t entityx = 0;

float DistanceBetweenCross(float X, float Y)
{
    float ydist = (Y - (ScreenCenterY));
    float xdist = (X - (ScreenCenterX));
    float Hypotenuse = sqrt(pow(ydist, 2) + pow(xdist, 2));
    return Hypotenuse;
}
bool GetClosestPlayerToCrossHair(Vector3 Pos, float& max, float aimfov)
{
    float Dist = DistanceBetweenCross(Pos.x, Pos.y);
    if (Dist < max)
    {
        max = Dist;
        AimFOV = aimfov;
        return true;
    }
    return false;
}
void AIms(uintptr_t ent)
{
    float max = AimFOV / 2;
    Vector3 rootHeadOut = Vector3();

    uintptr_t Mesh = read<uintptr_t>(ent + Offsets::Mesh);
    Vector3 rootHead = GetBoneWithRotation(Mesh, 7);
    rootHeadOut = WorldToScreen(rootHead);

    if (GetClosestPlayerToCrossHair(rootHeadOut, max, AimFOV))
        entityx = ent;
    return;
}


static bool InsideCircle(float xc, float yc, float r, float x, float y)
{

    float dx = xc - x;
    float dy = yc - y;
    return dx * dx + dy * dy <= r * r;
}

bool bumbum = true;
__forceinline void Clamp(Vector3& Ang) {
    if (Ang.x < 0.f)
        Ang.x += 360.f;

    if (Ang.x > 360.f)
        Ang.x -= 360.f;

    if (Ang.y < 0.f) Ang.y += 360.f;
    if (Ang.y > 360.f) Ang.y -= 360.f;
    Ang.z = 0.f;
}
#include <intrin.h>
__forceinline Vector3 CalcAngle(Vector3 Src, Vector3& Dst) {
    Vector3 Delta = Src - Dst;
    const auto sqrtss = [](float in) {
        __m128 reg = _mm_load_ss(&in);
        return _mm_mul_ss(reg, _mm_rsqrt_ss(reg)).m128_f32[0];
    };

    float hyp = sqrtss(Delta.x * Delta.x + Delta.y * Delta.y + Delta.z * Delta.z);

    Vector3 Rotation{};
    Rotation.x = RadianToDegree(acosf(Delta.z / hyp));
    Rotation.y = RadianToDegree(atanf(Delta.y / Delta.x));
    Rotation.z = 0;
    if (Delta.x >= 0.0f) Rotation.y += 180.0f;
    Rotation.x += 270.f;
    return Rotation;
}

Vector3 LGetViewAngles()
{
    Vector3 ViewAngle = read<Vector3>(PlayerController + Offsets::ControlRotation);
    return ViewAngle;
}

void normalize(Vector3& in)
{
    if (in.x > 89.f) in.x -= 360.f;
    else if (in.x < -89.f) in.x += 360.f;

    // in.y = fmodf(in.y, 360.0f);
    while (in.y > 180)in.y -= 360;
    while (in.y < -180)in.y += 360;
    in.z = 0;
}

Vector3 SmoothAim(Vector3 Camera_rotation, Vector3 Target, float SmoothFactor)
{
    Vector3 diff = Target - Camera_rotation;
    normalize(diff);
    return Camera_rotation + diff / SmoothFactor;
}

void RCS(Vector3 Target, Vector3 Camera_rotation, float SmoothFactor) {

    // Camera 2 Control space
    Vector3 ConvertRotation = Camera_rotation;
    normalize(ConvertRotation);

    // Calculate recoil/aimpunch
    auto ControlRotation = read<Vector3>(PlayerController + Offsets::ControlRotation);
    Vector3 DeltaRotation = ConvertRotation - ControlRotation;
    normalize(DeltaRotation);

    // Remove aimpunch from CameraRotation
    ConvertRotation = Target - (DeltaRotation * SmoothFactor);
    normalize(ConvertRotation);

    //Smooth the whole thing
    Vector3 Smoothed = SmoothAim(Camera_rotation, ConvertRotation, SmoothFactor);
    Smoothed -= (DeltaRotation / SmoothFactor);
    Clamp(Smoothed);
    // normalize(Smoothed);
    // *(float*)(PlayerController + Offsets::ControlRotation) = Smoothed.X;
    //*(float*)(PlayerController + 0x3F4) = Smoothed.Y;
    *(D3DXVECTOR3*)(PlayerController + Offsets::ControlRotation) = D3DXVECTOR3(Smoothed.x, Smoothed.y, 0);
    return;
}

float Smooth = 1.f;
void xasdasdascsa()
{
    //auto asdfasd = curlGetRequestt(xorstr_("https://apathe.net"));
    //(LocalPawn + 0xF28) + 0x210 = Matrix
    //(LocalPawn + 0xF28) + 0x240 = Pitch & Yaw
    bool IsDormant = read<bool>(entityx + Offsets::Dormant);
    uintptr_t Mesh = read<uintptr_t>(entityx + Offsets::Mesh);

    if (AcknowledgedPawn != 0)
    {
        uintptr_t DmgHandler = read<uintptr_t>(entityx + Offsets::DamageHandler);
        float Health = read<float>(DmgHandler + Offsets::Health);

        int EnemyTeam = read<int>(read<uintptr_t>(read<uintptr_t>(entityx + Offsets::PlayerState) + Offsets::TeamComponent) + Offsets::Team);
        int LocalTeam = read<int>(read<uintptr_t>(read<uintptr_t>(AcknowledgedPawn + Offsets::PlayerState) + Offsets::TeamComponent) + Offsets::Team);

        Vector3 aim_bone = GetBoneWithRotation(Mesh, (menu.items[12].value == 0 ? 8 : menu.items[12].value == 1 ? 7 : 5));
        Vector3 vHead = WorldToScreen(aim_bone);
        if (entityx != 0)
        {
            if (Health > 0.f && Health < 200.f)
            {
                if (menu.items[0].value != 2 ? (EnemyTeam != LocalTeam) : true)
                {
                    if (is_visible(Mesh))
                    {
                        if (vHead.x != 0.f && vHead.y != 0.f &&
                            aim_bone.x != 0.f && aim_bone.y != 0.f && aim_bone.z != 0.f)
                        {
                            if (GetAsyncKeyState(VK_LBUTTON) ?
                                InsideCircle(ScreenCenterX, ScreenCenterY, 180.f, vHead.x, vHead.y) :
                                InsideCircle(ScreenCenterX, ScreenCenterY, AimFOV, vHead.x, vHead.y))
                            {
                                if (DistanceBetweenCross(vHead.x, vHead.y))
                                {
                                    Vector3 AimAngles = CalcAngle(GetCameraCache().Position, aim_bone);
                                    Clamp(AimAngles);
                                    if (menu.items[16].value == 1) {
                                        RCS(AimAngles, GetCameraCache().Rotation, Smooth + 0.1f);
                                    }
                                    else
                                    {
                                        *(D3DXVECTOR3*)(PlayerController + Offsets::ControlRotation) = D3DXVECTOR3(AimAngles.x, AimAngles.y, 0);
                                        return;
                                    }
                                }
                            }
                        }
                    }
                }

            }
        }
    }
}


void Circle(int X, int Y, int radius, int numSides, DWORD Color)
{
    D3DXVECTOR2 Line[128];
    float Step = M_PI * 2.0 / numSides;
    int Count = 0;
    for (float a = 0; a < M_PI * 2.0; a += Step)
    {
        float X1 = radius * cos(a) + X;
        float Y1 = radius * sin(a) + Y;
        float X2 = radius * cos(a + Step) + X;
        float Y2 = radius * sin(a + Step) + Y;
        Line[Count].x = X1;
        Line[Count].y = Y1;
        Line[Count + 1].x = X2;
        Line[Count + 1].y = Y2;
        Count += 2;
    }
    xwwwwwwqq->Begin();
    xwwwwwwqq->Draw(Line, Count, Color);
    xwwwwwwqq->End();
}


bool isaimbotting;
DWORD Aimkey = NULL;
bool GetKafa()
{
    if (Aimkey != NULL)
        return (GetAsyncKeyState(Aimkey));
    else
        return true;
}

void asdfaewfacd()
{
    if (entityx)
    {
        if (GetKafa())
        {
            isaimbotting = true;
            xasdasdascsa();
        }
        else
        {
            isaimbotting = false;
        }
    }
    else
    {
        isaimbotting = false;
    }
}

//bool is_visible2(uintptr_t entity)
//{
//    //__try
//    //{
//        if (AcknowledgedPawn != NULL) {
//            auto LineOfSightAddr = read<uintptr_t>(PlayerController) + 0x6C0;
//            if (!LineOfSightAddr || LineOfSightAddr==0) return false;
//            Vector3 tmp = Vector3(0, 0, 0);
//            auto fLineOfSight = *((bool(__fastcall**)(uintptr_t, uintptr_t, Vector3*, bool))(LineOfSightAddr));
//            if (!fLineOfSight || fLineOfSight==0) return false;
//            return fLineOfSight((uintptr_t)PlayerController, entity, &tmp, true);
//        }
//    //}
//    //__except (1) { return false; }
//}

void EspLoop()
{
    pRadar.x = Width - 450;
    pRadar.y = ScreenCenterY - ScreenCenterY + 300;

    if (menu.items[9].value == 1)
    {
        DrawRect(pRadar.x, pRadar.y, 270, 270, TBlack2, draw.GetDevice());
        DrawBorder(pRadar.x, pRadar.y, 270, 270, 1, SilverWhite, draw.GetDevice());

        auto radar_posX = pRadar.x + 135;//155
        auto radar_posY = pRadar.y + 135;//165
        // alt & �st
        DrawLine(radar_posX, radar_posY, radar_posX, radar_posY + 135, SilverWhite, 1);
        DrawLine(radar_posX, radar_posY, radar_posX, radar_posY - 135, SilverWhite, 1);

        // sol & sa�
        DrawLine(radar_posX, radar_posY, radar_posX + 135, radar_posY, SilverWhite, 1);
        DrawLine(radar_posX, radar_posY, radar_posX - 135, radar_posY, SilverWhite, 1);

        DrawCircleFilled(radar_posX - 0.5f, radar_posY - 0.5f, 3, 50, Yellow, draw.GetDevice());
       // DrawRect(radar_posX - 0.5f, radar_posY - 0.5f, 3, 3, Yellow, draw.GetDevice());
    }
    std::vector<ValEntity> vEntityList = entityList;
    for (int i = 0; i < vEntityList.size(); i++)
    {
        ValEntity pEntity = vEntityList[i];
        if (AcknowledgedPawn != NULL && pEntity.Addr != NULL && pEntity.Addr != AcknowledgedPawn /*&& HealthLocal > 0.f*/)
        {
            uintptr_t Mesh = pEntity.Mesh;
            uintptr_t bonearray = read<uintptr_t>(Mesh + Offsets::StaticMesh);

            uintptr_t RootComp = read<uintptr_t>(pEntity.Addr + Offsets::RootComp);
            Vector3 RootPos = read<Vector3>(RootComp + Offsets::RelativeLocation);
            Vector3 vScreen = WorldToScreen(RootPos);
            uintptr_t LocalRootComp = read<uintptr_t>(AcknowledgedPawn + Offsets::RootComp);
            Vector3 LocalPos = read<Vector3>(LocalRootComp + Offsets::RelativeLocation);
            float fDistance = LocalPos.Distance(RootPos) / 100.f;
            //int ObjID = pEntity.ObjectID;
 

            
            char dist_[64];
            sprintf(dist_, xorstr_("[%.fm]"), fDistance);

            bool IsDormant = read<bool>(pEntity.Addr + Offsets::Dormant);

            int EnemyTeam = read<int>(read<uintptr_t>(read<uintptr_t>(pEntity.Addr + Offsets::PlayerState) + Offsets::TeamComponent) + Offsets::Team);
            int LocalTeam = read<int>(read<uintptr_t>(read<uintptr_t>(AcknowledgedPawn + Offsets::PlayerState) + Offsets::TeamComponent) + Offsets::Team);

            DWORD vis_col = IsDormant ? ARed : (is_visible(Mesh) ? Green : Red);
            if (menu.items[0].value != 2 ? (pEntity.Type == Player) : (pEntity.Type == Bot))
            {
                uintptr_t DmgHandler = read<uintptr_t>(pEntity.Addr + Offsets::DamageHandler);
                float Health = read<float>(DmgHandler + Offsets::Health);
               
                if (Health > 0.f && Health < 200.f)
                {
                    AIms(pEntity.Addr);
                    int L_ObjID = read<int>(AcknowledgedPawn + Offsets::ObjID);


                    DrawString(vScreen.x, vScreen.y, ASilverWhite, 0, L_ObjID);
                    char d_char[64];
                  
                   
                    bool IsDormant = read<bool>(pEntity.Addr + Offsets::Dormant);

                    uintptr_t ePlayerState = read<uintptr_t>(pEntity.Addr + Offsets::PlayerState);

                    Vector3 Head = GetBoneWithRotation(Mesh, 8);
                    Vector3 Foot = GetBoneWithRotation(Mesh, 0);

                    Vector3 vHead = WorldToScreen(Head);
                    Vector3 vFoot = WorldToScreen(Foot);

                    if (menu.items[0].value != 2 ? (EnemyTeam != LocalTeam) : true)
                    {
                        if (fDistance > 1.f)
                        {
                            if (menu.items[9].value == 1)
                                DrawRadar(RootPos, vis_col);

                            if (menu.items[1].value > 0) {
                                Vector3 Box = vHead - vFoot;
                                if (Box.y < 0)
                                    Box.y *= -1;
                                float BoxHeight = Box.y + 5;
                                float BoxWidth = Box.y / 2.4f;
                                //if (menu.items[1].value == 1) //Corner
                                //{
                                //    DrawCornerBox(vHead.x - BoxWidth / 2 + 1, vHead.y, BoxWidth, BoxHeight, 1.f, vis_col, draw.GetDevice());
                                //    DrawCornerBox(vHead.x - BoxWidth / 2 - 1, vHead.y, BoxWidth, BoxHeight, 1.f, vis_col, draw.GetDevice());
                                //    DrawCornerBox(vHead.x - BoxWidth / 2, vHead.y + 1, BoxWidth, BoxHeight, 1.f, vis_col, draw.GetDevice());
                               //    DrawCornerBox(vHead.x - BoxWidth / 2, vHead.y - 1, BoxWidth, BoxHeight, 1.f, vis_col, draw.GetDevice());
                                //}
                                //else if (menu.items[1].value == 2) //2D
                                    //DrawESPBox(vHead, vFoot, vis_col, IsDormant ? 5 : (is_visible(Mesh) ? 10 : 10));
                                //else if (menu.items[1].value == 3) //3D
                                   box3d(Foot, 39, 186, vis_col);
                            }

                            if (menu.items[2].value > 0) {
                                if (menu.items[2].value == 1)
                                    DrawLine(ScreenCenterX, ScreenCenterY - ScreenCenterY, vFoot.x, vFoot.y, vis_col, 2.f);
                                else if (menu.items[2].value == 2)
                                    DrawLine(ScreenCenterX, ScreenCenterY, vFoot.x, vFoot.y, vis_col, 2.f);
                                else if (menu.items[2].value == 3)
                                    DrawLine(ScreenCenterX, ScreenCenterY + ScreenCenterY, vFoot.x, vFoot.y, vis_col, 2.f);
                            }


                          
                            if (menu.items[5].value == 1)
                                if (menu.items[0].value != 2)
                                    DrawSkeleton(Mesh, true, IsDormant ? ASilverWhite : (is_visible(Mesh) ? SilverWhite : SilverWhite));
                                else
                                    DrawSkeletonBot(Mesh, IsDormant ? ASilverWhite : (is_visible(Mesh) ? SilverWhite : SilverWhite));
 
                            if (menu.items[3].value == 1) {
                                DrawCircleFilled(vHead.x, vHead.y, (vHead.y - vFoot.y) / 12, 100, IsDormant ? ACyan : (is_visible(Mesh) ? Cyan : Cyan), draw.GetDevice());
                                DrawCircleFilled(vHead.x, vHead.y, (vHead.y - vFoot.y) / 20, 100, IsDormant ? ACyan2 : (is_visible(Mesh) ? Cyan2 : Cyan2), draw.GetDevice());
                                //Circle(vHead.x, vHead.y, 10, 50, IsDormant ? ACyan2 : (is_visible(Mesh) ? Cyan2 : Cyan2));
                                //  DrawCircleFilled(vHead.x, vHead.y, 10, 50, vis_col, draw.GetDevice());
                            }
                            if (menu.items[6].value == 1) {
                                //
                                DrawString(vFoot.x - (strlen(d_char) * 7 / 2) + (Rect.w / 2) - 1, vFoot.y + 5, IsDormant ? ASilverWhite : (is_visible(Mesh) ? SilverWhite : SilverWhite), d_char);
                                DrawString(vFoot.x - (strlen(dist_) * 7 / 2) + (Rect.w / 2) - 1, vFoot.y + 17, IsDormant ? ASilverWhite : (is_visible(Mesh) ? SilverWhite : SilverWhite), dist_);

                                //DrawRect(vHead.x - (strlen(d_char) * 7 / 2) + (Rect.w / 2) - 27, vHead.y - 85 + (Rect.w / 2) + 62, (3 * 7 / 2) + 16, 16, IsDormant ? ared4 : (is_visible(Mesh) ? red4 : red4), draw.GetDevice());
                                //DrawRect(vHead.x - (strlen(d_char) * 7 / 2) + (Rect.w / 2) - 1, vHead.y - 85 + (Rect.w / 2) + 62, strlen(d_char) * 7 + 14, 16, IsDormant ? aglassblack : (is_visible(Mesh) ? glassblack : glassblack), draw.GetDevice());

                                //DrawString(vHead.x - (strlen(d_char) * 7 / 2) + (Rect.w / 2) - (fDistance < 10.f ? 17 : 21), vHead.y - 85 + (Rect.w / 2) + 63, IsDormant ? ASilverWhite : (is_visible(Mesh) ? SilverWhite : SilverWhite), dist_);
                                //DrawString(vHead.x - (strlen(d_char) * 7 / 2) + (Rect.w / 2) + 7, vHead.y - 87 + (Rect.w / 2) + 63, IsDormant ? D3DCOLOR_RGBA(255, 20, 20, 50) : (is_visible(Mesh) ? Green : D3DCOLOR_RGBA(255, 20, 20, 255)), d_char);
                            }


                            if (menu.items[4].value == 1) {
                                const int middle = vFoot.y - vHead.y;
                                const auto width = middle / 4.8;
                                const auto health_calc = std::clamp(static_cast<int>(Health) * middle / 100, 0, middle);
                                const auto health_color = color().hsv_to_rgb(int(Health) + 25, 1, 1, 255);
                                DrawString(vFoot.x - (strlen(string(std::to_string((int)Health) + xorstr_(" HP")).c_str()) * 7 / 2) + 5, vFoot.y + 5, IsDormant ? D3DCOLOR_RGBA(236, 236, 236, 255) : D3DCOLOR_RGBA(health_color.r, health_color.g, health_color.b, 255), string(std::to_string((int)Health) + " HP").c_str());
                                DrawString(vHead.x + width + 8, vHead.y - 1, IsDormant ? D3DCOLOR_RGBA(236, 236, 236, 255) : D3DCOLOR_RGBA(health_color.r, health_color.g, health_color.b, 255), string(std::to_string((int)Health) + " HP").c_str());
                                DrawBox(vHead.x - width, vHead.y, 4, std::clamp(100 * middle / 100, 0, middle) + 5, D3DCOLOR_RGBA(120, 120, 120, 250), D3DCOLOR_RGBA(120, 120, 120, 250), draw.GetDevice());
                                DrawRect(vHead.x - width, vHead.y + middle - health_calc, 4, health_calc + 5, D3DCOLOR_RGBA(health_color.r, health_color.g, health_color.b, 255), draw.GetDevice());
                            }
                        }
                    }
                }
            }
            else if (pEntity.Type == Abs)
            {
                // std::string get_Ab = GetAbilities(ObjID);
               //  if (get_Ab != xorstr_("NULL"))
              //   {
                if (AcknowledgedPawn != 0) {
                    if (menu.items[7].value == 1) {
                        // if (ObjID != 11952667 && ObjID != 11952969) {
                        DrawRect(vScreen.x - (strlen("a") * 7 / 2) + (Rect.w / 2) - 1, vScreen.y, 5, 5, Cyan, draw.GetDevice());
                        DrawString(vScreen.x - (strlen("Player") * 7 / 2) + (Rect.w / 2) - 1, vScreen.y + 7, IsDormant ? ASilverWhite : (is_visible(Mesh) ? SilverWhite : SilverWhite), "Player");
                        DrawString(vScreen.x - (strlen(dist_) * 7 / 2) + (Rect.w / 2) - 1, vScreen.y + 19, IsDormant ? ASilverWhite : (is_visible(Mesh) ? SilverWhite : SilverWhite), dist_);
                        //  }
                          /*else if (strstr(get_Ab.c_str(), xorstr_("Yoru"))) {
                              DrawString(vScreen.x, vScreen.y, Yellow, get_Ab.c_str());
                          }*/
                          //      }
                          //  }
                    }
                }
            }
            else if (pEntity.Type == sSpike)
            {
                if (menu.items[8].value == 1)
                {
                    int ascasasd = 56235;
                   // if (ObjID == 12062019) {
                        int asdfa = 12312;
                        int adsfgasdf = 12412;
                        double asdfasd = 11111111.0;
                        float cdsadfasxdxf = read<float>(pEntity.Addr + Offsets::SpikeTimer);//hh//12062019
                        char c_timer[64];

                        int fasdcas = 5124;
                        double wrwaerasfd = 135124123;
                        sprintf(c_timer, xorstr_("Spike ( %.2fs )"), cdsadfasxdxf);
                        DrawString(vScreen.x - (strlen(c_timer) * 7 / 2), vScreen.y - 5, Cyan, c_timer);
                        DrawLine(ScreenCenterX, ScreenCenterY + ScreenCenterY, vScreen.x, vScreen.y + 20, Cyan, 1.f);
                 //   }
                }
            }
        }
    }

    if (HealthLocal > 0.f)
    {
        if (ULevel == NULL || GameInstance == NULL || LocalPlayer == NULL || PlayerController == NULL)
        {
        }
        else {
            if ( menu.items[14].value == 1)
                Circle(ScreenCenterX, ScreenCenterY, (int)AimFOV, 50, SilverWhite);

            
            // { "5", "10", "15", "20", "25", "30", "45", "60", "90", "120", "180" };
            if (menu.items[13].value == 0)
                AimFOV = 10.f;
            else if (menu.items[13].value == 1)
                AimFOV = 20.f;
            else if (menu.items[13].value == 2)
                AimFOV = 30.f;
            else if (menu.items[13].value == 3)
                AimFOV = 40.f;
            else if (menu.items[13].value == 4)
                AimFOV = 50.f;
            else if (menu.items[13].value == 5)
                AimFOV = 60.f;
            else if (menu.items[13].value == 6)
                AimFOV = 90.f;
            else if (menu.items[13].value == 7)
                AimFOV = 120.f;
            else if (menu.items[13].value == 8)
                AimFOV = 180.f;
            else if (menu.items[13].value == 9)
                AimFOV = 240.f;
            else if (menu.items[13].value == 10)
                AimFOV = 360.f;

            if (menu.items[11].value == 0)
                Aimkey = VK_CAPITAL;
            else if (menu.items[11].value == 1)
                Aimkey = VK_MENU;
            else if (menu.items[11].value == 2)
                Aimkey = VK_LSHIFT;
            else if (menu.items[11].value == 3)
                Aimkey = VK_LBUTTON;
            else if (menu.items[11].value == 4)
                Aimkey = 'E';
            else if (menu.items[11].value == 5)
                Aimkey = 'F';
            else if (menu.items[11].value == 6)
                Aimkey = 'T';
            else if (menu.items[11].value == 7)
                Aimkey = 'V';
            else if (menu.items[11].value == 8)
                Aimkey = 'C';
            else if (menu.items[11].value == 9)
                Aimkey = 'X';
            else if (menu.items[11].value == 10)
                Aimkey = VK_RBUTTON;
           
            if (menu.items[15].value == 0)
                Smooth = 0.f;
            else if (menu.items[15].value == 1)
                Smooth = 3.5f;
            else  if (menu.items[15].value == 2)
                Smooth = 4.f;
            else  if (menu.items[15].value == 3)
                Smooth = 5.f;
            else  if (menu.items[15].value == 4)
                Smooth = 6.f;
            else  if (menu.items[15].value == 5)
                Smooth = 7.f;
            else if (menu.items[15].value == 6)
                Smooth = 8.f;
            else if (menu.items[15].value == 7)
                Smooth = 9.f;
            else if (menu.items[15].value == 8)
                Smooth = 10.f;
            else if (menu.items[15].value == 9)
                Smooth = 11.f;
            else if (menu.items[15].value == 10)
                Smooth = 12.f;
            else if (menu.items[15].value == 11)
                Smooth = 13.f;
            else if (menu.items[15].value == 12)
                Smooth = 14.f;
            else if (menu.items[15].value == 13)
                Smooth = 15.f;
            else if (menu.items[15].value == 14)
                Smooth = 16.f;
            else if (menu.items[15].value == 15)
                Smooth = 17.f;
            else if (menu.items[15].value == 16)
                Smooth = 18.f;
            else if (menu.items[15].value == 17)
                Smooth = 19.f;
            else if (menu.items[15].value == 18)
                Smooth = 20.f;
            else if (menu.items[15].value == 19)
                Smooth = 21.f;
            else if (menu.items[15].value == 20)
                Smooth = 22.f;

            if (menu.items[10].value != 0)
                asdfaewfacd();
        }
    }
}
void InitializeMenu()
{
    menu.Header(xorstr_("vxCheats - dev build"));

    menu.AddTab(xorstr_("Visual"));
    menu.AddItem(xorstr_("Team Select"), tSelect, 3, menu.items[0].value);
    menu.AddItem(xorstr_("Box ESP"), onOff, 2, menu.items[1].value);
    menu.AddItem(xorstr_("Line ESP"), lineStyle, 4, menu.items[2].value);
    menu.AddItem(xorstr_("Head ESP"), onOff, 2, menu.items[3].value);
    menu.AddItem(xorstr_("Health ESP"), onOff, 2, menu.items[4].value);
    menu.AddItem(xorstr_("Skeleton ESP"), onOff, 2, menu.items[5].value);
    menu.AddItem(xorstr_("Character ESP"), onOff, 2, menu.items[6].value);
    menu.AddItem(xorstr_("Ability ESP"), onOff, 2, menu.items[7].value);
    menu.AddItem(xorstr_("Spike ESP"), onOff, 2, menu.items[8].value);
    menu.AddItem(xorstr_("Draw Radar"), onOff, 2, menu.items[9].value);

    menu.AddTab(xorstr_("Aimbot"));
    menu.AddItem(xorstr_("Aimbot"), onOff, 2, menu.items[10].value);
    menu.AddItem(xorstr_("Aim Key"), keys, 11, menu.items[11].value);
    menu.AddItem(xorstr_("Aim Bone"), aimBone, 3, menu.items[12].value);
    menu.AddItem(xorstr_("Aim Fov"), aimFov, 11, menu.items[13].value);
    menu.AddItem(xorstr_("Draw Fov"), onOff, 2, menu.items[14].value);
    menu.AddItem(xorstr_("Smooth Aim"), aimTime, 21, menu.items[15].value);
    menu.AddItem(xorstr_("Control Recoil "), onOff, 2, menu.items[16].value);
    menu.items[16].value = 1;
    menu.AddTab(xorstr_("Menu Settings"));
    menu.AddItem(xorstr_("Dark Mode"), onOff, 2, menu.items[17].value);
}
bool trav = true;
void render()
{

    draw.GetDevice()->Clear(NULL, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, NULL);
    draw.GetDevice()->BeginScene();

   // input.Update();

    GameWnd = FindWindow(xorstr_("UnrealWindow"), 0);
   // RtlSecureZeroMemory(&wnd_str, sizeof(wnd_str));

    if (!GameWnd)
        ExitProcess(0);

    if (GameWnd == GetForegroundWindow() || GetActiveWindow() == GetForegroundWindow())
    {
        
        draw.GetDevice()->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
        menu.Render(GameWnd);
        
        EspLoop();
    }
    
    draw.GetDevice()->EndScene();
    draw.GetDevice()->Present(NULL, NULL, NULL, NULL);
}

IDirect3D9Ex* d3d;
void CleanuoD3D()
{
    if (draw.GetDevice() != NULL)
    {
        draw.GetDevice()->EndScene();
        draw.GetDevice()->Release();
    }
    if (d3d != NULL)
    {
        d3d->Release();
    }
}
unsigned __stdcall MainLoop(LPVOID lp)
{
  //  Lala(xorstr_("qwerwefv"));
    HideThread(GetCurrentThread());
    static RECT old_rc;
    ZeroMemory(&Message, sizeof(MSG));
    InitializeMenu();
    while (Message.message != WM_QUIT)
    {
        if (!draw.IsInitialized())
        {
            D3DXCreateFontA(draw.GetDevice(), 16, 7, FW_NORMAL, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                DEFAULT_QUALITY,    // default Quality
                DEFAULT_PITCH | FF_DONTCARE, xorstr_("Calibri"), &draw.font);
            D3DXCreateLine(draw.GetDevice(), &xwwwwwwqq);

            //  input.Init(MyWnd);
            draw.Init();
        }
        render();
        if (PeekMessage(&Message, MyWnd, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&Message);
            DispatchMessage(&Message);
        }
    }
   // Lala(xorstr_("qwerwefv"));
    DestroyWindow(MyWnd);
    CleanuoD3D();
    ExitProcess(0);
    return Message.wParam;
}

bool initD3D(HWND hWnd)
{
    if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &d3d)))
        exit(3);
   // d3d = Direct3DCreate9(D3D_SDK_VERSION);
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp, sizeof(d3dpp));

    d3dpp.hDeviceWindow = hWnd;
    d3dpp.MultiSampleQuality = D3DMULTISAMPLE_NONE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferCount = 1;
    d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
    d3dpp.BackBufferWidth = 0;
    d3dpp.BackBufferHeight = 0;
    d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
    d3dpp.Windowed = TRUE;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

    if (FAILED(d3d->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, 0, &draw.device)))
    {
        d3d->Release();
        exit(4);
    }
   // d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &draw.device);

    //D3DXCreateLine(draw.device, &xwwwwwwqq);
    d3d->Release();//boşalt

    return true;

}
string gen_random(const int len) {
    string tmp_s;
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    srand((unsigned)time(NULL) * _getpid());
    tmp_s.reserve(len);
    for (int i = 0; i < len; ++i)
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    return tmp_s;
}

void SetWindowToTarget()
{
    const char* wnd_str = xorstr_("UnrealWindow");
    GameWnd = FindWindow(wnd_str, 0);
    RtlSecureZeroMemory(&wnd_str, sizeof(wnd_str));
    ZeroMemory(&GameRect, sizeof(GameRect));
    GetWindowRect(GameWnd, &GameRect);
    Width = GameRect.right - GameRect.left;
    Height = GameRect.bottom - GameRect.top;
    DWORD dwStyle = GetWindowLong(GameWnd, GWL_STYLE);
    if (dwStyle & WS_BORDER)
    {
        GameRect.top += 32;
        Height -= 39;
    }
    ScreenCenterX = Width / 2;
    ScreenCenterY = Height / 2;
}

void ASCDAVSDFASCXD()
{
    while (!GetModuleHandleA(xorstr_("d3d9.dll"))) {
        Sleep(1);
    }
    std::string yey = gen_random(12);
    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = DefWindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = 0;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(RGB(0, 0, 0));
    wc.lpszMenuName = NULL;
    wc.lpszClassName = yey.c_str();
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    if (!RegisterClassEx(&wc))
        exit(1);
    SetWindowToTarget();
    MyWnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
        yey.c_str(),
        gen_random(8).c_str(),//""
        WS_POPUP | WS_VISIBLE,
        GameRect.left, GameRect.top, Width, Height,
        NULL, NULL, 0, NULL);
    SetLayeredWindowAttributes(MyWnd, RGB(0, 0, 0), 255, LWA_ALPHA);
    DwmExtendFrameIntoClientArea(MyWnd, &Margin);
    //Lala(xorstr_("qwerwefv"));
    _beginthreadex(0, 0, cache, 0, 0, 0);
   // Lala(xorstr_("qwerwefv"));
    if (initD3D(MyWnd))
    {
      //  Lala(xorstr_("qwerwefv"));
        _beginthreadex(0, 0, MainLoop, 0, 0, 0);
    }
   // Lala(xorstr_("qwerwefv"));
}


bool __stdcall DllMain(HINSTANCE hModule, DWORD dwAttached, LPVOID lpvReserved)
{  
  //  DisableThreadLibraryCalls(hModule);
    HideThread(hModule);
    if (dwAttached == DLL_PROCESS_ATTACH) {
      //  UnlinkModuleFromPEB(hModule);
      //  AllocConsole();
      //  freopen("CONOUT$", "w", stdout);
        ASCDAVSDFASCXD();
        CloseHandle(hModule);
    }
    return 1;
}
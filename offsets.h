#ifndef offsets_h
#define offsets_h

#include <vector>
#include <Windows.h>


struct Camera
{
    const wchar_t module_name[12] = L"server.dll";
    SIZE_T sigSize = 0;
    unsigned int main_offsets = 0x03E23D10;
    std::vector<unsigned int> offsets = { 0x2F8, 0x1F0, 0x100, 0xCC8};

    float old_value = 0.0f;
    float new_value = 1500.0f;
};

struct Fog
{
    const wchar_t module_name[19] = L"client.dll";
    SIZE_T sigSize = 0;
    unsigned int main_offsets = 0x04C29B70;
    std::vector<unsigned int> offsets = { 0x400 };

    int value = 0;
};

struct R_Farz
{
    const wchar_t module_name[19] = L"panorama.dll";
    SIZE_T sigSize = 0;
    unsigned int main_offsets = 0x00539E40;
    std::vector<unsigned int> offsets = { 0x18, 0x18, 0x10, 0x10, 0x138 };

    int value = -1;
};

struct DotPlus
{
    const wchar_t module_name[19] = L"client.dll";
    SIZE_T sigSize = 41;

    BYTE oldSig[41] = { 0xBA, 0xFF, 0xFF, 0xFF, 0xFF,
                       0x48, 0x8D, 0x0D, 0x57, 0x9B,
                       0xAD, 0x02, 0xBF, 0xFF, 0xFF,
                       0xFF, 0xFF, 0xE8, 0xFD, 0x6F,
                       0x9B, 0x00, 0x48, 0x85, 0xC0,
                       0x75, 0x0B, 0x48, 0x8B, 0x05,
                       0x49, 0x9B, 0xAD, 0x02, 0x48,
                       0x8B, 0x40, 0x08, 0x83, 0x38, 0x00 };

    char mask[41] = "xxxxxxx????xxxxxx????xxxxxxxx????xxx?xxx";
};

#endif // offsets_h

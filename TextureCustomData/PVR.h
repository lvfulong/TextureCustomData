#pragma once
#include <memory>
#ifdef WIN32
#pragma pack(push,1)
#endif
typedef struct
{
    uint32_t version;
    uint32_t flags;
    uint64_t pixelFormat;
    uint32_t colorSpace;
    uint32_t channelType;
    uint32_t height;
    uint32_t width;
    uint32_t depth;
    uint32_t numberOfSurfaces;
    uint32_t numberOfFaces;
    uint32_t numberOfMipmaps;
    uint32_t metadataLength;
#ifdef WIN32
} PVRv3TexHeader;
#pragma pack(pop)
#else
} __attribute__((packed)) PVRv3TexHeader;
#endif
bool isPvrV3(const char* pData, int nLength);
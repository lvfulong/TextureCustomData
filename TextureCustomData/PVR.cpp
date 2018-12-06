#include "PVR.h"

#define LAYA_HOST_IS_BIG_ENDIAN (bool)(*(unsigned short *)"\0\xff" < 0x100) 
#define LAYA_SWAP32(i)  ((i & 0x000000ff) << 24 | (i & 0x0000ff00) << 8 | (i & 0x00ff0000) >> 8 | (i & 0xff000000) >> 24)
#define LAYA_SWAP16(i)  ((i & 0x00ff) << 8 | (i &0xff00) >> 8)   
#define LAYA_SWAP_INT32_LITTLE_TO_HOST(i) ((LAYA_HOST_IS_BIG_ENDIAN == true)? LAYA_SWAP32(i) : (i) )
#define LAYA_SWAP_INT16_LITTLE_TO_HOST(i) ((LAYA_HOST_IS_BIG_ENDIAN == true)? LAYA_SWAP16(i) : (i) )
#define LAYA_SWAP_INT32_BIG_TO_HOST(i)    ((LAYA_HOST_IS_BIG_ENDIAN == true)? (i) : LAYA_SWAP32(i) )
#define LAYA_SWAP_INT16_BIG_TO_HOST(i)    ((LAYA_HOST_IS_BIG_ENDIAN == true)? (i):  LAYA_SWAP16(i) )

bool isPvrV3(const char* pData, int nLength)
{
    if (static_cast<size_t>(nLength) < sizeof(PVRv3TexHeader))
    {
        return false;
    }

    const PVRv3TexHeader* headerv3 = static_cast<const PVRv3TexHeader*>(static_cast<const void*>(pData));

    return LAYA_SWAP_INT32_BIG_TO_HOST(headerv3->version) == 0x50565203;
}
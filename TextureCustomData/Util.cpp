#include "stdafx.h"
#include "Util.h"
#include "assert.h"
#include "stdint.h"
#include "string.h"

int32_t getLong(const uint8_t* buf, ByteOrder byteOrder)
{
    if (byteOrder == littleEndian) {
        return   (uint8_t)buf[3] << 24 | (uint8_t)buf[2] << 16
            | (uint8_t)buf[1] << 8 | (uint8_t)buf[0];
    }
    else {
        return   (uint8_t)buf[0] << 24 | (uint8_t)buf[1] << 16
            | (uint8_t)buf[2] << 8 | (uint8_t)buf[3];
    }
}
long ul2Data(uint8_t* buf, uint32_t l, ByteOrder byteOrder)
{
    if (byteOrder == littleEndian) {
        buf[0] = (uint8_t)(l & 0x000000ff);
        buf[1] = (uint8_t)((l & 0x0000ff00) >> 8);
        buf[2] = (uint8_t)((l & 0x00ff0000) >> 16);
        buf[3] = (uint8_t)((l & 0xff000000) >> 24);
    }
    else {
        buf[0] = (uint8_t)((l & 0xff000000) >> 24);
        buf[1] = (uint8_t)((l & 0x00ff0000) >> 16);
        buf[2] = (uint8_t)((l & 0x0000ff00) >> 8);
        buf[3] = (uint8_t)(l & 0x000000ff);
    }
    return 4;
}
bool readJson(const char* file, char*& buffer, size_t& bufferLength)
{
    FILE* pf = NULL;
    pf = fopen(file, "rb");
    if (pf == NULL)
        return false;
    fseek(pf, 0, SEEK_END);
    int len = ftell(pf);
    fseek(pf, 0, SEEK_SET);
    buffer = new char[len + 1];
    int readlen = fread(buffer, 1, len, pf);
    if (readlen != len) {
        bufferLength = 0;
        fclose(pf);
        delete[] buffer;
        buffer = nullptr;
        return false;
    }
    fclose(pf);
    buffer[len] = 0;
    bufferLength = len;
    return true;
}
bool writeFileSync(const char* p_pszFile, char* p_pBuff, int p_nLen, int p_nEncode)
{
    if (!p_pszFile || strlen(p_pszFile) <= 1)
        return false;
    FILE* pFile = NULL;
    pFile = fopen(p_pszFile, "wb");
    if (pFile == NULL)
    {
        return false;
    }
    int ret = fwrite(p_pBuff, 1, p_nLen, pFile);
    if (ret<p_nLen) 
    {
        fclose(pFile);
    }
    fflush(pFile);
    fclose(pFile);
    return true;
}
bool readFileSync(const char* p_pszFile, Buffer& p_buf, int p_nEncode)
{
    if (!p_pszFile)
        return false;
    FILE* pf = NULL;
    pf = fopen(p_pszFile, "rb");
    if (pf == NULL)
        return false;
    fseek(pf, 0, SEEK_END);
    int len = ftell(pf);
    bool bAsText = p_nEncode != Buffer::raw;
    fseek(pf, 0, SEEK_SET);
    p_buf.create(len + (bAsText ? 1 : 0));
    int readlen = fread(p_buf.m_pPtr, 1, len, pf);
    if (readlen != len) {
        fclose(pf);
        p_buf.destory();
        return false;
    }
    fclose(pf);
    if (bAsText)
        p_buf.m_pPtr[len] = 0;
    return true;
}
//------------------------------------------------------------------------------
Buffer::Buffer()
{
    m_bNeedDel = false;
    m_pPtr = 0;
    m_nLen = 0;
}
Buffer::Buffer(int p_nSize)
{
    m_nLen = p_nSize;
    m_pPtr = new char[p_nSize];
    m_bNeedDel = true;
}
Buffer::Buffer(void* p_ptr, int p_nLength, bool p_bCpy, bool p_bManage)
{
    m_bNeedDel = p_bManage;
    if (p_bCpy)
    {
        m_bNeedDel = true;
    }
    m_pPtr = (char*)p_ptr;
    m_nLen = p_nLength;
}
Buffer::Buffer(char* p_pStr, encode p_end)
{
}
Buffer::~Buffer()
{
    if (m_bNeedDel)
        delete[] m_pPtr;
    m_pPtr = 0;
}
bool Buffer::create(int p_nLen)
{
    m_nLen = p_nLen;
    if (m_pPtr && m_bNeedDel)
    {
        delete[] m_pPtr;
    }
    m_pPtr = new char[m_nLen];
    m_bNeedDel = true;
    return m_pPtr != 0;
}
void Buffer::destory()
{
    m_nLen = 0;
    if (m_bNeedDel)
    {
        m_bNeedDel = false;
        delete[] m_pPtr;
        m_pPtr = 0;
    }
}
bool Buffer::toString(encode p_enc)
{
    return false;
}


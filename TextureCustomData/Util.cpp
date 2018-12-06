#include "stdafx.h"
#include "Util.h"
#include "assert.h"
#include "stdint.h"
#include "string.h"

bool readJson(const char* file, char*& buffer)
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
        fclose(pf);
        delete[] buffer;
        buffer = nullptr;
        return false;
    }
    fclose(pf);
    buffer[len] = 0;
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


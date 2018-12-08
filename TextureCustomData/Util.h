#pragma once
class Buffer
{
public:
    enum encode
    {
        raw = 0,		//原样。一般不是文本的。
        gbk = 1,		//是文本的。是gbk编码的
        utf8 = 2,		//是文本的。是utf8编码的。
    };

    Buffer();
    Buffer(int p_nSize);
    Buffer(void* p_ptr, int p_nLength, bool p_bCpy, bool p_bManage);
    Buffer(char* p_pStr, encode p_end);
    ~Buffer();
    bool create(int p_nLen);
    void destory();
    bool toString(encode p_enc);

public:
    char*			m_pPtr;
    char*			m_pCurPtr;
    int				m_nLen : 30;
    int				m_bAlign4 : 1;		//每次操作都按照4字节对齐？
    int				m_bNeedDel : 1;
};
bool readJson(const char* file, char*& buffer, size_t& bufferLength);
bool readFileSync(const char* p_pszFile, Buffer& p_buf, int p_nEncode = Buffer::raw);
bool writeFileSync(const char* p_pszFile, char* p_pBuff, int p_nLen, int p_nEncode = Buffer::raw);
#pragma once
#include <vector>

#define LAYA_MARKER  (JPEG_APP0 + 2)//ICC
#define LAYA_PROFILE  "LAYA_PROFILE"
#define LayaMin(x,y)  (((x) < (y)) ? (x) : (y))

struct ImageJPEG
{
    int width = 0;
    int height = 0;
    char* buffer = nullptr;
    ~ImageJPEG()
    {
        if (buffer)
        {
            delete[] buffer;
            buffer = nullptr;
        }
    }
};
/*quality 0 -- 100*/
void write_JPEG_file(char * filename, int quality, const ImageJPEG& image, char* data);
int read_JPEG_file(char * filename, ImageJPEG& image);

class JPEG
{
public:
    JPEG();
    void Read(const char* file);
    ~JPEG();
private:
    std::vector<char*> m_Chunks;
};

#pragma once
#include <vector>

struct ImagePNG
{
    int width = 0;
    int height = 0;
    char* buffer = nullptr;

    int bit_depth;
    int color_type; 
    int interlace_method;
    int compression_method;
    int filter_method;

    ~ImagePNG()
    {
        if (buffer)
        {
            delete[] buffer;
            buffer = nullptr;
        }
    }
};
bool ReadPNG(char *fileName, ImagePNG& image/*, unsigned char* data, int dataSize*/);
bool WritePNG(const ImagePNG& image, const char* p_pszFile, const char* userString, int compression_level = 0);//0-9 Ω®“È3-6
void AddLayaTrunkPNG(const char* inFile, const char* outFile, const char* userData, size_t userDataLength);
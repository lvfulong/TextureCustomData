// TextureCustomData.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "assert.h"
#include "stdint.h"
#include "string.h"
#include <PVRTexture.h>
#include <PVRTextureDefines.h>
#include "PVR.h"
#include "JPEG.h"
#include "PNG.h"
#include "Util.h"
#include<filesystem>  
/*
pArgv[1] inputPath
pArgv[2] outputPath
pArgv[3] customerStringData
*/

enum ImageType
{
    ImgType_unknow,
    ImgType_jpeg,
    ImgType_png,
    ImgType_pvrv3,
};

ImageType getImgType(const char* buffer, int length) {
    static int jpegID = 0x00ffd8ff;
    static int pngID = 0x474e5089;

    int idval = *(int*)buffer;
    if (idval == pngID)
        return ImgType_png;
    else if ((idval & 0xffffff) == jpegID)
        return ImgType_jpeg;
    else if (isPvrV3(buffer, length)) 
        return ImgType_pvrv3;
    return ImgType_unknow;
}

int main(int iArgc, char* pArgv[])
{
    if (iArgc < 5)
    {
        printf("lack arguments \n");
        return 1;
    }
    char* inputFilePath = pArgv[1];
    char* outputFilePath = pArgv[2];
    char* textFilePath = pArgv[3];

    int jpeg_quality = atoi(pArgv[4]);//75
    if (jpeg_quality < 0 || jpeg_quality > 100)
    {
        printf("jpeg_quality should be [0,100]\n");
        return 1;
    }

    printf("proccessing... %s %s %s %s \n", pArgv[1], pArgv[2], pArgv[3], pArgv[4]);

    Buffer fileBuffer;
    if (!readFileSync(inputFilePath, fileBuffer))
    {
        printf("open file %s failed\n", inputFilePath);
        return 1;
    }
    ImageType type = getImgType(fileBuffer.m_pPtr, fileBuffer.m_nLen);
    if (type == ImgType_pvrv3)
    {
       
        pvrtexture::CPVRTexture inputTexture(inputFilePath);
        pvrtexture::CPVRTexture outputTexture(inputTexture.getHeader(), inputTexture.getDataPtr());

        if (inputTexture.hasMetaData(0x4c415941, 0))//LAYA
        {
            assert(!outputTexture.hasMetaData(0x4c415941, 0));
            outputTexture.addMetaData(inputTexture.getMetaData(0x4c415941, 0));
            assert(outputTexture.hasMetaData(0x4c415941, 0));
        } 
        
        char* buffer = nullptr;
        size_t bufferLength = 0;
        if (readJson(textFilePath, buffer, bufferLength))
        {
            if (buffer && bufferLength > 0)
            {
                MetaDataBlock MetaBlock;
                MetaBlock.DevFOURCC = 0x4c415941;//LAYA
                MetaBlock.u32Key = 1;
                MetaBlock.u32DataSize = bufferLength + 1;
                MetaBlock.Data = (PVRTuint8*)buffer;
                outputTexture.addMetaData(MetaBlock);
                buffer = nullptr;
            }
        }
        outputTexture.saveFile(CPVRTString(outputFilePath, strlen(outputFilePath)));

        std::tr2::sys::path inputPath = (inputFilePath);
        std::string fileExtension = inputPath.extension().string();
        if (fileExtension != ".pvr")
        {
            if (std::tr2::sys::exists(outputFilePath))
            {
                std::tr2::sys::remove(outputFilePath);
            }
            std::string realOutputFile(outputFilePath);
            realOutputFile += ".pvr";
            std::rename(realOutputFile.c_str(), outputFilePath);
        }
        else
        {
            printf("open file %s failed\n", textFilePath);
        }
        if (buffer)
        {
            delete[] buffer;
        }
    }
    else if (type == ImgType_jpeg)
    {
        char* buffer = nullptr;
        size_t bufferLength = 0;
        if (readJson(textFilePath, buffer, bufferLength))
        {
            ImageJPEG image;
            if (read_JPEG_file(inputFilePath, image))
            {
                write_JPEG_file(outputFilePath, jpeg_quality, image, buffer, bufferLength + 1);
            }
        }
        else
        {
            printf("open file %s failed\n", textFilePath);
        }
        if (buffer)
        {
            delete[] buffer;
        }
    }
    else if (type == ImgType_png)
    {
        char* buffer = nullptr;
        size_t bufferLength = 0;
        if (readJson(textFilePath, buffer, bufferLength))
        {
            AddLayaTrunkPNG(inputFilePath, outputFilePath, buffer, bufferLength + 1);
            /*ImagePNG image;
            if (ReadPNG(inputFilePath, image))
            {
                WritePNG(image, outputFilePath, buffer, png_compression_level);
            }*/
        }
        else
        {
            printf("open file %s failed\n", textFilePath);
        }
        if (buffer)
        {
            delete[] buffer;
        }
    }
    return 0;
}


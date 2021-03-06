#include "PNG.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <png.h>
#include "util.h"
#include "assert.h"
#include <zlib.h>

//static const png_byte mng_LAYA[5] = { 76, 65, 89, 65, (png_byte) '\0' };
static const png_byte mng_LAYA[5] = { 'i',  'T',  'X', 't', (png_byte) '\0' };

void user_error_fn(png_structp png_ptr, png_const_charp error_msg)
{
    printf("read png error");
}

void user_warning_fn(png_structp png_ptr, png_const_charp warning_msg)
{
    printf("read warning");
}

bool ReadPNG(char *fileName, ImagePNG& image)
{
    png_structp png_ptr;
    png_infop   info_ptr;
    unsigned int rowbytes;
    png_uint_32 i;
    png_bytepp row_pointers;

    FILE *fp;

    if ((fp = fopen(fileName, "rb")) == NULL)
        return false;

    if ((png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, user_error_fn, user_warning_fn)) == NULL) {
        fclose(fp);
        return false;
    }
    if ((info_ptr = png_create_info_struct(png_ptr)) == NULL) {
        fclose(fp);
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return false;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        fclose(fp);
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return false;
    }

    png_init_io(png_ptr, fp);
    png_read_info(png_ptr, info_ptr);
    png_set_sig_bytes(png_ptr, 8);
    png_uint_32 width = 0;
    png_uint_32 height = 0;
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &image.bit_depth, &image.color_type, &image.interlace_method, &image.compression_method, &image.filter_method);

    image.width = width;
    image.height = height;

    //if (color_type & PNG_COLOR_MASK_ALPHA)
    //    png_set_strip_alpha(png_ptr);  //去掉alhpa信息了。TODO 以后要加上。
    if (image.bit_depth > 8)
        png_set_strip_16(png_ptr);  //16bit的通道要转换成8bit的
    if (image.color_type == PNG_COLOR_TYPE_GRAY || image.color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png_ptr);
    if (image.color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png_ptr);
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png_ptr);
    // if (color_type == PNG_COLOR_TYPE_RGB)
    png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);//PNG_FILLER_BEFORE);

    png_read_update_info(png_ptr, info_ptr);

    rowbytes = png_get_rowbytes(png_ptr, info_ptr);
    image.buffer = new char[height * rowbytes];
    if ((row_pointers = (png_bytepp)malloc(height * sizeof(png_bytep))) == NULL) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(fp);
        free(row_pointers);
        return false;
    }

    for (i = 0; i < height; i++)
        //row_pointers[*h - 1 - i] = image_data + i*rowbytes;
        row_pointers[i] = (png_bytep)image.buffer + i * rowbytes;

    png_read_image(png_ptr, row_pointers);
    free(row_pointers);
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    fclose(fp);
    return true;
}
bool SavePng(char* png_file_name, const ImagePNG& image, char **data, const char* userString, int compression_level)
{
    png_structp png_ptr;
    png_infop info_ptr;
    FILE *png_file = fopen(png_file_name, "wb");
    //assert(png_file);  
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL)
    {
        //cerr<<"ERROR:png_create_write_struct/n";
        if (png_file)
            fclose(png_file);
        return false;
    }
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL)
    {
        //cerr<<"ERROR:png_create_info_struct/n";
        if (png_file) {
            fclose(png_file);
        }
        png_destroy_write_struct(&png_ptr, NULL);
        return false;
    }
    png_init_io(png_ptr, png_file);
    //png_set_IHDR(png_ptr, info_ptr, image.width, image.height, image.bit_depth, image.color_type, image.interlace_method, image.compression_method, image.filter_method);
    png_set_IHDR(png_ptr, info_ptr, image.width, image.height, 8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    if (compression_level >= 0 && compression_level <= 9)
    {
        png_set_compression_level(png_ptr, compression_level); //0 - 9
    }
    //png_set_IHDR(png_ptr, info_ptr, image.width, image.height, image.bit_depth, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    //png_set_PLTE(png_ptr, info_ptr, palette, palette_len);  
    png_write_info(png_ptr, info_ptr);
    /*-------------------*/

    if (userString)
    {
        int length = strlen(userString);
        if (length > 0)
        {
            png_write_chunk(png_ptr, mng_LAYA, (png_const_bytep)userString, length + 1);
        }
    }

    png_bytepp row_pointers = new png_bytep[image.height];
    for (int i = 0; i < image.height; ++i)
    {
        row_pointers[i] = (png_bytep)data[i];
    }
    png_write_image(png_ptr, row_pointers);
    delete[] row_pointers;
    png_write_end(png_ptr, info_ptr);
    //png_free(png_ptr, palette);  
    //palette=NULL;  
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(png_file);
    return true;
}
bool WritePNG(const ImagePNG& image, const char* p_pszFile, const char* userString, int compression_level)
{
    char** pngline = new char*[image.height];
    for (int i = 0; i < image.height; i++) {
        pngline[i] = (char*)(image.buffer + image.width * 4 * i);
    }
    bool ret = SavePng((char*)p_pszFile, image, pngline, userString, compression_level);
    delete[] pngline;
    return ret;
}
#  define PNG_get_uint_32(buf) \
   (((png_uint_32)(*(buf)) << 24) + \
    ((png_uint_32)(*((buf) + 1)) << 16) + \
    ((png_uint_32)(*((buf) + 2)) << 8) + \
    ((png_uint_32)(*((buf) + 3))))
std::string makeUtf8TxtChunk(const std::string& keyword, const std::string& text)
{
    // Chunk structure: length (4 bytes) + chunk type + chunk data + CRC (4 bytes)
    // Length is the size of the chunk data
    // CRC is calculated on chunk type + chunk data

    // Chunk data format : keyword + 0x00 + compression flag (0x00: uncompressed - 0x01: compressed)
    //                     + compression method (0x00: zlib format) + language tag (null) + 0x00
    //                     + translated keyword (null) + 0x00 + text (compressed or not)

    // Build chunk data, determine chunk type
    std::string chunkData = keyword;
    static const char flags[] = { 0x00, 0x00, 0x00, 0x00, 0x00 };
    chunkData += std::string(flags, 5) + text;
    // Determine length of the chunk data
    uint8_t length[4];
    ul2Data(length, static_cast<uint32_t>(chunkData.size()), bigEndian);
    // Calculate CRC on chunk type and chunk data
    std::string chunkType = "iTXt";
    std::string crcData = chunkType + chunkData;
    uLong tmp = crc32(0L, Z_NULL, 0);
    tmp = crc32(tmp, (const Bytef*)crcData.data(), static_cast<uInt>(crcData.size()));
    uint8_t crc[4];
    ul2Data(crc, tmp, bigEndian);
    // Assemble the chunk
    return std::string((const char*)length, 4) + chunkType + chunkData + std::string((const char*)crc, 4);

}
void AddLayaTrunkPNG(const char* inFile, const char* outFile, const char* userData, size_t userDataLength)
{
    if (inFile == nullptr || strlen(inFile) == 0)
    {
        printf("inFile file invalid");
        return;
    }
    if (outFile == nullptr || strlen(outFile) == 0)
    {
        printf("outFile file invalid");
        return;
    }

    if (userData == nullptr || userDataLength <= 0)
    {
        return;
    }

    Buffer buf;
    if (!readFileSync(inFile, buf))
    {
        printf("Open file %s failed", inFile);
        return;
    }

    std::string trunk = makeUtf8TxtChunk("LAYA", std::string(userData, userDataLength));


    char* IHDR = buf.m_pPtr + 8;
    uint32_t IHDRLength = PNG_get_uint_32(IHDR)  + 4 * 3;
   
    size_t layaTrunkLength = trunk.size();

    size_t bufferLength = buf.m_nLen + layaTrunkLength;
    unsigned char* buffer = new unsigned char[bufferLength];
    memcpy(buffer, buf.m_pPtr, IHDRLength + 8);

    unsigned char* layaTrunk = buffer + IHDRLength + 8;

    memcpy(layaTrunk, trunk.c_str(), trunk.size());

    memcpy(buffer + 8 + IHDRLength + layaTrunkLength, IHDR + IHDRLength, buf.m_nLen - (IHDRLength + 8));

    if (!writeFileSync(outFile, (char*)buffer, bufferLength))
    {
        printf("Write file %s failed", outFile);
    }
    delete[] buffer;
}

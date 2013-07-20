/*
 * =====================================================================================
 *
 *       Filename:  gzip.c
 *
 *    Description:  gzip uncompress & compress
 *
 *        Version:  1.0
 *        Created:  07/16/2013 03:42:49 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  billowkiller (BK), billowkiller@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "zlib.h"
#include <assert.h>

#define CHUNK 16384

int zcompress(char *compressdata, char *uncompressdata, int size)
{
	int ret, have, compresspoint = 0, uncompresspoint = 0;
	z_stream strm;
	unsigned char out[CHUNK];

	/* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;

    ret = inflateInit2(&strm, 16+MAX_WBITS);
    if (ret != Z_OK)
        return ret;

    /* compress until end of file */
    do
	{
		if(size - uncompresspoint< CHUNK)
		{
			strm.avail_in = size;
		}
		else 
			strm.avail_in = CHUNK;

		if(strm.avail_in == 0)
			break;

		strm.next_in = uncompressdata + uncompresspoint;
		assert(ret != Z_STREAM_ERROR);
		do
		{
			strm.avail_out = CHUNK;
			strm.next_out = out;
			ret = deflate(&strm, Z_NO_FLUSH);
			switch(ret)
			{
				case Z_NEED_DICT:
					ret = Z_DATA_ERROR;
				case Z_DATA_ERROR:
				case Z_MEM_ERROR:
					(void)deflateEnd(&strm);
					return ret;
			}
			have = CHUNK - strm.avail_out;

			memcpy(compressdata+compresspoint, out, have);
			compresspoint += have;
		}while(strm.avail_out == 0);

		uncompresspoint += CHUNK;
	}while(ret != Z_STREAM_END);
	
	(void)deflateEnd(&strm);
	return ret = Z_STREAM_END ? Z_OK : Z_DATA_ERROR;

}

int decompress(char *compressdata, char *uncompressdata, int size)
{
    int ret, have, compresspoint = 0, uncompresspoint = 0;
    z_stream strm;
	unsigned char out[CHUNK];

    /* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    
    ret = inflateInit2(&strm, 16+MAX_WBITS);
    if(ret != Z_OK)
		return ret;

	do
	{
		if(size - compresspoint< CHUNK)
		{
			strm.avail_in = size;
		}
		else 
			strm.avail_in = CHUNK;

		if(strm.avail_in == 0)
			break;

		strm.next_in = compressdata + compresspoint;
		assert(ret != Z_STREAM_ERROR);
		do
		{
			strm.avail_out = CHUNK;
			strm.next_out = out;
			ret = inflate(&strm, Z_NO_FLUSH);
			switch(ret)
			{
				case Z_NEED_DICT:
					ret = Z_DATA_ERROR;
				case Z_DATA_ERROR:
				case Z_MEM_ERROR:
					(void)inflateEnd(&strm);
					return ret;
			}
			have = CHUNK - strm.avail_out;

			memcpy(uncompressdata+uncompresspoint, out, have);
			uncompresspoint += have;
		}while(strm.avail_out == 0);

			compresspoint += CHUNK;
	}while(ret != Z_STREAM_END);
	
	(void)inflateEnd(&strm);
	return ret = Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}

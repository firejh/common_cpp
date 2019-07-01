#include <string>
#include <iostream>
#include <memory>
#include <assert.h>
#include "zlib.h"
 
#define CHUNK 16384

int CompressString(const char* in_str,size_t in_len,
  std::string& out_str, int level)
{
  if(!in_str)
    return Z_DATA_ERROR;
 
  int ret, flush;
  unsigned have;
  z_stream strm;
 
  unsigned char out[CHUNK];
 
  /* allocate deflate state */
  strm.zalloc = Z_NULL;
  strm.zfree = Z_NULL;
  strm.opaque = Z_NULL;
  ret = deflateInit(&strm, level);
  if (ret != Z_OK)
    return ret;
 
  std::shared_ptr<z_stream> sp_strm(&strm,[](z_stream* strm){
     (void)deflateEnd(strm);
  });
  const char* end = in_str+in_len;
 
  size_t pos_index = 0;
  size_t distance = 0;
  /* compress until end of file */
  do {
    distance = end - in_str;
    strm.avail_in = (distance>=CHUNK)?CHUNK:distance;
    strm.next_in = (Bytef*)in_str;
 
    // next pos
    in_str+= strm.avail_in;
    flush = (in_str == end) ? Z_FINISH : Z_NO_FLUSH;
 
    /* run deflate() on input until output buffer not full, finish
      compression if all of source has been read in */
    do {
      strm.avail_out = CHUNK;
      strm.next_out = out;
      ret = deflate(&strm, flush);  /* no bad return value */
      if(ret == Z_STREAM_ERROR)
        break;
      have = CHUNK - strm.avail_out;
      out_str.append((const char*)out,have);
    } while (strm.avail_out == 0);
    if(strm.avail_in != 0);   /* all input will be used */
      break;
 
    /* done when last data in file processed */
  } while (flush != Z_FINISH);
  if(ret != Z_STREAM_END) /* stream will be complete */
    return Z_STREAM_ERROR;
 
  /* clean up and return */
  return Z_OK;
}

int DecompressString(const char* in_str,size_t in_len, std::string& out_str)
{
  if(!in_str)
    return Z_DATA_ERROR;
 
  int ret;
  unsigned have;
  z_stream strm;
  unsigned char out[CHUNK];
 
  /* allocate inflate state */
  strm.zalloc = Z_NULL;
  strm.zfree = Z_NULL;
  strm.opaque = Z_NULL;
  strm.avail_in = 0;
  strm.next_in = Z_NULL;
  ret = inflateInit(&strm);
  if (ret != Z_OK)
    return ret;
 
  std::shared_ptr<z_stream> sp_strm(&strm,[](z_stream* strm){
     (void)inflateEnd(strm);
  });
 
  const char* end = in_str+in_len;
 
  size_t pos_index = 0;
  size_t distance = 0;
 
  int flush = 0;
  /* decompress until deflate stream ends or end of file */
  do {
    distance = end - in_str;
    strm.avail_in = (distance>=CHUNK)?CHUNK:distance;
    strm.next_in = (Bytef*)in_str;
 
    // next pos
    in_str+= strm.avail_in;
    flush = (in_str == end) ? Z_FINISH : Z_NO_FLUSH;
 
    /* run inflate() on input until output buffer not full */
    do {
      strm.avail_out = CHUNK;
      strm.next_out = out;
      ret = inflate(&strm, Z_NO_FLUSH);
      if(ret == Z_STREAM_ERROR) /* state not clobbered */
        break;
      switch (ret) {
      case Z_NEED_DICT:
        ret = Z_DATA_ERROR;   /* and fall through */
      case Z_DATA_ERROR:
      case Z_MEM_ERROR:
        return ret;
      }
      have = CHUNK - strm.avail_out;
      out_str.append((const char*)out,have);
    } while (strm.avail_out == 0);
 
    /* done when inflate() says it's done */
  } while (flush != Z_FINISH);
 
  /* clean up and return */
  return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}

int main()
{
    std::string result;
    std::string test = "1111 aaaaaa 123456abcdfdddbdfdfererereee11111111111111111111111345545566666666666666666bbbb cccc";

    std::cout << "test len = " << test.size() << "\n";

    CompressString(test.c_str(), test.size(), result, 1);

    std::cout << "compress len = " << result.size() << "\n";
}

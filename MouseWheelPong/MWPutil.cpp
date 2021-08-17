#include "MWPutil.h"

static int seed1 = 333;
static int seed2 = 42;
static int seed3 = 1800;


void setMWPSeeds(int s1, int s2, int s3)
{
	seed1 = s1;
	seed2 = s2;
	seed3 = s3;
}


double MWPrand()
{
	seed1 = (171 * seed1) % 30269;
	seed2 = (172 * seed2) % 30307;
	seed3 = (170 * seed3) % 30323;

	return fmod((seed1 / 30269.0) + (seed2 / 30307.0) + (seed3 / 30323.0),1);
}


const uint64_t MAX_BIT_STREAM_CHUNKS = 100;
uint8_t littleEndian = 0;


//Values in OTF file are in big endian, so need to see if machine is little endian or big endian
//probably for future performance can just assume all machines are Little-Endian or take care of the differences in preprocessor. So it is not done at runtime with ternary
//maybe even would be cool to create an installer that installed the correctly compiled endianess code for your PC. The different versions would be compiled with the preprocessor and stored on an install disk together (images only stored once, while varied binaries stored differently).
//maybe would be even smart to have a special file format for little endian machines so you wouldn't even need to convert byte order
uint8_t testByteOrder()
{
	uint32_t x = 1;
	littleEndian = (int)(((char*)&x)[0]);
	return littleEndian;
}

inline
uint64_t converByteOrder64(uint8_t* in32)
{
	return (littleEndian ? (uint64_t)in32[0] << 56 | (uint64_t)in32[1] << 48 | (uint64_t)in32[2] << 40 | (uint64_t)in32[3] << 32 | (uint64_t)in32[4] << 24 | (uint64_t)in32[5] << 16 | (uint64_t)in32[6] << 8 | (uint64_t)in32[7] << 0 : *(uint64_t*)in32);
}


inline
uint32_t converByteOrder32(uint8_t* in32)
{
	uint32_t tmp = (littleEndian ? in32[0] << 24 | in32[1] << 16 | in32[2] << 8 | in32[3] << 0 : *(uint32_t*)in32);
	return tmp;
}


inline
uint16_t converByteOrder16(uint8_t* in32)
{
	uint16_t tmp = (littleEndian ? in32[0] << 8 | in32[1] << 0 : *(uint16_t*)in32);
	return tmp;
}


char cmpCharTag(const char* arr, const char* word)
{
	if (arr[0] != word[0] || arr[1] != word[1] || arr[2] != word[2] || arr[3] != word[3])
		return 0;
	return 1;
}

struct chunk
{
	uint32_t length; //length of chunk segment
	union {
		uint32_t chunkType;
		uint8_t chunkTypeChar[4]; //chunk tag, for example 'IHDR'
	};
	void* chunkSegment; //actual chunk
	uint32_t crc; //crc for chunk tag and chunk segment
};

struct PNGheader
{
	uint32_t width;
	uint32_t height;
	uint8_t bitDepth; //bits per pixel
	uint8_t colorType; //
	uint8_t compressionMethod; //Method 0 is ZLIB "deflate"  
	uint8_t filterMethod; //Only filter method 0 (adaptive filtering with five basic filter types) is defined in this International Standard
	uint8_t interlaceMethod; // Two values are defined in this International Standard: 0 (no interlace) or 1 (Adam7 interlace)
};

struct chunk readChunk(FILE* fp)
{
	struct chunk c;
	fread(&c, sizeof(uint32_t), 2, fp);
	c.length = converByteOrder32((uint8_t*)&(c.length));
	if (c.length != 0)
	{
		c.chunkSegment = malloc(c.length);
		fread(c.chunkSegment, c.length, 1, fp);
	}
	fread(&(c.crc), sizeof(uint32_t), 1, fp);
	return c;
}

void freeChunk(struct chunk c)
{
	if(c.length != 0)
		free(c.chunkSegment);
}


struct ChunkedBitStream
{
	void* bitStreamChunk;
	size_t length;
};

/*
	you will use:
	struct ChunkedBitStream cbs[MAX_BIT_STREAM_CHUNKS];
	int currentBitStreamChunk = 0;
	int numberOfChunksInBitStream = 0;
*/

int AddChunkToBitStream(ChunkedBitStream* cbs,void*chunk, uint32_t chunkLength, int numChunksInBitStream)
{
	cbs[numChunksInBitStream].bitStreamChunk = chunk;
	cbs[numChunksInBitStream].length = chunkLength;
	return numChunksInBitStream + 1;
}


//change to reading cbs by reference if going to need to modify it, but probably don't need to
inline uint8_t ReadAtMost8BitsFromBitStream(uint8_t readAmt, size_t & amountInChunk, size_t & amountInByte, int & currChunk, uint8_t* & chunkPtr, ChunkedBitStream* cbs, uint8_t numDataChunks)
{
	uint8_t shiftAmt = 8 - amountInByte;
	if (readAmt > amountInByte)
	{
		//assuming not more than 8 bits requested
		uint8_t val = (*chunkPtr & (0xFF << shiftAmt)) >> shiftAmt;
		readAmt = readAmt - amountInByte;
		amountInByte = 8 - readAmt;
		--amountInChunk;
		if (amountInChunk <= 0) //maybe ok to be 0? but not negative!
		{
			//get next data chunk
			++currChunk;
			if (currChunk == numDataChunks)
			{
				amountInChunk = 0;
				chunkPtr = NULL;
			}
			else
			{
				amountInChunk = cbs[currChunk].length;
				chunkPtr = (uint8_t*)cbs[currChunk].bitStreamChunk;
			}
		}
		else
		{
			++chunkPtr; //make sure not out of chunk
		}
		return ((*chunkPtr & (0xFF >> amountInByte)) << (8-shiftAmt)) + val;
	}
	else
	{
		amountInByte -= readAmt;
		return (*chunkPtr & ((0xFF>>(8-readAmt)) << shiftAmt)) >> shiftAmt; // can it be optimized?
	}
	
}

//if in network order, maybe just correct the byte order in here?
inline uint16_t ReadAtMost16BitsFromBitStream(uint8_t readAmt, size_t& amountInChunk, size_t& amountInByte, int& currChunk, uint8_t*& chunkPtr, ChunkedBitStream* cbs, uint8_t numDataChunks)
{
	uint16_t val = 0;
	uint8_t initReadAmt = readAmt;
	while (readAmt > amountInByte)
	{
		uint8_t shiftAmt = 8 - amountInByte;
		//assuming not more than 8 bits requested
		val |= ((uint16_t)((*chunkPtr & (0xFF << shiftAmt)) >> shiftAmt)) << initReadAmt-readAmt;
		readAmt = readAmt - amountInByte;
		amountInByte = 8;
		--amountInChunk;
		if (amountInChunk <= 0) //maybe ok to be 0? but not negative!
		{
				//get next data chunk
			++currChunk;
			if (currChunk == numDataChunks)
			{
				amountInChunk = 0;
				chunkPtr = NULL;
			}
			else
			{
				amountInChunk = cbs[currChunk].length;
				chunkPtr = (uint8_t*)cbs[currChunk].bitStreamChunk;
			}
		}
		else
		{
			++chunkPtr; //make sure not out of chunk
		}
	}
	if(readAmt > 0)
	{
		return val+(ReadAtMost8BitsFromBitStream(readAmt, amountInChunk, amountInByte, currChunk, chunkPtr, cbs, numDataChunks)<<(initReadAmt -readAmt));
	}
	return val;
}


int16_t DecodeWithHuffmanFromBitStream(int16_t * tablesym, int16_t * tablecnt, int n, size_t& amountInChunk, size_t& amountInByte, int& currChunk, uint8_t*& chunkPtr, ChunkedBitStream* cbs)
{
	uint16_t code = 0;
	uint8_t mask = (1 << (8 - amountInByte));
	uint16_t count = 0;
	for(int i = 1; i <= n; ++i)
	{
		if(amountInByte == 0)
		{
			--amountInChunk;
			if (amountInChunk <= 0) //maybe ok to be 0? but not negative!
			{
				//get next data chunk
				++currChunk;
				amountInChunk = cbs[currChunk].length;
				chunkPtr = (uint8_t*)cbs[currChunk].bitStreamChunk;
			}
			else
			{
				++chunkPtr; //make sure not out of chunk
			}
			mask = 1;
			amountInByte = 8;
		}
		code |= (*chunkPtr & mask) >> (8-amountInByte);
		--amountInByte;
		if (code < tablecnt[i]+count)//if code is in range of a valid code.
		{
			return tablesym[code];
		}
		mask <<= 1;
		code <<= 1;
		count = (count+ tablecnt[i]) << 1; //repeat what we did for getting start addresses of codes
	}
	return -1;
}


/*
 * Maximums for allocations and loops.  It is not useful to change these --
 * they are fixed by the deflate format.
 */
#define MAXBITS 15              /* maximum bits in a code */
#define MAXLCODES 286           /* maximum number of literal/length codes */
#define MAXDCODES 30            /* maximum number of distance codes */
#define MAXCODES (MAXLCODES+MAXDCODES)  /* maximum codes lengths to read */
#define FIXLCODES 288           /* number of fixed literal/length codes */
#define MAXDIST 32768           /* maximum match distance */

#include <bitset>

void processBitStream(struct PNGheader* header, struct PNG* p, ChunkedBitStream* cbs, uint8_t numDataChunks)
{
	//deflate/inflate compression with a sliding window of at most 32768 bytes
	int currChunk = 0;
	uint64_t currByteInImage = 0;
	if (header->compressionMethod == 0) //remember only IHDR compression method 0 is defined
	{
		uint8_t* chunkPtr = (uint8_t*)cbs[currChunk].bitStreamChunk;
		uint8_t CMF = *chunkPtr;
		uint8_t CM = CMF & 0xF; //CM = 8  denotes the "deflate" compression method with a window size up to 32K. CM = 15 is reserved.
		uint8_t CINFO = CMF >> 4; //For CM = 8, CINFO is the base-2 logarithm of the LZ77 window size, minus eight(CINFO = 7 indicates a 32K window size).
		++chunkPtr;
		uint8_t additionalFlags = *chunkPtr;
		uint8_t FCHECK = additionalFlags & 0x1F;
		uint8_t FDICT = (additionalFlags >> 5) & 0x1; //check if preset dictionary
		uint8_t FLEVEL = additionalFlags >> 6; //compression level
		++chunkPtr;

		

		uint16_t checksum = (((uint16_t)CMF) * 256 + ((uint16_t)additionalFlags)) % 31;//can be a shift instead of a multiplication
		const wchar_t* str = (checksum ? L"FCHECK BAD!" : L"FCHECK set right");

		OutputDebugString((L"CM: "+std::to_wstring(CM) + L" CINFO: " + std::to_wstring(CINFO) + L" FDICT: " + std::to_wstring(FDICT) + L" FLEVEL: " + std::to_wstring(FLEVEL) + L", " + str + L"\n").c_str());
		//test for BFINAL bit

		if (FDICT != 0)
		{
			//do dictionary stuff
		}

		uint8_t BFINAL = *chunkPtr & 0x1;
		uint8_t BTYPE = (*chunkPtr & (0x3 << 1)) >> 1;

		size_t amountInChunk = cbs[currChunk].length - 2;
		size_t amountInByte = 8 - 3;

		do //change to while loop == 0
		{
			OutputDebugString((L"BFINAL: " + std::to_wstring(BFINAL) + L" BTYPE: " + std::to_wstring(BTYPE) + L"\n").c_str());
			switch (BTYPE)
			{
				case 0:
					break;
				case 1:
					break;
				case 2: //Dynamic Huffman
				{
					//maybe amount in chunk, amount in byte, curr chnunk, chunk ptr, and cbs could all have been wrapped in a struct
					uint16_t HLIT = ((uint16_t)ReadAtMost8BitsFromBitStream(5, amountInChunk, amountInByte, currChunk, chunkPtr, cbs, numDataChunks)) + 257; //length of length table
					uint8_t HDIST = ReadAtMost8BitsFromBitStream(5, amountInChunk, amountInByte, currChunk, chunkPtr, cbs, numDataChunks) + 1; //length of distance table
					uint8_t HCLEN = ReadAtMost8BitsFromBitStream(4, amountInChunk, amountInByte, currChunk, chunkPtr, cbs, numDataChunks) + 4; //length of table to decompress length and distance table
					static const uint8_t order[19] = { 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15 }; //from spec
					int16_t lengths[MAXCODES];

					//there is now HCLEN 3-bit code lengths in the bit stream of the decompression table to decompress the 
					//  length and distance table for data. Read them in. They are stored based on the order array from the spec
					for (int i = 0; i < HCLEN; ++i)
						lengths[order[i]] = ReadAtMost8BitsFromBitStream(3, amountInChunk, amountInByte, currChunk, chunkPtr, cbs, numDataChunks);
					for (int i = HCLEN; i < 19; ++i)
						lengths[order[i]] = 0;

					int16_t tablecnt[MAXBITS + 1] = { 0 }, tablesym[MAXLCODES]; /* used to decompress tables, could reuse lencnt to save mem for future program*/
					int16_t lencnt[MAXBITS + 1] = { 0 }, lensym[20000];         /* lencode memory */
					int16_t distcnt[MAXBITS + 1] = { 0 }, distsym[MAXDCODES];       /* distcode memory */

					//Now count the number of code length occurences, like say lengths[2] is 4, increment the 4 bucket
					//should never be the case where bucket 0 has 19 in it. Cause that means all codes don't have lengths
					for (int i = 0; i < 19; ++i)
						++(tablecnt[lengths[i]]);
					//codes are compressed and just stored as lengths. And the spec defines how to recreate the codes from just the length information
					//first calculate the starting code for each length. remember shorter codes have lower numbers when reconstructed, so a 2 bit code is a lower 
					//  number than a 3 bit code and lengths[i] has a lower code number than lengths[i+1]
					int16_t next_code[MAXBITS + 1]; //next_code is start code for that length
					tablecnt[0] = 0;
					next_code[0] = 0;
					for (int i = 1; i <= MAXBITS; ++i) //iterate over all code lengths
						next_code[i] = (next_code[i - 1] + tablecnt[i - 1]) << 1; //codes start off right where they left off for prev code length + num of codes for that code length, but shifted over a column


					//this creates decompression table
					for (int i = 0; i < 19; ++i)
						if (lengths[i] != 0)
							tablesym[(next_code[lengths[i]])++] = i;

					for (int i = 0; i < HLIT + HDIST; ) //now decode length and distance tables using tablesym as huffman symbols
					{
						uint8_t val = DecodeWithHuffmanFromBitStream(tablesym, tablecnt, 19, amountInChunk, amountInByte, currChunk, chunkPtr, cbs);
						switch (val)
						{
						case 0:case 1:case 2: case 3:case 4:
						case 5:case 6:case 7: case 8:case 9:
						case 10:case 11:case 12: case 13:case 14:case 15:
						{
							lengths[i] = val;
							++i;
						} break;
						case 16: //RLE decoded symbol
						{
							uint8_t repeatAmt = ReadAtMost8BitsFromBitStream(2, amountInChunk, amountInByte, currChunk, chunkPtr, cbs, numDataChunks) + 3;
							for (int cpy = 0; cpy < repeatAmt; ++cpy)
								lengths[i + cpy] = lengths[i - 1];
							i += repeatAmt;
						} break;
						case 17: //RLE decoded symbol
						{
							uint8_t repeatAmt = ReadAtMost8BitsFromBitStream(3, amountInChunk, amountInByte, currChunk, chunkPtr, cbs, numDataChunks) + 3;
							for (int cpy = 0; cpy < repeatAmt; ++cpy)
								lengths[i + cpy] = 0;
							i += repeatAmt;
						} break;
						case 18: //RLE decoded symbol
						{
							uint8_t repeatAmt = ReadAtMost8BitsFromBitStream(7, amountInChunk, amountInByte, currChunk, chunkPtr, cbs, numDataChunks) + 11;
							for (int cpy = 0; cpy < repeatAmt; ++cpy)
								lengths[i + cpy] = 0;
							i += repeatAmt;
						} break;
						default:
						{
							OutputDebugStringA("ERROR CORRUPTED PNG");
						} break;

						}
					}

					//reconstruct length table
					for (int i = 0; i < HLIT; ++i)
					{
						++(lencnt[lengths[i]]);
					}
					lencnt[0] = 0;
					next_code[0] = 0;
					for (int i = 1; i <= MAXBITS; ++i)
					{
						next_code[i] = (next_code[i - 1] + lencnt[i - 1]) << 1;
					}

					for (int i = 0; i < HLIT; ++i)
					{
						if (lengths[i] != 0)
						{
							lensym[(next_code[lengths[i]])++] = i; //probably better way of storing in table, maybe not shift when creating next codes, but then do shifts in the decode
						}
					}
					//reconstruct distance table
					for (int i = 0; i < HDIST; ++i)
					{
						++(distcnt[lengths[i + HLIT]]);
					}
					distcnt[0] = 0;
					next_code[0] = 0;
					for (int i = 1; i <= MAXBITS; ++i)
					{
						next_code[i] = (next_code[i - 1] + distcnt[i - 1]) << 1;
					}

					for (int i = 0; i < HDIST; ++i)
					{
						if (lengths[i + HLIT] != 0)
						{
							distsym[(next_code[lengths[i + HLIT]])++] = i;
						}
					}

					//table are defined in spec, for lengths with ranges supply base length in lens and get the extra range bits from reading ExtraLengthBits amt
					static const int16_t lens[29] = { /* Size base for length codes 257..285 */
						3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31,
						35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258 };
					static const int16_t ExtraLengthBits[29] = { /* Extra bits for length codes 257..285 */
						0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
						3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0 };
					static const int16_t dists[30] = { /* Offset base for distance codes 0..29 */
						1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193,
						257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145,
						8193, 12289, 16385, 24577 };
					static const int16_t ExtraDistBits[30] = { /* Extra bits for distance codes 0..29 */
						0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6,
						7, 7, 8, 8, 9, 9, 10, 10, 11, 11,
						12, 12, 13, 13 };

					uint16_t unfilteredSymbol;
					do
					{
						unfilteredSymbol = DecodeWithHuffmanFromBitStream(lensym, lencnt, HLIT, amountInChunk, amountInByte, currChunk, chunkPtr, cbs);
						if(unfilteredSymbol < 256)
						{
							//output symbol here since it is a literal
							p->data[currByteInImage] = unfilteredSymbol;
							++currByteInImage;
						}
						else if(unfilteredSymbol > 256) //values 257..285 represent length codes (possibly in conjunction with extra bits following the symbol code)
						{
							//unfilteredSymbol represents the index to the lens and ExtraLengthBits table to reconstruct the length
							unfilteredSymbol -= 257;
							int16_t length = lens[unfilteredSymbol] + ReadAtMost8BitsFromBitStream(ExtraLengthBits[unfilteredSymbol], amountInChunk, amountInByte, currChunk, chunkPtr, cbs, numDataChunks);
							
							uint16_t distIndexSymbol = DecodeWithHuffmanFromBitStream(distsym, distcnt, HDIST, amountInChunk, amountInByte, currChunk, chunkPtr, cbs);
							uint16_t dist = dists[distIndexSymbol] + ReadAtMost16BitsFromBitStream(ExtraDistBits[distIndexSymbol], amountInChunk, amountInByte, currChunk, chunkPtr, cbs, numDataChunks);
							uint8_t dat = p->data[currByteInImage - dist];
							for (int i = 0; i < length; ++i)
							{
								p->data[currByteInImage] = dat;
								++currByteInImage;
							}
								

						}
					} while (unfilteredSymbol != 256);

					break;

				} break;
				case 3:
				{
					OutputDebugStringA("ERROR CORRUPTED PNG");
					return;
				} break;

			}


			break;//remove

		} while (BFINAL == 0);
	}
}

//for future parser, just read entire file at once. Will be faster and won't be continuosly allocing and deallocing chunks
struct PNG loadPNG(const char* filename)	
{
	struct PNG p = { 0 };
	FILE* file = fopen(filename, "rb");
	if (!file)
	{
		printf("ERROR in opening font file: %s\n", filename);
		return p;
	}

	//Allow for reading on big endian or little endian machines
	//this will set a global variable littleEndian
	testByteOrder();
	uint64_t signature;
	fread(&signature, sizeof(uint64_t), 1, file);
	//All values are are greater than a byte in PNG are in Network Byte Order (Big endian), so we must convert to machine endian
	signature = converByteOrder64((uint8_t*)&signature);
	//We will check to make sure PNG has correct signature (every PNG starts with this signature)
	if (signature != 0x89504E470D0A1A0A)
	{
		printf("Error in PNG header tag: %x\n", signature);
		return p;
	}
	//640x711 bit depth 24
	struct chunk c;
	struct PNGheader head;
	c = readChunk(file);
	//Every PNG file starts with IHDR if not then file is corrupt
	if (c.chunkType != FOURCC("IHDR"))
	{
		printf("Error reading IHDR chunk!\n");
		freeChunk(c);
		fclose(file);
		return p;
	}
	head = *((struct PNGheader*)c.chunkSegment);
	head.width = converByteOrder32((uint8_t*)&(head.width));
	head.height = converByteOrder32((uint8_t*)&(head.height));
	OutputDebugStringA("\n");
	OutputDebugStringA(filename);
	OutputDebugStringA(":\n");
	OutputDebugString((L"Bit Depth:" + std::to_wstring(head.bitDepth) + L" Color Type:" + std::to_wstring(head.colorType) + L"\n").c_str());
	OutputDebugString((L"Compression Method:" + std::to_wstring(head.compressionMethod) + L" Filter Method:" + std::to_wstring(head.filterMethod) + L"\n").c_str());
	OutputDebugString((L"Interlace Method:" + std::to_wstring(head.interlaceMethod) + L"\n").c_str());

	p.width = head.width;
	p.height = head.height;

	//Change below to malloc correct bit depth for real image (might just leave every channel 8-bits for clean output, so idk)
	p.data = (uint8_t*)malloc(4 * sizeof(char) * p.width * p.height);

	freeChunk(c);
	//check when you read end chunk

	struct ChunkedBitStream cbs[MAX_BIT_STREAM_CHUNKS];
	int currentBitStreamChunk = 0;
	int numberOfChunksInBitStream = 0;

	bool sRGBPresent = false;
	uint8_t renderingIntent; //maybe this has to be handled during bitstream processing?

	uint8_t numDataChunks = 0;
	while (c.chunkType != FOURCC("IEND"))
	{
		c = readChunk(file);

		switch (c.chunkType)
		{
			case FOURCC("sRGB"):
			{
				sRGBPresent = true;
				renderingIntent = *((uint8_t*)c.chunkSegment);
			} break;
			case FOURCC("IDAT"):
			{
				    numberOfChunksInBitStream = AddChunkToBitStream(cbs,c.chunkSegment,c.length,numberOfChunksInBitStream);
					c.length = 0; //stops deallocation of chunk
					++numDataChunks;
			} break;
			default:
				break;
		}

		freeChunk(c);
	}
	OutputDebugString((L"Number of Data Chunks:" + std::to_wstring(numDataChunks) + L"\n").c_str());
	//use numberOfChunksInBitStream
	processBitStream(&head, &p, cbs, numDataChunks);

	//https://www.color.org/icc-1_1998-09.pdf
	//https://www.color.org/ICC-1A_1999-04.PDF
	//try to find full version of https://www.sis.se/api/document/preview/562720/
	if (sRGBPresent)
	{
		switch (renderingIntent)
		{
			case 0:
			{
				//Perceptual - for images preferring good adaptation to the output device gamut at the expense of colorimetric accuracy, such as photographs.
			} break;
			case 1:
			{
				//Relative colorimetric - for images requiring colour appearance matching (relative to the output device white point), such as logos.
			} break;
			case 2:
			{
				//Saturation - for images preferring preservation of saturation at the expense of hue and lightness, such as charts and graphs.
			} break;
			case 3:
			{
				//Absolute colorimetric - for images requiring preservation of absolute colorimetry, such as previews of images destined for a different output device (proofs).
			} break;
			default:
			{
				//error
			} break;
		}
	}


	fclose(file);
	return p;
}
void freePNG(struct PNG p)
{
	free(p.image);
}



struct LinkedList createLinkedList()
{
	struct LinkedList l;
	l.head = NULL;
	l.tail = NULL;
	l.size = 0;
	return l;
}

void addTailLL(struct LinkedList* l, void* elem)
{
	// store the current tail temporarily
	if (l->tail == NULL)
	{
		l->head = (node*)malloc(sizeof(node));
		l->tail = l->head;
		l->tail->next = l->tail;
		l->tail->prev = l->tail;
	}
	else
	{
		node* tempTail = l->tail;

		// init new tail
		l->tail = (node*)malloc(sizeof(node));
		l->tail->prev = tempTail;
		l->tail->next = l->head;
		tempTail->next = l->tail;
		l->head->prev = l->tail;
	}
	l->tail->elem = elem;
	++l->size;
}
uint32_t size(struct LinkedList* l)
{
	return l->size;
}

void deleteLL(struct LinkedList* l, void (*cleanup)(void* ptr))
{
	if (l->head == NULL) return;
	struct node* it = l->head->next;
	while (it != l->head)
	{
		if (cleanup != NULL)
		{
			cleanup(it->elem);
		}
		struct node* tmp = it;
		it = it->next;
		free(tmp);
	}
	if (cleanup != NULL)
	{
		cleanup(l->head->elem);
	}
	free(l->head);
}
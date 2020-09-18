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


uint8_t littleEndian = 0;


//Values in OTF file are in big endian, so need to see if machine is little endian or big endian
uint8_t testByteOrder()
{
	uint32_t x = 1;
	littleEndian = (int)(((char*)&x)[0]);
	return littleEndian;
}

inline
uint64_t converByteOrder64(uint8_t* in32)
{
	uint64_t tmp = (littleEndian ? (uint64_t)in32[0] << 56 | (uint64_t)in32[1] << 48 | (uint64_t)in32[2] << 40 | (uint64_t)in32[3] << 32 | (uint64_t)in32[4] << 24 | (uint64_t)in32[5] << 16 | (uint64_t)in32[6] << 8 | (uint64_t)in32[7] << 0 : *(uint64_t*)in32);
	return tmp;
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
	uint8_t filterMethod; //
	uint8_t interlaceMethod; //
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

void processBitStream(struct PNGheader* header, struct chunk* c)
{
	//deflate/inflate compression with a sliding window of at most 32768 bytes
	if (header->compressionMethod == 0)
	{
		uint8_t* chunkPtr = (uint8_t*) c->chunkSegment;
		uint8_t CMF = *chunkPtr;
		uint8_t CM = CMF & 0xF; //CM = 8  denotes the "deflate" compression method with a window size up to 32K. CM = 15 is reserved.
		uint8_t CINFO = CMF >> 4; //For CM = 8, CINFO is the base-2 logarithm of the LZ77 window size, minus eight(CINFO = 7 indicates a 32K window size).
		++chunkPtr;
		uint8_t additionalFlags = *chunkPtr;
		uint8_t FCHECK = additionalFlags & 0x1F;
		uint8_t FDICT = (additionalFlags >> 5) & 0x1; //check if preset dictionary
		uint8_t FLEVEL = additionalFlags >> 6; //compression level
		++chunkPtr;
		
		uint16_t checksum = ((uint16_t)CMF)*256 + ((uint16_t)additionalFlags) % 31;
		const wchar_t* str = (checksum ? L"FCHECK BAD!" : L"FCHECK set right");

		OutputDebugString((std::to_wstring(CM) +L" "+ std::to_wstring(CINFO) + L" " + std::to_wstring(additionalFlags) + L" " + str + L"\n").c_str());
		//test for BFINAL bit
		if((*chunkPtr & (0x1 << 7)) == 0) //change to while loop
		{
			//(*chunkPtr & (0x3 << 6))>>6
			OutputDebugString((std::to_wstring((*chunkPtr & (0x3 << 6)) >> 6)+ L"\n").c_str());
			++chunkPtr;

		}
	}
}

const uint64_t MAX_BIT_STREAM_CHUNKS = 100;
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

void AddChunkToBitStream(ChunkedBitStream* cbs,void*chunk,uint32_t chunkLength, int* numberOfChunksInBitStream)
{
	cbs[*numberOfChunksInBitStream].bitStreamChunk = chunk;
	cbs[*numberOfChunksInBitStream].length = chunkLength;
	(*numberOfChunksInBitStream) = (*numberOfChunksInBitStream)+1;
}

const uint64_t MAX_NUMBER_OF_READABLE_BITS = 64;
uint64_t readBits(ChunkedBitStream* cbs, uint64_t numberOfBits,int * byteNumber,int * bitNumber, int * currentBitStreamChunk, int * numberOfChunksInBitStream)
{
	
	uint64_t bits = 0;
	if (*currentBitStreamChunk < *numberOfChunksInBitStream && MAX_NUMBER_OF_READABLE_BITS >= numberOfBits)
	{

	}
	return bits;
}


struct PNG loadPNG(const char* filename)	
{
	struct PNG p = { 0 };
	FILE* file;
	file = fopen(filename, "rb");
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
	OutputDebugString((L"\n" + std::to_wstring(head.bitDepth) + L" " + std::to_wstring(head.colorType) + L"\n").c_str());
	p.width = head.width;
	p.height = head.height;

	//Change below to malloc correct bit depth for real image (might just leave every channel 8-bits for clean output, so idk)
	p.image = (struct rgb*) malloc(4 * sizeof(char) * p.width * p.height);

	freeChunk(c);
	//check when you read end chunk

	struct ChunkedBitStream cbs[MAX_BIT_STREAM_CHUNKS];
	int currentBitStreamChunk = 0;
	int numberOfChunksInBitStream = 0;

	while (c.chunkType != FOURCC("IEND"))
	{
		c = readChunk(file);

		switch (c.chunkType)
		{
			case FOURCC("IDAT"):
				{
					AddChunkToBitStream(cbs,c.chunkSegment,c.length,&numberOfChunksInBitStream);
					c.length = 0;
				}
				break;
			default:
				break;
		}

		freeChunk(c);
	}

	cbs[numberOfChunksInBitStream].length = 0;



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
	l.head = (node*)malloc(sizeof(node));
	l.tail = l.head;
	l.size = 0;
	l.head->next = NULL;
	l.head->prev = NULL;
	l.head->elem = NULL;
	return l;
}
void addTailLL(struct LinkedList* l, void* elem)
{
		l->tail->elem = elem;
		l->tail->next = (node*)malloc(sizeof(node));
		l->tail->next->prev = l->tail;
		l->tail = l->tail->next;
		l->tail->elem = NULL;
		l->tail->next = NULL;
		++l->size;
}
uint32_t size(struct LinkedList* l)
{
	return l->size;
}

void deleteLL(struct LinkedList* l,void (*cleanup)(void*ptr))
{
	for(struct node* it = l->head; it != l->tail;)
	{
		if (cleanup != NULL)
		{
			cleanup(it->elem);
		}
		struct node* tmp = it;
		it = it->next;
		free(tmp);
	}
	free(l->tail);
}
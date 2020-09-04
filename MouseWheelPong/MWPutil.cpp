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
	uint32_t length;
	union {
		uint32_t chunkType;
		uint8_t chunkTypeChar[4];
	};
	void* chunkSegment;
	uint32_t crc;
};

struct PNGheader
{
	uint32_t width;
	uint32_t height;
	uint8_t bitDepth;
	uint8_t colorType;
	uint8_t compressionMethod;
	uint8_t filterMethod;
	uint8_t interlaceMethod;
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
	//OutputDebugString((std::wstring(1, c.chunkTypeChar[0]) + std::wstring(1, c.chunkTypeChar[1]) + std::wstring(1, c.chunkTypeChar[2]) + std::wstring(1, c.chunkTypeChar[3]) + L"\n").c_str());
	return c;
}

void freeChunk(struct chunk c)
{
	if(c.length != 0)
		free(c.chunkSegment);
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
	signature = converByteOrder64((uint8_t*)&signature);
	if (signature != 0x89504E470D0A1A0A)
	{
		printf("Error in PNG header tag: %x\n", signature);
		return p;
	}
	//640x711 bit depth 24
	struct chunk c;
	struct PNGheader head;
	c = readChunk(file);
	if (!cmpCharTag((const char*)&(c.chunkTypeChar[0]), "IHDR"))
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
	freeChunk(c);
	while (!cmpCharTag((const char *)&(c.chunkTypeChar[0]), "IEND"))
	{
		c = readChunk(file);
		if (cmpCharTag((const char*)&(c.chunkTypeChar[0]), "IDAT"))
		{

		}

		freeChunk(c);
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
	l.head = (node*)malloc(sizeof(node));
	l.tail = l.head;
	l.size = 0;
	l.head->next = NULL;
	l.head->elem = NULL;
	return l;
}
void addTailLL(struct LinkedList* l, void* elem)
{
		l->tail->elem = elem;
		l->tail->next = (node*)malloc(sizeof(node));
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
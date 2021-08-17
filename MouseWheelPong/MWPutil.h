#pragma once
#include "MWPheader.h"

#define FOURCC(String) ((((uint32_t)(String[0]))<<0)|(((uint32_t)(String[1]))<<8)|(((uint32_t)(String[2]))<<16)|(((uint32_t)(String[3]))<<24))

struct node
{
	void* elem;
	node* next;
	node* prev;
};


struct LinkedList
{
	uint32_t size;
	node* head;
	node* tail;
};


struct rgb
{
	unsigned char red;
	unsigned char green;
	unsigned char blue;
	unsigned char alpha;

	rgb(unsigned char Red, unsigned char Green, unsigned char Blue, unsigned char Alpha = 0)
	{
		red = Red;
		green = Green;
		blue = Blue;
		alpha = Alpha;
	}
	rgb()
	{
		red = 0;
		green = 0;
		blue = 0;
		alpha = 0;
	}
};

struct PNG
{
	uint32_t width;
	uint32_t height;
	union
	{
		struct rgb* image;
		uint8_t* data;
	};
};

uint8_t testByteOrder();
inline uint32_t converByteOrder32(uint8_t* in32);
inline uint16_t converByteOrder16(uint8_t* in32);
inline uint64_t converByteOrder64(uint8_t* in32);

struct PNG loadPNG(const char* filename);
void freePNG(struct PNG p);

void setMWPSeeds(int s1, int s2, int s3);
double MWPrand();


struct LinkedList createLinkedList();
void addTailLL(struct LinkedList* l, void* elem);
void* removeTailLL(struct LinkedList* l);
uint32_t size(struct LinkedList* l);
void deleteLL(struct LinkedList* l);
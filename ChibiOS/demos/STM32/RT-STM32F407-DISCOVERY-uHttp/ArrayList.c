#include <stdlib.h>

#include "hal.h"
#include "ITM_trace.h"
#include "chprintf.h"

#include "ArrayList.h"
#include "ArrayUtil.h"

extern ITMStream itm_port;

/*	Static functions  ----------------------------------------------*/
static void printElement(const Element* const);
static void shift(DeviceList* const list, int index, int rooms, Shift dir);
static void wide(DeviceList* const);

void init(DeviceList* const list)
{
	initWithSize(list, 16);
}

void initWithSize(DeviceList* const list, int size)
{
	initWithSizeAndIncRate(list, size, 50);
}

void initWithSizeAndIncRate(DeviceList* const list, int size, int rate)
{
	list->size = size;
	list->increment_rate = rate;
	list->elements = (Element*) chHeapAlloc( NULL, list->size * sizeof(Element));
	chprintf((BaseSequentialStream *)&itm_port, "list->elements: %i\n", *list->elements);
	list->current = -1;
}

void clear(DeviceList* const list)
{
	while(list->current >= 0)
	{
		list->elements[list->current] = (Element){0};
		list->current--;
	}
}

int set(DeviceList* const list, Element e, int index)
{
	if(index <= list->current)
	{
		list->elements[index] = e;
	}
	return 0;
}

Element* get(DeviceList* const list, int index)
{
	if(index <= list->current)
	{
		Element *e = &list->elements[index];
		return e;
	}
	return NULL;
}

int add(DeviceList* const list, Element e)
{
	if(++list->current < list->size)
	{
		chprintf((BaseSequentialStream *)&itm_port, "list->current: %i\n", list->current);
		chprintf((BaseSequentialStream *)&itm_port, "e.data: %i\n", e.data);
		list->elements[list->current] = e;
		chprintf((BaseSequentialStream *)&itm_port, "list->elements[list->current]: %i\n", list->elements[list->current]);
		return 1;
	}else
	{
		wide(list);
		list->elements[list->current] = e;
		return 1;
	}
	return 0;
}

static void wide(DeviceList* const list)
{
	list->size += list->increment_rate;
	Element *newArr = (Element*) calloc(sizeof(Element), list->size);
	arrayCopy(newArr, 0, list->elements, 0, list->current, list->size, sizeof(Element));
	free(list->elements);
	list->elements = newArr;
}

int insert(DeviceList* const list, Element e, int index)
{
	if(index <= list->current && ++list->current < list->size)
	{
		shift(list, index, 1, RIGHT);
		list->elements[index] = e;
		return 1;
	}
	return 0;
}

int lastIndexOf(const DeviceList* const list, Element e)
{
	int index = list->current;
	while(index > -1)
	{
		if(e.data == list->elements[index].data) return(list->current - index);
		index--;
	}
	return 0;
}

int indexOf(const DeviceList* const list, Element e)
{
	int index = 0;
	while(index <= list->current)
	{
		if(e.data == list->elements[index].data) return index;
		index++;
	}
	return 0;
}

int isEmpty(const DeviceList* const list)
{
	return list->current == -1;
}

Element *removeAt(DeviceList* const list, int index)
{
	if(list->current >= index)
	{
		Element *e = &list->elements[index];
		shift(list, index, 1, LEFT);
		list->current--;
		return e;
	}
	return NULL;
}

void print(const DeviceList* const list)
{
	int i;
	chprintf((BaseSequentialStream *)&itm_port, "print list->current: %i\n", list->current);
	for(i = 0; i <= list->current; i++)
	{
		chprintf((BaseSequentialStream *)&itm_port, "print list->elements[%d]: %i\n", i, list->elements[i]);
		Element e = list->elements[i];
		chprintf((BaseSequentialStream *)&itm_port, "print e.data: %i\n", e.data);
		printElement(&e);
	}
	chprintf((BaseSequentialStream *)&itm_port, "\n");
}

void clean(DeviceList* list)
{
	free(list->elements);
}

static void printElement(const Element* const e)
{
	chprintf((BaseSequentialStream *)&itm_port, "print e->data: %i ", e->data);
}

static void shift(DeviceList* const list, int index, int rooms, Shift dir)
{
	if(dir == RIGHT)
	{
		arrayCopy(list->elements, index + 1, list->elements, index, rooms, list->current, sizeof(Element));
	}else //SHIFT
	{
		arrayCopy(list->elements, index, list->elements, index + 1, rooms, list->current, sizeof(Element));
	}
}

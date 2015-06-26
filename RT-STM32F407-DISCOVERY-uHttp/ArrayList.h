/*! \file 	ArrayList.h
	\brief	List all modbus slaves in an array.
	
	Details.
 */
/*! \def 	DeviceElement
	 \brief	contains all parameters of a modbus device
	 
	 Details.
 */

#ifndef	ARRAYLIST_H
#define	ARRAYLIST_H

typedef struct
{
	int data;
}Element;

typedef enum
{
	CONNECTED,
	CONNECTIONLESS
}connection;

typedef struct
{
	int8_t 		address;
	connection 	state;
	uint16_t	TimeOut_request;
	uint16_t	requests;
	uint8_t		function;
	uint16_t	regStart;
	uint16_t	regCount;
	uint8_t		AHU_address;
	uint8_t		HRU_address;	
}DeviceElement;

typedef struct
{
	int current;
	int size;
	int increment_rate;
	Element* elements;
}DeviceList;

typedef enum
{
	RIGHT,
	LEFT
}Shift;

/*! \ingroup arrayList
 * \brief init
 *
 * \param list		ArrayList
 *
 * \return none
 */
void init(DeviceList* const);
 
/*! \ingroup arrayList
 * \brief initWithSize
 *
 * \param list		ArrayList
 * \param size		size of list
 *
 * \return none
 */
void initWithSize(DeviceList* const, int);
 
/*! \ingroup arrayList
 * \brief initWithSizeAndIncRate
 *
 * \param list		ArrayList
 * \param size		size of list
 * \param rate		increment rate
 *
 * \return none
 */
void initWithSizeAndIncRate(DeviceList* const, int, int);

/*! \ingroup arrayList
 * \brief clean
 *
 * \param list		ArrayList
 *
 * \return none
 */
void clean(DeviceList*);

/*! \ingroup arrayList
 * \brief add
 *
 * \param list		ArrayList
 * \param Element	data
 *
 * \return integer
 */
int add(DeviceList* const, Element);

/*! \ingroup arrayList
 * \brief insert
 *
 * \param list		ArrayList
 * \param Element	data
 * \param index		index in list
 *
 * \return integer
 */
int insert(DeviceList* const, Element, int);

/*! \ingroup arrayList
 * \brief removeAt
 *
 * \param list		ArrayList
 * \param index		index in list
 *
 * \return Element
 */
Element* removeAt(DeviceList* const, int);

/*! \ingroup arrayList
 * \brief clear
 *
 * \param list		ArrayList
 *
 * \return none
 */
void clear(DeviceList* const);

/*! \ingroup arrayList
 * \brief set
 *
 * \param list		ArrayList
 * \param Element	data
 * \param index		index in list
 *
 * \return integer
 */
int set(DeviceList* const, Element, int);

/*! \ingroup arrayList
 * \brief get
 *
 * \param list		ArrayList
 * \param index		index in list
 *
 * \return Element
 */
Element* get(DeviceList* const, int);

/*! \ingroup arrayList
 * \brief print
 *
 * \param list		ArrayList
 *
 * \return none
 */
void print(const DeviceList* const);

/*! \ingroup arrayList
 * \brief lastIndexOf
 *
 * \param list		ArrayList
 * \param Element	data
 *
 * \return integer
 */
int lastIndexOf(const DeviceList* const, Element);

/*! \ingroup arrayList
 * \brief indexOf
 *
 * \param list		ArrayList
 * \param Element	data
 *
 * \return integer
 */
int indexOf(const DeviceList* const, Element);

/*! \ingroup arrayList
 * \brief isEmpty
 *
 * \param list		ArrayList
 *
 * \return integer
 */
int isEmpty(const DeviceList* const);

#endif //ARRAYLIST_H

#ifndef	ARRAYUTIL_H
#define	ARRAYUTIL_H

#include <stddef.h>

/*! \ingroup arrayList
 * \brief copy Array
 *
 * \param dest			destination array
 * \param dindex		index to which we will start copying
 * \param src			source array
 * \param sIndex		index from wich we will start copying
 * \param len			number of elements that will be copied from source array
 * \param destArrLen	the length of the destination array
 * \param size			the size of the type of the array
 *
 * \return none
 */
void arrayCopy(void *dest, int dIndex, const void* src, int sIndex, int len, int destArrLen, size_t size);

#endif //ARRAYUTIL_H
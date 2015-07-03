/****************************************************
 *  QUEUE virtual Stream --------------------------->
 *  *************************************************
 *  Describe INTERFACE
 */
 
#define	SIZE_QUEUE	128

/*+++ Good Ol' Q-Style ****/
void Fill_queue(char);
char Empty_queue(void);

/*+++ Virtual Stream ****/
void En_queue(char);
char De_queue(void);

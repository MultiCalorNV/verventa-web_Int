/****************************************************
 *  	________                                            
 *	\_____  \   __ __   ____   __ __   ____       ____  
 *	 /  / \  \ |  |  \_/ __ \ |  |  \_/ __ \    _/ ___\ 
 *	/   \_/.  \|  |  /\  ___/ |  |  /\  ___/    \  \___ 
 *	\_____\ \_/|____/  \___  >|____/  \___  > /\ \___  >
 *		   \__>            \/             \/  \/     \/ 
 */
#include "queue.h"
#include "ch.h"

#include "ITM_trace.h"
#include "chprintf.h"

extern ITMStream itm_port;
 
char Queue[SIZE_QUEUE];

//Queue variables
char WritePos	= 0;
char ReadPos	= 0;
//Virtual Stream variables
int Wrpos		= 0;
int Rdpos		= 0;

void Fill_queue(char Element){

	if(WritePos <= (SIZE_QUEUE - 1)){
		Queue[WritePos] = Element;
		WritePos++;
		chprintf((BaseSequentialStream *)&itm_port, "WritePos: %d\n", WritePos);
	}
	if(ReadPos == (SIZE_QUEUE - 1)) WritePos = (WritePos + 1) % SIZE_QUEUE;
}

char Empty_queue(void){
	char Element;
	
	Element = Queue[ReadPos];
	
	if(ReadPos == (SIZE_QUEUE - 1)) ReadPos = (ReadPos + 1) % SIZE_QUEUE;
	if(ReadPos != WritePos){
		ReadPos++;
		chprintf((BaseSequentialStream *)&itm_port, "ReadPos: %d\n", ReadPos);
	}
	
	return Element;
}

void En_queue(char Element){
	if(Wrpos >= 2 * SIZE_QUEUE){
		Wrpos -= SIZE_QUEUE;
		Rdpos -= SIZE_QUEUE;
	}
	if((Wrpos - Rdpos) <= SIZE_QUEUE){
		Queue[Wrpos % SIZE_QUEUE] = Element;
		Wrpos++;
		chprintf((BaseSequentialStream *)&itm_port, "WritePos: %d\n", Wrpos);
	}
}

char De_queue(void){
	char Element;

	if(Wrpos > Rdpos){
		Element = Queue[Rdpos % SIZE_QUEUE];
		Rdpos++;
		chprintf((BaseSequentialStream *)&itm_port, "ReadPos: %d\n", Rdpos);

		return Element;
	}

	return 0;
}

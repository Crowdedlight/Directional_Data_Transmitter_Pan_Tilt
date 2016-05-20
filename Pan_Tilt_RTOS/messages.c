/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.:
*
* PROJECT....:
*
* DESCRIPTION: See module specification file (.h-file).
*
* Change Log:
*****************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 160512  FMA   Module created.
*
*****************************************************************************/

/***************************** Include files *******************************/
#include "messages.h"
/*****************************    Defines    *******************************/

#define MAX_MSG_SIZE 10

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

enum message_type { SET_TILT, SET_PAN } message;
enum state { CLEAR, RECIEVE, INTERPRET, RESPOND } state;

/*****************************   Functions   *******************************/

void messages_rx_task()
{
/*****************************************************************************
*   Input    :	Data sent from pc to uart0
*   Output   :
*   Function :	Interpt message and do what message says
******************************************************************************/

	INT8U temp;
	INT8U inMessage[MAX_MSG_SIZE];
	INT16U coordinates;
	INT8U pointer;

	state = CLEAR;

	while(1)
	{

		switch(state)
		{
			case CLEAR:
				//clearing variables
				pointer = 0;
				temp = 0;
				state = RECIEVE;

				break;
			case RECIEVE:
				//Recieving msg
				xQueueReceive(uart_rx_queue, &temp, portMAX_DELAY);

				if(temp == ';')
				{
					state = INTERPRET;
				}
				else
				{
					inMessage[pointer] = temp;
					pointer++;
				}


				break;
			case INTERPRET:
				//Read and use message
				pointer = 0;

				if(inMessage[pointer] == 'S' && inMessage[pointer+1] == 'T')
					message = SET_TILT; //Set position message
				else if(inMessage[pointer] == 'S' && inMessage[pointer+1] == 'P')
					message = SET_PAN;  //Datalog Message

				//Isolate coordinates, hardcoded to SET commands
				coordinates = strtoul(&inMessage[2], NULL, 10);

				//Do stuff based on message
				switch(message)
				{
				case SET_TILT:
					//Send to PID queue for tilt
					xQueueSend(pid_tilt_setp_queue, &coordinates, portMAX_DELAY);
					break;
				case SET_PAN:
					//Send to PID queue for pan
					xQueueSend(pid_pan_setp_queue, &coordinates, portMAX_DELAY);
					break;
				default:
					state = CLEAR;
					break;
				}

				state = RESPOND;

				break;
				case RESPOND:
					INT8U o = 'O';
					INT8U k = 'K';
					xQueueSendToBack(uart_tx_queue, &o, 0);
					xQueueSendToBack(uart_tx_queue, &k, 0);
					state = CLEAR;
					break;
			default:
				state = CLEAR;
				break;
		}
	}
}

/****************************** End Of Module *******************************/













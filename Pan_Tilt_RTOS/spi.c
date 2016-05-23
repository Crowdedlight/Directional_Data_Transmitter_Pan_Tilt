/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.: spi.c
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
* 130416  FMA   Module created.
*
*****************************************************************************/

/***************************** Include files *******************************/
#include "spi.h"
#include "EMP/emp_type.h"
#include "EMP/glob_def.h"

/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/
xQueueHandle spi_rx_queue;
xQueueHandle spi_tx_queue;

extern xQueueHandle uart_tx_queue;
/*****************************   Functions   *******************************/

INT32U writeSPI(int enable1, INT16U PAN, int enable2, INT16U TILT)
{
	INT8U first = 0;
	INT8U second = 0;
	INT8U third = 0;
	INT32U rxData = 0;

	// set slaveselect low
	GPIO_PORTD_DATA_R &= ~(1<<1); //ss low => Start Transmission

	SSI3_DR_R = (enable1 << 7) | (0<<6) | ((PAN >> 4) & 0b111111 ) << 0;
	while((SSI3_SR_R & (1<<0)) == 0);

	//Get return data
	//INT32U rxData = (SSI3_DR_R << 16);
	for(int i = 0; i <5; i++);
	first = SSI3_DR_R;

	SSI3_DR_R = ((PAN & 0b1111)<<4) | (enable2 << 3) | (0<<2) | ((TILT >> 8) & 0b11)<<0;
	while((SSI3_SR_R & (1<<0)) == 0);

	//Get return data
	//rxData |= (SSI3_DR_R << 8);
	for(int i = 0; i <5; i++);
	second = SSI3_DR_R;

	SSI3_DR_R = (TILT & 0b11111111);
	while((SSI3_SR_R & (1<<0)) == 0);

	//Get return data
	//rxData |= (SSI3_DR_R << 0);
	for(int i = 0; i <5; i++);
	third = SSI3_DR_R;

	for(int i = 0; i <15; i++);
	GPIO_PORTD_DATA_R |= (1<<1); //end transmission

	//save to queue
	rxData = (first << 16) | (second << 8) | (third);

	return rxData;

	//xQueueSendToBack(spi_rx_queue, &rxData, 10);
}

/*void spi_task()
{
	INT16U msg;
	while(1)
	{

		//If you can recieve element from queue send msg
		xQueueReceive(spi_tx_queue, &msg, portMAX_DELAY);

		//Start transmitting

		//PAN
		INT8U PAN = (msg & 0b0111111100000000)>>8;
		INT8U enPAN = (msg & 0b1000000000000000)>>15;
		//Tilt
		INT8U TILT = msg & 0b01111111;
		INT8U enTILT = (msg & 0b10000000)>>7;

		//Send data
		writeSPI(enPAN, PAN, enTILT, TILT);
	}
}*/


void spi_init()
/*****************************************************************************
*   Input    :
*   Output   :
*   Function : Enable the tx and rx fifos
******************************************************************************/
{
	// 1. Enable the SSI3 module using RCGCSSI register (page 346)
	SYSCTL_RCGCSSI_R = (1<<3); //SSI module 3

	// 2. Enable the clock to the portD GPIO module via the RCGCGPIO register (page 340)
	SYSCTL_RCGCGPIO_R |= (1<<3);//SYSCTL_RCGCGPIO_R1; //PORT D

	// 3. Set GPIO AFSEL bits for appropriate pins (page 671)
	GPIO_PORTD_AFSEL_R |= (1<<0)|(1<<2)|(1<<3); //|(1<<1); //PD0:3
	GPIO_PORTD_AFSEL_R &= ~(1<<1);

	// 4. Configure PMCn fields in the GPIOPCTL register to assign the SSI signals to appropriate pins (page 688, 1351)
	GPIO_PORTD_PCTL_R |= (1<<0)|(1<<4)|(1<<8)|(1<<12);// //PMC 0,1,2,3

	// 5. Program the GPIO_DEN register to enable digital functions for each pin
	GPIO_PORTD_DEN_R |= (1<<0)|(1<<1)|(1<<2)|(1<<3);
	GPIO_PORTD_DIR_R |= (1<<1);

	/// Frame Format

	// 1.Ensure that the SSE bit in SSICR1 register is clear before making any config changes
	SSI3_CR1_R &= ~(1<<1);

	// 2. Select slave or master for the SSI
	SSI3_CR1_R = 0x00000000; //Master

	// 3. Configure the SSI clock source by writing to the SSICC register
	SSI3_CC_R = 0x00;

	// 4. Configure the clock prescaler divisor by writing the SSI_CPSR register
	SSI3_CPSR_R |= 8; //Divisor 8 = 2 Mhz

	// 5. Write the SSI_CR0 register with the following config
	// : Serial clock rate (SCR)
	// : Desired clock phase/polarity, if using Freescale SPI mode (SPH and SPO) (PH = PHase , PO = POlarity)
	// : The Protocol mode: Freescale SPI, TI SSF, MICROWIRE (FRF)
	// : The data size (DSS)
	//		   	  PO=1   PH=1  Data = 8bit.     (Mode 1,1)
	SSI3_CR0_R = (1<<6)|(1<<7)|(0x7<<0);

	// 6. Optionally use uDMA channel.
	// NOPE

	// 7. Enable SSI by setting SSE bit in SSICR1 register
	SSI3_CR1_R |= (1<<1);

	//Set slaveselect high
	GPIO_PORTD_DATA_R |= (1<<1);

	//ColorCode for pins
	//
	// PD0 - clock 								=> HVID
	// PD1 - SlaveSelect 						=> BRUN
	// PD3 - MOSI (Master output Slave Input)	=> GRØN
	// PD2 - MISO (Master input Slave output)	=> ORANGE
	//
	// #################################################
}





/****************************** End Of Module *******************************/













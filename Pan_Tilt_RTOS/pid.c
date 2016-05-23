/*****************************************************************************
* University of Southern Denmark
*
* MODULENAME.: pid.c
*
* PROJECT....: PAN TILT
*
* DESCRIPTION:
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 160504  SSJ   Module created.
*
*****************************************************************************/

/***************************** Include files *******************************/
#include "pid.h"

/*****************************    Defines    *******************************/
// Gain for each subsystem
#define K (1/0.54)
#define PAN_P_GAIN  		(K*6.3) //6.3				//0.005				// Corresponding to Kc
#define PAN_I_GAIN			(K*0.1/10000)		//0.000005					// Corresponding to Kc*Ki
#define	PAN_D_GAIN			(K*1.26)			//0.08/10				// Corresponding to Kc*Kd
#define TILT_P_GAIN 		(K*5)				//0.005				// Corresponding to Kc
#define TILT_I_GAIN			(K*0.1/10000)		//0.000005					// Corresponding to Kc*Ki
#define	TILT_D_GAIN			(K*0.3) //166)			//0.08/10				// Corresponding to Kc*Kd


// Limits for size of error integral
#define MAX_INTEGRAL 100000	// TODO: Check if this should be changed
#define MIN_INTEGRAL -MAX_INTEGRAL

// Maximum allowed outputs from the PID subsystems
#define MAX_P_OUTPUT 0x7FFFFFFF // TODO: Should change to lower value (eg. 100) value to limit each part's output
#define MIN_P_OUTPUT -MAX_P_OUTPUT
#define MAX_I_OUTPUT 0x7FFFFFFF
#define MIN_I_OUTPUT -MAX_I_OUTPUT
#define MAX_D_OUTPUT 0x7FFFFFFF
#define MIN_D_OUTPUT -MAX_I_OUTPUT

// Limits for allowed set-points
#define MAX_PAN_SETP  540+200 // TODO: Check this value
#define MIN_PAN_SETP  540-200
#define MAX_TILT_SETP 1079
#define MIN_TILT_SETP 0

// Zones in which the PWM may operate
#define PAN_DEADZONE   100
#define PAN_DUTY_RANGE (970 - (500 + PAN_DEADZONE)) // Range of duty cycle in which the system function
#define TILT_DEADZONE   100
#define TILT_DUTY_RANGE (970 - (500 + TILT_DEADZONE)) // Range of duty cycle in which the system function

// Times the PID should run before reaching locked state
#define CLOSE_COUNT 40
#define CLOSE_ERROR 0

/*****************************   Constants   *******************************/

/*****************************   Functions   *******************************/
INT16S pan_p_part( INT16S err )
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	// Calculate output
	INT32S output;
	
	//if ( err < 20 && err > -20 )
	//	output = err * PAN_P_LARGE_GAIN;
	//else
		output = err * PAN_P_GAIN;

	if ( output > MAX_P_OUTPUT )
		return MAX_P_OUTPUT;
	else if ( output < MIN_P_OUTPUT )
		return MIN_P_OUTPUT;
	else
		return output;
}

INT16S tilt_p_part( INT16S err )
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	// Calculate output
	INT32S output = err * TILT_P_GAIN;
	
	//if ( err < 20 && err > -20 )
	//	output = err * TILT_P_LARGE_GAIN;
	//else
		output = err * TILT_P_GAIN;

	if ( output > MAX_P_OUTPUT )
		return MAX_P_OUTPUT;
	else if ( output < MIN_P_OUTPUT )
		return MIN_P_OUTPUT;
	else
		return output;
}

INT16S pan_i_part( INT16S err, INT32S * int_err, INT16U dt )
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	// Calculate error integral
	(*int_err) += err * dt; // OBS. dt is in units of ms/10

	// Check if maxed out
	if ( (*int_err) > MAX_INTEGRAL )
		(*int_err) = MAX_INTEGRAL;
	else if ( (*int_err) < MIN_INTEGRAL )
		(*int_err) = MIN_INTEGRAL;

	// Calculate output
	INT32S output = (*int_err) * PAN_I_GAIN;

	if ( output > MAX_I_OUTPUT )
		return MAX_I_OUTPUT;
	else if ( output < MIN_I_OUTPUT )
		return MIN_I_OUTPUT;
	else
		return output;
}

INT16S tilt_i_part( INT16S err, INT32S * int_err, INT16U dt )
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	// Calculate error integral
	(*int_err) += err * dt; // OBS. dt is in units of ms/10

	// Check if maxed out
	if ( (*int_err) > MAX_INTEGRAL )
		(*int_err) = MAX_INTEGRAL;
	else if ( (*int_err) < MIN_INTEGRAL )
		(*int_err) = MIN_INTEGRAL;

	// Calculate output
	INT32S output = (*int_err) * TILT_I_GAIN;

	if ( output > MAX_I_OUTPUT )
		return MAX_I_OUTPUT;
	else if ( output < MIN_I_OUTPUT )
		return MIN_I_OUTPUT;
	else
		return output;
}

INT16S pan_d_part( INT16S err, INT16S prev_err, INT16U dt )
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	// Calculate error derivative
	INT32S err_der = ( ( err - prev_err ) / dt ) * 10000; // OBS. dt is in units of ms/10. Multiply by 10000 to get err_der n counts/second

	// Calculate output
	INT32S output = err_der * PAN_D_GAIN;

	if ( output > MAX_D_OUTPUT )
		return MAX_D_OUTPUT;
	else if ( output < MIN_D_OUTPUT )
		return MIN_D_OUTPUT;
	else
		return output;
}

INT16S tilt_d_part( INT16S err, INT16S prev_err, INT16U dt )
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	// Calculate error derivative
	INT32S err_der = ( ( err - prev_err ) / dt ) * 10000; // OBS. dt is in units of ms/10. Multiply by 10000 to get err_der n counts/second

	// Calculate output
	INT32S output = err_der * TILT_D_GAIN;

	if ( output > MAX_D_OUTPUT )
		return MAX_D_OUTPUT;
	else if ( output < MIN_D_OUTPUT )
		return MIN_D_OUTPUT;
	else
		return output;
}

INT16U pan_calculate_duty ( INT16S err, INT16S * int_err, INT16S prev_err, INT16U dt )
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	INT32S output = 0;

	output += pan_p_part( err );
	output += pan_i_part( err, int_err,  dt );
	output += pan_d_part( err, prev_err, dt );
	output  = pan_convert_to_duty( output );

	return output;
}

INT16U tilt_calculate_duty ( INT16S err, INT16S * int_err, INT16S prev_err, INT16U dt )
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	INT32S output = 0;

	output += tilt_p_part( err );
	output += tilt_i_part( err, int_err,  dt );
	output += tilt_d_part( err, prev_err, dt );
	output  = tilt_convert_to_duty( output );

	return output;
}

INT16U pan_convert_to_duty( INT32S pid_out )
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	// Check if controller is maxed out
	if ( pid_out > 1000 )
		pid_out = 1000;
	else if ( pid_out < -1000 )
		pid_out = -1000;

	// Convert to scale of duty cycle
	INT32S duty = (pid_out * PAN_DUTY_RANGE) / 1000;

	// Shift out of motor's deadzone
	if ( duty > 0 )
		duty += PAN_DEADZONE;
	else if ( duty < 0 )
		duty -= PAN_DEADZONE;

	// Move 0-reference to 500
		duty += 500;

	return duty;
}

INT16U tilt_convert_to_duty( INT32S pid_out )
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	// Check if controller is maxed out
	if ( pid_out > 1000 )
		pid_out = 1000;
	else if ( pid_out < -1000 )
		pid_out = -1000;

	// Convert to scale of duty cycle
	INT32S duty = (pid_out * TILT_DUTY_RANGE) / 1000;

	INT32S debug5 = duty;
	INT32S debug7 = pid_out;

	// Shift out of motor's deadzone
	if ( duty > 0 )
		duty += TILT_DEADZONE;
	else if ( duty < 0 )
		duty -= TILT_DEADZONE;

	// Move 0-reference to 500
		duty += 500;

	return duty;
}

INT16U deg10_to_encoder_counts( INT16U deg10 )
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	deg10 *= 3;
	deg10 /= 10;
	return deg10;
}

INT16U validate_pan_setp( INT16U setp )
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	if ( setp > MAX_PAN_SETP )
		setp = MAX_PAN_SETP;
	else if ( setp < MIN_PAN_SETP )
		setp = MIN_PAN_SETP;

	return setp;
}

INT16U validate_tilt_setp( INT16U setp )
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	if ( setp > MAX_TILT_SETP )
		setp = MAX_TILT_SETP;
	else if ( setp < MIN_TILT_SETP ) // NOTE: This is redundant when MIN_TILT_SETP = 0
		setp = MIN_TILT_SETP;

	return setp;
}

INT16S check_tilt_err_direction( INT16S err )
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	if ( err < -540 )
		err = err + 1080;
	else if ( err > 540 )
		err = err - 1080;

	return err;
}

void controller_task()
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	/********** Create variables ************/
	// State
	enum pid_state_type { CALIBRATE, CONTROL, LOCKED };
	enum pid_state_type pid_state = CALIBRATE;

	// Time passed since last sample
	INT32U dt = 0;

	// Current positions
	INT16U pan_pos  = 0;
	INT16U tilt_pos = 0;

	// Set-points (desired positions)
	INT16U pan_setp  = 640;	// NOTE: 700 is used for calibration
	INT16U tilt_setp = 700;	// NOTE: 700 is used for calibration

	// Current errors
	INT16S pan_err  = 0;
	INT16S tilt_err = 0;

	// Previous errors
	INT16S pan_prev_err  = 0;
	INT16S tilt_prev_err = 0;

	// Integrals of errors
	INT32S pan_int_err  = 0;
	INT32S tilt_int_err = 0;

	// Output from controller
	INT16U pan_duty  = 500;
	INT16U tilt_duty = 500;

	//Callibrate control
	BOOLEAN pan_calibrated  = FALSE;
	BOOLEAN tilt_calibrated = FALSE;
	
	// Close to setp counter
	INT8U pan_close_count  = 0;
	INT8U tilt_close_count = 0;


	/********** Initialize*******************/
	// Set up Timer0
	timer0_setup();

	// Wait till data ready (tilt must be read first)
	//xQueueReceive( pid_tilt_pos_queue, &tilt_pos, portMAX_DELAY );
	//xQueueReceive( pid_pan_pos_queue,  &pan_pos,  portMAX_DELAY );

	// Calculate first errors
	//pan_err  = pan_setp  - pan_pos;
	//tilt_err = tilt_setp - tilt_pos;

	// Start timing
	timer0_start();

	/********** Superloop *******************/
	while (1) {
		// Wait for new position data (tilt must be read first)
		xQueueReceive( pid_tilt_pos_queue, &tilt_pos, portMAX_DELAY );
		xQueueReceive( pid_pan_pos_queue,  &pan_pos,  portMAX_DELAY );
		
		// Check and reset timer
		dt = timer0_read() / 1600;	// Converted to ms/10
		timer0_clear();

		// Calculate errors
		pan_err  = pan_setp - pan_pos;
		tilt_err = tilt_setp - tilt_pos;

		tilt_debug = tilt_setp;
		tilt_err2 = tilt_err;

		switch ( pid_state ) {
			case CALIBRATE:
				// Check if near calibration set-point
				if ( !pan_calibrated && ( pan_err > -10 && pan_err < 10 ) ) {
					pan_duty = 500;
					pan_calibrated = TRUE;
				}
				else if ( !pan_calibrated ) {
					pan_duty = 650;
				}
			
				if ( !tilt_calibrated && ( tilt_err > -10 && tilt_err < 10 ) ) {
					tilt_duty = 500;
					tilt_calibrated = TRUE;
				}
				else if ( !tilt_calibrated ) {
					tilt_duty = 650;
				}
				
				// Send duty cycles to queue (tilt must be send first)
				xQueueSendToBack( pid_tilt_duty_queue, &tilt_duty, portMAX_DELAY );
				xQueueSendToBack( pid_pan_duty_queue,  &pan_duty,  portMAX_DELAY );
		
				// Check if calibrated
				if ( pan_calibrated && tilt_calibrated ) {
					pid_state = CONTROL;
				}
				
				break;
			case CONTROL:			
				// Check which direction is shortest and convert tilt_err accordingly
				tilt_err = check_tilt_err_direction( tilt_err );

				// Calculate controller signal
				pan_duty  = pan_calculate_duty(  pan_err,  &pan_int_err,  pan_prev_err,  dt);
				tilt_duty = tilt_calculate_duty( tilt_err, &tilt_int_err, tilt_prev_err, dt);

				// Update prev_err
				pan_prev_err  = pan_err;
				tilt_prev_err = tilt_err;
				
				// Check if close to set-point
				if ( pan_err <= CLOSE_ERROR && pan_err >= -CLOSE_ERROR ) {
					if ( ++pan_close_count >=  CLOSE_COUNT ) {
						pan_close_count = CLOSE_COUNT;
						pan_duty    = 500;
						pan_int_err = 0;
					}
				}
				else {
					pan_close_count = 0;
				}
				if ( tilt_err <= CLOSE_ERROR && tilt_err >= -CLOSE_ERROR ) {
					if ( ++tilt_close_count >= CLOSE_COUNT ) {
						tilt_close_count = CLOSE_COUNT;
						tilt_duty    = 500;
						tilt_int_err = 0;
					}
				}
				else {
					tilt_close_count = 0;
				}
				if ( pan_close_count == CLOSE_COUNT && tilt_close_count == CLOSE_COUNT ) {
					pid_state = LOCKED;
				}
					
				// Send duty cycles to queue (tilt must be send first)
				xQueueSendToBack( pid_tilt_duty_queue, &tilt_duty, portMAX_DELAY );
				xQueueSendToBack( pid_pan_duty_queue,  &pan_duty,  portMAX_DELAY );

				// Check if new set-point has been received
				if ( xQueueReceive( pid_tilt_setp_queue, &tilt_setp, 0 ) ) {
					tilt_setp = deg10_to_encoder_counts( tilt_setp ); // Convert from 10th of degrees to encoder counts
					//tilt_setp = validate_tilt_setp( tilt_setp ); // Make sure tilt_setp is within the boundary
				}
				if ( xQueueReceive( pid_pan_setp_queue,  &pan_setp,  0 ) ) {
					pan_setp = deg10_to_encoder_counts( pan_setp );  // Convert from 10th of degrees to encoder counts
					//pan_setp = validate_pan_setp( pan_setp ); // Make sure pan_setp is within the boundary
				}
				break;
			case LOCKED:
				// Update prev_err
				pan_prev_err  = pan_err;
				tilt_prev_err = tilt_err;
				
				//Debug
				//INT8U lockuart = 'L';
				//xQueueSendToBack(uart_tx_queue, &lockuart, 0);

				// Check if still close to set-point
				if ( pan_err > CLOSE_ERROR || pan_err < -CLOSE_ERROR || tilt_err > CLOSE_ERROR || tilt_err < -CLOSE_ERROR )
					pid_state = CONTROL;

				// Check if new set-point has been received
				if ( xQueueReceive( pid_tilt_setp_queue, &tilt_setp, 0 ) ) {
					tilt_setp = deg10_to_encoder_counts( tilt_setp ); // Convert from 10th of degrees to encoder counts
					//tilt_setp = validate_tilt_setp( tilt_setp ); // Make sure tilt_setp is within the boundary
				}
				if ( xQueueReceive( pid_pan_setp_queue,  &pan_setp,  0 ) ) {
					pan_setp = deg10_to_encoder_counts( pan_setp );  // Convert from 10th of degrees to encoder counts
					//pan_setp = validate_pan_setp( pan_setp ); // Make sure pan_setp is within the boundary
				}
					
				break;
			default:
				break;
		}
	}
}

/****************************** End Of Module *******************************/

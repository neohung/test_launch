/*
 * NCN_MDK.h
 *
 *  Created on: 2015-6-30
 *      Author: Jake
 *      Vision: v1_1_7
 *      */
/******************************
 *		version:v1_1_0
 * 			1:Modify the spi upload function.
 * 			2:Add timer task.
 * 		version:v1_1_1
 * 			1:Add interface to set the App version.
 * 		version:v1_1_5
 * 			1.Add battery function.
 * 		version:v1_1_6
 * 			1.Change the timer 1 to timer 0
 * 			2.Change the system clock 8M to 16M.
 * 		version:v1_1_7
 * 			1.Use the new command version
 * 		version:v1_1_9
 * 			1.Add low power function
 * 			  New function:1) void LP_flag_set(unsigned char flag)
 * 			  			   2) unsigned char LP_flag_get(void)
 */

#ifndef NCN_MDK_H_
#define NCN_MDK_H_
#ifdef __cplusplus
	extern "C" {
#endif
///* Pins have been used:
// * P1.1	SOMI of SPI
// * P1.2	SIMO of SPI
// * P1.3 Interrupt pin of Module
// * P1.4 CLK of SPI
// * P1.5 CS of SPI
// * P3.5 Control pin for 3V3 switch
// * P3.6 Control pin for 5V switch
// *
// * Interrupts source have been used:
// * 1. USCIAB0RX_VECTOR for SPI
// *
// * */
typedef void (*MY_VOID_UNSINGED_CHAR)(unsigned char data);
#define SPI_READY 0
/*
 * For setup the Nexpaq communication core, and initialize the related hardware */
/*
 * */
void _NCN_System_setup(void);

/*
 * */
void _NCN_System_loop(void);

/*
 * */
unsigned char NCN_System_upload_command(unsigned int cmd, unsigned char *data, unsigned char len);
/*
 * Function:		NCN_System_upload
 * Description:
 * Parameters:		1. 	data 	(unsigned char *)
 * 						The address of the array will be send to phone
 * 					2.	length 	(unsigned char )
 * 						The length of the array will be send to phone
 * Return:			(unsigned char)
 * 					1.SPI BUS is busy
 * 					2.
 * 					3.length is too bigger
 * 					4.SPI upload buff is not empty
 */
unsigned char NCN_System_upload(unsigned char *data, unsigned char len);
/*
 * Function:		NCN_upload_check
 * Description:		Check whether the upload function is ready now.
 * Parameters:		None
 * Return:			(unsigned char)
 * 					0.SPI is ready to sent command
 * 					1.SPI BUS is busy
 * 					4.SPI upload buff is not empty
 */
unsigned char NCN_upload_check(void);
/*
 * Function:		Timer_register_task
 * Description:		Add a new timer task to the task list.The most task number can be 4.
 * Parameters:		1. 	callBack 	((void *)void)
 * 						The address of the call back function that will be call when the counter is finished.
 * 						If there is not is call back function,set this parameter by 0.
 * 					2.	timer		(unsigned int)
 * 						The counter will be set.The unit is MS.The range is (1~65535)
 * 					3.	flag_p		(unsigned char*)
 * 						The address of the variable that will be add by 1 when the counter is finished.
 * 						If there is not a variable,set this parameter by 0.
 * 					If the timer is 0,or the callBack and the flag_p both are 0,this task task will be ignored.
 * Return:			(unsigned char)
 * 					0xFE:	This task is invalid.
 * 					0xFF:	The task fifo is full.
 * 					else:	The task ID.
 */
unsigned char Timer_register_task(void (*callBack)(void),unsigned int timer,unsigned char *flag_p);
/*
 * Function:		Timer_register_task
 * Description:		Cancel a timer task in the task list.The most task number can be 4.
 * Parameters:		1. 	task_ID		(unsigned char)
 * 						The ID of the task which will be cancel.
 * Return:			(unsigned char)
 * 					0x00:	Successful
 * 					0x01:	Failed
 */
unsigned char Timer_cancel_task(unsigned char task_ID);
/*
 * Function:		NCN_version_set
 * Description:		Set the app version.The version can be asked by using command 07 E9
 * Parameters:		1. 	Hbit		(unsigned char)
 * 						The high bit of the version.
 * 					2. 	Mbit		(unsigned char)
 * 						The middle bit of the version.
 * 					3. 	Lbit		(unsigned char)
 * 						The low bit of the version.
 * 					The version is v H_M_L.
 * Return:			NONE
 */
void NCN_version_set(unsigned char Hbit,unsigned char Mbit,unsigned char Lbit);
/*
 * Function:		NCN_USB_event
 * Description:		Sent a request to Station for USB event.Just be used on the modules with USB function.
 * Parameters:		1. 	request		(unsigned char)
 * 						0x00	Ask for connecting.
 * 						0x01	Ask for disconnecting.
 * Return:			NONE
 */
void NCN_USB_event(unsigned char request);
/*
 * Function:		NCN_System_Battery_message
 * Description:		Sent a battery message to Phone.
 * Parameters:		1. 	data		(unsigned char *)
 * 						data[0]		battery status:
 * 									0x01:BAT_CHARGING
 * 									0x02:BAT_DISCHARGING
 * 									0x03:BAT_STANDBY
 * 									0x04:BAT_FULL
 *						data[1].data[2]	The voltage of the battery
 *					2.	len			(unsigned char)
 *						The length of the battery message.Now only this format can be parsed.
 *						So the len need to be set as 3.
 * Return:			NONE
 */
unsigned char NCN_System_Battery_message(unsigned char *data, unsigned char len);
/*
 * Function:		usb_task_set
 * Description:		Set a function as the usb called back function,when the USB command (0x1F00)is received,
 * 					this function will be called.
 * Parameters:		1. 	func		(MY_VOID_UNSINGED_CHAR)
 *						The function will be called when received the USB command.
 *						The MY_VOID_UNSINGED_CHAR is same as (void)*(unsigned char data).
 *						The data will be set as the same value with the parameter of the USB command.
 * Return:			NONE
 */
void usb_task_set(MY_VOID_UNSINGED_CHAR func);
/*
 * Function:		bat_task_set
 * Description:		Set a function as the bat called back function,when the BAT command (0x4602)is received,
 * 					this function will be called.
 * Parameters:		1. 	func		(MY_VOID_UNSINGED_CHAR)
 *						The function will be called when received the BAT command.
 *						The MY_VOID_UNSINGED_CHAR is same as (void)*(unsigned char data).
 *						The data will be set as the same value with the parameter of the BAT command.
 * Return:			NONE
 */
void bat_task_set(MY_VOID_UNSINGED_CHAR func);
/*
 * Function:		LP_flag_set
 * Description:		Set the low power flag when the IC need to be sleep mode.
 * Parameters:		1. 	flag		(unsigned char)
 *						0x01:Enter sleep mode.
 *						0x00:Exit sleep mode.
 * Return:			NONE
 */
void LP_flag_set(unsigned char flag);
/*
 * Function:		LP_flag_get
 * Description:		Get value of the low power flag.
 * Parameters:		NONE
 *						0x01:Enter sleep mode.
 *						0x00:Exit sleep mode.
 * Return:			(unsigned char)
 *						0x01:In the sleep mode.
 *						0x00:In the normal mode.
 */
unsigned char LP_flag_get(void);
#ifdef __cplusplus
	}
#endif

#endif /* NCN_MDK_H_ */

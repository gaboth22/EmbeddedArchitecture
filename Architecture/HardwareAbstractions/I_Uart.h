#ifndef I_UART_H
#define I_UART_H

#include "types.h"

typedef struct _UartApi_t UartApi_t;

typedef struct
{
	const UartApi_t *api;
} I_Uart_t;

struct _UartApit_t
{
	/*
	 * Send one byte via the UART peripheral
	 */
	void (*SendByte)(I_Uart_t *instance, uint8_t byte);

	/*
	 * Get the event generated when a byte is received via UART
	 */
	I_Event_t * (*GetOnByteReceivedEvent)(I_Uart_t *instance);
};

#define Uart_SendByte(_instance, _byte) \
        (_instance)->api->SendByte(_instance, _byte) \

#define Uart_GetOnByteReceivedEvent(_instance) \
        (_instance)->api->GetOnByteReceivedEvent(_instance) \

#endif

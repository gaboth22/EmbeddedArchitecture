#ifndef I_UART_H
#define I_UART_H

#include "I_Event.h"
#include "types.h"

typedef struct _UartApi_t UartApi_t;
typedef uint32_t Baud_t;

typedef struct
{
	const UartApi_t *api;
} I_Uart_t;

struct _UartApi_t
{
	/*
	 * Send one byte via the UART peripheral
	 */
	void (*SendByte)(I_Uart_t *instance, uint8_t byte);

	/*
	 * Get the event generated when a byte is received via UART
	 */
	I_Event_t * (*GetOnByteReceivedEvent)(I_Uart_t *instance);

	/*
	 * Update the baud rate to a supported baud
	 */
	void (*UpdateBaud)(I_Uart_t *instance, Baud_t baud);

	/*
	 * Disable RX
	 */
	void (*DisableRx)(I_Uart_t *instance);

	/*
	 * Enable RX
	 */
	void (*EnableRx)(I_Uart_t *instance);
};

#define Uart_SendByte(_instance, _byte) \
        (_instance)->api->SendByte(_instance, _byte) \

#define Uart_GetOnByteReceivedEvent(_instance) \
        (_instance)->api->GetOnByteReceivedEvent(_instance) \

#define Uart_UpdateBaud(_instance, _baud) \
        (_instance)->api->UpdateBaud(_instance, _baud) \

#define Uart_DisableRx(_instance) \
        (_instance)->api->DisableRx(_instance) \

#define Uart_EnableRx(_instance) \
        (_instance)->api->EnableRx(_instance) \

#endif

#ifndef I_CAMERA_H
#define I_CAMERA_h

#include "I_Event.h"
#include "types.h"

typedef struct
{
	void *image;
	uint16_t imageSize;
} CameraImage_t;

typedef struct _CameraApi_t CameraApi_t;

typedef struct
{
	const CameraApi_t *api;
} I_Camera_t;

struct _CameraApi_t
{
	/*
	 * Method to set configuration of camera
	 */
	void (*SetConfiguration)(I_Camera_t *instance, const void *configuration);

	/*
	 * Start an image capture
	 */
	void (*StartImageCapture)(I_Camera_t *instance);

	/*
	 * Get event that publishes when image capture finishes
	 */
	I_Event_t * (*GetOnImageCaptureDoneEvent)(I_Camera_t *instance);
};

#endif

#ifndef __DRGN_CAMERA_H__
#define __DRGN_CAMERA_H__

#include "gfc_vector.h"
#include "gfc_shape.h"

typedef struct
{
	Vector2D pos; //position of the camera
	Vector2D size; //size of the screen the camera should render; width, height
	Rect bounds; //bounds of the camera
	Bool bind; //set true if camera is to be kept in bounds
}
DRGN_Camera;

/*
* @brief gets the camera's postion
* @return the camera's position represented as a Vector2D
*/
Vector2D drgn_cameraGetPosition();

/*
* @brief get the camera's offset
* @return the negative values of the camera's x and y values as a Vector2D
*/
Vector2D drgn_cameraGetOffset();

/*
* @brief get the camera's bounds
* @return the bounds of the camera in Rect object form
*/
Rect drgn_cameraGetBounds();

/*
* @brief gets camera size
* @return the camera's size as a Vector2D
*/
Vector2D drgn_cameraGetSize();

/*
* @brief sets the camera's postion
* @param pos a Vector2D that is set as the camera's new position
*/
void drgn_cameraSetPosition(Vector2D pos);

/*
* @brief set's camera's bounds
* @param bounds the bounds to be set
*/
void drgn_cameraSetBounds(Rect bounds);

/*
* @brief sets whether camera should be bound
* @param bind the bool to determine bind status
*/
void drgn_cameraSetBind(Bool bind);
/*
* @brief keeps the camera in bounds
*/

/*
* @brief sets camera size
* @param size the size to be set
*/
void drgn_cameraSetSize(Vector2D size);

void drgn_cameraApplyBounds();

void drgn_cameraCenterOn(Vector2D center);
#endif

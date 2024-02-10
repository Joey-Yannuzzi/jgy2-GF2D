#include "simple_logger.h"
#include "drgn_camera.h"

static DRGN_Camera _camera = { 0 };

Vector2D drgn_cameraGetPosition()
{
	return (_camera.pos);
}

Vector2D drgn_cameraGetOffset()
{
	return (vector2d(-_camera.pos.x, -_camera.pos.y));
}

Rect drgn_cameraGetBounds()
{
	return (_camera.bounds);
}

Vector2D drgn_cameraGetSize()
{
	return (_camera.size);
}

void drgn_cameraSetPosition(Vector2D pos)
{
	vector2d_copy(_camera.pos, pos);

	if (_camera.bind)
	{
		drgn_cameraApplyBounds();
	}
}

void drgn_cameraSetBounds(Rect bounds)
{
	gfc_rect_copy(_camera.bounds, bounds);
}

void drgn_cameraSetBind(Bool bind)
{
	_camera.bind = bind;
}

void drgn_cameraSetSize(Vector2D size)
{
	vector2d_copy(_camera.size, size);
}

void drgn_cameraApplyBounds()
{
	if ((_camera.pos.x + _camera.size.x) > (_camera.bounds.w + _camera.bounds.x))
	{
		_camera.pos.x = (_camera.bounds.w + _camera.bounds.x) - _camera.size.x;
	}

	if ((_camera.pos.y + _camera.size.y) > (_camera.bounds.h + _camera.bounds.y))
	{
		_camera.pos.y = (_camera.bounds.h + _camera.bounds.y) - _camera.size.y;
	}

	if (_camera.pos.x < _camera.bounds.x)
	{
		_camera.pos.x = _camera.bounds.x;
	}

	if (_camera.pos.y < _camera.bounds.y)
	{
		_camera.pos.y = _camera.bounds.y;
	}
}

void drgn_cameraCenterOn(Vector2D center)
{
	Vector2D pos;
	pos.x = center.x - (_camera.size.x * 0.5);
	pos.y = center.y - (_camera.size.y * 0.5);
	drgn_cameraSetPosition(pos);
}
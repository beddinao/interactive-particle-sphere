#ifndef	GOL_H
# define	GOL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include <limits.h>
#include <math.h>
#include "MLX42.h"

#ifndef TRUE
# define TRUE 1
#endif

#ifndef FALSE
# define FALSE 0
#endif

#define	C_COUNT		30
#define	DEF_PPC		1
#define	DEF_FPG		4
#define	MIN_WIDTH		600
#define	MIN_HEIGHT	300
#define	DEF_WIDTH		1600
#define	DEF_HEIGHT	800
#define	BG_COLOR		0x000000
#define	CELL_COLOR	0xF0EEE9

typedef struct mouse_data
{
	int		init_cursor_x;
	int		init_cursor_y;
	bool		cursor_pressed;
	int		event_counter;
}	mouse_data;

typedef struct map_data
{
	float		**particles;
	int		particle_count;
	int		radius;
	float		x_angle;
	float		y_angle;
}	world_data;

typedef struct data
{
	mlx_t		*mlx_ptr;
	mlx_image_t	*mlx_img;
	int		width;
	int		height;
	world_data	*_world;
	mouse_data	*_mouse;
	int		PPC;
	int		FPG;
	//
	int		cur_frame;
	int		last_resize_w;
	int		last_resize_h;
	//
}	data;

#endif

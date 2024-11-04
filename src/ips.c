#include "ips.h"

void	free_particles(float **particles, int particle_count) {
	for (int y = 0; particles[y] && y < particle_count; y++)
		free(particles[y]);
	free(particles);
	particles = NULL;
}

int	release(data *_data, int status) {

	if (!_data)
		return status;

	if (_data->mlx_ptr && _data->mlx_img)
		mlx_delete_image(_data->mlx_ptr, _data->mlx_img);

	if (_data->mlx_ptr)
		mlx_terminate(_data->mlx_ptr);

	if (_data->_world && _data->_world->particles)
		free_particles(_data->_world->particles, _data->_world->particle_count);

	if (_data->_mouse)
		free(_data->_mouse);

	if (_data->_world)
		free(_data->_world);
	
	free(_data);

	return	status;
}

void	close_handle(void *p) {
	data	*_data = (data*)p;
	exit( release(_data, 0) );
}

void	resize_handle(int w, int h, void *p) {
	data	*_data = (data*)p;

	if (h > MIN_HEIGHT && w > MIN_WIDTH
		&& h < INT_MAX && w < INT_MAX) {
		_data->height = h;
		_data->width = w;
		//
		if (abs(h - _data->last_resize_h) > 30 && abs(w - _data->last_resize_w) > 30) {
			//
			_data->last_resize_w = w;
			_data->last_resize_h = h;
		}
		if (!mlx_resize_image(_data->mlx_img, _data->width, _data->height))
			exit( release(_data, 1) );
	}
}

void	key_handle(mlx_key_data_t keydata, void *p) {
	if (keydata.action == MLX_RELEASE)	return;

	data	*_data = (data*)p;
	if (keydata.key == MLX_KEY_ESCAPE)
		close_handle(p);
	else if (keydata.key == MLX_KEY_UP && _data->FPG < 100)
		_data->FPG += 1;
	else if (keydata.key == MLX_KEY_DOWN && _data->FPG > 1)
		_data->FPG -= 1;
}

void	scroll_handle(double xdelta, double ydelta, void *param) {
	data	*_data = (data*)param;

	if (ydelta > 0 && _data->PPC < _data->width && _data->PPC < _data->height)
		_data->PPC += 1;
	else if (ydelta < 0 && _data->PPC > 1)
		_data->PPC -= 1;
}

void	mouse_handle(mouse_key_t button, action_t action, modifier_key_t mods, void *param) {
	data	*_data = (data*)param;
	///
	if (!_data->_mouse->cursor_pressed && action == MLX_PRESS)
		_data->_mouse->cursor_pressed = 1;
	else if (_data->_mouse->cursor_pressed && action == MLX_RELEASE)
		_data->_mouse->cursor_pressed = 0;
}

void	cursor_handle(double xpos, double ypos, void *param) {
	data	*_data = (data*)param;
	int	diff_x, diff_y,
		new_center_x, new_center_y,
		margin_x, margin_y;
	//
	if (!_data->_mouse->cursor_pressed) {
		_data->_mouse->init_cursor_x = xpos;
		_data->_mouse->init_cursor_y = ypos;
	}
	else {
		diff_x = (_data->_mouse->init_cursor_x - (int)xpos) / 5;
		diff_y = (_data->_mouse->init_cursor_y - (int)ypos) / 5;

		printf("diff_x: %i, diff_y: %i\n", diff_x, diff_y);

		/*margin_y = _data->_world->rows / (2 * _data->PPC);
		margin_x = _data->_world->columns / (2 * _data->PPC);

		new_center_x = _data->_world->center_x + diff_x/_data->PPC;
		new_center_y = _data->_world->center_y + diff_y/_data->PPC;

		if (new_center_x - margin_x > 0 && new_center_x + margin_x < _data->_world->columns)
			_data->_world->center_x = new_center_x;
		if (new_center_y - margin_y > 0 && new_center_y + margin_y < _data->_world->rows)
			_data->_world->center_y = new_center_y;*/
	}
}


void	build_particles(data *_data) {
	_data->_world->particles = malloc( (_data->_world->particle_count + 1) * sizeof(float*) );
	if (!_data->_world->particles)
		exit( release(_data, 1) );
	for (int y = 0; y < _data->_world->particle_count; y++) {
		_data->_world->particles[y] = malloc( 3 * sizeof(float) );
		if (!_data->_world->particles[y]) exit( release(_data, 1) );
		memset(_data->_world->particles[y], 0, sizeof(float) * 3);
	}
	_data->_world->particles[ _data->_world->particle_count ] = NULL;
}

void	draw_bg(data* _data, int color) {
	for (int y = 0; y < _data->height; y++) 
		for (int x = 0; x < _data->width; x++)
			mlx_put_pixel(_data->mlx_img, x, y, color);
}

void	draw_particles(data *_data) {
	printf("draw particles\n");
}

float	__calc_new_range(float old_value, float old_min, float old_max, float new_min, float new_max) {
	if (old_value == old_min)
		return(new_min);
	return (((old_value - old_min) * (new_max - new_min)) / (old_max - old_min)) + new_min;
}

void	loop_hook(void *p) {
	data	*_data = (data*)p;
	//
	if (_data->cur_frame >= _data->FPG) {
		_data->cur_frame = 0;
		draw_bg(_data, BG_COLOR << 8 | 0xFF);
		draw_particles(_data);
	}
	else	_data->cur_frame += 1;
}

int	rand_num(int min, int max) {
	return	rand() % (max - min + 1) + min;
}

void	init_world(data *_data) {
	if (!_data)	return;
	
	srand(time(NULL));

	build_particles(_data);
}

int			main(int c, char **v) {
	if (c > 2)	return 1;

	data	*_data = malloc(sizeof(data));
	if (!_data)
		return 1;
	memset(_data, 0, sizeof(data));

	_data->_world = malloc(sizeof(world_data));
	if (!_data->_world) {
		free(_data);
		return 1;
	}
	_data->_mouse = malloc(sizeof(mouse_data));
	if (!_data->_mouse) {
		free(_data->_world);
		free(_data);
		return 1;
	}
	memset(_data->_world, 0, sizeof(world_data));
	memset(_data->_mouse, 0, sizeof(mouse_data));

	//	TODO
	signal(SIGINT, exit);
	signal(SIGILL, SIG_IGN);
	signal(SIGFPE, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGTERM, exit);
	signal(SIGSEGV, exit);
	//

	_data->width = DEF_WIDTH;
	_data->height = DEF_HEIGHT;

	_data->mlx_ptr = mlx_init(_data->width, _data->height, "Game of life", true);
	if (!_data->mlx_ptr) return release(_data, 1);

	_data->mlx_img = mlx_new_image(_data->mlx_ptr, _data->width, _data->height);
	if (!_data->mlx_img) return release(_data, 1);

	_data->PPC = DEF_PPC;
	_data->FPG = c == 2 ? DEF_FPG : 1;
	_data->_world->center_x = (_data->width / 2) * _data->PPC;
	_data->_world->center_y = (_data->height / 2) * _data->PPC;
	draw_bg(_data, BG_COLOR << 8 | 0xFF);
	
	init_world(_data);
	mlx_image_to_window(_data->mlx_ptr, _data->mlx_img, 0, 0);
	mlx_close_hook(_data->mlx_ptr, close_handle, _data);
	mlx_key_hook(_data->mlx_ptr, key_handle, _data);
	mlx_resize_hook(_data->mlx_ptr, resize_handle, _data);
	mlx_set_window_limit(_data->mlx_ptr, MIN_WIDTH, MIN_HEIGHT, INT_MAX, INT_MAX);
	mlx_scroll_hook(_data->mlx_ptr, scroll_handle, _data);
	mlx_cursor_hook(_data->mlx_ptr, cursor_handle, _data);
	mlx_mouse_hook(_data->mlx_ptr, mouse_handle, _data);
	mlx_loop_hook(_data->mlx_ptr, loop_hook, _data);
	mlx_loop(_data->mlx_ptr);
}

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

float	__calc_new_range(float old_value, float old_min, float old_max, float new_min, float new_max) {
	if (old_value == old_min)
		return(new_min);
	return (((old_value - old_min) * (new_max - new_min)) / (old_max - old_min)) + new_min;
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

void	init_particles_position(data *_data) {
	int		cur_particle = 0;
	float		x, y, z;
	float		circ_y;
	float		radius;
	
	int	pos_x = _data->width / 2 - _data->_world->radius,
		pos_y = _data->height / 2 - _data->_world->radius;

	for (float i = 0; i <= M_PI; i += M_PI / C_COUNT) {
		radius = sin(i);
		circ_y = cos(i);
		for (float j = 0; j <= M_PI * 2; j += M_PI / C_COUNT) {
			x = cos(j) * radius;
			z = sin(j) * radius;
			// Y
			y = cos(_data->_world->y_angle) * circ_y - sin(_data->_world->y_angle) * z;
			z = sin(_data->_world->y_angle) * circ_y + cos(_data->_world->y_angle) * z;
			// X
			x = cos(_data->_world->x_angle) * x + sin(_data->_world->x_angle) * z;
			z = -sin(_data->_world->x_angle) * x - cos(_data->_world->x_angle) * z;
			//
			_data->_world->particles[cur_particle][0] = (int)__calc_new_range(x, -1, 1, pos_x, pos_x + _data->_world->radius * 2);
			_data->_world->particles[cur_particle][1] = (int)__calc_new_range(y, -1, 1, pos_y, pos_y + _data->_world->radius * 2);
			_data->_world->particles[cur_particle][2] = z;
			_data->_world->particles[cur_particle][3] = x;
			_data->_world->particles[cur_particle][4] = y;
			cur_particle += 1;
		}
	}
}

void	cursor_handle(double xpos, double ypos, void *param) {
	data	*_data = (data*)param;
	int	diff_x, diff_y;
	//
	if (!_data->_mouse->cursor_pressed) {
		_data->_mouse->init_cursor_x = xpos;
		_data->_mouse->init_cursor_y = ypos;
	}
	else {
		diff_x = ((int)xpos - _data->_mouse->init_cursor_x) / 5;
		diff_y = ((int)ypos - _data->_mouse->init_cursor_y) / 5;


		//printf("diff_x: %i, diff_y: %i\n", diff_x, diff_y);
		_data->_world->x_angle += diff_x; // -1000
		while (_data->_world->x_angle < 0) 
			_data->_world->x_angle = 360 - (0 - _data->_world->x_angle);
		while (_data->_world->x_angle > 360)
			_data->_world->x_angle = 0 + (_data->_world->x_angle - 360);


		_data->_world->y_angle += diff_y;
		while (_data->_world->y_angle < 0)
			_data->_world->y_angle = 360 - (0 - _data->_world->y_angle);
		while (_data->_world->y_angle > 360)
			_data->_world->y_angle = 0 + (_data->_world->y_angle - 360);
		
		//printf("new_angle_x: %i, new_angle_y: %i\n", _data->_world->x_angle, _data->_world->y_angle);
		if (diff_x || diff_y)
			init_particles_position(_data);
	}
}


void	build_particles(data *_data) {
	_data->_world->particles = malloc( (_data->_world->particle_count + 1) * sizeof(float*) );
	if (!_data->_world->particles)
		exit( release(_data, 1) );
	for (int y = 0; y < _data->_world->particle_count; y++) {
		_data->_world->particles[y] = malloc( 5 * sizeof(float) );
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
	//
	for (int i = 0; i < _data->_world->particle_count && _data->_world->particles[i]; i++) {
		if (_data->_world->particles[i][0] > 0 && _data->_world->particles[i][1] > 0
			&& _data->_world->particles[i][0] < _data->width && _data->_world->particles[i][1] < _data->height)
			mlx_put_pixel(_data->mlx_img, _data->_world->particles[i][0], _data->_world->particles[i][1],
				0xFFFFFF << 8 | (int)__calc_new_range(_data->_world->particles[i][2], -2, 2, 10, 255));
	}
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
	for (float i = 0; i <= M_PI; i += M_PI / C_COUNT)
		for (float j = 0; j < M_PI * 2; j += M_PI / C_COUNT)
			_data->_world->particle_count += 1;

	build_particles(_data);
	_data->_world->x_angle = 10;
	_data->_world->y_angle = 20;
	init_particles_position(_data);
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

	_data->mlx_ptr = mlx_init(_data->width, _data->height, "particle sphere", true);
	if (!_data->mlx_ptr) return release(_data, 1);

	_data->mlx_img = mlx_new_image(_data->mlx_ptr, _data->width, _data->height);
	if (!_data->mlx_img) return release(_data, 1);

	_data->PPC = DEF_PPC;
	_data->FPG = DEF_FPG;

	_data->_world->radius = _data->height / 3;
	if (_data->_world->radius >= _data->width)
		_data->_world->radius = _data->width / 3;

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

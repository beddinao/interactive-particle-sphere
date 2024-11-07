#include "ips.h"

void	free_particles(particle **particles, int particle_count) {
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

int	rand_num(int min, int max) {
	return	rand() % (max - min + 1) + min;
}

float	__calc_new_range(float old_value, float old_min, float old_max, float new_min, float new_max) {
	if (old_value == old_min)
		return(new_min);
	return (((old_value - old_min) * (new_max - new_min)) / (old_max - old_min)) + new_min;
}

void init_particles_position(data *_data) {
	int	cur_particle = 0;
	float	x, y, z;
	float	circ_y, temp_x, radius;
	int	pos_x = _data->width / 2 - _data->_world->radius,
		pos_y = _data->height / 2 - _data->_world->radius;
	float	x_angle = _data->_world->x_angle * (M_PI / 180.0f),
		y_angle = _data->_world->y_angle * (M_PI / 180.0f);

	float	dir_x = _data->_world->radius;
	float	dir_y = 0,
		dir_z = 0;

	_data->_world->f_dir_x = dir_x;
	_data->_world->f_dir_y = cos(y_angle) * dir_y - sin(y_angle) * dir_z;
	_data->_world->f_dir_z = sin(y_angle) * dir_y + cos(y_angle) * dir_z;
	temp_x = _data->_world->f_dir_x;
	_data->_world->f_dir_x = cos(x_angle) * temp_x + sin(x_angle) * _data->_world->f_dir_z;
	_data->_world->f_dir_z = -sin(x_angle) * temp_x + cos(x_angle) * _data->_world->f_dir_z;
	_data->_world->f_dir_x += _data->center_x;
	_data->_world->f_dir_y += _data->center_y;

	dir_x = 0;
	dir_y = _data->_world->radius;
	dir_z = 0;

	_data->_world->s_dir_x = dir_x;
	_data->_world->s_dir_y = cos(y_angle) * dir_y - sin(y_angle) * dir_z;
	_data->_world->s_dir_z = sin(y_angle) * dir_y + cos(y_angle) * dir_z;
	temp_x = _data->_world->s_dir_x;
	_data->_world->s_dir_x = cos(x_angle) * temp_x + sin(x_angle) * _data->_world->s_dir_z;
	_data->_world->s_dir_z = -sin(x_angle) * temp_x + cos(x_angle) * _data->_world->s_dir_z;
	_data->_world->s_dir_x += _data->center_x;
	_data->_world->s_dir_y += _data->center_y;

	for (float i = 0; i <= M_PI; i += M_PI / C_COUNT) {
		radius = sin(i);
		circ_y = cos(i);
		for (float j = 0; j <= M_PI * 2; j += M_PI / C_COUNT) {
			x = cos(j) * radius;
			z = sin(j) * radius;
			// Y
			y = cos(y_angle) * circ_y - sin(y_angle) * z;
			z = sin(y_angle) * circ_y + cos(y_angle) * z;
			// X
			temp_x = x;
			x = cos(x_angle) * temp_x + sin(x_angle) * z;
			z = -sin(x_angle) * temp_x - cos(x_angle) * z;

			_data->_world->particles[cur_particle]->x = (int)__calc_new_range(x, -1, 1, pos_x, pos_x + _data->_world->radius * 2);
			_data->_world->particles[cur_particle]->y = (int)__calc_new_range(y, -1, 1, pos_y, pos_y + _data->_world->radius * 2);
			_data->_world->particles[cur_particle]->z = z;
			_data->_world->particles[cur_particle]->eo_x = x;
			_data->_world->particles[cur_particle]->eo_y = y;

			if (_data->_world->particles[cur_particle]->real) {
				_data->_world->particles[cur_particle]->t_x = _data->center_x - (_data->_world->particles[cur_particle]->x - _data->center_x) * _data->_world->coef;
				_data->_world->particles[cur_particle]->t_y = _data->center_y - (_data->_world->particles[cur_particle]->y - _data->center_y) * _data->_world->coef;
			}
			cur_particle += 1;
		}
	}
}

void	mini_init(data *_data) {
	float	x, y, z;
	float	x_angle = _data->_world->x_angle * (M_PI / 180.0f),
		y_angle = _data->_world->y_angle * (M_PI / 180.0f);
	int	pos_x = _data->width / 2 - _data->_world->radius,
		pos_y = _data->height / 2 - _data->_world->radius;
	float	temp_y, temp_x;

	for (int i = 0; i < _data->_world->particle_count; i++) {
		x = _data->_world->particles[i]->eo_x;
		y = _data->_world->particles[i]->eo_y;
		z = _data->_world->particles[i]->z;
			temp_y = y;

			y = cos(y_angle) * temp_y - sin(y_angle) * z;
			z = sin(y_angle) * temp_y + cos(y_angle) * z;
			// X
			temp_x = x;
			x = cos(x_angle) * temp_x + sin(x_angle) * z;
			z = -sin(x_angle) * temp_x - cos(x_angle) * z;

			_data->_world->particles[i]->x = (int)__calc_new_range(x, -1, 1, pos_x, pos_x + _data->_world->radius * 2);
			_data->_world->particles[i]->y = (int)__calc_new_range(y, -1, 1, pos_y, pos_y + _data->_world->radius * 2);
			_data->_world->particles[i]->z = z;
			_data->_world->particles[i]->eo_x = x;
			_data->_world->particles[i]->eo_y = y;
	}

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
			_data->_world->radius = _data->height / 3;
			if (_data->_world->radius >= _data->width)
				_data->_world->radius = _data->width / 3;
			_data->center_x = _data->width / 2;
			_data->center_y = _data->height / 2;
			//
			_data->last_resize_w = w;
			_data->last_resize_h = h;
			//
			init_particles_position(_data);
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

	if (ydelta > 0 && _data->_world->coef < 2) {
		_data->_world->coef += 0.1;
		init_particles_position(_data);
	}
	else if (ydelta < 0 && _data->_world->coef > -2) {
		_data->_world->coef -= 0.1;
		init_particles_position(_data);
	}

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
	float	diff_x, diff_y;
	float	delta_x, delta_y;
	int	dis;
	//
	if (!_data->_mouse->cursor_pressed) {
		_data->_mouse->init_cursor_x = xpos;
		_data->_mouse->init_cursor_y = ypos;
		//
		if (_data->_mouse->event_counter >= 1) {
			_data->_mouse->event_counter = 0;

			for (int i = 0; i < _data->_world->particle_count; i++) {
				dis = sqrt( pow(fabs(_data->_world->particles[i]->x - xpos), 2) + pow(fabs(_data->_world->particles[i]->y - ypos), 2)  );
				//
				if (dis < _data->_world->radius / 2) {
					mlx_put_pixel(_data->mlx_img, _data->_world->particles[i]->x, _data->_world->particles[i]->y, 0x26609eFF);
					delta_x = _data->_world->particles[i]->x - _data->center_x;
					delta_y = _data->_world->particles[i]->y - _data->center_y;

					/*if (x > 0) _data->_world->particles[i]->t_x = center_x - (_data->_world->particles[i]->x - center_x) * _data->_world->coef;
					  else	 _data->_world->particles[i]->t_x = center_x + (center_x - _data->_world->particles[i]->x) * _data->_world->coef;
					  if (y > 0) _data->_world->particles[i]->t_y = center_y - (_data->_world->particles[i]->y - center_y) * _data->_world->coef;
					  else	 _data->_world->particles[i]->t_y = center_y + (center_y - _data->_world->particles[i]->y) * _data->_world->coef;*/

					_data->_world->particles[i]->t_x = _data->center_x + _data->_world->coef * delta_x * -1;
					_data->_world->particles[i]->t_y = _data->center_y + _data->_world->coef * delta_y * -1;

					_data->_world->particles[i]->real = TRUE;
				}
				else	_data->_world->particles[i]->real = FALSE;
			}
		}
		else	_data->_mouse->event_counter += 1;
	}
	else {
		if (_data->_mouse->event_counter >= 1) {
			_data->_mouse->event_counter = 0;
			diff_x = _data->_mouse->init_cursor_x - (int)xpos;
			diff_y = _data->_mouse->init_cursor_y - (int)ypos;

			diff_x = diff_x > 0 ? 1 : diff_x < 0 ? -1 : 0;
			diff_y = diff_y > 0 ? 1 : diff_y < 0 ? -1 : 0;

			//printf("x_angle before: %f\n", _data->_world->x_angle);
			_data->_world->x_angle += diff_x;
			while (_data->_world->x_angle < 0) 
				_data->_world->x_angle = 360 - (0 - _data->_world->x_angle);
			while (_data->_world->x_angle > 360)
				_data->_world->x_angle = 0 + (_data->_world->x_angle - 360);
			//printf("x_angle after: %f\n", _data->_world->x_angle);

			//printf("y_angle before: %f\n", _data->_world->y_angle);
			_data->_world->y_angle += diff_y;
			while (_data->_world->y_angle < 0)
				_data->_world->y_angle = 360 - (0 - _data->_world->y_angle);
			while (_data->_world->y_angle > 360)
				_data->_world->y_angle = 0 + (_data->_world->y_angle - 360);
			//printf("y_angle after: %f\n", _data->_world->y_angle);

			if (diff_x || diff_y)
				init_particles_position(_data);
		}
		else	_data->_mouse->event_counter += 1;
	}
}


void	build_particles(data *_data) {
	_data->_world->particles = malloc( (_data->_world->particle_count + 1) * sizeof(particle) );
	if (!_data->_world->particles)
		exit( release(_data, 1) );
	for (int y = 0; y < _data->_world->particle_count; y++) {
		_data->_world->particles[y] = malloc( sizeof(particle) );
		if (!_data->_world->particles[y])
			exit( release(_data, 1) );
		memset(_data->_world->particles[y], 0, sizeof(particle));
	}
	_data->_world->particles[ _data->_world->particle_count ] = NULL;
}

void	draw_bg(data* _data) {
	int	color, c;
	for (int y = 0; y < _data->height; y++) {
		color = (int)__calc_new_range(y, 0, _data->height, 50, 0);
		for (int x = 0; x < _data->width; x++) {
			if (y < _data->height / 2 && (!(x % 20) || !(y % 20)))	c = 0x1a;
			else			c = color;
			mlx_put_pixel(_data->mlx_img, x, y, (c << 24) + (c << 16) + (c << 8) + 0xFF);
		}
	}
}

void plotLine(data *_data, int x0, int y0, int x1, int y1, int color)
{
	int dx =  abs(x1-x0), sx = x0<x1 ? 1 : -1;
	int dy = -abs(y1-y0), sy = y0<y1 ? 1 : -1;
	int err = dx+dy, e2;

	for(;;){
		if (x0 > 0 && y0 > 0 && x0 < _data->width && y0 < _data->height) 
			mlx_put_pixel(_data->mlx_img, x0,y0, color);

		if (x0==x1 && y0==y1) break;
		e2 = 2*err;
		if (e2 >= dy) { err += dy; x0 += sx; }
		if (e2 <= dx) { err += dx; y0 += sy; }
	}
}

void	draw_particles(data *_data) {
	//
	float		x0, y0, x1, y1;
	int		color, result;

	for (int i = 0, j; i < _data->_world->particle_count - 2 && _data->_world->particles[i + 2]; i += 2) {
		j = i + 2;
		if (((i + 2) % 61) <= 1) {
			j = i - ((61 - 3) + ((i + 2) % 61));
			if (j < 0) continue;
		}
		x0 = _data->_world->particles[i]->real ? _data->_world->particles[i]->t_x : _data->_world->particles[i]->x;
		y0 = _data->_world->particles[i]->real ? _data->_world->particles[i]->t_y : _data->_world->particles[i]->y;
		x1 = _data->_world->particles[j]->real ? _data->_world->particles[j]->t_x : _data->_world->particles[j]->x;
		y1 = _data->_world->particles[j]->real ? _data->_world->particles[j]->t_y : _data->_world->particles[j]->y;

			color = (int)__calc_new_range(_data->_world->particles[i]->y,
				_data->center_y - _data->_world->radius, _data->center_y + _data->_world->radius, 0, 80);

			result = (color << 16) + (color << 8) + color;
			
			if (i + 60 < _data->_world->particle_count ) {
				int x2 = _data->_world->particles[i + 60]->real ? _data->_world->particles[i + 60]->t_x : _data->_world->particles[i + 60]->x;
				int y2 = _data->_world->particles[i + 60]->real ? _data->_world->particles[i + 60]->t_y : _data->_world->particles[i + 60]->y;
				plotLine(_data, x0, y0, x2, y2, result << 8 | 0xFF);
			}
			if (i + 62 < _data->_world->particle_count ) {
				int x2 = _data->_world->particles[i + 62]->real ? _data->_world->particles[i + 62]->t_x : _data->_world->particles[i + 62]->x;
				int y2 = _data->_world->particles[i + 62]->real ? _data->_world->particles[i + 62]->t_y : _data->_world->particles[i + 62]->y;
				plotLine(_data, x0, y0, x2, y2, result << 8|0xFF);
			}

			color = (int)__calc_new_range(_data->_world->particles[i]->z, -1.5, 1.5, 255, 0);
			result = (color << 16) + (color << 8) + color;
			if (x0 > 0 && y0 > 0 && x0 < _data->width && y0 < _data->height)
				mlx_put_pixel(_data->mlx_img, x0, y0, result << 8 | 0xFF);
	}
}

void	loop_hook(void *p) {
	data	*_data = (data*)p;
	//
	if (_data->cur_frame >= _data->FPG) {
		_data->cur_frame = 0;

		if (!_data->_mouse->cursor_pressed) {
			_data->_world->x_angle += 3;
			if (_data->_world->x_angle >= 360)
				_data->_world->x_angle = 0;
			_data->_world->y_angle += 1;
			if (_data->_world->y_angle >= 360)
				_data->_world->y_angle = 0;
			mini_init(_data);
		}

		draw_bg(_data);
		draw_particles(_data);
		plotLine(_data, _data->center_x, _data->center_y, _data->_world->f_dir_x, _data->_world->f_dir_y, 0x3882d1FF);
		plotLine(_data, _data->center_x, _data->center_y, _data->_world->s_dir_x, _data->_world->s_dir_y, 0x3882d1FF);
	}
	else	_data->cur_frame += 1;
}

void	init_world(data *_data) {
	if (!_data)	return;


	int		x = 0, y = 0;

	srand(time(NULL));
	for (float i = 0; i <= M_PI; i += M_PI / C_COUNT) {
		x = 0;
		for (float j = 0; j < M_PI * 2; j += M_PI / C_COUNT) {
			_data->_world->particle_count += 1;
			x++;
		}
		//printf("x: %i\n", x);
		y++;
	}
	//printf("y: %i\n", y );

	_data->_world->coef = -1.5;
	_data->_world->x_angle = 10;
	_data->_world->y_angle = 20;
	build_particles(_data);
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
	//signal(SIGFPE, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGTERM, exit);
	//signal(SIGSEGV, exit);
	//

	_data->width = DEF_WIDTH;
	_data->height = DEF_HEIGHT;
	_data->center_x = _data->width / 2;
	_data->center_y = _data->height / 2;

	_data->mlx_ptr = mlx_init(_data->width, _data->height, "particle sphere", true);
	if (!_data->mlx_ptr) return release(_data, 1);

	_data->mlx_img = mlx_new_image(_data->mlx_ptr, _data->width, _data->height);
	if (!_data->mlx_img) return release(_data, 1);

	_data->PPC = DEF_PPC;
	_data->FPG = DEF_FPG;

	_data->_world->radius = _data->height / 3;
	if (_data->_world->radius >= _data->width)
		_data->_world->radius = _data->width / 3;

	draw_bg(_data);

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

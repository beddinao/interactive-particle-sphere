# *`interactive-particle-sphere`*


void	draw_circle(data *_data, int s_x, int s_y, int height, int width) {
	float			radian;
	float			x, y;
	int			acc_x, acc_y;

	for (float i = 0; i < 360; i++) {
		radian = i * (M_PI / 180.0);
		x = cos(radian);
		y = sin(radian);
		acc_x = __calc_new_range(x, -1, 1, s_x, s_x + width);
		acc_y = __calc_new_range(y, -1, 1, s_y, s_y + height);
		mlx_put_pixel(_data->mlx_img, acc_x, acc_y, 0xFFFFFFFF);
	}
}

void	draw_sphere(data *_data, int s_x, int s_y, int height, int width) {
	float		radian, circ_y;
	float		x, z, y;
	float		acc_x, acc_y;
	float		radius;

	for (float i = 0; i <= M_PI; i += M_PI / 30) {
		radius = sin(i);
		circ_y = cos(i);
		//
		for (float j = 0; j < M_PI * 2; j += M_PI / 30) {
			x = cos(j) * radius;
			z = sin(j) * radius;

			y = cos(20) * circ_y - sin(20) * z;
			z = sin(20) * circ_y + cos(20) * z;
			//
			x = cos(10) * x + sin(10) * z;
			z = -sin(10) * x - cos(10) * z;

			//printf("x: %f, y: %f\n", x, y);

			acc_x = __calc_new_range(x, -1, 1, s_x, s_x + width);
			acc_y = __calc_new_range(y, -1, 1, s_y, s_y + height);
			//if (acc_x > 0 && acc_y > 0 && acc_x < _data->width && acc_y < _data->height)
				mlx_put_pixel(_data->mlx_img, acc_x, acc_y, 0xFFFFFF << 8 |(int)__calc_new_range(z, -1, 1, 0, 255));
		}
	}
}

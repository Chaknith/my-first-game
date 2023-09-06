#define is_down(b) input->buttons[b].is_down
#define pressed(b) (input->buttons[b].is_down && input->buttons[b].changed)
#define released(b) (!input->buttons[b].is_down && input->buttons[b].changed)

float player_1_p, player_1_dp;
float player_2_p, player_2_dp;
int arena_half_size_x = 85, arena_half_size_y = 45;
int player_half_size_x = 2.5, player_half_size_y = 12;
float ball_p_x, ball_p_y, ball_dp_x= 100, ball_dp_y;
bool player_1_alive = true;
bool player_2_alive = true;

internal void
simulate_game(Input* input, float dt) {
	clear_screen(0x000000);
	draw_rect(0, 0, arena_half_size_x, arena_half_size_y, 0xffaa33);

	float player_1_ddp = 0.f;
	if (is_down(BUTTON_UP)) player_1_ddp += 1400;
	if (is_down(BUTTON_DOWN)) player_1_ddp -= 1400;

	float player_2_ddp = 0.f;
	if (is_down(BUTTON_W)) player_2_ddp += 1400;
	if (is_down(BUTTON_S)) player_2_ddp -= 1400;

	// Player 1
	// Friction calculation
	player_1_ddp -= player_1_dp * 10.f;

	// Acceleration calculation
	player_1_p = player_1_p + player_1_dp * dt + player_1_ddp * dt * dt * .5f;
	player_1_dp = player_1_dp + player_1_ddp * dt;

	// Player 2
	// Friction calculation
	player_2_ddp -= player_2_dp * 10.f;

	// Acceleration calculation
	player_2_p = player_2_p + player_2_dp * dt + player_2_ddp * dt * dt * .5f;
	player_2_dp = player_2_dp + player_2_ddp * dt;

	// Player's collision with the wall
	// check if the player position plus the half size is greater than the arena half size => If yes then we are colliding.
#define wall_collision(p, v)\
if (p + player_half_size_y > arena_half_size_y) {\
	p = arena_half_size_y - player_half_size_y;\
	v = 0;\
}else if (p - player_half_size_y < -arena_half_size_y) {\
	p = -arena_half_size_y + player_half_size_y;\
	v = 0;\
}
	wall_collision(player_1_p, player_1_dp)
	wall_collision(player_2_p, player_2_dp)

	ball_p_x += ball_dp_x * dt;
	ball_p_y += ball_dp_y * dt;

	// Ball collides the player
	bool ball_collide_player_1_x = ball_p_x + 1 > 80 - player_half_size_x;
	bool ball_miss_player_1_y = ball_p_y - 1 > player_1_p + player_half_size_y || ball_p_y + 1 < player_1_p - player_half_size_y;
	if (ball_collide_player_1_x && ball_miss_player_1_y) {
		player_1_alive = false;
	}
	else if (ball_collide_player_1_x && player_1_alive) {
		ball_dp_x = -ball_dp_x;
	}
	bool ball_collide_player_2_x = ball_p_x - 1 < -80 + player_half_size_x;
	bool ball_miss_player_2_y = ball_p_y - 1 > player_2_p + player_half_size_y || ball_p_y + 1 < player_2_p - player_half_size_y;
	if (ball_collide_player_2_x && ball_miss_player_2_y) {
		player_2_alive = false;
	}
	else if (ball_collide_player_2_x && player_2_alive) {
		ball_dp_x = -ball_dp_x;
	}

	// Ball collides the wall
	if (ball_p_y + 1 > arena_half_size_y) {
		ball_dp_y = -ball_dp_y;
	} else if (ball_p_y - 1 < -arena_half_size_y) {
		ball_dp_y = -ball_dp_y;
	}

	// Draw pong
	draw_rect(ball_p_x, ball_p_y, 1, 1, 0xffffff);

	// Draw players
	draw_rect(80, player_1_p, player_half_size_x, player_half_size_y, 0xff0000);
	draw_rect(-80, player_2_p, player_half_size_x, player_half_size_y, 0xff0000);
}
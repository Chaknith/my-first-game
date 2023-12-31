#define is_down(b) input->buttons[b].is_down
#define pressed(b) (input->buttons[b].is_down && input->buttons[b].changed)
#define released(b) (!input->buttons[b].is_down && input->buttons[b].changed)

int initial_ball_speed = 140;

float player_1_p_y, player_1_dp;
int player_1_p_x = 80;
float player_2_p_y, player_2_dp;
int player_2_p_x = -80;
int arena_half_size_x = 85, arena_half_size_y = 45;
int player_half_size_x = 2.5, player_half_size_y = 12, ball_half_size = 1;
float ball_p_x, ball_p_y, ball_dp_x= initial_ball_speed, ball_dp_y;
int player_1_score = 0;
int player_2_score = 0;

internal void
simulate_player(float *p, float *dp, float ddp, float dt) {
	// Friction calculation
	ddp -= *dp * 10.f;

	// Acceleration calculation
	*p = *p + *dp * dt + ddp * dt * dt * .5f;
	*dp = *dp + ddp * dt;

	if (*p + player_half_size_y > arena_half_size_y) {
		*p = arena_half_size_y - player_half_size_y; 
		*dp = 0; 
	}
	else if (*p - player_half_size_y < -arena_half_size_y) {
		*p = -arena_half_size_y + player_half_size_y; 
		*dp = 0; 
	}
}

internal bool
aabb_vs_aabb(float p1x, float p1y, float hs1x, float hs1y,
	float p2x, float p2y, float hs2x, float hs2y) {
	return(p1x + hs1x > p2x - hs2x &&
		p1x - hs1x < p2x + hs2x &&
		p1y + hs1y > p2y - hs2y &&
		p1y - hs1y < p2y + hs2y);
}

enum Gamemode {
	GM_MENU,
	GM_GAMEPLAY,
};

Gamemode current_gamemode;
int hot_button;
bool enemy_is_ai;

internal void
simulate_game(Input* input, float dt) {
	clear_screen(0x000000);
	draw_rect(0, 0, arena_half_size_x, arena_half_size_y, 0xffaa33);

	if (current_gamemode == GM_GAMEPLAY) {
		if (pressed(BUTTON_ESCAPE)) current_gamemode = GM_MENU;
		
		float player_1_ddp = 0.f;
		if (is_down(BUTTON_UP)) player_1_ddp += 1400;
		if (is_down(BUTTON_DOWN)) player_1_ddp -= 1400;

		float player_2_ddp = 0.f;
		if (enemy_is_ai) {
			if (is_down(BUTTON_W)) player_2_ddp += 1400;
			if (is_down(BUTTON_S)) player_2_ddp -= 1400;
		}
		else {
			//if (ball_p_y > player_2_p_y + 2.f) player_2_ddp += 1000;
			//if (ball_p_y < player_2_p_y + 2.f) player_2_ddp -= 1000;
			player_2_ddp = (ball_p_y - player_2_p_y) * 100;
			if (player_2_ddp > 1000) player_2_ddp = 1000;
			if (player_2_ddp < -1000) player_2_ddp = -1000;
		}

		// Player 1
		simulate_player(&player_1_p_y, &player_1_dp, player_1_ddp, dt);

		// Player 2
		simulate_player(&player_2_p_y, &player_2_dp, player_2_ddp, dt);

		ball_p_x += ball_dp_x * dt;
		ball_p_y += ball_dp_y * dt;

		bool ballCollidesWithPlayer1 = aabb_vs_aabb(ball_p_x, ball_p_y, ball_half_size, ball_half_size, player_1_p_x, player_1_p_y, player_half_size_x, player_half_size_y);
		bool ballCollidesWithPlayer2 = aabb_vs_aabb(ball_p_x, ball_p_y, ball_half_size, ball_half_size, player_2_p_x, player_2_p_y, player_half_size_x, player_half_size_y);

		if (ballCollidesWithPlayer1) {
			ball_p_x = player_1_p_x - player_half_size_x - ball_half_size;
			ball_dp_x = -ball_dp_x;
			ball_dp_y = (ball_p_y - player_1_p_y) * 2 + player_1_dp * .75f;
		}
		else if (ballCollidesWithPlayer2) {
			ball_p_x = player_2_p_x + player_half_size_x + ball_half_size;
			ball_dp_x = -ball_dp_x;
			ball_dp_y = (ball_p_y - player_2_p_y) * 2 + player_2_dp * .75f;
		}

		// Ball collides the top and bottom wall
		if (ball_p_y + ball_half_size > arena_half_size_y) {
			ball_p_y = arena_half_size_y - ball_half_size;
			ball_dp_y = -ball_dp_y;
		}
		else if (ball_p_y - ball_half_size < -arena_half_size_y) {
			ball_p_y = -arena_half_size_y + ball_half_size;
			ball_dp_y = -ball_dp_y;
		}

		// Ball collides the left and right wall
		if (ball_p_x + ball_half_size > arena_half_size_x) {
			ball_p_x = 0;
			ball_p_y = 0;
			ball_dp_x = -initial_ball_speed;
			ball_dp_y = 0;
			player_2_score++;
		}
		else if (ball_p_x - ball_half_size < -arena_half_size_x) {
			ball_p_x = 0;
			ball_p_y = 0;
			ball_dp_y = 0;
			ball_dp_x = initial_ball_speed;
			player_1_score++;
		}

		draw_score(player_1_score, -10, 40, 1.f, 0xbbffbb);
		draw_score(player_2_score, 10, 40, 1.f, 0xbbffbb);

		// Draw pong
		draw_rect(ball_p_x, ball_p_y, ball_half_size, ball_half_size, 0xffffff);

		// Draw players
		draw_rect(player_1_p_x, player_1_p_y, player_half_size_x, player_half_size_y, 0xff0000);
		draw_rect(player_2_p_x, player_2_p_y, player_half_size_x, player_half_size_y, 0xff0000);
	}
	else if (current_gamemode == GM_MENU) {
		if (pressed(BUTTON_LEFT) || pressed(BUTTON_RIGHT)) {
			hot_button = !hot_button;
		}
		if (pressed(BUTTON_ENTER)) {
			current_gamemode = GM_GAMEPLAY;
			enemy_is_ai = hot_button ? 0 : 1;
		}
		if (hot_button == 0) {
			draw_text("PONG", -5, 30, 1, 0x000000);
			draw_text("SINGLE PLAYER", -80, -10, 1, 0x000000);
			draw_text("MULTIPLAYER", 20, -10, 1, 0x00ff00);
		}
		else {
			draw_text("PONG", -5, 30, 1, 0x000000);
			draw_text("SINGLE PLAYER", -80, -10, 1, 0x00ff00);
			draw_text("MULTIPLAYER", 20, -10, 1, 0x000000);
		}
	}
}
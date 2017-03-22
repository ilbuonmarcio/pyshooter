import pygame
from pygame.locals import *
import math
import sys
from random import randint

debugger = True
show_hitboxes = False
bullet_dim = 10
player_dim = 64

def random_bullets(num_of_bullets=256):
	for i in range(0, num_of_bullets):
		x, y = randint(0, GAME_WIDTH), randint(0, GAME_HEIGHT)
		pygame.Surface.blit(game_window, bullet_sprite, (x, y))
		if show_hitboxes:
			pygame.draw.rect(game_window, (255, 255, 0), [x, y, bullet_sprite.get_width(), bullet_sprite.get_height()], 1)

def calc_player_angle_by_mouse_pos():
	global player_sprite
	global player_angle
	adiacent = player_x - mouse_pos[0]
	opposite = player_y - mouse_pos[1]
	try:
		if opposite <= 0:
			player_angle = 180 + math.degrees(math.atan(adiacent / opposite))
		else:
			player_angle = math.degrees(math.atan(adiacent / opposite))
	except:
		player_angle = 0

def reset_player_under_area():
	global player_x
	global player_y
	if player_x < 20 + player_sprite_default.get_width():
		player_x = 20 + player_sprite_default.get_width()
	if player_x > GAME_WIDTH - 20 - player_sprite_default.get_width():
		player_x = GAME_WIDTH - 20 - player_sprite_default.get_width()
	if player_y < 20 + player_sprite_default.get_height():
		player_y = 20 + player_sprite_default.get_height()
	if player_y > GAME_HEIGHT - 20 - player_sprite_default.get_height():
		player_y = GAME_HEIGHT - 20 - player_sprite_default.get_height()

def fire():
	if pygame.mouse.get_pressed()[0]:
		bullet_list.append([player_x, player_y, player_angle])
	for bullet in bullet_list:
		if bullet[0] < 0 or bullet[0] > GAME_WIDTH or bullet[1] < 0 or bullet[1] > GAME_HEIGHT:
			bullet_list.remove(bullet)
		bullet[0] -= math.sin(math.radians(bullet[2])) * delta_time * bullet_speed
		bullet[1] -= math.cos(math.radians(bullet[2])) * delta_time * bullet_speed


if __name__ == "__main__":

	pygame.init()

	RESOLUTION = GAME_WIDTH, GAME_HEIGHT = pygame.display.Info().current_w, pygame.display.Info().current_h
	game_window = pygame.display.set_mode(RESOLUTION, FULLSCREEN|HWSURFACE|HWACCEL)

	player_sprite_default = pygame.transform.scale(pygame.image.load("sprites/players/player.png"), (player_dim, player_dim))
	player_sprite = None
	player_x, player_y = pygame.display.Info().current_w // 2 - player_sprite_default.get_width() // 2, pygame.display.Info().current_h // 2 - player_sprite_default.get_height() // 2
	player_angle = 0
	player_speed = 500
	bullet_sprite = pygame.transform.scale(pygame.image.load("sprites/objects/bullet.png"), (bullet_dim, bullet_dim))
	bullet_speed = 500
	bullet_list = []

	FPS = 240
	clock = pygame.time.Clock()
	game_ended = False
	pygame.key.set_repeat(100, 10)
	while not game_ended:

		delta_time = clock.tick(FPS) / 1000.0

		# Event Handling
		for event in pygame.event.get():
			if event.type == QUIT:
				game_ended = True
			if event.type == KEYDOWN:
				if event.key == K_F4:
					game_ended = True
				if event.key == K_w:
					player_y -= player_speed * delta_time
				if event.key == K_a:
					player_x -= player_speed * delta_time
				if event.key == K_s:
					player_y += player_speed * delta_time
				if event.key == K_d:
					player_x += player_speed * delta_time

		# Game Logic
		mouse_pos = pygame.mouse.get_pos()
		calc_player_angle_by_mouse_pos()
		reset_player_under_area()
		fire()

		# Display Update
		pygame.Surface.fill(game_window, (0, 50, 50))
		pygame.draw.rect(game_window, (0, 10, 10),
						 (0, 0, GAME_WIDTH, GAME_HEIGHT), 60)
		player_sprite = pygame.transform.rotate(player_sprite_default, player_angle)
		pygame.Surface.blit(game_window, player_sprite,
							(player_x - player_sprite.get_width() // 2, player_y - player_sprite.get_height() // 2))
		for bullet in bullet_list:
			pygame.Surface.blit(game_window, pygame.transform.rotate(bullet_sprite, bullet[2]), (bullet[0], bullet[1]))
		if show_hitboxes:
			pygame.draw.rect(game_window, (255, 255, 0),
							 (player_x - player_sprite.get_width() // 2, player_y - player_sprite.get_height() // 2,
							  player_sprite.get_width(), player_sprite.get_height()), 1)


		pygame.display.flip()

	pygame.quit()
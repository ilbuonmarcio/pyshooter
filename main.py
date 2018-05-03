import random
import math
import os
import pygame
from pygame.locals import *

__version__ = "ALPHA"
__author__ = "github.com@marcioz98"

os.environ['SDL_VIDEO_CENTERED'] = '1'

pygame.init()

GAME_RES = WIDTH, HEIGHT = 1920, 1080
FPS = 60
BG_COLOR = (51, 51, 51)


def scale_image(image, factor):
	return pygame.transform.scale(
		image,
		(
			int(image.get_rect().width * factor),
			int(image.get_rect().height * factor)
		)
	)


def rotate_image_centered(image, rect, angle):
    rotated_image = pygame.transform.rotate(image, angle)
    rotated_rect = rotated_image.get_rect(center=rect.center)
    return rotated_image, rotated_rect


class Player(pygame.sprite.Sprite):

	def __init__(self, image, x, y, speed, angle):
		pygame.sprite.Sprite.__init__(self)
		self.default_image = image
		self.image = image
		self.rect = self.image.get_rect()
		self.rect.x = x
		self.rect.y = y
		self.rect.center = self.rect.x, self.rect.y
		self.speed = speed
		self.angle = angle

	def move(self, direction):
		if direction in ('up', 'down', 'left', 'right'):
			dtime = clock.get_time()
			if direction == 'up':
				self.rect.y -= self.speed * dtime
			if direction == 'down':
				self.rect.y += self.speed * dtime
			if direction == 'left':
				self.rect.x -= self.speed * dtime
			if direction == 'right':
				self.rect.x += self.speed * dtime
		else:
			raise Exception('InvalidDirectionException')

		self._keep_player_into_screen()

	def rotate(self, dangle):
		self.angle += dangle
		self.image, self.rect = rotate_image_centered(self.default_image, self.rect, self.angle)

	def set_angle(self, new_angle):
		self.angle = new_angle
		self.image, self.rect = rotate_image_centered(self.default_image, self.rect, self.angle)

	def _keep_player_into_screen(self):
		if self.rect.x < 0:
			self.rect.x = 0
		if self.rect.x + self.rect.width > WIDTH:
			self.rect.x = WIDTH - self.rect.width
		if self.rect.y < 0:
			self.rect.y = 0
		if self.rect.y + self.rect.height > HEIGHT:
			self.rect.y = HEIGHT - self.rect.height


class Asteroid(pygame.sprite.Sprite):

	def __init__(self, image, x, y):
		pygame.sprite.Sprite.__init__(self)
		self.image = scale_image(image, random.choice([0.8, 1.0, 1.2, 1.4, 1.7]))
		self.rect = self.image.get_rect()
		self.rect.x = x
		self.rect.y = y
		self.rect.center = self.rect.x, self.rect.y

	def rotate(self, dangle):
		self.angle += dangle
		self.image, self.rect = rotate_image_centered(self.default_image, self.rect, self.angle)


class AsteroidManager:

	def __init__(self, asteroids):
		self.asteroids = asteroids

	def move_asteroids(self):
		for asteroid in asteroids:
			print(asteroid.rect.x, asteroid.rect.y)


class InputManager:

	def __init__(self, player):
		self.player = player

	def handle_keyboard(self):
		self.set_angle_by_mouse_position()
		self.handle_keyboard_player_inputs()

	def set_angle_by_mouse_position(self):
		mouse_position = pygame.mouse.get_pos()
		adiacent = self.player.rect.centerx - mouse_position[0]
		opposite = self.player.rect.centery - mouse_position[1]

		if opposite == 0:
			new_player_angle = 0
		elif opposite < 0:
			new_player_angle = 180 + math.degrees(
				math.atan(adiacent / opposite)
			)
		else:
			new_player_angle = math.degrees(
				math.atan(adiacent / opposite)
			)

		self.player.set_angle(new_player_angle)

	def handle_keyboard_player_inputs(self):
		dtime = clock.get_time()
		keys_pressed = pygame.key.get_pressed()
		if keys_pressed[K_w]:
			self.player.move('up')
		if keys_pressed[K_s]:
			self.player.move('down')
		if keys_pressed[K_a]:
			self.player.move('left')
		if keys_pressed[K_d]:
			self.player.move('right')

	def DEBUG_show_player_ray(self):
		pygame.draw.line(window_surface, (255, 0, 0), (player.rect.centerx, player.rect.centery), (pygame.mouse.get_pos()[0], pygame.mouse.get_pos()[1]))


player_image = pygame.image.load(os.getcwd() + '/sprites/players/player.png')
player_image = scale_image(player_image, 4)


player = Player(player_image, WIDTH//2, HEIGHT//2, 1.25, 0)
player_group = pygame.sprite.GroupSingle(player)

asteroid_image = pygame.image.load(os.getcwd() + '/sprites/objects/enemy.png')
num_asteroids = 8
asteroids = [
	Asteroid(
		asteroid_image,
		random.randint(0, WIDTH),
		random.randint(0, HEIGHT)
	) for _ in range(0, num_asteroids)
]

asteroids_group = pygame.sprite.Group(asteroids)


input_manager = InputManager(player)

window_surface = pygame.display.set_mode(GAME_RES, HWSURFACE|HWACCEL|DOUBLEBUF)
pygame.display.set_caption(f"PyShooter - Version: {__version__} - {__author__}")

clock = pygame.time.Clock()

game_ended = False
while not game_ended:

	for event in pygame.event.get():
		if event.type == QUIT:
			game_ended = True
		if event.type == KEYDOWN:
			if event.key == K_ESCAPE:
				game_ended = True

	input_manager.handle_keyboard()

	window_surface.fill(BG_COLOR)

	player_group.draw(window_surface)
	asteroids_group.draw(window_surface)

	pygame.display.update()
	clock.tick(FPS)

pygame.quit()

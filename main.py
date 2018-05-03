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
			image.get_rect().width * factor,
			image.get_rect().height * factor
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

	def move(self):
		pass

	def rotate(self, dangle):
		self.angle += dangle
		self.image, self.rect = rotate_image_centered(self.default_image, self.rect, self.angle)

	def set_angle(self, new_angle):
		self.angle = new_angle
		self.image, self.rect = rotate_image_centered(self.default_image, self.rect, self.angle)

player_image = pygame.image.load(os.getcwd() + '/sprites/players/player.png')
player_image = scale_image(player_image, 4)


player = Player(player_image, WIDTH//2, HEIGHT//2, 10, 0)
player_group = pygame.sprite.GroupSingle(player)

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


	player.rotate(0.5)

	window_surface.fill(BG_COLOR)

	player_group.draw(window_surface)

	pygame.display.update()
	clock.tick(FPS)

pygame.quit()

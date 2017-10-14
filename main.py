import sys
import math
import pygame
import random


class Settings:

	def __init__(self):
		pass

	def read_settings(self, filename):
		with open(filename, "r") as input_file:
			pass


class Lagometer:

	def __init__(self):
		pass

	def draw(self):
		pass


class GameLogicController:

	def __init__(self):
		pass

	def loop(self):
		pass


class EventController:

	def __init__(self):
		pass

	def get_buttons(self):
		pass


class Enemy:

	def __init__(self):
		pass

	def draw(self):
		pass


class Player:

	def __init__(self, x, y):
		self.x = x
		self.y = y

	def __calc_player_angle(self):
		pass

	def __keep_player_under_area(self):
		pass

	def __check_collosions(self):
		pass

	def is_dead(self):
		pass

	def __fire(self):
		pass

	def draw(self):
		pass


class VFX:

	def __init__(self):
		pass

	def draw(self):
		pass

	def __draw_background_particles(self):
		pass

	def __draw_player_backfire(self):
		pass

	def __draw_background_planet(self):
		pass

	def __draw_shooting_stars(self):
		pass


if __name__ == "__main__":
	gc = GameLogicController()
	gc.loop()

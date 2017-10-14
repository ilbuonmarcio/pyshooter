import sys
import math
import pygame
import random


class Settings:
	"""
	Game Settings controller

	It reads from a file and acts as a database
	of informations in which the game supplies
	"""

	def __init__(self):
		pass

	def read_settings(self, filename):
		with open(filename, "r") as input_file:
			pass


class Lagometer:
	"""
	Minecraft's lagometer python similar implementation

	Simple graph that shows you information about
	game's ingame performance
	"""

	def __init__(self):
		pass

	def draw(self):
		pass


class GameLogicController:
	"""
	Entire Game's Logic Controller

	It handles all the game logic
	"""

	def __init__(self):
		pass

	def loop(self):
		pass


class EventController:
	"""
	Game's Event Controller

	It handles all the game events
	"""

	def __init__(self):
		pass

	def get_buttons(self):
		pass


class Enemy:
	"""
	Enemy logical representation
	"""

	def __init__(self):
		pass

	def draw(self):
		pass


class Player:
	"""
	Player logical representation
	"""

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


class VFXController:
	"""
	Visual Effects Controller

	From this you can decide to draw or not vfxs
	"""

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

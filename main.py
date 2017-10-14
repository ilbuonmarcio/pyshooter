import sys
import math
import random
import pygame
from pygame.locals import *


class Settings:
	"""
	Game Settings controller

	It reads from a file and acts as a database
	of informations in which the game supplies
	"""

	def __init__(self, filename="settings.txt"):
		self.filename = filename
		self.resolution = 800, 600
		self.FPS = 240
		self.window_parameters = HWACCEL
		self.game_caption = "Default Game Title"
		self.read_settings()

	def read_settings(self):
		with open(self.filename, "r") as input_file:

			for line in input_file:
				line = line.split(';')

				if line[0] == "resolution" and 'x' in line[1]:
					resolution = line[1].split('x')
					self.resolution = int(resolution[0]), int(resolution[1])

				if line[0] == "fps":
					self.FPS = int(line[1])

				if line[0] == "gamecaption":
					self.game_caption = line[1][:len(line[1])-1]

				if line[0] == "windowparameters":

					parameters = []
					accepted_parameters = {
						"FULLSCREEN" : FULLSCREEN,
						"HWACCEL" : HWACCEL,
						"HWSURFACE" : HWSURFACE,
						"DOUBLEBUF" : DOUBLEBUF
					}

					for parameter in line[1].replace("\n", "").split("|"):

						if parameter in accepted_parameters:
							parameters.append(accepted_parameters[parameter])

		for parameter in parameters:
			self.window_parameters = self.window_parameters | parameter

	def get_resolution(self):
		return self.resolution

	def get_window_parameters(self):
		return self.window_parameters

	def get_fps(self):
		return self.FPS

	def get_game_caption(self):
		return self.game_caption


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


class TimeController:
	"""
	Game's Time Controller

	It handles general game timings
	"""

	def __init__(self):
		self.clock = pygame.time.Clock()


class GameLogicController:
	"""
	Entire Game's Logic Controller

	It handles all the game logic
	"""

	def __init__(self):
		self.settings = Settings()
		self.event_controller = EventController()
		self.time_controller = TimeController()
		self.event_controller.set_game_controller(self)
		self.window = pygame.display
		try:
			self.surface = self.window.set_mode(
				self.settings.get_resolution(),
				self.settings.get_window_parameters()
			)
		except Exception(e): # If screen resolution too big, set default
			self.surface = self.window.set_mode(
				(0, 0),
				self.settings.get_window_parameters()
			)

		self.window.set_caption(self.settings.get_game_caption())
		self.FPS = self.settings.get_fps()
		self.game_ended = False

	def loop(self):
		while not self.game_ended:

			self.event_controller.parse_events()

			self.time_controller.clock.tick(self.FPS)

			self.surface.fill((125, 125, 125))

			self.window.update()

			if self.game_ended:
				self.close_game()

	def end_gameplay(self):
		self.game_ended = True

	def close_game(self):
		pygame.quit()
		exit()




class EventController:
	"""
	Game's Event Controller

	It handles all the game events
	"""

	def __init__(self):
		pass

	def set_game_controller(self, game_controller):
		self.game_controller = game_controller

	def parse_events(self):
		for event in pygame.event.get():
			if event.type == QUIT:
				self.game_controller.end_gameplay()

			if event.type == KEYDOWN:
				if event.key == K_F4 or event.key == K_ESCAPE:
					self.game_controller.end_gameplay()

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

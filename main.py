import random
import math
import os
import pygame
from pygame.locals import *

__version__ = "BETA"
__author__ = "github.com@marcioz98"

os.environ['SDL_VIDEO_CENTERED'] = '1'

pygame.init()

GAME_RES = WIDTH, HEIGHT = int(pygame.display.Info().current_w * 0.7), int(pygame.display.Info().current_h * 0.7)
ASTEROIDS_SPAWN_SPOTS = [
    [-WIDTH * 0.25, -HEIGHT * 0.25],
    [-WIDTH * 0.25,  0            ],
    [-WIDTH * 0.25,  HEIGHT * 0.25],
    [-WIDTH * 0.25,  HEIGHT * 0.5 ],
    [-WIDTH * 0.25,  HEIGHT * 0.75],
    [-WIDTH * 0.25,  HEIGHT       ],
    [-WIDTH * 0.25,  HEIGHT * 1.25],

    [-WIDTH * 0.25,  HEIGHT * 1.25],
    [            0,  HEIGHT * 1.25],
    [ WIDTH * 0.25,  HEIGHT * 1.25],
    [  WIDTH * 0.5,  HEIGHT * 1.25],
    [ WIDTH * 0.75,  HEIGHT * 1.25],
    [        WIDTH,  HEIGHT * 1.25],
    [ WIDTH * 1.25,  HEIGHT * 1.25],

    [ WIDTH * 1.25,  HEIGHT       ],
    [ WIDTH * 1.25,  HEIGHT * 0.75],
    [ WIDTH * 1.25,  HEIGHT * 0.5 ],
    [ WIDTH * 1.25,  HEIGHT * 0.25],
    [ WIDTH * 1.25,  0            ],
    [ WIDTH * 1.25, -HEIGHT * 0.25],

    [ WIDTH       , -HEIGHT * 0.25],
    [ WIDTH * 0.75, -HEIGHT * 0.25],
    [ WIDTH * 0.5 , -HEIGHT * 0.25],
    [ WIDTH * 0.25, -HEIGHT * 0.25],
    [            0, -HEIGHT * 0.25]
]

FPS = 240
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
        self.bullet_speed = 1.3

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

    def fire(self):
        bullets_group.add(Bullet(bullet_image, self.rect.centerx, self.rect.centery, self.angle, self.bullet_speed))


class Bullet(pygame.sprite.Sprite):

    def __init__(self, image, x, y, angle, speed):
        pygame.sprite.Sprite.__init__(self)
        self.default_image = image
        self.rect = self.default_image.get_rect()
        self.rect.x = x
        self.rect.y = y
        self.rect.center = self.rect.x, self.rect.y
        self.speed = speed
        self.angle = angle
        self.image, self.rect = rotate_image_centered(self.default_image, self.rect, self.angle)

    def move(self):
        dtime = clock.get_time()
        self.rect.x -= math.sin(math.radians(self.angle)) * dtime
        self.rect.y -= math.cos(math.radians(self.angle)) * dtime


class BulletManager:

    def __init__(self, bullets):
        self.bullets = bullets

    def manage(self):
        self.move_bullets()
        self.destroy_hitted_asteroids()
        self.delete_if_out_of_screen()

    def move_bullets(self):
        for bullet in self.bullets:
            bullet.move()

    def delete_if_out_of_screen(self):
        for bullet in self.bullets:
            if bullet.rect.x + bullet.rect.width < 0:
                self.bullets.remove(bullet)
            elif bullet.rect.x > WIDTH:
                self.bullets.remove(bullet)
            elif bullet.rect.y + bullet.rect.height < 0:
                self.bullets.remove(bullet)
            elif bullet.rect.y > HEIGHT:
                self.bullets.remove(bullet)

    def destroy_hitted_asteroids(self):
        for bullet in self.bullets:
            for asteroid in pygame.sprite.spritecollide(bullet, asteroids_group, False):
                asteroid.respawn()
                self.bullets.remove(bullet)



class Asteroid(pygame.sprite.Sprite):

    def __init__(self, image, x, y):
        pygame.sprite.Sprite.__init__(self)
        self.default_image = scale_image(image, random.choice([0.8, 1.0, 1.2, 1.4, 1.7]))
        self.image = self.default_image
        self.rect = self.image.get_rect()
        self.rect.x = x
        self.rect.y = y
        self.rect.center = self.rect.x, self.rect.y
        self.speed = 0.5

    def set_angle(self, new_angle):
        self.angle = new_angle
        self.image, self.rect = rotate_image_centered(self.default_image, self.rect, self.angle)

    def rotate_by_player_position(self):
        player_position = player.rect.centerx, player.rect.centery
        adiacent = self.rect.centerx - player_position[0]
        opposite = self.rect.centery - player_position[1]

        if opposite == 0:
            new_asteroid_angle = 0
        elif opposite < 0:
            new_asteroid_angle = 180 + math.degrees(
                math.atan(adiacent / opposite)
            )
        else:
            new_asteroid_angle = math.degrees(
                math.atan(adiacent / opposite)
            )

        self.set_angle(new_asteroid_angle)

    def move_into_player(self):
        dtime = clock.get_time()
        self.rect.x -= math.sin(math.radians(self.angle)) * self.speed * dtime
        self.rect.y -= math.cos(math.radians(self.angle)) * self.speed * dtime

    def respawn(self):
        self.rect = self.image.get_rect()
        spot = random.choice(ASTEROIDS_SPAWN_SPOTS)
        self.rect.x = spot[0]
        self.rect.y = spot[1]
        self.rect.center = self.rect.x, self.rect.y


class AsteroidManager:

    def __init__(self, asteroids):
        self.asteroids = asteroids

    def manage(self):
        self.rotate_asteroids()
        self.move_asteroids_into_player()

    def move_asteroids_into_player(self):
        for asteroid in asteroids:
            asteroid.move_into_player()

    def rotate_asteroids(self):
        for asteroid in self.asteroids:
            asteroid.rotate_by_player_position()


class PlayerInputManager:

    def __init__(self, player):
        self.player = player

    def handle_keyboard(self):
        self.handle_keyboard_player_inputs()
        self.handle_mouse_player_inputs()
        self.set_player_angle_by_mouse_position()

    def set_player_angle_by_mouse_position(self):
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

    def handle_mouse_player_inputs(self):
        mouse_buttons_pressed = pygame.mouse.get_pressed()
        if mouse_buttons_pressed[0]:
            self.player.fire()

    def DEBUG_show_player_ray(self):
        pygame.draw.line(window_surface, (255, 0, 0), (player.rect.centerx, player.rect.centery), (pygame.mouse.get_pos()[0], pygame.mouse.get_pos()[1]))


player_image = pygame.image.load(os.getcwd() + '/sprites/players/player.png')
player_image = scale_image(player_image, 2.5)

player = Player(player_image, WIDTH//2, HEIGHT//2, 1.25, 0)
player_group = pygame.sprite.GroupSingle(player)

bullet_image = pygame.image.load(os.getcwd() + '/sprites/objects/fire.png')
bullet_image = scale_image(bullet_image, 1.3)
bullets_group = pygame.sprite.Group()

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


player_input_manager = PlayerInputManager(player)
asteroids_manager = AsteroidManager(asteroids_group)
bullets_manager = BulletManager(bullets_group)

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

    player_input_manager.handle_keyboard()
    asteroids_manager.manage()
    bullets_manager.manage()

    window_surface.fill(BG_COLOR)

    bullets_group.draw(window_surface)
    player_group.draw(window_surface)
    asteroids_group.draw(window_surface)

    pygame.display.set_caption(f"PyShooter - Version: {__version__} - {__author__} - {clock.get_fps()}")

    pygame.display.update()
    clock.tick(FPS)

pygame.quit()

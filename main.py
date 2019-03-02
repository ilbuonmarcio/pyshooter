import random
import math
import os
import pygame
from pygame.locals import *

__version__ = "BETA"
__author__ = "github.com@marcioz98"

os.environ['SDL_VIDEO_CENTERED'] = '1'

pygame.init()

GAME_RES = WIDTH, HEIGHT = int(pygame.display.Info().current_w), int(pygame.display.Info().current_h)
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
BG_COLOR = (
    random.randint(51, 123),
    random.randint(51, 123),
    random.randint(51, 123)
)


def scale_image(image, factor):
    return pygame.transform.scale(
        image,
        (
            int(image.get_rect().width * factor),
            int(image.get_rect().height * factor)
        )
    )

def scale_image_to_specific_resolution(image, resolution):
    return pygame.transform.scale(
            image,
            (
                resolution[0],
                resolution[1]
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
        self.angle = angle + random.random() * random.randint(-3, 4)
        self.image, self.rect = rotate_image_centered(self.default_image, self.rect, self.angle)

    def move(self):
        dtime = clock.get_time()
        self.rect.x -= math.sin(math.radians(self.angle)) * dtime * self.speed
        self.rect.y -= math.cos(math.radians(self.angle)) * dtime * self.speed


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


class Explosion(pygame.sprite.Sprite):

    def __init__(self, x, y, size, animation_time=0.25):
        pygame.sprite.Sprite.__init__(self)
        self.images = explosion_images
        self.current_animation_index = 0
        self.image = self.images[self.current_animation_index]
        self.rect = self.image.get_rect()
        self.rect.x = x
        self.rect.y = y
        self.animation_time = animation_time
        self.delta_animation_time = self.animation_time / len(self.images)
        self.current_animation_time = 0

    def animate(self):
        t = clock.get_time() / 1000
        self.current_animation_time += t
        if self.current_animation_time >= self.delta_animation_time:
            self.current_animation_time -= self.delta_animation_time
            self.current_animation_index += 1
            if self.current_animation_index < len(self.images):
                self.image = self.images[self.current_animation_index]
            else:
                explosion_group.remove(self)


class GameSettings:

    def __init__(self, invincible=False):
        self.invincible = invincible


class ExplosionManager:

    def __init__(self, explosion_group):
        self.explosions = explosion_group

    def manage(self):
        self.animate_explosions()

    def animate_explosions(self):
        for explosion in self.explosions:
            explosion.animate()


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
        self.alive = True

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
        explosion_group.add(Explosion(self.rect.x, self.rect.y, self.rect.width))
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


class PlayerManager:

    def __init__(self, player, game_settings):
        self.player = player
        self.game_settings = game_settings

    def manage(self):
        self.handle_keyboard_player_inputs()
        self.handle_mouse_player_inputs()
        self.set_player_angle_by_mouse_position()
        self.kill_if_touched()

    def set_player_angle_by_mouse_position(self):
        mouse_position = pygame.mouse.get_pos()
        adiacent = self.player.rect.centerx - mouse_position[0]
        opposite = self.player.rect.centery - mouse_position[1]

        if opposite < 0:
            new_player_angle = 180 + math.degrees(
                math.atan(adiacent / opposite)
            )
        elif opposite > 0:
            new_player_angle = math.degrees(
                math.atan(adiacent / opposite)
            )
        elif opposite == 0:
            if adiacent >= 0 :
                new_player_angle = 90
            else:
                new_player_angle = -90

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

    def kill_if_touched(self):
        if len(pygame.sprite.spritecollide(self.player, asteroids_group, False, collided=pygame.sprite.collide_circle_ratio(0.5))) > 0 and not self.game_settings.invincible:
            global game_ended
            game_ended = True

    def DEBUG_show_player_ray(self):
        pygame.draw.line(window_surface, (255, 0, 0), (player.rect.centerx, player.rect.centery), (pygame.mouse.get_pos()[0], pygame.mouse.get_pos()[1]))


class BackgroundMovingObject:

    def __init__(self):
        self.x = random.randint(0, WIDTH)
        self.y = random.randint(0, HEIGHT)
        self.speed = random.randrange(1, 3) / 20
        self.angle = random.randint(0, 360)

    def move(self):
        dtime = clock.get_time()
        self.x -= math.sin(math.radians(self.angle)) * dtime * self.speed
        self.y -= math.cos(math.radians(self.angle)) * dtime * self.speed


class Star(BackgroundMovingObject):

    def __init__(self):
        BackgroundMovingObject.__init__(self)
        self.size = random.randint(2, 8)
        self.color = (
            random.randint(230, 255),
            random.randint(230, 255),
            random.randint(230, 255)
        )

    def move(self):
        BackgroundMovingObject.move(self)

        if self.x + self.size < 0:
            self.x = WIDTH
        if self.x > WIDTH:
            self.x = 0
        if self.y + self.size < 0:
            self.y = HEIGHT
        if self.y > HEIGHT:
            self.y = 0

    def draw(self, surface):
        pygame.draw.rect(surface, self.color, (int(self.x), int(self.y), self.size, self.size))



class BackgroundEffectsManager:

    def __init__(self, num_stars):
        self.num_stars = num_stars
        self.stars = [Star() for _ in range(0, self.num_stars)]

    def manage(self):
        self.move()

    def move(self):
        for star in self.stars:
            star.move()

    def draw(self, surface):
        for star in self.stars:
            star.draw(surface)

# ghost_surface = pygame.Surface((WIDTH, HEIGHT), SRCALPHA)
# ghost_surface.fill((0, 0, 0, 50))

player_image = pygame.image.load(os.getcwd() + '/sprites/players/player.png')
player_image = scale_image(player_image, 2.5)

player = Player(player_image, WIDTH//2, HEIGHT//2, 1.25, 0)
player_group = pygame.sprite.GroupSingle(player)

bullet_image = pygame.image.load(os.getcwd() + '/sprites/objects/bullet.png')
bullet_image = scale_image(bullet_image, 4)
bullets_group = pygame.sprite.Group()

explosion_images = [
            scale_image(pygame.image.load(filename), 2)
            for filename in (
                "./sprites/effects/explosion/explosion0.png",
                "./sprites/effects/explosion/explosion1.png",
                "./sprites/effects/explosion/explosion2.png",
                "./sprites/effects/explosion/explosion3.png",
                "./sprites/effects/explosion/explosion4.png",
                "./sprites/effects/explosion/explosion5.png",
                "./sprites/effects/explosion/explosion6.png",
            )
        ]

explosion_group = pygame.sprite.Group()

asteroid_image = pygame.image.load(os.getcwd() + '/sprites/objects/enemy.png')
asteroid_image = scale_image(asteroid_image, 2)
num_asteroids = 18
asteroids = [
    Asteroid(
        asteroid_image,
        *random.choice(ASTEROIDS_SPAWN_SPOTS),
    ) for _ in range(0, num_asteroids)
]

asteroids_group = pygame.sprite.Group(asteroids)

game_settings = GameSettings(invincible=True)
player_manager = PlayerManager(player, game_settings)
explosion_manager = ExplosionManager(explosion_group)
asteroids_manager = AsteroidManager(asteroids_group)
bullets_manager = BulletManager(bullets_group)
background_effects_manager = BackgroundEffectsManager(200)

window_surface = pygame.display.set_mode(GAME_RES, HWSURFACE|HWACCEL|DOUBLEBUF|FULLSCREEN)
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

    player_manager.manage()
    explosion_manager.manage()
    asteroids_manager.manage()
    bullets_manager.manage()
    background_effects_manager.manage()

    window_surface.fill(BG_COLOR)
    # window_surface.blit(ghost_surface, (0, 0))

    background_effects_manager.draw(window_surface)

    explosion_group.draw(window_surface)
    bullets_group.draw(window_surface)
    player_group.draw(window_surface)
    asteroids_group.draw(window_surface)

    pygame.display.set_caption(f"PyShooter - Version: {__version__} - {__author__} - {clock.get_fps()}")

    pygame.display.update()
    clock.tick(FPS)

pygame.quit()

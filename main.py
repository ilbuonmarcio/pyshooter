# developer: marcioz98
# email: marciozgaming@gmail.com

import pygame
from pygame.locals import *
import math
from random import randint, random

debugger = False
show_hitboxes = True
bullet_dim = 8
player_dim = 64
num_of_particles = randint(100, 350)
num_of_enemies = randint(2, 4)


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


def draw_background_particles(num_of_particles=num_of_particles):
    for particle in particles:
        if particle[0] > GAME_WIDTH:
            particles.remove(particle)
            particles.append([0, randint(0, GAME_HEIGHT), randint(1, 3),
                  (randint(200, 255), randint(200, 255), randint(200, 255)), random() * 5.0])
        pygame.draw.rect(game_window, particle[3], (particle[0], particle[1], particle[2], particle[2]))
        particle[0] += particle[4]


def draw_background_planet():
    global planet_x
    global planet_y
    global planet_dim
    global planet_color
    global planet_speed
    if planet_x > GAME_WIDTH + planet_dim:
        planet_dim = randint(36, 400)
        planet_color = (randint(35, 110), randint(35, 110), randint(35, 110))
        planet_x, planet_y = 0 - planet_dim, randint(GAME_HEIGHT // 2, GAME_HEIGHT) - planet_dim
        planet_speed = random() * 1.2
    pygame.draw.circle(game_window, planet_color, (planet_x, planet_y), planet_dim)
    planet_x += math.ceil(planet_speed)

def draw_enemies():
    for enemy in enemies:
        adiacent = enemy[1] - player_x
        opposite = enemy[2] - player_y
        try:
            if opposite <= 0:
                enemy[4] = 180 + math.degrees(math.atan(adiacent / opposite))
            else:
                enemy[4] = math.degrees(math.atan(adiacent / opposite))
        except:
            enemy[4] = 0

        enemy[1] -= math.sin(math.radians(enemy[4])) * delta_time * enemy[3]
        enemy[2] -= math.cos(math.radians(enemy[4])) * delta_time * enemy[3]

        pygame.Surface.blit(game_window, enemy[0], (enemy[1], enemy[2]))


if __name__ == "__main__":

    pygame.init()

    RESOLUTION = GAME_WIDTH, GAME_HEIGHT = pygame.display.Info().current_w, pygame.display.Info().current_h
    game_window = pygame.display.set_mode(RESOLUTION, FULLSCREEN | HWSURFACE | HWACCEL)

    particles = [[randint(0, GAME_WIDTH), randint(0, GAME_HEIGHT), randint(1, 3),
                  (randint(200, 255), randint(200, 255), randint(200, 255)), random() * 5.0] for _ in range(0, num_of_particles)]

    enemy_sprite = pygame.image.load("sprites/objects/enemy.png")

    enemies = [[pygame.transform.scale(enemy_sprite, (randint(30, 60), randint(30, 60))),
                randint(0, GAME_WIDTH), randint(0, GAME_HEIGHT), randint(200, 300), None] for _ in range(0, num_of_enemies)]

    player_sprite_default = pygame.transform.scale(pygame.image.load("sprites/players/player.png"),
                                                   (player_dim, player_dim))
    player_sprite = None
    player_x, player_y = pygame.display.Info().current_w // 2 - player_sprite_default.get_width() // 2, pygame.display.Info().current_h // 2 - player_sprite_default.get_height() // 2
    player_angle = 0
    player_speed = 1250

    planet_dim = randint(36, 400)
    planet_color = (randint(35, 110), randint(35, 110), randint(35, 110))
    planet_x, planet_y = 0 - planet_dim, randint(GAME_HEIGHT // 2, GAME_HEIGHT) - planet_dim
    planet_speed = random() * 1.2


    bg_sprite = pygame.transform.scale(pygame.image.load("sprites/environment/bg.jpg"), (GAME_WIDTH, GAME_HEIGHT))

    bullet_sprite = pygame.transform.scale(pygame.image.load("sprites/objects/bullet.png"), (bullet_dim, bullet_dim))
    bullet_speed = 1750
    bullet_list = []

    FPS = 240
    clock = pygame.time.Clock()
    game_ended = False
    pygame.key.set_repeat(10, 10)
    while not game_ended:

        delta_time = clock.tick(FPS) / 1000.0

        # Event Handling
        for event in pygame.event.get():
            if event.type == QUIT:
                game_ended = True
            if event.type == KEYDOWN:
                if event.key == K_F4 or event.key == K_ESCAPE:
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
        pygame.Surface.fill(game_window, (0, 0, 0))
        draw_background_planet()
        draw_background_particles()
        # pygame.Surface.blit(game_window, bg_sprite, (0, 0))
        player_sprite = pygame.transform.rotate(player_sprite_default, player_angle)
        pygame.Surface.blit(game_window, player_sprite,
                            (player_x - player_sprite.get_width() // 2, player_y - player_sprite.get_height() // 2))

        draw_enemies()

        for bullet in bullet_list:
            pygame.Surface.blit(game_window, pygame.transform.rotate(bullet_sprite, bullet[2]), (bullet[0], bullet[1]))
            if show_hitboxes:
                pygame.draw.rect(game_window, (255, 255, 0),
                                 (bullet[0], bullet[1],
                                  bullet_dim, bullet_dim), 1)
        if show_hitboxes:
            pygame.draw.rect(game_window, (255, 255, 0),
                             (player_x - player_sprite.get_width() // 2, player_y - player_sprite.get_height() // 2,
                              player_sprite.get_width(), player_sprite.get_height()), 1)
            pygame.draw.rect(game_window, (255, 255, 0),
                             (planet_x - planet_dim, planet_y - planet_dim, planet_dim * 2, planet_dim * 2), 1)
            for enemy in enemies:
                pygame.draw.rect(game_window, (255, 255, 0),
                                 (enemy[1], enemy[2], enemy[0].get_width(), enemy[0].get_height()), 1)

        pygame.display.flip()

    pygame.quit()

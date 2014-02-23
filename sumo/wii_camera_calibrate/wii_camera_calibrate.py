#pygame draw
import serial
import pygame
from pygame.locals import *
from sys import exit
from random import *

ser = serial.Serial(13, 115200, timeout=1000)
pygame.init()
SCREEN_DEFAULT_SIZE = (1024, 800)
SCREEN_DEFAULT_COLOR = (0, 0 ,0)

screen = pygame.display.set_mode(SCREEN_DEFAULT_SIZE, 0, 32)
screen.fill(SCREEN_DEFAULT_COLOR)

while 1:
    temp = ser.readline()
    temp = temp.strip()
    temp = temp.split(' ')
    screen.fill(SCREEN_DEFAULT_COLOR)
    for s in temp:
        s = s.split(',')
        pygame.draw.circle(screen, (250,250,250), (1024-int(s[1]), 800-int(s[2])), int(s[3])*10)
    pygame.display.update()

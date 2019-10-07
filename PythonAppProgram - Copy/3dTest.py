from collections import deque

import pygame
from pygame.locals import *
from OpenGL.GL import *
from OpenGL.GLU import *
import random
import sys
import time
from serial.tools import list_ports
import serial
import numpy as np
import threading

# JOYSTICK
import pygame
from pygame import locals

vertices = (
    (1, -1, -1),
    (1, 1, -1),
    (-1, 1, -1),
    (-1, -1, -1),
    (1, -1, 1),
    (1, 1, 1),
    (-1, -1, 1),
    (-1, 1, 1),
)

edges = (
    (0, 1),
    (0, 3),
    (0, 4),
    (2, 1),
    (2, 3),
    (2, 7),
    (6, 3),
    (6, 4),
    (6, 7),
    (5, 1),
    (5, 4),
    (5, 7)
)

positions = [
    0,
    50,
    100,
    150,
    200,
    250,
    300,
    350
]
texts = [
    "Placa 1",
    "Placa 2",
    "Placa 3",
    "Placa 4",
    "Placa 5",
    "Placa 6",
    "Placa 7",
    "Placa 8"
]

cubePositions = (
    (positions[0], 0, 0),
    (positions[1], 0, 0),
    (positions[2], 0, 0),
    (positions[3], 0, 0),
    (positions[4], 0, 0),
    (positions[5], 0, 0),
    (positions[6], 0, 0),
    (positions[7], 0, 0)
)


def Cube():
    glBegin(GL_LINES)
    for edge in edges:
        for vertex in edge:
            glVertex3fv(vertices[vertex])
    glEnd()

def axis():
    glColor3f(0, 0, 1)

    glBegin(GL_LINES)
    glVertex3f(0, 0, 0)
    glVertex3f(0, 0, 2)
    glEnd()

    glColor3f(0, 1, 0)

    glBegin(GL_LINES)
    glVertex3f(0, 0, 0)
    glVertex3f(0, 2, 0)
    glEnd()

    glColor3f(1, 0, 0)

    glBegin(GL_LINES)
    glVertex3f(0, 0, 0)
    glVertex3f(2, 0, 0)
    glEnd()

    glColor3f(1, 1, 1)

keys = {
    pygame.K_a: 0,
    pygame.K_s: 0,
    pygame.K_d: 0,
    pygame.K_w: 0,
    pygame.K_r: 0,
    pygame.K_t: 0,
    pygame.K_RETURN: 0
}

def drawText(position, textString, color):
    font = pygame.font.Font(None, 64)
    textSurface = font.render(textString, True, (color[0], color[1], color[2], 255), (0, 0, 0, 255))
    textData = pygame.image.tostring(textSurface, "RGBA", True)
    glRasterPos3d(*position)
    glDrawPixels(textSurface.get_width(), textSurface.get_height(), GL_RGBA, GL_UNSIGNED_BYTE, textData)

def main():
    rolido = 0
    cabeceo = 0
    orientacion = 0

    targetRolido = 0
    targetCabeceo = 0
    targetOrientacion = 0

    targetOrientacion2 = 0
    targetCabeceo2 = 0
    targetRolido2 = 0

    pygame.init()
    display = (800, 600)
    pygame.display.set_mode(display, DOUBLEBUF | OPENGL)

    gluPerspective(45, (display[0] / display[1]), 0.1, 50.0)

    # glTranslatef(0.0, 0.0, -1)

    glRotatef(0, 0, 0, 0)
    contador = 0

    while True:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                quit()

            if event.type == pygame.KEYDOWN:

                keys[event.key] = 1
            if event.type == pygame.KEYUP:
                keys[event.key] = 0

        texto = ""
        color = (255, 255, 255)


        if keys[pygame.K_a]:
            targetRolido -= 1
            texto = "ROLIDO = %d" % targetRolido
            color = (0, 0, 255)

        elif keys[pygame.K_d]:
            targetRolido += 1
            texto = "ROLIDO %d " % targetRolido
            color = (0, 0, 255)

        elif keys[pygame.K_s]:
            targetCabeceo -= 1
            texto = "CABECEO %d" % targetCabeceo
            color = (255, 0, 0)

        elif keys[pygame.K_w]:
            targetCabeceo += 1
            texto = "CABECEO %d" % targetCabeceo
            color = (255, 0, 0)

        elif keys[pygame.K_r]:
            targetOrientacion -= 1
            texto = "ORIENTACION %d" % targetOrientacion
            color = (0, 255, 0)

        elif keys[pygame.K_t]:
            targetOrientacion += 1
            texto = "ORIENTACION %d" % targetOrientacion
            color = (0, 255, 0)

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)

        # nos movemos para atras
        glPushMatrix()
        glTranslatef(0, 0, -10)

        drawText((- 8, 6, -10), "Presionar teclas A S D W R T", (255, 255, 255))
        drawText((- 4, 4, -10), texto, color)



        # yaw - orientacion
        glRotatef(orientacion, 0, 1, 0)

        # roll - rolido
        glRotatef(rolido, 0, 0, 1)

        # pitch - cabeceo
        glRotatef(cabeceo, 1, 0, 0)

        axis()

        Cube()

        glPopMatrix()

        if keys[pygame.K_RETURN]:
            orientacion = 0
            rolido = 0
            cabeceo = 0

            targetOrientacion2 = targetOrientacion
            targetRolido2 = targetRolido
            targetCabeceo2 = targetCabeceo

        orientacion += (targetOrientacion2 - orientacion)/100

        if abs(orientacion - targetOrientacion2) < 0.1:
            orientacion = targetOrientacion2
            rolido += (targetRolido2 - rolido)/100
            if abs(rolido - targetRolido2) < 0.1:
                rolido = targetRolido2
                cabeceo += (targetCabeceo2 - cabeceo)/100

        #print(orientacion, rolido, cabeceo)

        # if abs(orientacion - targetOrientacion) < 0.01 and abs(rolido - targetRolido) < 0.01 and abs(cabeceo - targetCabeceo) < 0.01:
        #     onFresh = True
        # else:
        #     onFresh = False

        #if onFresh:
            #print("Refresh")
        if contador > 10:
            pygame.display.flip()
            contador = 0

        contador += 1
        #pygame.time.wait(10)


if __name__ == "__main__":
    main()

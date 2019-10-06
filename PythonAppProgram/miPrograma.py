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


def drawText(position, textString):
    font = pygame.font.Font(None, 64)
    textSurface = font.render(textString, True, (255, 255, 255, 255), (0, 0, 0, 255))
    textData = pygame.image.tostring(textSurface, "RGBA", True)
    glRasterPos3d(*position)
    glDrawPixels(textSurface.get_width(), textSurface.get_height(), GL_RGBA, GL_UNSIGNED_BYTE, textData)

rolido = np.zeros(8)
cabeceo = np.zeros(8)
orientacion = np.zeros(8)

dataQueue = []


codes = {
    "1": 0,
    "2": 1,
    "3": 2,
    "4": 3,
    "5": 4,
    "6": 5,
    "7": 6,
    "8": 7
}

def decodeInfo(text):
    try:
        code = text[0]

        mode = text[1]

        signo = text[2]

        angulo = int(text[3]) * 100 + int(text[4]) * 10 + int(text[5])

        if signo == "-":
            angulo = -angulo

        return code, mode, angulo
    except:
        print("¡Some problems receiving!")

        return None

comunic = None
conectado = False
queue = []

def threadGetData():
    global comunic
    global conectado

    while 1:
        print("hello", conectado)
        pygame.time.wait(10)

        if not conectado:
            try:
                ports = serial.tools.list_ports.comports()

                available_ports = []

                for p in ports:
                    available_ports.append(p.device)

                print("Puertos disponibles: " + str(available_ports))

                comunic = serial.Serial(available_ports[0], baudrate=9600, timeout=1)
                print("Conectado!")
                #print(data)
                conectado = True
            except:
                comunic = None
                print("Intentando conectar ... ")

        else:
            try:
                data = comunic.readline()

                print("data", data)

                data = data.decode("ascii")
                dataString = str(data)
                print("Informacion obtenida: ", dataString)
                if dataString != "":
                    try:
                        code, mode, angulo = decodeInfo(dataString)
                        queue.append({
                            "code": code,
                            "mode": mode,
                            "angulo": angulo
                        })

                    except:
                        print("Información corrompida")
            except:
                print("Error with decoding")



def main():
    global comunic

    status = 0

    pygame.init()
    display = (800, 600)
    pygame.display.set_mode(display, DOUBLEBUF | OPENGL)

    gluPerspective(45, (display[0]/display[1]), 0.1, 50.0)

    #glTranslatef(0.0, 0.0, -1)


    glRotatef(0, 0, 0, 0)
    rotation = 0
    rotationB = 0
    upPressed = 0
    downPressed =0
    leftPressed = 0
    rightPressed = 0

    camPosition = 0
    updateCounter = 0

    comunic = None

    t1 = threading.Thread(target = threadGetData, args = ())
    t1.start()

    while True:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                quit()
            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_LEFT:
                    if status > 0:
                        status -= 1
                if event.key == pygame.K_RIGHT:
                    if status < 8:
                        status += 1

                    rightPressed = 1
                if event.key == pygame.K_UP:
                    upPressed = 1
                if event.key == pygame.K_DOWN:
                    downPressed = 1

            if event.type == pygame.KEYUP:
                if event.key == pygame.K_LEFT:
                    leftPressed = 0
                if event.key == pygame.K_RIGHT:
                    rightPressed = 0
                if event.key == pygame.K_UP:
                    upPressed = 0
                if event.key == pygame.K_DOWN:
                    downPressed = 0


        if len(queue) > 0:

            try:
                placa, modo, angulo = queue.pop()

                if modo == "R":
                    rolido[codes[modo]] = angulo
                if modo == "C":
                    cabeceo[codes[modo]] = angulo
                if modo == "O":
                    orientacion[codes[modo]] = angulo
            except:
                print("Algo raro sucedio")

        #print(status)

        targetPostion = -positions[status]

        camPosition += (targetPostion - camPosition)/10

        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT)

        glTranslatef(camPosition, 0.0, -10)

        for i in range(len(positions)):
            drawText((positions[i]-2, 6, -10), texts[i])

        for i in range(len(cubePositions)):
            glTranslate(
                cubePositions[i][0],
                cubePositions[i][1],
                cubePositions[i][2]
            )

            #rolido
            glRotatef(rolido[i], 0, 0, 1)
            # orientacion
            glRotatef(orientacion[i], 0, 1, 0)
            #cabeceo
            glRotatef(cabeceo[i], 1, 0, 0)

            Cube()
            # cabeceo
            glRotatef(cabeceo[i], -1, 0, 0)
            # orientacion
            glRotatef(orientacion[i], 0, -1, 0)
            # rolido
            glRotatef(rolido[i], 0, 0, -1)



            glTranslate(
                -cubePositions[i][0],
                -cubePositions[i][1],
                -cubePositions[i][2]
            )

        #glTranslatef(-camPosition, 0.0, -15)

        glTranslatef(-camPosition, 0.0, 10)

        pygame.display.flip()
        pygame.time.wait(10)


if __name__ == "__main__":
    main()

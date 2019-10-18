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

def Cube():
    glBegin(GL_LINES)
    for edge in edges:
        for vertex in edge:
            glVertex3fv(vertices[vertex])
    glEnd()


def drawText(position, textString, color):
    font = pygame.font.Font(None, 50)
    textSurface = font.render(textString, True, (color[0], color[1], color[2], 255), (0, 0, 0, 255))
    textData = pygame.image.tostring(textSurface, "RGBA", True)
    glRasterPos3d(*position)
    glDrawPixels(textSurface.get_width(), textSurface.get_height(), GL_RGBA, GL_UNSIGNED_BYTE, textData)

def smallDrawText(position, textString, color):
    font = pygame.font.Font(None, 30)
    textSurface = font.render(textString, True, (color[0], color[1], color[2], 255), (0, 0, 0, 255))
    textData = pygame.image.tostring(textSurface, "RGBA", True)
    glRasterPos3d(*position)
    glDrawPixels(textSurface.get_width(), textSurface.get_height(), GL_RGBA, GL_UNSIGNED_BYTE, textData)

rolido = np.zeros(8)
cabeceo = np.zeros(8)
orientacion = np.zeros(8)

showRolido = np.zeros(8)
showCabeceo = np.zeros(8)
showOrientacion = np.zeros(8)

targetShowRolido = np.zeros(8)
targetShowCabeceo = np.zeros(8)
targetShowOrientacion = np.zeros(8)

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
        code = int(text[0])

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
iv = ""

def threadGetData():
    global comunic
    global conectado
    global showCabeceo, showRolido, showOrientacion, targetShowCabeceo, targetShowOrientacion, targetShowRolido

    while 1:
        pygame.time.wait(10)

        if not conectado:
            try:
                ports = serial.tools.list_ports.comports()

                available_ports = []

                for p in ports:
                    available_ports.append(p.device)

                print("Puertos disponibles: " + str(available_ports))
                port = input("Seleccione nombre de puerto: ")

                comunic = serial.Serial(port, baudrate=4800, timeout=1)

                print("Conectado!")
                #print(data)
                conectado = True
            except:
                comunic = None
                print("Intentando conectar ... ")

        else:
            try:
                # global iv
                # while 1:
                #     try:
                #         iv = comunic.read()
                #         print(iv)
                #     except:
                #         pass
                iv = ""
                data = ""
                while iv != 'w':
                    try:
                        iv = comunic.read().decode("ascii")
                        data += iv
                    except:
                        pass

                data = data[-7:-1]


                print("data", data)

                data = data
                dataString = str(data)
                print("Informacion obtenida: ", dataString)
                if dataString != "":
                    try:
                        code, mode, angulo = decodeInfo(dataString)

                        print(code, mode, angulo)
                        code -= 1

                        if mode == "R":
                            rolido[code] = angulo
                        if mode == "C":
                            cabeceo[code] = angulo
                        if mode == "O":
                            orientacion[code] = angulo

                        print(code, orientacion)

                        print("Informacion actualizada")
                    except:
                        print("Información corrompida")
            except:
                print("Error with decoding")

keys = {
    pygame.K_RETURN: 0
}


def main():
    global comunic
    global showCabeceo, showRolido, showOrientacion, targetShowCabeceo, targetShowOrientacion, targetShowRolido
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

        enterPressed = False

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

                if event.type == pygame.KEYDOWN:
                    keys[event.key] = 1
                    if event.key == pygame.K_RETURN:
                        enterPressed = True
                if event.type == pygame.KEYUP:
                    keys[event.key] = 0



        #print(status)

        targetPostion = -positions[status]

        camPosition += (targetPostion - camPosition)/10

        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT)

        glTranslatef(camPosition, 0.0, -10)

        for i in [status-1, status, status + 1]:
            if i < 0:
                continue
            if i >= 8:
                continue

            drawText((positions[i]-2, 6, -10), texts[i], (255, 255, 255))
            smallDrawText((positions[i]-2.5, 5, -10), "(Presionar Enter)",(255,255,255))

            drawText((positions[i]-4, -6, -18), "Orientacion: %d " % orientacion[i], (0, 255, 0))
            drawText((positions[i]-4, -8, -18), "Rolido: %d " % rolido[i], (0, 0, 255))
            drawText((positions[i]-4, -10, -18), "Cabeceo: %d " % cabeceo[i], (255, 0, 0))


            glTranslate(
                cubePositions[i][0],
                cubePositions[i][1],
                cubePositions[i][2]
            )

            # yaw - orientacion
            glRotatef(-showOrientacion[i], 0, 1, 0)

            # roll - rolido
            glRotatef(showRolido[i], 0, 0, 1)

            # pitch - cabeceo
            glRotatef(showCabeceo[i], 1, 0, 0)

            axis()
            Cube()

            # pitch - cabeceo
            glRotatef(-showCabeceo[i], 1, 0, 0)
            # roll - rolido
            glRotatef(-showRolido[i], 0, 0, 1)

            # yaw - orientacion
            glRotatef(showOrientacion[i], 0, 1, 0)





            glTranslate(
                -cubePositions[i][0],
                -cubePositions[i][1],
                -cubePositions[i][2]
            )

            showOrientacion[i] += (targetShowOrientacion[i] - showOrientacion[i]) / 30
            if abs(showOrientacion[i] - targetShowOrientacion[i]) < 0.5:
                showOrientacion[i] = targetShowOrientacion[i]
                showRolido[i] += (targetShowRolido[i] - showRolido[i]) / 30
                if abs(showRolido[i] - targetShowRolido[i]) < 0.5:
                    showRolido[i] = targetShowRolido[i]
                    showCabeceo[i] += (targetShowCabeceo[i] - showCabeceo[i]) / 30

            if enterPressed:
                showOrientacion[i] = 0
                showCabeceo[i] = 0
                showRolido[i] = 0

                targetShowRolido[i] = rolido[i]
                targetShowOrientacion[i] = orientacion[i]
                targetShowCabeceo[i] = cabeceo[i]

        #glTranslatef(-camPosition, 0.0, -15)

        glTranslatef(-camPosition, 0.0, 10)

        pygame.display.flip()
        #pygame.time.wait(10)


if __name__ == "__main__":
    main()

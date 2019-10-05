from serial import *
import serial.tools.list_ports

from collections import deque

import pygame
from pygame.locals import *
from OpenGL.GL import *
from OpenGL.GLU import *
import random
import sys
import time
import serial

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


def Cube():
    glBegin(GL_LINES)
    for edge in edges:
        for vertex in edge:
            glVertex3fv(vertices[vertex])
    glEnd()


# Inicio del script
print("SERIAL SCRIPT START")

# esto me devuelve en ports la lista de puertos a disposicion
ports = serial.tools.list_ports.comports()

available_ports = []

# itero por los puertos disponibles y guardo el nombre del puerto
for p in ports:
    available_ports.append(p.device)
print("Puertos disponibles: " + str(available_ports))


# Decode mesege, por convencion para cada dato que me llega, primero me llega el ID, despues Roll, head y Orientation
# ID

def decodeInformation(data):
    dataStringList = data.split(':')
    id = dataStringList[0]
    roll = int(dataStringList[1], 10)
    head = int(dataStringList[2], 10)
    ortientation = int(dataStringList[3], 10)
    dataIntList = [id, roll, head, ortientation]
    return dataIntList


# INICIO COMUNICACION DESDE PYTHON
# Me voy a intentar comunicar al primer puerto guardado. Fijarse que sea el que esta en uso
try:
    print("Puerto utilizado: " + str(available_ports[0]))
    comunic = serial.Serial(available_ports[0], baudrate=9600, timeout=1)

    pygame.init()
    display = (800, 600)
    pygame.display.set_mode(display, DOUBLEBUF | OPENGL)

    gluPerspective(45, (display[0] / display[1]), 0.1, 50.0)

    glTranslatef(0.0, 0.0, -8)

    glRotatef(0, 0, 0, 0)
    # Angulos actuales del cubo
    anglerollx = 0
    angleheady = 0
    # Las colas donde estan los compare Values. valores con los que voy a comparar mis angulos actuales a ver si
    # necesito rotar para algun lado
    ValueQueueX = deque()
    ValueQueueX = deque.append(0)
    ValueQueueY = deque()
    ValueQueueY = deque.append(0)
    # Los valores que van a ser los pops de mis colas para poder ir llevando mis angulos del cubo a estos valores,
    # que anteriormente fueron cargados directo del serial a la cola
    CompareValueX = 0
    CompareValueY = 0

    while 1:
        # data2bytes = bytes(data2send, 'utf-8')
        data = comunic.readline().decode('utf-8')
        dataString = str(data)
        # data = "1:+000:+002:+116"
        print("Informacion recibida: " + dataString)
        if (not (dataString == '')):
            dataList = decodeInformation(dataString)
            print(dataList)

            # Cargo mis valores de comparacion:
            # valor de comparacion en X
            if (len(ValueQueueX) != 0):
                CompareValueX = ValueQueueX.popleft()
            # valor de comparacion en Y
            if (len(ValueQueueY) != 0):
                CompareValueY = ValueQueueY.popleft()
            # Cargo la cola X en caso de ser necesario, es decir si hubo un cambio de posicion en X
            if (dataList[2] != CompareValueX):
                ValueQueueX.append(dataList[2])
            # Cargo la cola Y en caso de ser necesario, es decir si hubo un cambio de posicion en Y
            if (dataList[1] != CompareValueY):
                ValueQueueY.append(dataList[1])

            # Evento para cerrar el juego
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    pygame.quit()
                    quit()

            ##4 CASOS DE CHEQUEO DE MOVIMIENTO X E Y
            # si se esta moviendo en ambos sentidos para el lado negativo

            if ((anglerollx > CompareValueX) and (angleheady > CompareValueY)):
                # rotacion en y:
                glRotatef(-1, 0, 1, 0)
                angleheady = angleheady - 1
                # rotacion en x:
                glRotatef(-1, 1, 0, 0)
                anglerollx = anglerollx - 1

            # si se esta moviendo en sentido positivo x y negativo y
            elif ((anglerollx < CompareValueX) and (angleheady > CompareValueY)):
                # rotacion en y:
                glRotatef(-1, 0, 1, 0)
                angleheady = angleheady - 1
                # rotacion en x:
                glRotatef(1, 1, 0, 0)
                anglerollx = anglerollx + 1

            # si esta moviendo en sentido negativo x y positivo y
            elif ((anglerollx > CompareValueX) and (angleheady < CompareValueY)):
                # rotacion en y:
                glRotatef(1, 0, 1, 0)
                angleheady = angleheady + 1
                # rotacion en x:
                glRotatef(-1, 1, 0, 0)
                anglerollx = anglerollx - 1

            # si esta moviendo en sentido positivo x y positivo y
            elif ((anglerollx < CompareValueX) and (angleheady < CompareValueY)):
                # rotacion en y:
                glRotatef(1, 0, 1, 0)
                angleheady = angleheady + 1
                # rotacion en x:
                glRotatef(1, 1, 0, 0)
                anglerollx = anglerollx + 1

            # CASOS DE MOVIMIENTO UNICO EN X O EN Y

            # si esta moviendo en sentido positivo x
            elif ((anglerollx < CompareValueX)):
                # rotacion en x:
                glRotatef(1, 1, 0, 0)
                anglerollx = anglerollx + 1
            # si esta moviendo en sentido negativo x
            elif ((anglerollx > CompareValueX)):
                # rotacion en x:
                glRotatef(-1, 1, 0, 0)
                anglerollx = anglerollx - 1

            # si esta moviendo en sentido positivo y
            elif ((angleheady < CompareValueY)):
                # rotacion en x:
                glRotatef(1, 0, 1, 0)
                angleheady = angleheady + 1
            # si esta moviendo en sentido negativo y
            elif ((angleheady > CompareValueY)):
                # rotacion en x:
                glRotatef(-1, 0, 1, 0)
                angleheady = angleheady - 1
            else:
                glRotatef(0, 0, 0, 0)

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
            Cube()
            pygame.display.flip()
            print("guardado x: " + str(anglerollx))
            print("angulo guardado y " + str(angleheady))
        else:
            print(data)

except IndexError:
    print("No hay puerto conectado")
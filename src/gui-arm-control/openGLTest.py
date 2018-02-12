from OpenGL.GL import shaders
from OpenGL.arrays import vbo
from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *
import glfw
import glm

from ctypes import sizeof, c_float, c_void_p, c_uint, string_at
from linalg import matrix as m
import numpy as np

verticiesOld = (
    (1, -1, -1),
    (1, 1, -1),
    (-1, 1, -1),
    (-1, -1, -1),
    (1, -1, 1),
    (1, 1, 1),
    (-1, -1, 1),
    (-1, 1, 1)
)

edgesOld = (
    (0,1),
    (0,3),
    (0,4),
    (2,1),
    (2,3),
    (2,7),
    (6,3),
    (6,4),
    (6,7),
    (5,1),
    (5,4),
    (5,7)
)

def c_matrix(matrix):
    return (c_float*16)(*m.column_major(matrix))

def fixIndices(EBO):
    for index in EBO:
        for i in range(0, 3):
            index[i] -= 1

def main():
    # Initialize the library
    if not glfw.init():
        return


    # Create a windowed mode window and its OpenGL context
    window = glfw.create_window(800, 600, "Hello World", None, None)
    if not window:
        glfw.terminate()
        return

    # Make the window's context current
    glfw.make_context_current(window)

    print(glGetString(GL_VERSION))
    print(glGetString(GL_SHADING_LANGUAGE_VERSION))

    cylinder = openObj('cylinderPoked.obj')


    #Create the VBO
    cylinderVertices = np.array(cylinder['vertices'], dtype='f')
    cylinderVBO = vbo.VBO(cylinderVertices)

    #Create the index buffer object
    cylinderFaces = np.array(cylinder['polygon_faces'], dtype=np.int32)
    fixIndices(cylinderFaces)
    cylinderEBO = vbo.VBO(cylinderFaces, target=GL_ELEMENT_ARRAY_BUFFER)

    halfCylinderFaces = np.empty([int(len(cylinderFaces) / 2), 3], dtype=np.int32)
    currentIndex = 0
    for i in range(0, len(cylinderFaces)):
        if(i%2 == 0):
            halfCylinderFaces[currentIndex] = (cylinderFaces[i])
            currentIndex += 1
    halfCylinderEBO = vbo.VBO(halfCylinderFaces, target=GL_ELEMENT_ARRAY_BUFFER)


    #Now create the shaders
    VERTEX_SHADER = shaders.compileShader("""
    #version 330
    layout(location = 0) in vec4 position;
    
    uniform mat4 model_matrix;
    uniform mat4 view_matrix;
    uniform mat4 projection_matrix;
    
    void main()
    {
        gl_Position = projection_matrix * view_matrix * model_matrix * vec4(position.x, position.y, position.z, 1.0);
    }
    """, GL_VERTEX_SHADER)

    FRAGMENT_SHADER = shaders.compileShader("""
    #version 330
    out vec4 outputColor;
    void main()
    {
        outputColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
    }
    """, GL_FRAGMENT_SHADER)

    shader = shaders.compileProgram(VERTEX_SHADER, FRAGMENT_SHADER)


    modelMatrixUniLocation = glGetUniformLocation(shader, "model_matrix")
    viewMatrixUniLocation = glGetUniformLocation(shader, "view_matrix")
    projectionMatrixUniLocation = glGetUniformLocation(shader, "projection_matrix")
    # colorUniLocation = glGetUniformLocation(shaderProgram, "object_color") TODO

    glUseProgram(shader)
    glEnable(GL_DEPTH_TEST)
    # glEnable(GL_CULL_FACE)
    # glCullFace(GL_BACK)

    currentRotation = 0.0

    # Loop until the user closes the window
    while not glfw.window_should_close(window):
        # Render here, e.g. using pyOpenGL

        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT)

        # generate model matrix
        modelMatrix = glm.mat4()
        # modelMatrix = glm.scale(modelMatrix, glm.vec3(1.0 + currentHeight, 1.0 + currentHeight, 1.0 + currentHeight))
        modelMatrix = glm.rotate(modelMatrix, np.pi * currentRotation / 2.0, (1.0, 0.0, 0.0))

        modelview = m.identity()
        # modelview = m.mul(modelview, m.scale(1.0 + currentHeight, 1.0 + currentHeight, 1.0 + currentHeight))
        modelview = m.mul(modelview, m.translate(tx=currentRotation))


        # generate view matrix

        # viewMatrix = glm.lookAt(glm.vec3(1.0,1.0,1.0), glm.vec3(0.0,0.0,0.0), glm.vec3(0.0,1.0,0.0)) idk if this line works
        viewMatrix = glm.mat4()

        # generate projection matrix
        # projectionMatrix = glm.perspective(45.0, (600.0 / 800.0), -1, 1) idk if this line works
        orhtoProjectionMatrix = glm.ortho( -2 , 2 , -2 , 2 , -2 , 2 )

        projectionMatrix = m.identity()
        projectionMatrix = m.mul(projectionMatrix, m.frustum(-2, 2, -2, 2, 8, 16))
        projectionMatrix = m.mul(projectionMatrix, m.translate(tz=(-12)))
        projectionMatrix = m.mul(projectionMatrix, m.scale(1.5, 1.5, 1.5))

        # orthoProjectionMatrix = m.mul(projectionMatrix, m.ortho(-2, 2, -2, 2, -2, 2))

        glUniformMatrix4fv(modelMatrixUniLocation, 1, GL_FALSE, c_matrix(modelMatrix))
        glUniformMatrix4fv(viewMatrixUniLocation, 1, GL_FALSE, c_matrix(viewMatrix))
        glUniformMatrix4fv(projectionMatrixUniLocation, 1, GL_FALSE, c_matrix(projectionMatrix))

        cylinderVBO.bind()

        glEnableVertexAttribArray(0)
        glVertexAttribPointer(0, 3, GL_FLOAT, False, 0, c_void_p(0))

        # draw cylinder
        cylinderEBO.bind()
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)
        glDrawElements(GL_TRIANGLES, len(cylinderFaces) * 3, GL_UNSIGNED_INT, None)
        cylinderEBO.unbind()

        # draw half the triangles in the cylinder
        halfCylinderEBO.bind()
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL)
        # glDrawElements(GL_TRIANGLES, len(halfCylinderFaces) * 3, GL_UNSIGNED_INT, None)
        halfCylinderEBO.unbind()

        cylinderVBO.unbind()

        currentRotation += 0.01

        # Swap front and back buffers
        glfw.swap_buffers(window)

        # Poll for and process events
        glfw.poll_events()

    glfw.terminate()


def openObj(filename):
    f = open(filename)
    n = 1
    res = {}
    res['vertices'] = []
    res['triangle_faces'] = []
    res['quad_faces'] = []
    res['polygon_faces'] = []
    res['normals'] = []
    for line in f:
        if line[:2] == "v ":
            index1 = line.find(" ") +1 #first number index
            index2 = line.find(" ",index1+1)  # second number index
            index3 = line.find(" ",index2+1) # third number index

            vertex = (float(line[index1:index2]),float(line[index2:index3]),float(line[index3:-1]))
            vertex = (round(vertex[0],6),round(vertex[1],6),round(vertex[2],6))
            res['vertices'].append(vertex)

        elif line[:2] == "vn":
            index1 = line.find(" ") +1 #first number index
            index2 = line.find(" ",index1+1)  # second number index
            index3 = line.find(" ",index2+1) # third number index

            normal = (float(line[index1:index2]),float(line[index2:index3]),float(line[index3:-1]))
            normal = (round(normal[0],2),round(normal[1],2),round(normal[2],2))
            res['normals'].append(normal)

        elif line[0] == "f":
            string = line.replace("//","/")
            #---------------------------------------------------
            i = string.find(" ")+1
            face  = []
            for item in range(string.count(" ")):
                if string.find(" ",i) == -1:
                    face.append(string[i:-1])
                    break
                face.append(string[i:string.find(" ",i)])
                i = string.find(" ",i) +1
            #---------------------------------------------------
            if string.count("/") == 3:
                res['triangle_faces'].append(tuple(face))
            elif string.count("/") == 4:
                res['quad_faces'].append(tuple(face))
            else:
                res['polygon_faces'].append(tuple(face))
    f.close()
    return res

def generateEdges(parsedObj):
    result = []
    for face in parsedObj['polygon_faces']:
        for i in range(0, len(face) - 1):
            result.append((int(face[i]), int(face[i+1])))
        result.append((int(face[0]), int(face[len(face) - 1])))
    return tuple(result)



main()
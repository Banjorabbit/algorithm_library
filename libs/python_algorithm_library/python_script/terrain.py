import numpy as np
from pyqtgraph.Qt import QtCore, QtWidgets
import pyqtgraph.opengl as gl
import sys
from opensimplex import OpenSimplex

class Terrain(object):
    def __init__(self):
        # setup the view window
        self.app = QtWidgets.QApplication(sys.argv)
        self.window = gl.GLViewWidget()
        self.window.setGeometry(0,110, 1920, 1080)
        self.window.show()
        self.window.setWindowTitle('Terrain')
        self.window.setCameraPosition(distance=30, elevation = 8)

        self.nsteps = 1
        self.offset = 0
        self.ypoints = np.arange(-16, 16, self.nsteps)
        self.xpoints = np.arange(-16, 16, self.nsteps)
        self.nfaces = len(self.ypoints)

        # perlin noise
        self.noise = OpenSimplex(0)

        verts, faces, colors = self.mesh()
        
        self.mesh1 = gl.GLMeshItem(
            vertexes = verts,
            faces = faces, 
            faceColors = colors,
            smooth = False, 
            drawEdges = True,
        )

        self.mesh1.setGLOptions('additive')
        self.window.addItem(self.mesh1)

    def mesh(self, offset=0, height=2.5):
        
        faces = []
        colors = []
        verts = np.array([
            [
                x, y, height * self.noise.noise2(x=n / 4 + offset, y= m / 4 + offset)
            ] for n, x in enumerate(self.xpoints) for m, y in enumerate(self.ypoints)
        ], dtype=np.float32)

        for m in range(self.nfaces - 1):
            yoff = m * self.nfaces
            for n in range(self.nfaces - 1):
                faces.append([n + yoff, yoff + n + self.nfaces, yoff + n + self.nfaces + 1])
                faces.append([n + yoff, yoff + n + 1, yoff + n + self.nfaces + 1])
                colors.append([n / self.nfaces,1 - n / self.nfaces, m / self.nfaces, 0.7])
                colors.append([n / self.nfaces,1 - n / self.nfaces, m / self.nfaces, 0.8])

        colors = np.array(colors)
        faces = np.array(faces)

        return verts, faces, colors


    def start(self):
        if (sys.flags.interactive != 1):
            QtWidgets.QApplication.instance().exec_()

    def update(self):
        verts, faces, colors = self.mesh(offset=self.offset)

        self.mesh1.setMeshData(
            vertexes = verts, faces = faces, faceColors = colors
        )
        self.offset -= 0.04

    def animation(self):
        timer = QtCore.QTimer()
        timer.timeout.connect(self.update)
        timer.start(10)
        self.start()


if __name__ == '__main__':
    t = Terrain()
    t.animation()
#!/usr/bin/python2
"""
Emulate an oscilloscope.  Requires the animation API introduced in
matplotlib 1.0 SVN.
This example is from http://matplotlib.org/examples/animation/strip_chart_demo.html (with some modifications)
"""
import numpy as np
from matplotlib.lines import Line2D
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import math

scroll_steps_accumulated = 0

class Scope(object):
    def __init__(self, ax, maxt=10, dt=0.02, zoom_step=1.5):
        self.ax = ax
        self.dt = dt
        self.maxt = maxt
        self.tdata = [0]
        self.ydata = [0]
        self.line = Line2D(self.tdata, self.ydata)
        self.ax.add_line(self.line)
        self.ax.set_ylim(-1000, 10000)
        self.ax.set_xlim(0, self.maxt)
        self.zoom_step = zoom_step

    def resize_y(self, vert_buffer=0.2):
        vert_limits = self.ax.get_ylim()
        ymin = vert_limits[0]
        ymax = vert_limits[1]

        ydata_max = 0.0
        ydata_min = 0.0

        ymin_OutOfBounds = False
        ymax_OutOfBounds = False

        for i in range(len(self.ydata)):
            if self.ydata[i] > ydata_max:
                ydata_max = self.ydata[i]
            if self.ydata[i] < ydata_min:
                ydata_min = self.ydata[i]

        ymax = ydata_max + vert_buffer
        ymin = ydata_min - vert_buffer

        self.ax.set_ylim(ymin, ymax)

    def update(self, y):
        lastt = self.tdata[-1]
        if lastt > self.tdata[0] + self.maxt:  # reset the arrays
            self.tdata = [self.tdata[-1]]
            self.resize_y()
            self.ydata = [self.ydata[-1]]
            self.ax.set_xlim(self.tdata[0], self.tdata[0] + self.maxt)
            self.ax.figure.canvas.draw()

        t = self.tdata[-1] + self.dt
        self.tdata.append(t)
        self.ydata.append(y)
        self.line.set_data(self.tdata, self.ydata)
        return self.line,

    def time_zoom_handler(self, event):

        zoom_multiplier = 1.0
        zoom_multiplier *= math.exp(event.step*self.zoom_step)

        self.maxt *= zoom_multiplier

        if self.tdata[-1] < self.tdata[0] + self.maxt:
            self.line.set_data(self.tdata, self.ydata)
            self.ax.set_xlim(self.tdata[0], self.tdata[-1])
            self.ax.figure.canvas.draw()
            self.tdata = [self.tdata[-1]]
            self.ydata = [self.ydata[-1]]
            self.ax.set_xlim(self.tdata[0], self.tdata[0] + self.maxt)

def emitter(p=0.03):
    # Sine wave instead...
    t = 0.0
    while True:
        t += 0.01
        yield math.sin(t*10.0)

fig, ax = plt.subplots()
scope = Scope(ax, zoom_step=0.2)
f2 = ax.get_figure()

# pass a generator in "emitter" to produce data for the update func
ani = animation.FuncAnimation(fig, scope.update, emitter, interval=2,
                              blit=True)

f2.canvas.mpl_connect('scroll_event', scope.time_zoom_handler)


plt.show()
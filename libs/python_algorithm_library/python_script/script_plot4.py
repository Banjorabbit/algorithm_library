import PySimpleGUI as sg
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import io
from PIL import Image

layout = [
    [sg.Button('Add Sinusoid'), sg.Button('Add Squarewave')],
    [sg.Listbox(values=[], size=(30, 6), key='-WAVEFORMS-'), sg.Button('Remove Waveform')],
    [sg.Canvas(key='-CANVAS-')],
    [sg.Button('Save Figure'), sg.Button('Exit')]
]

window = sg.Window('Waveform Plotter', layout, finalize=True)




# Create a single figure and axis outside the loop
#fig, ax = plt.subplots()
#canvas = FigureCanvasTkAgg(fig, master=None)

waveforms = []
#plot_waveforms(waveforms, ax)  # Initial plot

while True:
    event, values = window.read()

    if event == sg.WINDOW_CLOSED or event == 'Exit':
        break

window.close()

import PySimpleGUI as sg
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import io
from PIL import Image

def plot_waveforms(waveforms, ax):
    ax.clear()  # Clear the plot
    for waveform in waveforms:
        if waveform['type'] == 'sinusoid':
            x = np.linspace(0, 2*np.pi, 100)
            y = np.sin(x * waveform['frequency'])
            ax.plot(x, y, label=f"Sinusoid {waveform['frequency']} Hz")
        elif waveform['type'] == 'squarewave':
            x = np.linspace(0, 2*np.pi, 100)
            y = np.sign(np.sin(x * waveform['frequency']))
            ax.plot(x, y, label=f"Squarewave {waveform['frequency']} Hz")

    if waveforms:
        ax.legend()
    ax.set_xlabel('Time')
    ax.set_ylabel('Amplitude')
    ax.set_title('Waveforms')
    ax.grid(True)

layout = [
    [sg.Button('Add Sinusoid'), sg.Button('Add Squarewave')],
    [sg.Listbox(values=[], size=(30, 6), key='-WAVEFORMS-'), sg.Button('Remove Waveform')],
    [sg.Canvas(key='-CANVAS-')],
    [sg.Button('Save Figure'), sg.Button('Exit')]
]

waveforms = []

# Create a single figure and axis outside the loop
fig, ax = plt.subplots()
canvas = FigureCanvasTkAgg(fig, master=None)

window = sg.Window('Waveform Plotter', layout, finalize=True)

while True:
    event, values = window.read()

    if event == sg.WINDOW_CLOSED or event == 'Exit':
        break
    elif event == 'Add Sinusoid':
        frequency = sg.popup_get_text('Enter frequency (Hz)', 'Add Sinusoid', default_text='1')
        if frequency:
            waveforms.append({'type': 'sinusoid', 'frequency': float(frequency)})
            window['-WAVEFORMS-'].update([f"{waveform['type']} {waveform['frequency']} Hz" for waveform in waveforms])
            plot_waveforms(waveforms, ax)
            canvas.draw()
            canvas.get_tk_widget().pack(side='top', fill='both', expand=1)
    elif event == 'Add Squarewave':
        frequency = sg.popup_get_text('Enter frequency (Hz)', 'Add Squarewave', default_text='1')
        if frequency:
            waveforms.append({'type': 'squarewave', 'frequency': float(frequency)})
            window['-WAVEFORMS-'].update([f"{waveform['type']} {waveform['frequency']} Hz" for waveform in waveforms])
            plot_waveforms(waveforms, ax)
            canvas.draw()
            canvas.get_tk_widget().pack(side='top', fill='both', expand=1)
    elif event == 'Remove Waveform':
        if len(values['-WAVEFORMS-']) > 0:
            selected_index = values['-WAVEFORMS-'][0],
            waveforms.pop(window['-WAVEFORMS-'].get_indexes()[0])
            window['-WAVEFORMS-'].update([f"{waveform['type']} {waveform['frequency']} Hz" for waveform in waveforms])
            plot_waveforms(waveforms, ax)
            canvas.draw()
            canvas.get_tk_widget().pack(side='top', fill='both', expand=1)
    elif event == 'Save Figure':
        if len(waveforms) > 0:
            img_data = io.BytesIO()
            fig.savefig(img_data, format='png')
            img_data.seek(0)
            img = Image.open(img_data)
            img.save('waveforms.png')
            sg.popup('Figure saved as waveforms.png')

window.close()

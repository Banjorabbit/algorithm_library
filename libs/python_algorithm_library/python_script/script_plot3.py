import PySimpleGUIQt as sg
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.backends.backend_agg import FigureCanvasAgg
import io
from PIL import Image

def create_waveforms(waveforms):
    fig = plt.figure()
    for waveform in waveforms:
        if waveform['type'] == 'sinusoid':
            x = np.linspace(0, 2*np.pi, 100)
            y = np.sin(x * waveform['frequency'])
            plt.plot(x, y, label=f"Sinusoid {waveform['frequency']} Hz")
        elif waveform['type'] == 'squarewave':
            x = np.linspace(0, 2*np.pi, 100)
            y = np.sign(np.sin(x * waveform['frequency']))
            plt.plot(x, y, label=f"Squarewave {waveform['frequency']} Hz")
    if waveforms:
        plt.legend()
    plt.xlabel('Time')
    plt.ylabel('Amplitude')
    plt.title('Waveforms')
    plt.grid(True)
    return plt.gcf()

def draw_figure(element, figure):
    plt.close('all')        # erases previously drawn plots
    canv = FigureCanvasAgg(figure)
    buf = io.BytesIO()
    canv.print_figure(buf, format='png')
    if buf is None:
        return None
    buf.seek(0)
    element.update(data=buf.read())
    return canv

layout = [
    [sg.Button('Add Sinusoid'), sg.Button('Add Squarewave'), sg.Button('Remove Waveform')],
    [sg.Listbox(values=[], size=(30, 6), key='-WAVEFORMS-'), sg.Image(key='-IMAGE-')],
    [sg.Canvas(key='-CANVAS-')],
    [sg.Button('Save Figure'), sg.Button('Exit')]
]

# create start window
waveforms = []
window = sg.Window('Waveform Plotter', layout, finalize=True)
image_element = window['-IMAGE-']   
fig = create_waveforms(waveforms)
draw_figure(image_element, fig)

while True:
    event, values = window.read()
    print(event, values)

    if event == sg.WINDOW_CLOSED or event == 'Exit':
        break
    elif event == 'Add Sinusoid':
        frequency = sg.popup_get_text('Enter frequency (Hz)', 'Add Sinusoid', default_text='100')
        if frequency:
            waveforms.append({'type': 'sinusoid', 'frequency': float(frequency)})
            window['-WAVEFORMS-'].update([f"{waveform['type']} {waveform['frequency']} Hz" for waveform in waveforms])
            fig = create_waveforms(waveforms)
            draw_figure(image_element, fig)
    elif event == 'Add Squarewave':
        frequency = sg.popup_get_text('Enter frequency (Hz)', 'Add Squarewave', default_text='100')
        if frequency:
            waveforms.append({'type': 'squarewave', 'frequency': float(frequency)})
            window['-WAVEFORMS-'].update([f"{waveform['type']} {waveform['frequency']} Hz" for waveform in waveforms])
            fig = create_waveforms(waveforms)
            draw_figure(image_element, fig)
    elif event == 'Remove Waveform':
        selected_indexes = values['-WAVEFORMS-']
        if selected_indexes:
            selected_item = selected_indexes[0]  # Get the selected item from the listbox
            for idx, waveform_str in enumerate([f"{waveform['type']} {waveform['frequency']} Hz" for waveform in waveforms]):
                if waveform_str == selected_item:
                    selected_index = idx
                    break
            waveforms.pop(selected_index)
            window['-WAVEFORMS-'].update([f"{waveform['type']} {waveform['frequency']} Hz" for waveform in waveforms])
            fig = create_waveforms(waveforms)
            draw_figure(image_element, fig)
    elif event == 'Save Figure':
        if len(waveforms) > 0:
            img_data = io.BytesIO()
            fig.savefig(img_data, format='png')
            img_data.seek(0)
            img = Image.open(img_data)
            img.save('waveforms.png')
            sg.popup('Figure saved as waveforms.png')

window.close()

import numpy as np
import matplotlib.pyplot as plt
from matplotlib.widgets import Button
import mplcursors

class WaveformPlotter:
    def __init__(self):
        self.fig, self.ax = plt.subplots()
        self.ax.set_xlabel('Time')
        self.ax.set_ylabel('Amplitude')
        self.ax.set_title('Waveform Plot')

        self.lines = []
        self.cursor = mplcursors.cursor(hover=True)
        self.cursor.connect("add", self.on_cursor_hover)

        self.sinusoid_button = Button(plt.axes([0.01, 0.9, 0.1, 0.05]), 'Add Sinusoid')
        self.sinusoid_button.on_clicked(self.add_sin)
        
        self.square_wave_button = Button(plt.axes([0.12, 0.9, 0.1, 0.05]), 'Add Square Wave')
        self.square_wave_button.on_clicked(self.add_square)
        
        self.save_button = Button(plt.axes([0.23, 0.9, 0.1, 0.05]), 'Save as PNG')
        self.save_button.on_clicked(self.save_plot)

    def add_sin(self, event):
        x = np.linspace(0, 10, 1000)
        y = np.sin(x)
        line, = self.ax.plot(x, y, label='Sinusoid')
        remove_button = Button(plt.axes([0.8, 0.9 - 0.05 * len(self.lines), 0.1, 0.04]), 'Remove')
        remove_button.on_clicked(lambda event, l=line, b=remove_button: self.remove_line(l, b))
        self.lines.append((line, remove_button))
        self.ax.legend()
        self.fig.canvas.draw_idle()

    def add_square(self, event):
        x = np.linspace(0, 10, 1000)
        y = np.sign(np.sin(x))
        line, = self.ax.plot(x, y, label='Square Wave')
        remove_button = Button(plt.axes([0.8, 0.9 - 0.05 * len(self.lines), 0.1, 0.04]), 'Remove')
        remove_button.on_clicked(lambda event, l=line, b=remove_button: self.remove_line(l, b))
        self.lines.append((line, remove_button))
        self.ax.legend()
        self.fig.canvas.draw_idle()

    def remove_line(self, line, button):
        if (line, button) in self.lines:
            line.remove()
            button.disconnect(button.on_clicked)
            button.ax.remove()
            self.lines.remove((line, button))
            # Remove line label from legend
            handles, labels = self.ax.get_legend_handles_labels()
            label_index = [i for i, lab in enumerate(labels) if lab == line.get_label()]
            if label_index:
                del handles[label_index[0]]
                del labels[label_index[0]]
                self.ax.legend(handles, labels)
            if self.fig.canvas:  # Check if canvas exists before drawing
                self.fig.canvas.draw_idle()

    def save_plot(self, event):
        filename = input("Enter filename to save plot (without extension): ")
        if not filename:
            filename = "waveform_plot"
        plt.savefig(f"{filename}.png")
        print(f"Plot saved as {filename}.png")

    def on_cursor_hover(self, event):
        if event.artist in [line for line, _ in self.lines]:  # Check if the line still exists
            event.artist.draw(self.fig.canvas.get_renderer())

    def show(self):
        plt.show()

if __name__ == "__main__":
    plotter = WaveformPlotter()
    plotter.show()

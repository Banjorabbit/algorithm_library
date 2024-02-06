# write a python script that creates an interactive GUI where the user can plot different waveforms, like for instance a sinusoid or a square wave. The user should be able to change the frequency and amplitude of the waveforms. The user should also be able to save the plot as a .png file.

import matplotlib.pyplot as plt
import numpy as np

import tkinter as tk
from tkinter import filedialog

class WaveformPlotter:
    def __init__(self, master):
        self.master = master
        self.master.title("Waveform Plotter")
        self.master.geometry("500x300")
        self.master.resizable(True, True)

        self.amplitude_label = tk.Label(self.master, text="Amplitude:")
        self.amplitude_label.grid(row=0, column=0, padx=10, pady=10)
        self.amplitude_entry = tk.Entry(self.master)
        self.amplitude_entry.grid(row=0, column=1, padx=10, pady=10)

        self.frequency_label = tk.Label(self.master, text="Frequency:")
        self.frequency_label.grid(row=1, column=0, padx=10, pady=10)
        self.frequency_entry = tk.Entry(self.master)
        self.frequency_entry.grid(row=1, column=1, padx=10, pady=10)

        self.plot_button = tk.Button(self.master, text="Plot", command=self.plot_waveform)
        self.plot_button.grid(row=2, column=0, padx=10, pady=10)

        self.save_button = tk.Button(self.master, text="Save", command=self.save_plot)
        self.save_button.grid(row=2, column=1, padx=10, pady=10)

    def plot_waveform(self):
        amplitude = float(self.amplitude_entry.get())
        frequency = float(self.frequency_entry.get())
        x = np.linspace(0, 2 * np.pi, 1000)
        y = amplitude * np.sin(2 * np.pi * frequency * x)
        plt.plot(x, y)
        plt.xlabel("Time")
        plt.ylabel("Amplitude")
        plt.title("Sine Waveform")
        plt.show()

    def save_plot(self):
        file_path = filedialog.asksaveasfilename(defaultextension=".png")
        if file_path:
            amplitude = float(self.amplitude_entry.get())
            frequency = float(self.frequency_entry.get())
            x = np.linspace(0, 2 * np.pi, 1000)
            y = amplitude * np.sin(2 * np.pi * frequency * x)
            plt.plot(x, y)
            plt.xlabel("Time")
            plt.ylabel("Amplitude")
            plt.title("Sine Waveform")
            plt.savefig(file_path)
            plt.close()

if __name__ == "__main__":
    root = tk.Tk()
    app = WaveformPlotter(root)
    root.mainloop()
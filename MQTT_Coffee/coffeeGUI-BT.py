import tkinter as tk
from PIL import ImageTk
import socket

# -----------------------------------------------------------------------------
#
# -----------------------------------------------------------------------------

class MainWindow():

    def __init__(self, main):

        # canvas for image
        self.canvas = tk.Canvas(main, width=800, height=600)
        self.canvas.grid(row=0, column=0)

        # images
        self.my_images = []
        self.my_images.append(ImageTk.PhotoImage(file = "Coffee_1.jpg"))
        self.my_images.append(ImageTk.PhotoImage(file = "Coffee_2.jpg"))
        self.my_image_number = 0

        # set first image on canvas
        self.image_on_canvas = self.canvas.create_image(
            0, 0, anchor = tk.NW, image = self.my_images[self.my_image_number])

        # button to change image and turn on/off coffee maker
        self.button_txt = tk.StringVar()
        self.button_txt.set('OFF')
        self.button = tk.Button(main, textvariable=self.button_txt, command=self.onButton, height= 1, width=5)
        self.button.place(x=375,y=500)

        # socket for bluetooth
        #self.s = socket.socket(socket.AF_BLUETOOTH,
        #                       socket.SOCK_STREAM,
        #                       socket.BTPROTO_RFCOMM)
        #self.s.connect(('B8:27:EB:BD:1F:E4', 3))
        #self.s.send(bytes('OFF', 'UTF-8'))

    # -------------------------------------------------------------------------
    #
    # -------------------------------------------------------------------------

    def onButton(self):

        if self.my_image_number == 0:
            self.my_image_number = 1
            self.button_txt.set('ON')
         #   self.s.send(bytes('ON', 'UTF-8'))
        else:
            self.my_image_number = 0
            self.button_txt.set('OFF')
         #   self.s.send(bytes('OFF', 'UTF-8'))

        # change image
        self.canvas.itemconfig(self.image_on_canvas,
                               image = self.my_images[self.my_image_number])

# -----------------------------------------------------------------------------
#
# -----------------------------------------------------------------------------
def main():

    root = tk.Tk()
    root.title("Coffee Maker")
    MainWindow(root)
    root.mainloop()

# -----------------------------------------------------------------------------
#
# -----------------------------------------------------------------------------
if __name__ == "__main__":
    main()

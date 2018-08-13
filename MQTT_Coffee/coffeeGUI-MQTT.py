import tkinter as tk
from PIL import ImageTk
import paho.mqtt.client as mqtt
import os

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
        self.button_txt.set('Connecting...')
        self.button = tk.Button(main, textvariable=self.button_txt, command=self.onButton, height= 1, width=9)
        self.button.place(x=360,y=500)

        # connect to mqtt broker
        self.mqttClient = self.connect()
        self.mqttClient.on_message = self.on_message  # attach function to callback
        self.mqttClient.on_subscribe = self.on_subscribe  # attach function to callback
        self.mqttClient.subscribe('CoffeeMaker/Status')
        self.mqttClient.loop_start()  # start the loop
        self.mqttClient.publish('CoffeeMaker/Status', 'STATUS')

    # -----------------------------------------------------------------------------
    # Prints messages received to console
    # -----------------------------------------------------------------------------
    def on_message(self, client, userdata, message):

        print("message received ", message.payload)

        if message.payload == b'ON':
            self.my_image_number = 1
            self.button_txt.set('ON')
        elif message.payload == b'OFF':
            self.my_image_number = 0
            self.button_txt.set('OFF')

        # change image
        self.canvas.itemconfig(self.image_on_canvas,
                               image = self.my_images[self.my_image_number])

    # -----------------------------------------------------------------------------
    # Lets us know that we subscribed successfully
    # -----------------------------------------------------------------------------
    def on_subscribe(self, client, userdata, mid, granted_qos):
        print("subscribed successfully")

    # -----------------------------------------------------------------------------
    # Connects to MQTT Broker
    # -----------------------------------------------------------------------------
    def connect(self):

        # brokerAddr = '192.168.1.169'    # Address of lab broker
        # brokerAddr = 'iot.eclipse.org'  # Test broker outside of lab
        print("creating new instance")
        mqttClient = mqtt.Client()  # create new instance
        print("connecting to broker")
        mqttClient.connect('192.168.1.169') #connect to broker
        #mqttClient.connect('iot.eclipse.org')  # connect to broker
        print("connected to broker")

        return mqttClient

    # -------------------------------------------------------------------------
    #
    # -------------------------------------------------------------------------
    def onButton(self):

        if self.my_image_number == 0:
            self.mqttClient.publish('CoffeeMaker/Power', 'ON')
        else:
            self.mqttClient.publish('CoffeeMaker/Power', 'OFF')

# -----------------------------------------------------------------------------
#
# -----------------------------------------------------------------------------
def main():

    try:
        root = tk.Tk()
        root.title("Coffee Maker")
        MainWindow(root)
        root.mainloop()
    except:
        os._exit(0)

# -----------------------------------------------------------------------------
#
# -----------------------------------------------------------------------------
if __name__ == "__main__":
    main()

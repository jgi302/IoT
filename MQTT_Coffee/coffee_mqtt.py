#!/usr/bin/python3
import paho.mqtt.client as mqtt
import RPi.GPIO as GPIO
import sys
import time
import os

# -----------------------------------------------------------------------------
# Connects to MQTT Broker
# -----------------------------------------------------------------------------
def connect(BROKER):
    while 1:
        try:
            print("creating new instance")
            mqtt_client = mqtt.Client() #create new instance
            print("connecting to broker")
            mqtt_client.connect(BROKER) #connect to broker
            print("connected to broker")
            return mqtt_client
        except:
            time.sleep(1)

# -----------------------------------------------------------------------------
# Prints messages received to console
# -----------------------------------------------------------------------------
def on_message(client, userdata, message):
    global coffee_status
    global coffee_pin
    global mqtt_client

    if message.topic == 'CoffeeMaker/Power':
        if message.payload == b'ON':
            GPIO.output(coffee_pin, GPIO.LOW)
            coffee_status = message.payload
            mqtt_client.publish('CoffeeMaker/Status', coffee_status)
            print("message received ", message.payload)
        elif message.payload == b'OFF':
            GPIO.output(coffee_pin, GPIO.HIGH)
            coffee_status = message.payload
            mqtt_client.publish('CoffeeMaker/Status', coffee_status)
            print("message received ", message.payload)
    elif message.topic == 'CoffeeMaker/Status' \
            and message.payload == b'STATUS':
        mqtt_client.publish('CoffeeMaker/Status', coffee_status)
        print("message received ", message.payload)

# -----------------------------------------------------------------------------
# Lets us know that we subscribed successfully
# -----------------------------------------------------------------------------
def on_subscribe(client, userdata, mid, granted_qos):
    print("subscribed successfully")

# -----------------------------------------------------------------------------
# Global
# -----------------------------------------------------------------------------
if (len(sys.argv) != 2):
    print('error, sample invocation is: $python3 mqtt_mock_subscriber.py <broker_address>')
    sys.exit(-1)

BROKER = sys.argv[1]
mqtt_client = connect(BROKER)
coffee_status = 'OFF'
coffee_pin = 14

# -----------------------------------------------------------------------------
# Main
# -----------------------------------------------------------------------------
def main():
    global mqtt_client
    global coffee_pin
    global coffee_status

    mqtt_client.on_message = on_message  # attach function to callback
    mqtt_client.on_subscribe = on_subscribe  # attach function to callback
    mqtt_client.subscribe('CoffeeMaker/Power')
    mqtt_client.subscribe('CoffeeMaker/Status')
    mqtt_client.loop_start()  # start the loop

    coffee_pin = 14
    GPIO.setmode(GPIO.BCM)
    GPIO.setwarnings(False)
    GPIO.setup(coffee_pin, GPIO.OUT)
    GPIO.output(coffee_pin, GPIO.HIGH)

    mqtt_client.publish('CoffeeMaker/Status', coffee_status)
    while 1:
        try:
            time.sleep(1)
        except KeyboardInterrupt:
            print("Closing socket")
            GPIO.cleanup()
            mqtt_client.loop_stop()
            os._exit(0)
        except:
            continue
# -----------------------------------------------------------------------------
#
# -----------------------------------------------------------------------------
if __name__ == "__main__":
    main()

import socket
import paho.mqtt.client as mqtt #import the client1
import time
import sys

# socket for bluetooth
s = socket.socket(socket.AF_BLUETOOTH,
                  socket.SOCK_STREAM,
                  socket.BTPROTO_RFCOMM)
s.connect(('B8:27:EB:BD:1F:E4', 3))

# -----------------------------------------------------------------------------
# Prints messages received to console
# -----------------------------------------------------------------------------
def on_message(client, userdata, message):
    global s
    if message.topic == 'CoffeeMaker/Power':
        s.send(message.payload)
        print("message received ", message.payload)
    elif message.topic == 'CoffeeMaker/Status' \
            and message.payload == b'STATUS':
        s.send(message.payload)
        print("message received ", message.payload)

# -----------------------------------------------------------------------------
# Lets us know that we subscribed successfully
# -----------------------------------------------------------------------------
def on_subscribe(client, userdata, mid, granted_qos):
    print("subscribed successfully")

# -----------------------------------------------------------------------------
# Connects to MQTT Broker
# -----------------------------------------------------------------------------
def connect(BROKER):

    #brokerAddr = '192.168.1.169'    # Address of lab broker
    #brokerAddr = 'iot.eclipse.org'  # Test broker outside of lab
    print("creating new instance")
    mqttClient = mqtt.Client() #create new instance
    print("connecting to broker")
    #mqttClient.connect(BROKER) #connect to broker
    mqttClient.connect('iot.eclipse.org') #connect to broker

    print("connected to broker")

    return mqttClient

# -----------------------------------------------------------------------------
# Connects to BROKER and publishes data to TOPIC
# -----------------------------------------------------------------------------
def main():

    if(len(sys.argv) != 2):
        print('error, sample invocation is: $python3 mqtt_mock_subscriber.py <broker_address>')
        sys.exit(-1)

    BROKER = sys.argv[1]

    mqttClient = connect(BROKER)
    mqttClient.on_message = on_message  # attach function to callback
    mqttClient.on_subscribe = on_subscribe  # attach function to callback
    mqttClient.subscribe('CoffeeMaker/Power')
    mqttClient.subscribe('CoffeeMaker/Status')
    mqttClient.loop_start()  # start the loop
    mqttClient.publish('CoffeeMaker/Status', 'STATUS')

    try:
        while True:
            data = s.recv(1024)
            mqttClient.publish('CoffeeMaker/Status', data)
    except KeyboardInterrupt:
        mqttClient.loop_stop() #stop the loop
        time.sleep(2)
        print()
        print('ending program')

# -----------------------------------------------------------------------------
#
# -----------------------------------------------------------------------------
if __name__ == "__main__":
    main()
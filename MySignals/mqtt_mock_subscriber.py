import paho.mqtt.client as mqtt
import time
import sys

# -----------------------------------------------------------------------------
# Prints messages received to console
# -----------------------------------------------------------------------------
def on_message(client, userdata, message):
    print("message received " ,str(message.payload.decode("utf-8")))

# -----------------------------------------------------------------------------
# Lets us know that we subscribed successfully
# -----------------------------------------------------------------------------
def on_subscribe(client, userdata, mid, granted_qos):
    print("subscribed to topic")

# -----------------------------------------------------------------------------
# Connects to MQTT Broker
# -----------------------------------------------------------------------------
def connect(BROKER):

    #brokerAddr = "192.168.1.169"    # Address of lab broker
    #brokerAddr = "test.mosquitto.org" # Test broker outside of lab
    print("creating new instance")
    mqttClient = mqtt.Client() #create new instance
    print("connecting to broker")
    mqttClient.connect(BROKER) #connect to broker
    print("connected to broker")

    return mqttClient

# -----------------------------------------------------------------------------
# Connects to BROKER and publishes data to TOPIC
# -----------------------------------------------------------------------------
def main():

    if(len(sys.argv) != 3):
        print('error, sample invocation is: $python3 mqtt_mock_subscriber.py <topic> <broker_address>')
        sys.exit(-1)

    TOPIC = sys.argv[1]
    BROKER = sys.argv[2]

    mqttClient = connect(BROKER)
    mqttClient.on_message = on_message  # attach function to callback
    mqttClient.on_subscribe = on_subscribe  # attach function to callback
    mqttClient.subscribe(TOPIC)
    mqttClient.loop_start()  # start the loop

    try:
        while True:
            time.sleep(1)
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
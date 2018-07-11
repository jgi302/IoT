import paho.mqtt.client as mqtt
import time
import sys

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
        print('error, sample invocation is: $python3 mqtt_mock_publisher.py <topic> <broker_address>')
        sys.exit(-1)

    TOPIC = sys.argv[1]
    BROKER = sys.argv[2]
    TEMP = 0

    mqttClient = connect(BROKER)
    mqttClient.loop_start()  # start the loop
    print('beginning publishing')
    try:
        while True:
            mqttClient.publish(TOPIC, TEMP)
            print(TOPIC, TEMP)
            TEMP = TEMP + 5
            time.sleep(2)
    except KeyboardInterrupt:
        print()
        print('ending publishing')
        mqttClient.loop_stop() #stop the loop

# -----------------------------------------------------------------------------
#
# -----------------------------------------------------------------------------
if __name__ == "__main__":
    main()
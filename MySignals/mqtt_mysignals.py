import serial
import paho.mqtt.client as mqtt

# -----------------------------------------------------------------------------
# Connects to MQTT Broker
# -----------------------------------------------------------------------------
def connect():

    brokerAddr = "192.168.1.169"    # Address of lab broker
    #brokerAddr = "test.mosquitto.org" # Test broker outside of lab

    print("creating new instance")
    mqttClient = mqtt.Client() #create new instance
    print("connecting to broker")
    mqttClient.connect(brokerAddr) #connect to broker
    print("connected to broker")

    return mqttClient

# -----------------------------------------------------------------------------
# Reads from MySignals Serial and Publish to Broker
# -----------------------------------------------------------------------------
def publish_loop(ser, mqttClient):

    while True:
        try:
            msg = ser.readline()
            msg = msg.decode('utf_8')
            msg = msg.rstrip('\n')

            if '/' in msg:
                data = msg.split('/')
                topic = "MySignals/" + data[0]
                print("Publishing message to topic", topic, "Data =", data[1])
                mqttClient.publish(topic, data[1])

        except UnicodeDecodeError:
            continue
        except KeyboardInterrupt:
            raise

# -----------------------------------------------------------------------------
# Connects to Broker and publishes sensor data provided by MySignals
# -----------------------------------------------------------------------------
def main():

    ser = serial.Serial('/dev/ttyACM0', 115200)
    ser.flushInput()

    mqttClient = connect()
    mqttClient.loop_start()

    try:
        publish_loop(ser, mqttClient)
    except KeyboardInterrupt:
        print()
        mqttClient.loop_stop()

# -----------------------------------------------------------------------------
#
# -----------------------------------------------------------------------------
if __name__ == "__main__":
    main()
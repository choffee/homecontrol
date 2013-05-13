#!/usr/bin/env python

# Monitor the feed for power control commands
# 
# Attach to the mqtt power/<power_num>/set_state 
# And send the set states to the right switches.
#


import mosquitto
import serial

DEV='/dev/ttyUSB0'

ser = serial.Serial(DEV, 9600, timeout=2)
print ser.readline()



def on_connect(obj, rc):
    print("rc: "+str(rc))

def on_message(obj, msg):
    print(msg.topic+" "+str(msg.qos)+" "+str(msg.payload))
    # Expect /power/A2/set_state and "on" or "off" as the payload
    # Get the number from the topic
    dev = msg.topic.split("/")[2]
    print "dev: " + dev
    # This should be a bit clever but for now just pass it on
    ser.write('RF  %s%s\n' % (dev, msg.payload))
    #ser.write('RF  A2off\n')


def on_publish(obj, mid):
    print("mid: "+str(mid))

def on_subscribe(obj, mid, granted_qos):
    print("Subscribed: "+str(mid)+" "+str(granted_qos))

def on_log(obj, level, string):
    print(string)

# If you want to use a specific client id, use
# mqttc = mosquitto.Mosquitto("client-id")
# but note that the client id must be unique on the broker. Leaving the client
# id parameter empty will generate a random id for you.
mqttc = mosquitto.Mosquitto("test-client")
mqttc.on_message = on_message
mqttc.on_connect = on_connect
mqttc.on_publish = on_publish
mqttc.on_subscribe = on_subscribe
# Uncomment to enable debug messages
#mqttc.on_log = on_log
mqttc.username_pw_set("homenet","homenet")
mqttc.connect("192.168.122.74", port=1883, keepalive=60)
mqttc.subscribe("/power/+/set_state", 0)


rc = 0
while rc == 0:
    rc = mqttc.loop()

print("rc: "+str(rc))
ser.close()

#!/usr/bin/env python

# This is a bluemon to mqtt bus converter
# You have to setup bluemon to monitor your phone
# Then this script should publish levels to /bluetooth/<device>/level on mqtt
# It's all hard coded and no errors are handled right now but it does seem to work!

import dbus
import gobject
import mosquitto
import threading
import logging as log

seen_counts = {}

# Setup a loop
from dbus.mainloop.glib import DBusGMainLoop
DBusGMainLoop(set_as_default=True)

# Get onto the bus
system_bus = dbus.SystemBus()

# Get a proxy
blueproxy = system_bus.get_object('cx.ath.matthew.bluemon.server',
                                  '/cx/ath/matthew/bluemon/Bluemon')
bluemon = dbus.Interface(blueproxy, "cx.ath.matthew.bluemon.Bluemon")

# Get on the mqtt bus
mqttc = mosquitto.Mosquitto("bluemon")
# Uncomment to enable debug messages
#mqttc.on_log = on_log
#mqttc.username_pw_set("homenet","homenet")
def on_publish(mosq, obj, mid):
    log.debug("Message %s published.", mid)

mqttc.on_publish = on_publish

def on_connect(mosq, obj, rc):
    if rc == 0:
        log.debug("Connected okay")
    else:
        log.error("Not connected")

mqttc.on_connect = on_connect

mqttc.connect("192.168.1.107", port=1883, keepalive=60)

def connect_handler(sender=None):
    log.debug( "got connect from %r", sender)
    (addr, status, level) = bluemon.Status(sender)
    # Send the level to the mqtt bus
    mqttc.publish("/bluetooth/%s/level" % addr, "%d" % level, 1)
    if seen_counts.has_key(addr):
        seen_counts[addr] = 2
    else:
        seen_counts[addr] = 1
        mqttc.publish("/bluetooth/%s/state" % addr, "arrived", 1)







system_bus.add_signal_receiver(connect_handler,
                               dbus_interface='cx.ath.matthew.bluemon.ProximitySignal',
                               signal_name="Connect")
def disconnect_handler(sender=None):
      log.debug("got disconnect from %r", sender)

system_bus.add_signal_receiver(disconnect_handler,
                               dbus_interface='cx.ath.matthew.bluemon.ProximitySignal',
                               signal_name="Disconnect")



#def handler(sender=None):
#      print "got signal from %r" % sender

#blueproxy.connect_to_signal("Hello", handler, sender_keyword='sender')

class mqttThread(threading.Thread):
    def run(self):
        while mqttc.loop(2) == 0:
            for addr, num in seen_counts.items():
                if num < 1:
                    del seen_counts[ addr ]
                    mqttc.publish("/bluetooth/%s/state" % addr, "gone", 1)
                else:
                    seen_counts[addr] = num - 1


mqt = mqttThread()

mqt.start()

# Now loop and wait
loop = gobject.MainLoop()
loop.run()


import argparse
import itertools
import json
import os
import socket
import sys
import time
import uuid
from PyQt5 import QtCore, QtGui, QtWidgets
from zeroconf import Zeroconf, ServiceBrowser

import common
import driver_auto as driver

APPNAME = 'CheckMeet'

def sendudp(ip, port, msg):
    assert(len(msg) <= common.MAX_JSON_LENGTH)
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP
    try:
        sock.sendto(bytes(msg, 'utf-8'), (ip, port))
    except OSError as e:
        common.log(f'Couldn\'t send over UDP: {e.strerror}')

def __safe_get_status(getter_fn, fallback_value, name, duration_limit=0.25):
    t0 = time.monotonic()
    try:
        value = getter_fn()

    except:
        value = fallback_value
        common.log(f'Error fetching info about {name}: {sys.exc_info()[0]}')

    finally:
        duration = time.monotonic() - t0
        if duration > duration_limit:
            common.log(f'Warning: fetching {name} info took too long ({duration} s)')

    return value

# Returns a tuple containing webcam & microphone status.
# Previous return value is passed as last_status. On first call, None is passed.
# counter goes up to args.send_rate-1, then back to 0. It can be used to do stuff every X iterations.
# Before quitting, the function is called one more time with counter==-1. It sends out an "everything off" message in this case.
def loopbody(args, counter, last_status, enabled_devices):
    if counter >= 0:
        # Normal call
        fallback_status = last_status if last_status is not None else (False, False)
        status = (
            __safe_get_status(driver.is_webcam_used, fallback_status[0], 'webcam'),
            __safe_get_status(driver.is_microphone_used, fallback_status[1], 'microphone')
        )
    else:
        # Application quitting
        status = (False, False)

    msg = json.dumps(
        {
            "version": 1,
            "webcam": status[0],
            "microphone": status[1],
            "senderId": args.sender_id
        },
        separators=(',', ':')
    )

    common.log(msg)
    # Send message if status changed, or every Xth round
    if status != last_status or (counter % args.send_rate)==0:
        common.log('Sending UDP message...')
        for ip, port in enabled_devices:
            sendudp(ip, port, msg)

    return status

class App(QtWidgets.QApplication):
    onDeviceAppeared = QtCore.pyqtSignal(str, list, int)
    onDeviceDisappeared = QtCore.pyqtSignal(str)

    def __init__(self, args):
        QtWidgets.QApplication.__init__(self, sys.argv)
        self.setQuitOnLastWindowClosed(False)

        self.nearbyDevices = {}
        self.onDeviceAppeared.connect(self.deviceAppeared)
        self.onDeviceDisappeared.connect(self.deviceDisappeared)

        self.menu = QtWidgets.QMenu()
        self.devicesMenu = self.menu.addMenu("Nearby devices")
        self.devicesMenu.triggered.connect(self.toggleDevice)
        self.menu.addAction("Add device manually", self.addDeviceManually)
        self.exitAction = self.menu.addAction("Exit", self.shutdown)

        style = self.style()
        icon = QtGui.QIcon(style.standardPixmap(QtWidgets.QStyle.SP_ArrowUp))
        self.trayIcon = QtWidgets.QSystemTrayIcon(icon)
        self.trayIcon.setContextMenu(self.menu)
        self.trayIcon.setToolTip(APPNAME)

        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.onTick)

        self.args = args
        self.last_status = None
        self.icounter = itertools.cycle(itertools.islice(itertools.count(start=0), self.args.send_rate))

    def start(self):
        self.onTick()
        self.timer.start(self.args.query_interval*1000)
        self.trayIcon.show()
        driver.show_notification(APPNAME, 'Service started!✨')

        if len(self.args.ip) > 0:
            self.deviceAppeared(f"[Added on command line]", self.args.ip, self.args.port)

    def onTick(self):
        counter = next(self.icounter)
        self.last_status = loopbody(self.args, counter, self.last_status, self.getEnabledDevices())

    def shutdown(self):
        loopbody(self.args, -1, self.last_status, self.getEnabledDevices())
        common.log('Shutting down')
        self.exit()

    def deviceAppeared(self, name, addresses, port):
        print(f"deviceAppeared {name} {addresses} {port}")
        if name in self.nearbyDevices:
            print(f"Duplicate device {name}")
            return

        updatesEnabled = True
        self.nearbyDevices[name] = {
            'addresses': addresses,
            'port': port,
            'updatesEnabled': updatesEnabled,
        }
        a = self.devicesMenu.addAction(f"{name} {addresses}:{port}")
        a.setCheckable(True)
        a.setChecked(updatesEnabled)
        self.nearbyDevices[name]['action'] = a

    def deviceDisappeared(self, name):
        print(f"deviceDisappeared {name}")
        if name not in self.nearbyDevices:
            print(f"Device not found {name}")
            return

        self.devicesMenu.removeAction(self.nearbyDevices[name]['action'])
        del self.nearbyDevices[name]

    def toggleDevice(self, action):
        if action.isCheckable():
            self.nearbyDevices[action.text()]["updatesEnabled"] = action.isChecked()

    def getEnabledDevices(self):
        result = []

        for _, device in self.nearbyDevices.items():
            if device['updatesEnabled']:
                for ip in device['addresses']:
                    result.append((ip, device['port']))

        return result

    def addDeviceManually(self):
        text, ok = QtWidgets.QInputDialog.getText(None, 'CheckMeet', 'Please provide IP address of device:')
        if ok:
            self.deviceAppeared(f"[Manually added]", [text], self.args.port)

    def add_service(self, zeroconf, serviceType, name):
        info = zeroconf.get_service_info(serviceType, name)
        self.onDeviceAppeared.emit(info.server, info.parsed_addresses(), info.port)

    def remove_service(self, zeroconf, serviceType, name):
        info = zeroconf.get_service_info(serviceType, name)
        self.onDeviceDisappeared.emit(info.server)

    def update_service(self, zeroconf, serviceType, name):
        pass


def main():
    parser = argparse.ArgumentParser(description='Service')
    parser.add_argument('--port', default=26999, type=int, help='Use UDP port')
    parser.add_argument('--query_interval', default=1, type=int, help='Query status every X seconds')
    parser.add_argument('--send_rate', default=10, type=int, help='Send every Xth status')
    parser.add_argument('--sender_id', default=str(uuid.uuid4()), help='Unique ID identifying this computer')
    parser.add_argument('ip', nargs='*', help='Send UDP packets to these IP adresses')
    args = parser.parse_args()

    zconf = Zeroconf()
    app = App(args)
    ServiceBrowser(zconf, "_checkmeet._udp.local.", app)
    try:
        app.start()
        exit_status = app.exec_()
    finally:
        zconf.close()

    sys.exit(exit_status)

if __name__ == '__main__':
    main()

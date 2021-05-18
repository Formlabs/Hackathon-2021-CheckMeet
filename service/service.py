import argparse
import itertools
import json
import os
import socket
import sys
import time
import uuid
from PyQt5 import QtCore, QtGui, QtWidgets

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
def loopbody(args, counter, last_status):
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
        for ip in args.ip:
            sendudp(ip, args.port, msg)

    return status

class App(QtWidgets.QApplication):
    def __init__(self, args):
        QtWidgets.QApplication.__init__(self, sys.argv)

        self.menu = QtWidgets.QMenu()
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

    def getIpAddress(self):
        text, ok = QtWidgets.QInputDialog.getText(None, 'CheckMeet', 'Please provide IP address of device:')
        if ok:
            self.args.ip.append(text)
        else:
            driver.show_notification(APPNAME, 'Could not start service')
            sys.exit(1) # can't use self.exit() here because the Qt event loop is not running yet

    def start(self):
        if len(self.args.ip)==0:
            self.getIpAddress()

        self.onTick()
        self.timer.start(self.args.query_interval*1000)
        self.trayIcon.show()
        driver.show_notification(APPNAME, 'Service started!✨')

    def onTick(self):
        counter = next(self.icounter)
        self.last_status = loopbody(self.args, counter, self.last_status)

    def shutdown(self):
        loopbody(self.args, -1, self.last_status)
        common.log('Shutting down')
        self.exit()

def main():
    parser = argparse.ArgumentParser(description='Service')
    parser.add_argument('--port', default=26999, type=int, help='Use UDP port')
    parser.add_argument('--query_interval', default=1, type=int, help='Query status every X seconds')
    parser.add_argument('--send_rate', default=10, type=int, help='Send every Xth status')
    parser.add_argument('--sender_id', default=str(uuid.uuid4()), help='Unique ID identifying this computer')
    parser.add_argument('ip', nargs='*', help='Send UDP packets to these IP adresses')
    args = parser.parse_args()

    app = App(args)
    app.start()
    sys.exit(app.exec_())

if __name__ == '__main__':
    main()

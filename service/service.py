import argparse
import itertools
import json
import socket
import sys
import time
import uuid
from PyQt5 import QtCore, QtGui, QtWidgets

import common
import driver_auto as driver

def sendudp(ip, port, msg):
    assert(len(msg) <= common.MAX_JSON_LENGTH)
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP
    try:
        sock.sendto(bytes(msg, 'utf-8'), (ip, port))
    except OSError as e:
        common.log(f'Couldn\'t send over UDP: {e.strerror}')

def __safe_get_status(getter_fn, fallback_value, name):
    try:
        value = getter_fn()
    except:
        value = fallback_value
        common.log(f'Error fetching info about {name}: {sys.exc_info()[0]}')
    return value

def loopbody(args, counter, last_status):
    if counter >= 0:

        fallback_status = last_status if last_status is not None else (False, False)
        status = (
            __safe_get_status(driver.is_webcam_used, fallback_status[0], 'webcam'),
            __safe_get_status(driver.is_microphone_used, fallback_status[1], 'microphone')
        )
    else:
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
    if status != last_status or counter==0:
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
        icon = QtGui.QIcon(style.standardPixmap(QtWidgets.QStyle.SP_FileIcon))
        self.trayIcon = QtWidgets.QSystemTrayIcon(icon)
        self.trayIcon.setContextMenu(self.menu)

        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.onTick)

        self.args = args
        self.last_status = None
        self.icounter = itertools.cycle(itertools.islice(itertools.count(start=0), self.args.send_rate))

    def start(self):
        self.onTick()
        self.timer.start(self.args.query_interval*1000)
        self.trayIcon.show()

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
    parser.add_argument('ip', nargs='+', help='Send UDP packets to these IP adresses')
    args = parser.parse_args()

    driver.show_notification('I am on a meeting', 'Service started!✨')

    app = App(args)
    app.start()
    sys.exit(app.exec_())

if __name__ == '__main__':
    main()

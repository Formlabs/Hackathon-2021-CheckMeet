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

from google_calendar import get_end_of_current_meeting_utctimestamp

APPNAME = 'I am on a meeting'

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

# Returns a tuple containing webcam & microphone status.
# Previous return value is passed as last_status. On first call, None is passed.
# counter goes up to args.send_rate-1, then back to 0. It can be used to do stuff every X iterations.
# Before quitting, the function is called one more time with counter==-1. It sends out an "everything off" message in this case.
def loopbody(args, counter, last_status):
    if counter >= 0:
        # Normal call
        fallback_status = last_status if last_status is not None else (False, False, None)

        if not args.enable_google_calendar:
            end_of_current_meeting = None
        elif (counter % args.google_calendar_rate) == 0:
            common.log('Querying google calendar...')
            end_of_current_meeting = __safe_get_status(get_end_of_current_meeting_utctimestamp, fallback_status[2], 'calendar')
        else:
            end_of_current_meeting = last_status[2]

        status = (
            __safe_get_status(driver.is_webcam_used, fallback_status[0], 'webcam'),
            __safe_get_status(driver.is_microphone_used, fallback_status[1], 'microphone'),
            end_of_current_meeting,
        )
    else:
        # Application quitting
        status = (False, False, None)

    obj = {
        "version": 2,
        "webcam": status[0],
        "microphone": status[1],
        "senderId": args.sender_id,
    }

    if status[2] != None:
        obj["countDownTarget"] = status[2]

    msg = json.dumps(obj, separators=(',', ':'))

    common.log(msg)
    if status != last_status or (counter % args.send_rate) == 0:
        common.log('Sending UDP message...')
        for ip in args.ip:
            sendudp(ip, args.port, msg)

    return status

class App(QtWidgets.QApplication):
    def __init__(self, args):
        QtWidgets.QApplication.__init__(self, sys.argv)
        self.setOrganizationName(common.ORGANIZATION_NAME)
        self.setApplicationName(common.APPLICATION_NAME)

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
        rate = args.send_rate * args.google_calendar_rate
        self.icounter = itertools.cycle(itertools.islice(itertools.count(start=0), rate))

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
    parser.add_argument('--google_calendar_rate', default=300, type=int, help='Query calendar every X seconds')
    parser.add_argument('--enable_google_calendar', action='store_true', help='Use google calendar')
    parser.add_argument('--send_rate', default=10, type=int, help='Send every Xth status')
    parser.add_argument('--sender_id', default=str(uuid.uuid4()), help='Unique ID identifying this computer')
    parser.add_argument('ip', nargs='+', help='Send UDP packets to these IP adresses')
    args = parser.parse_args()

    driver.show_notification(APPNAME, 'Service started!✨')

    app = App(args)
    app.start()
    sys.exit(app.exec_())

if __name__ == '__main__':
    main()

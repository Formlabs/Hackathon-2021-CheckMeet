import argparse
import datetime
import itertools
import json
import platform
import socket
import sys
import time

if platform.system()=='Windows':
    from driver_win import is_webcam_used, is_microphone_used
elif platform.system()=='Darwin':
    from driver_mac import is_webcam_used, is_microphone_used
else:
    print('This platform is not supported')
    sys.exit(2)

def sendudp(ip, port, msg):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP
    sock.sendto(bytes(msg, 'utf-8'), (ip, port))

def main():
    parser = argparse.ArgumentParser(description='Service')
    parser.add_argument('--port', default=26999, type=int, help='Use UDP port')
    parser.add_argument('--query_interval', default=1, type=int, help='Query status every X seconds')
    parser.add_argument('--send_rate', default=10, type=int, help='Send every Xth status')
    parser.add_argument('ip', nargs='+', help='Send UDP packets to these IP adresses')
    args = parser.parse_args()

    for counter in itertools.count(start=0):
        msg = json.dumps({
            "version": 1,
            "webcam": is_webcam_used(),
            "microphone": is_microphone_used(),
        })
        now = datetime.datetime.now()
        ts = f'{now.hour:02}:{now.minute:02}:{now.second:02}'
        print(f'[{ts}] {msg}')
        if counter % args.send_rate == 0:
            print('Sending over UDP')
            for ip in args.ip:
                sendudp(ip, args.port, msg)
        time.sleep(args.query_interval)

if __name__ == '__main__':
    main()

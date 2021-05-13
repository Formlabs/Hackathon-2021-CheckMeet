import argparse
import datetime
import itertools
import json
import platform
import socket
import sys
import time
import uuid

MAX_JSON_LENGTH = 250

if platform.system()=='Windows':
    from driver_win import is_webcam_used, is_microphone_used
elif platform.system()=='Darwin':
    from driver_mac import is_webcam_used, is_microphone_used
else:
    print('This platform is not supported')
    sys.exit(2)

def sendudp(ip, port, msg):
    assert(len(msg) <= MAX_JSON_LENGTH)
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP
    sock.sendto(bytes(msg, 'utf-8'), (ip, port))

def loopbody(args, counter, last_status):
    if counter >= 0:
        status = (is_webcam_used(), is_microphone_used())
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

    now = datetime.datetime.now()
    ts = f'{now.hour:02}:{now.minute:02}:{now.second:02}'
    print(f'[{ts}] {msg}')
    if status != last_status or counter==0:
        print('Sending over UDP')
        for ip in args.ip:
            sendudp(ip, args.port, msg)

    return status

def main():
    parser = argparse.ArgumentParser(description='Service')
    parser.add_argument('--port', default=26999, type=int, help='Use UDP port')
    parser.add_argument('--query_interval', default=1, type=int, help='Query status every X seconds')
    parser.add_argument('--send_rate', default=10, type=int, help='Send every Xth status')
    parser.add_argument('--sender_id', default=str(uuid.uuid4()), help='Unique ID identifying this computer')
    parser.add_argument('ip', nargs='+', help='Send UDP packets to these IP adresses')
    args = parser.parse_args()

    try:
        last_status = None
        for counter in itertools.cycle(itertools.islice(itertools.count(start=0), args.send_rate)): # repeat [0, send_rate-1] ad infinitum
            last_status = loopbody(args, counter, last_status)
            time.sleep(args.query_interval)
    except KeyboardInterrupt as e:
        loopbody(args, -1, last_status)
        print('User pressed Ctrl+C, aborting')

if __name__ == '__main__':
    main()

import argparse
import itertools
import json
import socket
import sys
import time
import uuid

import common
from driver_auto import is_webcam_used, is_microphone_used, show_notification

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
            __safe_get_status(is_webcam_used, fallback_status[0], 'webcam'),
            __safe_get_status(is_microphone_used, fallback_status[1], 'microphone')
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

def main():
    parser = argparse.ArgumentParser(description='Service')
    parser.add_argument('--port', default=26999, type=int, help='Use UDP port')
    parser.add_argument('--query_interval', default=1, type=int, help='Query status every X seconds')
    parser.add_argument('--send_rate', default=10, type=int, help='Send every Xth status')
    parser.add_argument('--sender_id', default=str(uuid.uuid4()), help='Unique ID identifying this computer')
    parser.add_argument('ip', nargs='+', help='Send UDP packets to these IP adresses')
    args = parser.parse_args()

    show_notification('I am on a meeting', 'Service started!✨')

    try:
        last_status = None
        for counter in itertools.cycle(itertools.islice(itertools.count(start=0), args.send_rate)): # repeat [0, send_rate-1] ad infinitum
            last_status = loopbody(args, counter, last_status)
            time.sleep(args.query_interval)
    except KeyboardInterrupt as e:
        loopbody(args, -1, last_status)
        common.log('User pressed Ctrl+C, aborting')

if __name__ == '__main__':
    main()

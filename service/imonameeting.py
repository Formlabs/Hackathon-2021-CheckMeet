import datetime
import platform
import sys
import time

if platform.system()=='Windows':
    from driver_win import is_webcam_used, is_microphone_used
elif platform.system()=='Darwin':
    from driver_mac import is_webcam_used, is_microphone_used
else:
    print('This platform is not supported')
    sys.exit(2)

def main():
    while True:
        webcam = 'ON' if is_webcam_used() else 'OFF'
        microphone = 'ON' if is_microphone_used() else 'OFF'
        now = datetime.datetime.now()
        ts = f'{now.hour:02}:{now.minute:02}:{now.second:02}'
        print(f'[{ts}] camera {webcam}, microphone {microphone}')
        time.sleep(1)

import subprocess

import common

def is_used(device_path):
    p = subprocess.run(f'fuser {device_path}',
        shell=True,
        capture_output=True,
        text=True,
    )
    return p.stdout != ''

def is_microphone_used():
    return is_used('/dev/snd/pcmC*D*c')

def is_webcam_used():
    return is_used('/dev/video*')

def show_notification(title, message):
    common.log(f'Linux driver says {title}: {message}')

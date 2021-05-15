import json

import common

def load_json():
    with open('fake.json', 'r') as f:
        return json.load(f)

def is_webcam_used():
    obj = load_json()
    return bool(obj['webcam'])

def is_microphone_used():
    obj = load_json()
    return bool(obj['microphone'])

def show_notification(title, message):
    common.log(f'Fake driver says {title}: {message}')

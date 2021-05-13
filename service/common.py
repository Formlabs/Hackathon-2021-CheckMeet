import datetime

MAX_JSON_LENGTH = 250

ORGANIZATION_NAME = 'Formlabs'
APPLICATION_NAME = 'iamonameeting'

def log(msg):
    now = datetime.datetime.now()
    ts = f'{now.hour:02}:{now.minute:02}:{now.second:02}'
    print(f'[{ts}] {msg}')

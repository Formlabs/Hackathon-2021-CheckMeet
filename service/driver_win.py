import itertools
import winreg
from win10toast import ToastNotifier

def is_used_internal(root, key_path):
    with winreg.OpenKey(root, key_path) as key:
        for idx in itertools.count(start=0):
            try:
                subkey_name = winreg.EnumKey(key, idx)
            except OSError:
                # we're out of subkeys
                return False

            with winreg.OpenKey(key, subkey_name) as subkey:
                try:
                    value,value_type = winreg.QueryValueEx(subkey, 'LastUsedTimeStop')
                except OSError:
                    continue

                assert(value_type == winreg.REG_QWORD)
                if (value == 0):
                    return True

def is_used(device_name, old_root):
    if old_root:
        root = winreg.HKEY_LOCAL_MACHINE
    else:
        root = winreg.HKEY_CURRENT_USER

    root_path = fr'SOFTWARE\Microsoft\Windows\CurrentVersion\CapabilityAccessManager\ConsentStore\{device_name}'
    return is_used_internal(root, root_path) or is_used_internal(root, fr'{root_path}\NonPackaged')

def is_microphone_used():
    return is_used('microphone', False)

def is_webcam_used():
    return is_used('webcam', False)

def show_notification(title, message):
    toaster = ToastNotifier()
    toaster.show_toast(title, message)

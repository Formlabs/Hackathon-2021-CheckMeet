import itertools
import winreg
import plyer.platforms.win.notification # without this, pyinstaller will miss the dependency
from plyer import notification


# Windows stores "last use" information for webcams in the registry here:
#     HKCU\SOFTWARE\Microsoft\Windows\CurrentVersion\CapabilityAccessManager\ConsentStore\webcam
# Similarly, microphone usage information is stored here:
#     HKCU\SOFTWARE\Microsoft\Windows\CurrentVersion\CapabilityAccessManager\ConsentStore\microphone
# For every Windows store application, a subkey exists in the above paths; for every regular application using the devices,
# a subkey exists under the NonPackaged subkey.
# The application-specific subkeys contain two values named LastUsedTimeStart and LastUsedTimeStop.
# When an application uses the device, LastUsedTimeStop is 0.

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
    notification.notify(title, message)

import AVFoundation
import Cocoa
import CoreAudio
import CoreMediaIO
import struct
import time

# This polling solution is based on
# https://github.com/sindresorhus/is-camera-on/blob/main/Sources/IsCameraOn/IsCameraOn.swift
# https://stackoverflow.com/questions/39574616/how-to-detect-microphone-usage-on-os-x
# https://stackoverflow.com/questions/42681127/coremediaio-incorrectly-updated-properties-kcmiodevicepropertydeviceisrunningso
# https://pyobjc.readthedocs.io/en/latest/
# https://docs.python.org/3/library/struct.html
#
# The step_event_handler_loop part is probably a bit hacky.


def step_event_handler_loop():
    # As described in https://stackoverflow.com/questions/39574616/how-to-detect-microphone-usage-on-os-x
    # and https://stackoverflow.com/questions/42681127/coremediaio-incorrectly-updated-properties-kcmiodevicepropertydeviceisrunningso
    # this device query approach requires the event handler to refresh stuff that we poll.
    # Given 'None' for date, this will run the event handler once and returns.
    # Note that it's probably a hack, a deeper insight into Cocoa would be helpful. For now it'll do.
    Cocoa.NSRunLoop.currentRunLoop().runMode_beforeDate_(Cocoa.NSDefaultRunLoopMode, None)


def is_webcam_used():

    step_event_handler_loop()

    # enumerate devices
    camera_ids = {cam.connectionID(): cam for cam in AVFoundation.AVCaptureDevice.devicesWithMediaType_(
        AVFoundation.AVMediaTypeVideo)}

    # CoreMediaIO is used
    opa = CoreMediaIO.CMIOObjectPropertyAddress(
        CoreMediaIO.kCMIODevicePropertyDeviceIsRunningSomewhere,
        CoreMediaIO.kCMIOObjectPropertyScopeWildcard,
        CoreMediaIO.kCMIOObjectPropertyElementWildcard
    )

    for camera_id in camera_ids:
        response = CoreMediaIO.CMIOObjectGetPropertyData(camera_id, opa, 0, None, 4, None, None)
        # See kCMIODevicePropertyDeviceIsRunningSomewhere in CMIOHardwareDevice.h, the return value is a UInt32
        # (4 bytes) which is 1 or 0. The PyObjc version of CoreMediaIO.CMIOObjectGetPropertyData returns a 4-tuple
        # because python functions can't take output arguments. See the original function in CMIOHardwareObject.h.
        # The last element (at index 3) the result is a pure byte array, we use struct.unpack to interpret it as a
        # 4 byte unsigned int (I).
        camera_is_on = bool(struct.unpack('I', response[3])[0])
        if camera_is_on:
            return True
    return False


def is_microphone_used():

    step_event_handler_loop()

    # enumerate devices
    mic_ids = {mic.connectionID(): mic for mic in AVFoundation.AVCaptureDevice.devicesWithMediaType_(
        AVFoundation.AVMediaTypeAudio)}

    # CoreAudio is used
    opa = CoreAudio.AudioObjectPropertyAddress(
        CoreAudio.kAudioDevicePropertyDeviceIsRunningSomewhere,
        CoreAudio.kAudioObjectPropertyScopeWildcard,
        CoreAudio.kAudioObjectPropertyElementWildcard
    )

    for mic_id in mic_ids:
        response = CoreAudio.AudioObjectGetPropertyData(mic_id, opa, 0, [], 4, None)
        # See the comments in is_webcam_used
        mic_is_on = bool(struct.unpack('I', response[2])[0])
        if mic_is_on:
            return True
    return False

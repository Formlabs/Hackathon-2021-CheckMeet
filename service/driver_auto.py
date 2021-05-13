import platform

if platform.system()=='Windows':
    from driver_win import is_webcam_used, is_microphone_used
elif platform.system()=='Darwin':
    from driver_mac import is_webcam_used, is_microphone_used
else:
    print('This platform is not supported')
    sys.exit(2)

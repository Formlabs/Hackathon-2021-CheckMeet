import platform

if platform.system()=='Windows':
    from driver_win import is_webcam_used, is_microphone_used, show_notification
elif platform.system()=='Darwin':
    from driver_mac import is_webcam_used, is_microphone_used, show_notification
elif platform.system()=='Linux':
    from driver_linux import is_webcam_used, is_microphone_used, show_notification
else:
    print('This platform is not supported')
    sys.exit(2)

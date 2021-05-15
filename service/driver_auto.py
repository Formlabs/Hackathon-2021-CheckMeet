import platform

if platform.system()=='Windows':
    import driver_win as driver
elif platform.system()=='Darwin':
    import driver_mac as driver
elif platform.system()=='Linux':
    import driver_linux as driver
else:
    print('This platform is not supported')
    sys.exit(2)


is_webcam_used = driver.is_webcam_used
is_microphone_used = driver.is_microphone_used
show_notification = driver.show_notification

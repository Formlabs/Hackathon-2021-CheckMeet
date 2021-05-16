#!/usr/bin/env python3
from __future__ import print_function
import sys

if (sys.version_info.major, sys.version_info.minor) < (3,6):
    print('This script needs at least Python v3.6')
    sys.exit(1)

import service
service.main()

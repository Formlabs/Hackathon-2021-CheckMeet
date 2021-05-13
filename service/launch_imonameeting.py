from __future__ import print_function
import sys

if sys.version_info.major < 3 or (sys.version_info.major == 3 and sys.version_info.minor < 6):
    print('This script needs at least Python v3.6')
    sys.exit(1)

import imonameeting
imonameeting.main()

# Ari Mahpour
# 07/21/2021
# This Pytest module compiles and loads the Arduino with an INO file

import subprocess
import sys
import platformio


result = subprocess.run(['platformio', '--version'], capture_output=True, text=True)
output = result.stdout.strip()

print(output)
print(type(result))

if sys.platform.startswith("linux"):  # could be "linux", "linux2", "linux3", ...
   print("MY os is LINUX")
elif(sys.platform == "darwin"):
    print("MY os is MACOS")
elif(sys.platform == "nt"):
    print("MY os is windows")
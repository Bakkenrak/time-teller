import time
import Adafruit_BBIO.GPIO as GPIO
from tell_time import tell

pin = "P9_12"

GPIO.setup(pin, GPIO.IN)
GPIO.add_event_detect(pin, GPIO.RISING)

try:
    while True:
        if GPIO.event_detected(pin):
            print("Signal detected")
            tell()
        time.sleep(0.05)

except KeyboardInterrupt:
    print("Keyboard interrupt")
    GPIO.cleanup()
    print("GPIO cleaned")

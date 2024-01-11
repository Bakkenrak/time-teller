from datetime import datetime, timedelta
from os import getcwd, path
from pydub import AudioSegment
from pydub.playback import play

# Round the current time to the nearest value divisible by 10 or 15
def round_minutes(dt):
    minutes = dt.minute
    secondOfHour = minutes * 60 + dt.second

    # Divisors to round to
    divisors = [10*60, 15*60]

    # Find the divisor closest to the current second
    closest_divisor = min(divisors, key=lambda x: min(x - secondOfHour % x, secondOfHour % x))

    # Calculate the diff of the current second to the closest divisor
    remainder = secondOfHour % closest_divisor

    # If the remainder is greater than or equal to half the divisor, round up, otherwise round down
    if remainder >= closest_divisor / 2:
        roundedSecond = secondOfHour - remainder + closest_divisor
    else:
        roundedSecond = secondOfHour - remainder

    minutes = int(roundedSecond / 60)

    # If rounding up brought us to 60 minutes, we need to roll over to the next hour
    if minutes == 60:
        return dt.replace(hour=dt.hour + 1, minute=0, second=0)
    else:
        return dt.replace(minute=minutes, second=0)

def get_rounded_hour_12h_format(dt):
    hour = dt.hour
    minutes = dt.minute

    # If minutes are at least 30, we need to roll over to the next hour
    if minutes >= 30:
        hour += 1

    if hour == 0:
        return 12
    elif hour > 12:
        return hour - 12
    else:
        return hour

def play_sound(relative_path, file_name):
    absolute_path = path.join(getcwd(), "sounds", relative_path, str(file_name) + ".mp3")
    song = AudioSegment.from_file(absolute_path)
    print("loaded")
    play(song)

def load_sound(relative_path, file_name):
    absolute_path = path.join(getcwd(), "sounds", relative_path, str(file_name) + ".mp3")
    return AudioSegment.from_file(absolute_path)

def tell():
    now = datetime.now()
    rounded = round_minutes(now)

    minutes = rounded.minute
    hours = get_rounded_hour_12h_format(rounded)

    print(f"{now.hour}:{now.minute}:{now.second}")
    print(f"{hours} - {minutes}")

    play(es_ist)
    if (minutes > 0):
        play(minute_sounds[minutes])
    play(hour_sounds[hours])
    if (minutes == 0):
        play(minute_sounds[minutes])


es_ist = load_sound("", "es_ist")

minute_sounds = {
    0: load_sound("minutes", 0),
    10: load_sound("minutes", 10),
    15: load_sound("minutes", 15),
    20: load_sound("minutes", 20),
    30: load_sound("minutes", 30),
    40: load_sound("minutes", 40),
    45: load_sound("minutes", 45),
    50: load_sound("minutes", 50)
}

hour_sounds = {
    1: load_sound("hours", 1),
    2: load_sound("hours", 2),
    3: load_sound("hours", 3),
    4: load_sound("hours", 4),
    5: load_sound("hours", 5),
    6: load_sound("hours", 6),
    7: load_sound("hours", 7),
    8: load_sound("hours", 8),
    9: load_sound("hours", 9),
    10: load_sound("hours", 10),
    11: load_sound("hours", 11),
    12: load_sound("hours", 12)
}

play(es_ist)

print("Sounds loaded")
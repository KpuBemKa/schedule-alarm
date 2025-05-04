import random


def generate_daily_schedule_dict():
    result = bytearray(b"")
    schedule_items = random.randint(5, 10)
    result.extend(schedule_items.to_bytes(length=4, byteorder="little"))
    
    print(f"Schedule items: {schedule_items}")

    day_second = random.randint(0, 3600)
    for i in range(0, schedule_items):
        action_type = random.randint(0, 1)
        print(f"Day second: {day_second} | Action: {action_type}")
        
        result.extend(day_second.to_bytes(length=4, byteorder="little"))
        result.extend(action_type.to_bytes(length=1, byteorder="little"))

        day_second += random.randint(0, 3600)

    return result


def generate_daily_schedule():
    result = bytearray(b"")
    schedule_items = random.randint(5, 10)
    result.extend(schedule_items.to_bytes(length=4, byteorder="little"))
    
    print(f"Schedule items: {schedule_items}")

    day_second = random.randint(0, 3600)
    for i in range(0, schedule_items):
        action_type = random.randint(0, 1)
        print(f"Day second: {day_second} | Action: {action_type}")
        
        result.extend(day_second.to_bytes(length=4, byteorder="little"))
        result.extend(action_type.to_bytes(length=1, byteorder="little"))

        day_second += random.randint(0, 3600)

    return result


with open("F:/dev/projects/school-alarm/data_image/schedule.bin", "wb") as f:
    f.write(b"\x03")

    for i in range(0, 31):
        print(f"Day #{i}")
        f.write(generate_daily_schedule())        
        print("----\n")

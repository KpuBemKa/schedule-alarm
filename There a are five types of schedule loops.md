There a are five types of schedule loops:
1. Daily
2. Weekly
3. Monthly
4. Yearly
5. Custom

I think schedule structures should look like this:

- Daily:
    - A collection of items containing:
        - Second of the day
        - Action type

- Weekly:
    - A collection of items containing:
        - Day of the week
        - Schedule for the day (i.e. daily schedule structure)

- Monthly:
    - A collection of items containing:
        - A day of the month (any value from 1 to 31)
        - Schedule for the day (i.e. daily schedule structure)

- Yearly:
    - A collection of items containing:
        - Number of the month (any value from 1 to 12)
        - Schedule for the month (i.e. monthly schedule structure)

- Custom:
    - Start date, which will be converted to UNIX time in seconds
    - Loop time given using an arbitrary unit, which will be converted in seconds
    - A collection of items containing:
        - Offsets from the start date (in seconds)
        - Action type

Separating these types of loops in separate schedule structures would allow for an easier time implementing things like monthly schedule, where user just inputs the schedules for days from 1-31, which eliminates the need for accounting for different month sizes or leap years. Same thing could be applied for yearly schedule. If assuming the year is always a leap year, user could be prompted for a schedule for any day of the month, eliminating the hastle with leap years.`




---


can you also add a serialization functionality for them like this:

- schedule is serialized into pure bytes
- all data is little-endian
- day schedule:
    - first 4 bytes indicate the amount of schedule points in this day schedule
    - schedule items are grouped into 5 bytes:
        - first 4 bytes store the day second
        - 5th byte stores the action type
- monthly schedule:
    - always contains 31 items, each item being a day schedule, which is stored exactly as a regular day schedule
- leave weekly, yearly and custom schedules unimplemented for now
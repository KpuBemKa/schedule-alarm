// Constants for action types
const ActionType = {
    CLOSE_RELAY: 0,
    OPEN_RELAY: 1,
};

// Simple Action class
class Action {
    constructor(type) {
        this.type = type;
    }

    serialize() {
        return new Uint8Array([this.type]);
    }
}

// Represents a single schedule point within a day
class DayScheduleItem {
    constructor(secondOfDay, action) {
        this.secondOfDay = secondOfDay; // integer (0-86399)
        this.action = action; // instance of Action
    }

    serialize() {
        const buffer = new ArrayBuffer(5);
        const view = new DataView(buffer);
        view.setUint32(0, this.secondOfDay, true); // little-endian
        view.setUint8(4, this.action.type);
        return new Uint8Array(buffer);
    }
}

class CustomScheduleItem {
    constructor(offsetSecond, action) {
        this.offsetSecond = offsetSecond;
        this.action = action;
    }

    serialize() {
        const buffer = new ArrayBuffer(9);
        const view = new DataView(buffer);
        view.setBigUint64(0, this.offsetSecond, true); // little-endian
        view.setUint8(8, this.action.type);
        return new Uint8Array(buffer);
    }
}

// Holds a list of schedule points for a single day
class DaySchedule {
    constructor() {
        this.items = [];
    }

    addItem(secondOfDay, actionType) {
        this.items.push(new DayScheduleItem(secondOfDay, new Action(actionType)));
    }

    removeItem(index) {
        if (index >= 0 && index < this.items.length) {
            this.items.splice(index, 1);
        }
    }

    clear() {
        this.items = [];
    }

    serialize() {
        const header = new ArrayBuffer(4);
        new DataView(header).setUint32(0, this.items.length, true);
        const itemBuffers = this.items.map(item => item.serialize());
        return new Uint8Array([...new Uint8Array(header), ...itemBuffers.flat()]);
    }
}

// Weekly schedule with 7 day slots
class WeeklySchedule {
    constructor() {
        this.days = Array.from({ length: 7 }, () => new DaySchedule());
    }

    getDaySchedule(dayIndex) {
        if (dayIndex < 0 || dayIndex >= 7) {
            throw new Error("Invalid day index (must be 0–6 for days 1–7)");
        }
        return this.days[dayIndex];
    }

    serialize() {
        const serializedDays = this.days.map(day => day.serialize());
        const totalLength = serializedDays.reduce((sum, arr) => sum + arr.length, 0);
        const output = new Uint8Array(totalLength);

        let offset = 0;
        for (const dayData of serializedDays) {
            output.set(dayData, offset);
            offset += dayData.length;
        }

        return output;
    }
}

// Monthly schedule with 31 day slots
class MonthlySchedule {
    constructor() {
        this.days = Array.from({ length: 31 }, () => new DaySchedule());
    }

    getDaySchedule(dayIndex) {
        if (dayIndex < 0 || dayIndex >= 31) {
            throw new Error("Invalid day index (must be 0–30 for days 1–31)");
        }
        return this.days[dayIndex];
    }

    serialize() {
        const serializedDays = this.days.map(day => day.serialize());
        const totalLength = serializedDays.reduce((sum, arr) => sum + arr.length, 0);
        const output = new Uint8Array(totalLength);

        let offset = 0;
        for (const dayData of serializedDays) {
            output.set(dayData, offset);
            offset += dayData.length;
        }

        return output;
    }
}

class YearlySchedule {
    constructor() {
        this.months = Array.from({ length: 12 }, () => new MonthlySchedule());
    }

    getMonthSchedule(monthIndex) {
        if (monthIndex < 0 || monthIndex >= 12) {
            throw new Error("Invalid month index (must be 0–11 for months 1–12)");
        }
        return this.months[monthIndex];
    }

    serialize() {
        throw new Error("Not implemented.");
    }
}

class CustomSchedule {
    constructor() {
        this.startTime = 0;
        this.loopTime = 0;
        this.items = [];
    }

    serialize() {
        const header = new ArrayBuffer(16);
        const headerView = new DataView(header)
        headerView.setBigUint64(0, this.startTime, true);
        headerView.setBigUint64(0, this.loopTime, true);

        const itemBuffers = this.items.map(item => item.serialize());
        return new Uint8Array([...new Uint8Array(header), ...itemBuffers.flat()]);
    }
}

class Schedule {
    static SCHEDULE_TYPES = ["daily", "weekly", "monthly", "yearly", "custom"];

    constructor(scheduleType = null) {
        this.schedules = {
            daily: new DaySchedule(),
            weekly: new WeeklySchedule(),
            monthly: new MonthlySchedule(),
            yearly: new YearlySchedule(),
            custom: new CustomSchedule(),
        };
        this.scheduleType = scheduleType;     // "daily", "monthly", etc.
    }

    // Set schedule type and initialize corresponding structure
    setScheduleType(type) {
        if (!Schedule.SCHEDULE_TYPES.includes(type))
            throw new Error(`Unsupported schedule type: ${type}`);

        this.scheduleType = type;
    }

    setDaySchedule({ schedule = [], monthIndex = 0, dayIndex = 0 }) {
        switch (this.scheduleType) {
            case "daily":
                this.schedules.daily.items = schedule;
                break;

            case "weekly":
                this.schedules.weekly.getDaySchedule(dayIndex).items = schedule;
                break;

            case "monthly":
                this.schedules.monthly.getDaySchedule(dayIndex).items = schedule;
                break;

            case "yearly":
                this.schedules.yearly.getMonthSchedule(monthIndex).getDaySchedule(dayIndex).items = schedule;
                break;


            case "custom":
                throw new Error("Custom schedule type does not have day schedules. Use setSchedule() instead.")

            default:
                throw new Error(`Unsupported operation for current schedule type: ${this.scheduleType}`);
        }
    }

    setSchedule(schedule) {
        switch (this.scheduleType) {
            case "daily":
                this.schedules.daily.items = schedule;
                break;

            case "weekly":
                if (schedule.length != 7)
                    throw new RangeError("A weekly schedule should have 7 items.");

                this.schedules.weekly.items = schedule;
                break;

            case "monthly":
                if (schedule.length != 31)
                    throw new RangeError("A monthly schedule should have 31 items.");

                this.schedules.monthly.items = schedule;
                break;

            case "yearly":
                if (schedule.length != 12)
                    throw new RangeError("A yearly schedule should have 12 items.");

                this.schedules.yearly.items = schedule;
                break;

            case "custom":
                this.schedules.custom.items = schedule;
                break;

            default:
                throw new Error(`Unsupported operation for current schedule type: ${this.scheduleType}`);
        }
    }

    setCustomScheduleParams(newStartTime, newLoopTime)
    {
        this.schedules.custom.startTime = newStartTime;
        this.schedules.custom.loopTime = newLoopTime;
    }

    // Add a schedule point
    // addItem({ dayIndex = 0, secondOfDay, actionType }) {
    //     switch (this.scheduleType) {
    //         case "daily":
    //             this.schedules.daily.addItem(secondOfDay, actionType);
    //             break;

    //         case "monthly":
    //             this.schedules.monthly.getDaySchedule(dayIndex).addItem(secondOfDay, actionType);
    //             break;

    //         default:
    //             throw new Error(`Unsupported operation for current schedule type: ${this.scheduleType}`);
    //     }
    // }

    // Remove an item by index (and dayIndex for monthly)
    // removeItem({ dayIndex = 0, itemIndex }) {
    //     switch (this.scheduleType) {
    //         case "daily":
    //             this.schedules.daily.removeItem(itemIndex);
    //             break;

    //         case "monthly":
    //             this.schedules.monthly.getDaySchedule(dayIndex).removeItem(itemIndex);
    //             break;

    //         default:
    //             throw new Error(`Unsupported operation for current schedule type: ${this.scheduleType}`);
    //     }
    // }

    // Clear the schedule
    // clear() {
    //     switch (this.scheduleType) {
    //         case "daily":
    //             this.schedules.daily.clear();
    //             break;

    //         case "monthly":
    //             this.schedules.monthly.forEach(day => day.clear());
    //             break;

    //         default:
    //             throw new Error(`Unsupported operation for current schedule type: ${this.scheduleType}`);
    //     }
    // }

    // Access schedule items
    getDaySchedule({ monthIndex = 0, dayIndex = 0 }) {
        switch (this.scheduleType) {
            case "daily":
                return this.schedules.daily.items;

            case "monthly":
                return this.schedules.monthly.getDaySchedule(dayIndex).items;

            case "yearly":
                return this.schedules.yearly.getMonthSchedule(monthIndex).getDaySchedule(dayIndex).items;

            default:
                throw new Error(`Unsupported operation for current schedule type: ${this.scheduleType}`);
        }
    }

    // Serialize entire schedule
    serialize() {
        return this.schedules[this.scheduleType].serialize();
    }

    // Debug print
    print() {
        console.log(`Current schedule type: ${this.scheduleType}`);

        if (this.scheduleType === "daily") {
            this.schedules.daily.items.forEach((item, idx) =>
                console.log(`[${idx}] ${item.secondOfDay}s → Action ${item.action.type}`)
            );
        } else if (this.scheduleType === "monthly") {
            this.schedules.monthly.days.forEach((day, d) => {
                console.log(`Day ${d + 1}:`);
                day.items.forEach((item, i) =>
                    console.log(`  [${i}] ${item.secondOfDay}s → Action ${item.action.type}`)
                );
            });
        }
    }
}

window.Schedule = Schedule
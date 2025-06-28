// Constants for action types
const ActionType = Object.freeze({
    CLOSE_RELAY: 0,
    OPEN_RELAY: 1,
});

const InvalidReason = {
    OVERLAP: 0,
    DUPLICATE: 1,
    LOOP_OVERFLOW: 2,
    UNORDERED: 3,

    toString(reason) {
        switch (reason) {
            case InvalidReason.OVERLAP:
                return "Some schedule points overlap";

            case InvalidReason.DUPLICATE:
                return "Some schedule points are marked to fire at the same time";

            case InvalidReason.LOOP_OVERFLOW:
                return "Some schedule activation time points exceed the maximum value";

            case InvalidReason.UNORDERED:
                return "Some schedule items are not in order. If you see this, something has definitely has gone wrong under the hood. Report this incedent to your provider";

            default:
                throw new TypeError(`Unknown InvalidReason: ${reason}`);
        }
    }
}

class InvalidScheduleException extends Error {
    constructor(message, params) {
        super(message);
        this.name = this.constructor.name;
        this.params = params;
    }
}

// Simple Action class
class Action {
    /**
     * @type { ActionType } type 
     */
    #type

    constructor(type) {
        this.type = type;
    }

    get type() { return this.#type }
    set type(newType) {
        if (!Number.isInteger(newType))
            throw new TypeError(`Invalid ActionType; Got: ${newType}`);

        if (newType < ActionType.CLOSE_RELAY || newType > ActionType.OPEN_RELAY)
            throw new RangeError(`Invalid ActionType; Got: ${newType}`);

        this.#type = newType;
    }

    serialize() {
        return new Uint8Array([this.type]);
    }

    getInverse() {
        switch (this.type) {
            case ActionType.CLOSE_RELAY:
                return ActionType.OPEN_RELAY;

            case ActionType.OPEN_RELAY:
                return ActionType.CLOSE_RELAY;

            default:
                throw new TypeError(`Unknown action type: ${this.type}`);
        }
    }
}

class SchedulePoint {
    /** @type { Number } timeOffset */
    #timeOffset;
    /** @type { Action } action */
    #action;
    /** @type { Number } duration */
    #duration;

    /**
     * 
     * @param { Number } timeOffset 
     * @param { Action } action 
     * @param { Number } duration 
     */
    constructor(timeOffset, action, duration) {
        this.timeOffset = timeOffset;
        this.action = action;
        this.duration = duration;
    }

    get timeOffset() { return this.#timeOffset; }
    get action() { return this.#action; }
    get duration() { return this.#duration; }

    set timeOffset(newValue) {
        if (!Number.isInteger(newValue))
            throw new TypeError(`Invalid timeOffset type; Got: ${newValue} with type: ${typeof newValue}`);

        if (newValue < 0 || newValue > Number.MAX_SAFE_INTEGER)
            throw new RangeError(`Invalid timeOffset value: ${newValue}`);

        this.#timeOffset = newValue;
    }

    set action(newValue) {
        if (Number.isInteger(newValue))
            newValue = new Action(newValue);

        if (!(newValue instanceof Action))
            throw new TypeError(`Invalid Action type; Got: ${newValue} with type: ${typeof newValue}`);

        this.#action = newValue;
    }

    set duration(newValue) {
        if (!Number.isInteger(newValue))
            throw new TypeError(`Invalid duration; Got: ${newValue} with type: ${typeof newValue}`);

        if (newValue < 0 || newValue > Number.MAX_SAFE_INTEGER)
            throw new RangeError(`Invalid duration value: ${newValue}`);

        this.#duration = newValue;
    }
}

class CustomSchedule {
    /** @type { Number } */
    #startTime
    /** @type { Number } */
    #loopTime
    /** @type { [SchedulePoint] } */
    #items

    /**
     * 
     * @param { Number } startTime 
     * @param { Number } loopTime 
     * @param { [SchedulePoint] } items 
     */
    constructor(startTime = 0, loopTime = 1, items = []) {
        this.startTime = startTime;
        this.loopTime = loopTime;
        this.items = items;
    }

    get startTime() { return this.#startTime; }
    set startTime(newValue) {
        if (!Number.isInteger(newValue))
            throw new TypeError(`Invalid startTime; Got: ${newValue} with type: ${typeof newValue}`);

        if (newValue < 0 || newValue > Number.MAX_SAFE_INTEGER)
            throw new RangeError(`Invalid startTime value: ${newValue}`);

        this.#startTime = newValue
    }

    get loopTime() { return this.#loopTime; }
    set loopTime(newValue) {
        if (!Number.isInteger(newValue))
            throw new TypeError(`Invalid loopTime; Got: ${newValue} with type: ${typeof newValue}`);

        if (newValue < 1 || newValue > Number.MAX_SAFE_INTEGER)
            throw new RangeError(`Invalid loopTime value: ${newValue}`);

        this.#loopTime = newValue
    }

    get items() { return this.#items; }
    set items(newSchedule) {
        if (!Array.isArray(newSchedule))
            throw new TypeError("Schedule should be an array.");

        // attempt to construct the SchedulePoint objects if not already
        newSchedule = newSchedule.map(schedulePoint => {
            if (schedulePoint instanceof SchedulePoint)
                return schedulePoint;

            return new SchedulePoint(schedulePoint.timeOffset, schedulePoint.action, schedulePoint.duration);
        });

        const invalidItems = this.getIndexesOfInvalidItems(newSchedule);
        if (invalidItems.length != 0)
            throw new InvalidScheduleException("Invalid schedule formation.", { invalidPoints: invalidItems });

        this.#items = newSchedule
    }

    /**
     * 
     * @param { [SchedulePoint | undefined] } itemsToCheck 
     * @param { Number | undefined } startTime 
     * @param { Number | undefined } loopTime 
     * @returns { [{ index: Number, reason: InvalidReason }] }
     */
    getIndexesOfInvalidItems(itemsToCheck, startTime, loopTime) {
        loopTime = (loopTime === null || loopTime === undefined)
            ? this.loopTime
            : loopTime;

        startTime = (startTime === null || startTime === undefined)
            ? this.startTime
            : startTime;

        itemsToCheck = (itemsToCheck === null || itemsToCheck === undefined)
            ? this.items
            : itemsToCheck;

        const invalidIndexes = [];

        // skip last item to avoid triggering out-of-bounds
        for (let i = 0; i < itemsToCheck.length - 1; ++i) {
            const current = itemsToCheck[i];
            const next = itemsToCheck[i + 1];

            // check if the schedule is ordered by timeOffset
            if (current.timeOffset > next.timeOffset) {
                invalidIndexes.push({ index: i, reason: InvalidReason.UNORDERED });
                continue; // other checks won't work correctly if the list is not in order
            }

            // check for overlapping schedule points
            if (current.timeOffset + current.duration > next.timeOffset)
                invalidIndexes.push({ index: i, reason: InvalidReason.OVERLAP }, { index: i + 1, reason: InvalidReason.OVERLAP });

            // check for schedule points with the same day time
            if (current.timeOffset === next.timeOffset)
                invalidIndexes.push({ index: i, reason: InvalidReason.DUPLICATE }, { index: i + 1, reason: InvalidReason.DUPLICATE });

            // check for overflow beyond to loop end
            if (current.timeOffset > loopTime)
                invalidIndexes.push({ index: i, reason: InvalidReason.LOOP_OVERFLOW });
        }

        // check the last item here
        if (itemsToCheck.length > 0) {
            const lastIndex = itemsToCheck.length - 1;

            // check for overflow
            if (itemsToCheck.at(lastIndex).timeOffset > loopTime)
                invalidIndexes.push({ index: lastIndex, reason: InvalidReason.LOOP_OVERFLOW });
        }

        // return invalidIndexes.filter((value, index, self) => self.indexOf(value) === index);
        return Array.from(
            new Map(invalidIndexes.map(obj => [JSON.stringify(obj), obj])).values()
        );
    }

    /**
     * 
     * @param { [SchedulePoint | undefined] } itemsToCheck 
     * @param { Number | undefined } startTime 
     * @param { Number | undefined } loopTime 
     * @returns { boolean }
     */
    isScheduleValid(itemsToCheck, startTime, loopTime) {
        return this.getIndexesOfInvalidItems(itemsToCheck, startTime, loopTime).length == 0;
    }

    /**
     * @returns { {startTime: Number, loopTime: Number, items: [{timeOffset: Number, action: Action}]} }
     */
    toSystemSchedule() {
        /** @type { [{timeOffset: Number, action: Number}] } */
        const transformedItems = [];
        this.items.forEach(item => {
            transformedItems.push({
                timeOffset: item.timeOffset,
                action: item.action
            });

            if (item.duration != 0) {
                transformedItems.push({
                    timeOffset: item.timeOffset + item.duration,
                    action: item.action.getInverse()
                });
            }
        });

        transformedItems.sort((a, b) => a.timeOffset - b.timeOffset);

        return { startTime: this.startTime, loopTime: this.loopTime, items: transformedItems };
    }

    serialize() {
        const schedule = this.toSystemSchedule();

        const header = new ArrayBuffer(20);
        const headerView = new DataView(header)
        headerView.setBigUint64(0, BigInt(schedule.startTime), true);
        headerView.setBigUint64(8, BigInt(schedule.loopTime), true);
        headerView.setUint32(16, schedule.items.length, true);

        const itemsByteLength = schedule.items.length * 5;
        const itemsBuffer = new ArrayBuffer(itemsByteLength);
        const itemsView = new DataView(itemsBuffer);
        let bufferOffset = 0;        

        schedule.items.forEach(item => {
            itemsView.setUint32(bufferOffset + 0, item.timeOffset, true); // little-endian
            itemsView.setUint8(bufferOffset + 4, item.action.type);
            bufferOffset += 5;
        });

        return new Uint8Array([...new Uint8Array(header), ...new Uint8Array(itemsBuffer)]);
    }
}

// Holds a list of schedule points for a single day
class DaySchedule {
    #customSchedule;

    /**
     * @param { [SchedulePoint] } items 
     */
    constructor(items) {
        this.#customSchedule = new CustomSchedule(0, 86_000, items);
    }

    get schedule() { return this.#customSchedule.items; }
    set schedule(newSchedule) { this.#customSchedule.items = newSchedule }

    /**
     * @param { [SchedulePoint] | undefined } scheduleToCheck 
     */
    getIndexesOfInvalidItems(scheduleToCheck) {
        return this.#customSchedule.getIndexesOfInvalidItems(
            scheduleToCheck,
            this.#customSchedule.startTime,
            this.#customSchedule.loopTime,
        );
    }

    /**
     * @param { [SchedulePoint | undefined] } scheduleToCheck 
     */
    isScheduleValid(scheduleToCheck) {
        return this.#customSchedule.isScheduleValid({
            startTime: this.#customSchedule.startTime,
            loopTime: this.#customSchedule.loopTime,
            itemsToCheck: scheduleToCheck
        });
    }

    serialize() {
        const transformedSchedule = this.#customSchedule.toSystemSchedule();

        const header = new ArrayBuffer(4);
        new DataView(header).setUint32(0, transformedSchedule.items.length, true);

        const itemBuffers = transformedSchedule.items.map(item => {
            const buffer = new ArrayBuffer(5);
            const view = new DataView(buffer);
            view.setUint32(0, item.daySecond, true); // little-endian
            view.setUint8(4, item.action.type);
            return new Uint8Array(buffer);
        });

        return new Uint8Array([...new Uint8Array(header), ...itemBuffers.flat()]);
    }
}

class MutlipleDaysSchedule {
    /** @type [DaySchedule] */
    #days;

    /**
     * @param { Number } daysCount 
     * @param { [DaySchedule] | [[SchedulePoint]] } newSchedule An array of DaySchedules or an array of arrays of schedule points
     */
    constructor(daysCount, newSchedule = []) {
        this.#days = Array.from({ length: daysCount }, () => new DaySchedule());
        this.setSchedule(newSchedule);
    }

    get length() { return this.#days.length };
    get count() { return this.#days.length };

    /**
     * 
     * @param { [DaySchedule] | [[SchedulePoint]] } newSchedule An array of DaySchedules or an array of arrays of schedule points
     */
    setSchedule(newSchedule = []) {
        if (!Array.isArray(newSchedule))
            throw new TypeError("Schedule should be an array.");

        // reset the schedule if it's an empty array
        if (newSchedule.length === 0) {
            this.#days.forEach(day => day.schedule = []);
            return;
        }

        if (newSchedule.length != this.#days.length)
            throw new RangeError(`New multiple-days schedule should have either ${this.length} items or 0 items.`);

        const invalidDays = this.getInvalidDays(newSchedule);
        if (invalidDays.length !== 0)
            throw new InvalidScheduleException('Invalid schedule formation.', { invalidDays: invalidDays });

        this.#days.forEach((_, index) => this.setDaySchedule(index, newSchedule[index]));
    }

    /**
     * 
     * @param { Number } dayIndex 
     * @param { Number } newSchedule 
     */
    setDaySchedule(dayIndex, newSchedule) {
        if (dayIndex >= this.#days.length)
            throw new RangeError(`Invalid week day index. Must be in the [0-${this.#days.length - 1}] range.`);

        const daySchedule =
            (newSchedule instanceof DaySchedule)
                ? newSchedule
                : new DaySchedule(newSchedule);

        const invalidItems = daySchedule.getIndexesOfInvalidItems();
        if (invalidItems.length != 0)
            throw new InvalidScheduleException('Failed to set weekly schedule day.', invalidItems);

        this.#days[dayIndex] = daySchedule;
    }

    /**
     * 
     * @param { Number } dayIndex 
     * @returns { [SchedulePoint] }
     */
    getDaySchedule(dayIndex) {
        if (dayIndex >= this.#days.length)
            throw new RangeError(`Invalid week day index. Must be in the [0-${this.#days.length - 1}] range.`);

        return this.#days[dayIndex].schedule;
    }

    /**
     * @param { [[SchedulePoint]] } daysToCheck
     * @returns { [{dayIndex: Number, invalidPoints: [{ index: Number, reason: InvalidReason }]}] }
     */
    getInvalidDays(daysToCheck) {
        const dayChecker = new DaySchedule();
        const invalidDays = [];
        daysToCheck.forEach((day, index) => {
            const dayInvalidPoints = dayChecker.getIndexesOfInvalidItems(day);
            if (dayInvalidPoints.length === 0)
                return;

            invalidDays.push({ dayIndex: index, invalidPoints: dayInvalidPoints });
        });

        return invalidDays;
    }

    serialize() {
        const serializedDays = this.#days.map(day => day.serialize());
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

/* Weekly schedule with 7 day slots */
class WeeklySchedule extends MutlipleDaysSchedule {
    constructor(newSchedule = []) {
        super(7, newSchedule);
    }
}

/* Monthly schedule with 31 day slots */
class MonthlySchedule extends MutlipleDaysSchedule {
    constructor(newSchedule = []) {
        super(31, newSchedule);
    }
}

class YearlySchedule {
    /** @type [MonthlySchedule] */
    #months;

    constructor() {
        this.#months = Array.from({ length: 12 }, () => new MonthlySchedule());
    }

    /**
     * 
     * @param { [MonthlySchedule | [[[SchedulePoint]]]] } newSchedule 
     * An array of month schedules or an array of arrays of arrays of schedule points
     */
    setSchedule(newSchedule) {
        if (!Array.isArray(newSchedule))
            throw new TypeError("Schedule should be an array.");

        // Reset the schedule if it's an empty array
        if (newSchedule.length === 0) {
            this.#months = Array.from({ length: 12 }, () => new MonthlySchedule());
            return;
        }

        if (newSchedule.length != this.#months.length)
            throw new RangeError("Yearly schedule should contain either 12 items or 0 items.");

        this.#months.forEach((month, index) => {
            if (newSchedule[index] instanceof MonthlySchedule)
                month = newSchedule[index];
            else
                month.setSchedule(newSchedule[index]);
        });
    }

    /**
     * 
     * @param { Number } monthIndex 
     * @param { Number } dayIndex 
     * @param { [DaySchedule | [SchedulePoint]] } newSchedule A day schedule of an array of schedule points
     */
    setDaySchedule(monthIndex, dayIndex, newSchedule) {
        this.#months[monthIndex].setDaySchedule(dayIndex, newSchedule);
    }

    /**
     * 
     * @param { Number } monthIndex 
     * @param { Number } dayIndex 
     */
    getDaySchedule(monthIndex, dayIndex) {
        return this.getMonthSchedule(monthIndex).getDaySchedule(dayIndex);
    }

    /**
     * 
     * @param { Number } monthIndex 
     */
    getMonthSchedule(monthIndex) {
        if (monthIndex >= 12) {
            throw new RangeError("Invalid month index (must be up to 11)");
        }

        return this.#months[monthIndex];
    }

    serialize() {
        const serializedMonths = this.#months.map(day => day.serialize());
        const totalLength = serializedMonths.reduce((sum, arr) => sum + arr.length, 0);
        const output = new Uint8Array(totalLength);

        let offset = 0;
        for (const dayData of serializedMonths) {
            output.set(dayData, offset);
            offset += dayData.length;
        }

        return output;
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
        this.scheduleType = scheduleType; // one option from SCHEDULE_TYPES
    }

    /**
     * 
     * @param { String } type One of the strings from `Schedule.SCHEDULE_TYPES`
     */
    setScheduleType(type) {
        if (!Schedule.SCHEDULE_TYPES.includes(type))
            throw new Error(`Unsupported schedule type: ${type}`);

        this.scheduleType = type;
    }

    setDaySchedule({ schedule = [], monthIndex = 0, dayIndex = 0 }) {
        switch (this.scheduleType) {
            case "daily":
                this.schedules.daily.schedule = schedule;
                break;

            case "weekly":
                this.schedules.weekly.setDaySchedule(dayIndex);
                break;

            case "monthly":
                this.schedules.monthly.setDaySchedule(dayIndex, schedule);
                break;

            case "yearly":
                this.schedules.yearly.setDaySchedule(monthIndex, dayIndex, schedule);
                break;

            case "custom":
                this.schedules.custom.items = schedule;
                break;

            default:
                throw new Error(`Unsupported operation for current schedule type: ${this.scheduleType}`);
        }
    }

    setSchedule(schedule) {
        switch (this.scheduleType) {
            case "daily":
                this.schedules.daily.schedule = schedule;
                break;

            case "weekly":
                this.schedules.weekly.setSchedule(schedule);
                break;

            case "monthly":
                this.schedules.monthly.setSchedule(schedule);
                break;

            case "yearly":
                this.schedules.yearly.setSchedule(schedule);
                break;

            case "custom":
                this.schedules.custom.items = schedule;
                break;

            default:
                throw new Error(`Unsupported operation for current schedule type: ${this.scheduleType}`);
        }
    }

    setCustomScheduleParams(newStartTime, newLoopTime) {
        this.schedules.custom.startTime = newStartTime;
        this.schedules.custom.loopTime = newLoopTime;
    }

    // Access schedule items
    getDaySchedule({ monthIndex = 0, dayIndex = 0 }) {
        switch (this.scheduleType) {
            case "daily":
                return this.schedules.daily.schedule;

            case "weekly":
                return this.schedules.weekly.getDaySchedule(dayIndex);

            case "monthly":
                return this.schedules.monthly.getDaySchedule(dayIndex);

            case "yearly":
                return this.schedules.yearly.getMonthSchedule(monthIndex).getDaySchedule(dayIndex);

            case "custom":
                throw new Error("Custom schedule type does not have day schedules.")

            default:
                throw new Error(`Unsupported operation for current schedule type: ${this.scheduleType}`);
        }
    }

    getInvalidItemsDaily(dailySchedule = null) {
        return this.schedules.daily.getIndexesOfInvalidItems(dailySchedule);
    }

    getInvalidItemsWeekly() { return this.schedules.weekly.getInvalidIndexes(); }
    getInvalidItemsCustom() { return this.schedules.custom.getIndexesOfInvalidItems(); }

    // Serialize the selected schedule
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
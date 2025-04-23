class SchedulePoint {
    constructor(offset, action, duration) {
        this.offset = offset; // seconds from loop start
        this.action = action; // 'open' or 'close'
        this.duration = duration; // in seconds
    }
}

class Schedule {
    constructor(startTime = Date.now() / 1000, loopDuration = 86400) {
        this.startTime = Math.floor(startTime); // Unix timestamp in seconds
        this.loopDuration = loopDuration; // in seconds
        this.schedulePoints = []; // array of SchedulePoint
    }

    // Add a new schedule point
    addPoint(offset, action, duration) {
        if (!['open', 'close'].includes(action)) {
            throw new Error(`Invalid action "${action}". Must be 'open' or 'close'.`);
        }
        this.schedulePoints.push(new SchedulePoint(offset, action, duration));
    }

    // Update a point by index
    updatePoint(index, newOffset, newAction, newDuration) {
        if (index < 0 || index >= this.schedulePoints.length) return false;

        if (!['open', 'close'].includes(newAction)) return false;

        this.schedulePoints[index] = new SchedulePoint(newOffset, newAction, newDuration);
        return true;
    }

    // Delete a point by index
    deletePoint(index) {
        if (index < 0 || index >= this.schedulePoints.length) return false;
        this.schedulePoints.splice(index, 1);
        return true;
    }

    // Clear the entire schedule
    clear() {
        this.schedulePoints = [];
    }

    // Get the current offset within the loop
    getLoopTime(now = Date.now() / 1000) {
        return (Math.floor(now) - this.startTime) % this.loopDuration;
    }

    // Get active points at current time (could return multiple)
    getActivePoints(now = Date.now() / 1000) {
        const loopTime = this.getLoopTime(now);
        return this.schedulePoints.filter(pt => loopTime === pt.offset);
    }

    // Get serialized (JSON) schedule for storage or sending
    toJSON() {
        return {
            startTime: this.startTime,
            loopDuration: this.loopDuration,
            schedulePoints: this.schedulePoints.map(pt => ({
                offset: pt.offset,
                action: pt.action,
                duration: pt.duration
            }))
        };
    }

    // Load from JSON
    static fromJSON(json) {
        const schedule = new Schedule(json.startTime, json.loopDuration);
        json.schedulePoints.forEach(pt => {
            schedule.addPoint(pt.offset, pt.action, pt.duration);
        });
        return schedule;
    }
}

class ScheduleModel {
    constructor() {
        this.schedules = {
            yearly: new Schedule()
        };
        this.loopDuration = loopDuration; // in seconds
        this.schedulePoints = []; // array of SchedulePoint
    }

    static createYearlyLoop() {
        
    }
}
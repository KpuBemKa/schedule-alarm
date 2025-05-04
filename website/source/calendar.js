const monthNames = [
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
];

const monthDayCounts = [31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31];

function monthIndexToName(monthIndex) {
    return monthNames[monthIndex];
}

function createMonthBlock(monthIndex, onDayClick) {
    let title = "";
    let dayCount = "";

    if (monthIndex >= 0) {
        title = monthIndexToName(monthIndex);
        dayCount = monthDayCounts[monthIndex];
    } else {
        title = "Month";
        dayCount = 31;
    }

    const monthDiv = document.createElement('div');
    monthDiv.className = 'c-month';

    const titleEl = document.createElement('h3');
    titleEl.textContent = title;
    monthDiv.appendChild(titleEl);

    const daysGrid = document.createElement('div');
    daysGrid.className = 'c-days';

    for (let day = 1; day <= dayCount; day++) {
        const dayEl = document.createElement('div');
        dayEl.className = 'c-day';
        dayEl.textContent = day;
        dayEl.addEventListener('click', () => {
            onDayClick(monthIndex, day);
        });
        daysGrid.appendChild(dayEl);
    }

    monthDiv.appendChild(daysGrid);
    return monthDiv;
}

function createMonthCalendar(targetDivClass, monthIndex, onDayClick) {
    const calendarContainers = document.querySelectorAll(`.${targetDivClass}`);
    calendarContainers.forEach(containerEl => {
        containerEl.innerHTML = '';
        const monthEl = createMonthBlock(monthIndex, onDayClick);
        containerEl.appendChild(monthEl);
    });
}

function createYearCalendar(targetDivClass, onDayClick) {
    const calendarContainers = document.querySelectorAll(`.${targetDivClass}`);
    calendarContainers.forEach(containerEl => {
        containerEl.innerHTML = '';
        // const yearDiv = document.createElement('div');
        // yearDiv.className = "c-schedule-container";

        for (let month = 0; month < 12; month++) {
            const monthEl = createMonthBlock(month, onDayClick);
            // yearDiv.appendChild(monthEl);
            containerEl.appendChild(monthEl);
        }

        // containerEl.appendChild(yearDiv);
    });
}

/// -------------

function timeToSeconds(hhmmss) {
    const [h, m, s] = hhmmss.split(':').map(Number);
    return h * 3600 + m * 60 + s;
}

function secondsToTime(seconds) {
    const h = String(Math.floor(seconds / 3600)).padStart(2, '0');
    const m = String(Math.floor((seconds % 3600) / 60)).padStart(2, '0');
    const s = String(seconds % 60).padStart(2, '0');
    return `${h}:${m}:${s}`;
}

function createDailyScheduleRow(rowsContainer, secondOfDay, actionType) {
    const row = document.createElement('div');
    row.classList.add('d-schedule-row', 'd-margin-top-10');

    const timeInput = Object.assign(document.createElement('input'), {
        type: 'time',
        step: 1,
        value: secondsToTime(secondOfDay),
    });

    const actionSelect = document.createElement('select');
    actionSelect.innerHTML = `
        <option value="${ActionType.CLOSE_RELAY}">Close Relay</option>
        <option value="${ActionType.OPEN_RELAY}">Open Relay</option>
    `;
    actionSelect.value = actionType;

    const removeBtn = document.createElement('button');
    removeBtn.textContent = '✕';
    removeBtn.className = "d-button";

    removeBtn.addEventListener('click', () => {
        row.remove();
    });

    row.append(timeInput, actionSelect, removeBtn);
    rowsContainer.appendChild(row);
}

function generateDailyScheduleRows(rowsContainer, initialSchedule = []) {
    rowsContainer.innerHTML = "";
    
    initialSchedule.forEach(item => {
        createDailyScheduleRow(rowsContainer, item.daySecond, item.fireAction)
    });
}


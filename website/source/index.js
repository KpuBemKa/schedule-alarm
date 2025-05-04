const POPUP_WINDOW_ID = "d-day-schedule-popup";

const schedule = new Schedule();


function showTab(tabId) {
    document.querySelectorAll('.tab').forEach(tab => {
        tab.classList.remove('active');
    });

    document.querySelectorAll('.tab-content').forEach(content => {
        content.classList.remove('active');
    });

    document.querySelector(`.tab[onclick*="${tabId}"]`).classList.add('active');
    document.getElementById(tabId).classList.add('active');
}

function scheduleTypeSelectChanged() {
    const newType = document.getElementById("schedule-loop-type").value;
    const targetClass = `d-${newType}-view`;
    console.log(targetClass);

    const viewsHolder = document.getElementById("d-schedule-views");
    // console.log(viewsHolder.children);
    for (view of viewsHolder.children) {
        const viewClassList = view.classList;

        if (viewClassList.contains(targetClass)) {
            viewClassList.remove("d-hidden");
            continue;
        }

        view.classList.add("d-hidden");
    }

    schedule.setScheduleType(newType);
}

function onYearlyDayClick(month, day) {
    if (schedule.scheduleType != "yearly") {
        throw TypeError("onDayClick event fired for the yearly schedule when current schedule type is not yearly.");
    }

    showPopup({ monthIndex: month, dayIndex: day });
}

function onMonthlyDayClick(_, day) {
    if (schedule.scheduleType != "monthly") {
        throw TypeError("onDayClick event fired for the monthly schedule when current schedule type is not monthly.");
    }

    showPopup({ dayIndex: day });
}

function showPopup({ monthIndex = -1, dayIndex = 0 }) {
    document.getElementById(POPUP_WINDOW_ID).classList.remove("d-hidden");
    const rowsContainer = document.querySelector(`#${POPUP_WINDOW_ID} .d-schedule-rows-container`);
    const storedSchedule = schedule.getDaySchedule({ monthIndex: monthIndex, dayIndex: dayIndex });
    generateDailyScheduleRows(rowsContainer, (storedSchedule != undefined && storedSchedule.length != 0) ? storedSchedule : [{ daySecond: 0, fireAction: 0 }]);

    let headerText;
    if (monthIndex >= 0)
        headerText = `${monthIndexToName(monthIndex)} ${dayIndex}`;
    else
        headerText = `Day: ${dayIndex}`

    const editorHeader = document.querySelector(`#${POPUP_WINDOW_ID} #popup-header`);
    editorHeader.innerHTML = headerText;

    document.querySelector(`#${POPUP_WINDOW_ID} #d-popup-monthIndex`).value = monthIndex;
    document.querySelector(`#${POPUP_WINDOW_ID} #d-popup-dayIndex`).value = dayIndex;
}

function closePopup(promptConfirm = true) {
    // check if the confirmation should be asked, then, if confirmation declined, skip the closing process
    if (promptConfirm && !confirm("Close the editor without saving?")) {
        return;
    }

    document.getElementById(POPUP_WINDOW_ID).classList.add("d-hidden");
}

function savePopupSchedule() {
    const scheduleRows = document.querySelectorAll(`#${POPUP_WINDOW_ID} .d-schedule-row`);
    console.log(scheduleRows);

    const newDayschedule = [];
    scheduleRows.forEach(scheduleRow => {
        const time = scheduleRow.children[0].value;
        const action = scheduleRow.children[1].value;

        newDayschedule.push({
            daySecond: timeToSeconds(time),
            fireAction: action
        });
    });

    newDayschedule.sort((lhs, rhs) => {
        if (rhs.daySecond > lhs.daySecond)
            return -1
        else if (rhs.daySecond < lhs.daySecond)
            return 1

        return 0;
    });

    const monthIndex = document.querySelector(`#${POPUP_WINDOW_ID} #d-popup-monthIndex`).value;
    const dayIndex = document.querySelector(`#${POPUP_WINDOW_ID} #d-popup-dayIndex`).value;

    // console.log(`Day index: ${dayIndex}, Month index: ${monthIndex}`);

    schedule.setDaySchedule({
        schedule: newDayschedule,
        monthIndex: Number(monthIndex),
        dayIndex: Number(dayIndex)
    });

    closePopup(promptConfirm = false);
}

function loadSchedule() {
    const deviceSchedule = fetchSchedule();
    schedule.setScheduleType(deviceSchedule.scheduleType);
    schedule.setSchedule(deviceSchedule.schedule);

    generateDailyScheduleRows(
        document.querySelector(`.d-daily-view .d-schedule-rows-container`),
        (deviceSchedule.scheduleType === "daily") ? storedSchedule : [{ daySecond: 0, fireAction: 0 }]
    );

    document.getElementById("schedule-loop-type").value = deviceSchedule.scheduleType;

    scheduleTypeSelectChanged();
}
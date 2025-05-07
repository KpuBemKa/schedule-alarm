const POPUP_WINDOW_ID = "d-day-schedule-popup";
const WEEK_DAYS = ["Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"];

const schedule = new Schedule();


function showTab(tabGroup, tabId) {
    document.querySelectorAll(`.${tabGroup} .tab`).forEach(tab => {
        tab.classList.remove('active');
    });

    document.querySelectorAll(`.${tabGroup}.tab-content`).forEach(content => {
        content.classList.remove('active');
    });

    document.querySelector(`.${tabGroup} .tab[onclick*="${tabId}"]`).classList.add('active');
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


function generateWeeklyView() {
    const weekViewTabGroup = "weekly-schedule";

    const tabButtonsContainer = document.querySelector(".tab-group-weekly-schedule.tabs");
    WEEK_DAYS.forEach(weekDay => {
        weekDayButton = document.createElement("button");
        weekDayButton.innerHTML = weekDay;
        weekDayButton.classList = "tab";
        weekDayButton.type = "button";

        // `setAttribute()` adds the onclick attribute inside the html itself,
        // which the `showTab()` then uses to find the button when user clicks it
        weekDayButton.setAttribute("onclick", `showTab('tab-group-${weekViewTabGroup}', '${weekViewTabGroup}-${weekDay.toLowerCase()}')`);

        tabButtonsContainer.appendChild(weekDayButton);
    });

    const WEEK_DAYSContainer = document.querySelector(".d-weekly-view .d-schedule-wrapper");
    WEEK_DAYS.forEach(weekDay => {
        const weekDayContainer = document.createElement("div");
        weekDayContainer.classList.add(`tab-group-${weekViewTabGroup}`, "tab-content");
        const containerId = `${weekViewTabGroup}-${weekDay.toLowerCase()}`
        weekDayContainer.id = containerId;

        const scheduleContainer = document.createElement("div");
        scheduleContainer.classList.add("d-schedule-rows-container");
        weekDayContainer.appendChild(scheduleContainer);
        WEEK_DAYSContainer.appendChild(weekDayContainer);

        generateDailyScheduleRows(
            document.querySelector(`#${containerId} .d-schedule-rows-container`),
            [{ daySecond: 0, fireAction: 0 }]
        );

        const addRowButtonContainer = document.createElement("div");
        addRowButtonContainer.classList.add("d-row", "d-justify-end", "d-margin-top-10");

        const addRowButton = document.createElement("button");
        addRowButton.classList.add("d-button", "d-add-schedule-point");
        addRowButton.type = "button";
        addRowButton.onclick = () => createDailyScheduleRow(document.querySelector(`#${containerId} .d-schedule-rows-container`), 0, 0);
        addRowButton.innerHTML = '+';

        addRowButtonContainer.appendChild(addRowButton);
        weekDayContainer.appendChild(addRowButtonContainer);
    });

    showTab(`tab-group-${weekViewTabGroup}`, `${weekViewTabGroup}-mon`)
}

function loadSchedule() {
    const deviceSchedule = fetchSchedule();
    schedule.setScheduleType(deviceSchedule.scheduleType);
    schedule.setSchedule(deviceSchedule.schedule);

    generateDailyScheduleRows(
        document.querySelector(`.d-daily-view .d-schedule-rows-container`),
        (deviceSchedule.scheduleType === "daily") ? deviceSchedule.schedule : [{ daySecond: 0, fireAction: 0 }]
    );

    if (deviceSchedule.scheduleType === 'weekly') {
        if (WEEK_DAYS.length != deviceSchedule.schedule.length) {
            throw new RangeError(`Device schedule does not have the correct amount of days for a weekly schedule type. Device days: ${deviceSchedule.schedule.length}; Required days: ${WEEK_DAYS.length}`);
        }

        for (let i = 0; i < WEEK_DAYS.length; ++i) {
            const location = document.querySelector(
                `#${weekViewTabGroup}-${WEEK_DAYS[i].toLowerCase()} .d-schedule-rows-container`
            );
            const data = deviceSchedule.schedule[i];

            generateDailyScheduleRows(location, data);
        }
    }

    document.getElementById("schedule-loop-type").value = deviceSchedule.scheduleType;

    scheduleTypeSelectChanged();
}
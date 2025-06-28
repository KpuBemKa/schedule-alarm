const POPUP_WINDOW_ID = "d-day-schedule-popup";
const WEEK_DAYS = ["Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"];


const schedule = new Schedule();


/**
 * Should be called when the schedule type selector is changed by the user
 */
function onScheduleTypeSelectChanged() {
    const newType = getScheduleTypeSelection();
    const targetClass = `d-${newType}-view`;
    // console.log(targetClass);

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

/**
 * Is passed to the calendar constructor as a callback when user clicks on a day in the yearly schedule. 
 * @param { Number } month [0; 11]
 * @param { Number } day [0; 30]
 */
function onYearlyDayClick(month, day) {
    if (schedule.scheduleType != "yearly") {
        throw TypeError("onDayClick event fired for the yearly schedule when current schedule type is not yearly.");
    }

    showPopup({ monthIndex: month, dayIndex: day });
}

/**
 * Is passed to the calendar constructor as a callback when user clicks on a day in the monthly schedule. 
 * @param { undefined } _ Unused
 * @param { Number } day [0; 30]
 */
function onMonthlyDayClick(_, day) {
    if (schedule.scheduleType != "monthly") {
        throw TypeError("onDayClick event fired for the monthly schedule when current schedule type is not monthly.");
    }

    showPopup({ dayIndex: day });
}

/**
 * @returns { String } Currently selected option in the schedule type selector
 */
function getScheduleTypeSelection() {
    return document.getElementById("schedule-loop-type").value;
}

/**
 * Extracts the schedule from the day-like schedule containers: day, custom & week day schedules.
 * Also, sorts the schedule by time in ascending order
 * @param { String } rowsSelector 
 * @returns { [SchedulePoint] }
 */
function getScheduleFromBaseRows(rowsSelector) {
    let scheduleData = [];

    document.querySelectorAll(rowsSelector).forEach(scheduleRow => {
        scheduleData.push(new SchedulePoint(
            timeToSeconds(scheduleRow.children[0].value),
            Number(scheduleRow.children[1].value),
            Number(scheduleRow.children[2].children[0].value)
        ));
    });

    scheduleData.sort((a, b) => a.timeOffset - b.timeOffset);

    return scheduleData;
}

/**
 * Hides all divs within the divs with class `tabGroup`,
 * and leaves only one div with `tabId` id shown
 * @param { String } tabGroup Tab group to work in
 * @param { String } tabId Id of the tab to leave shown
 */
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

/**
 * Generates the weekly schedule tab
 */
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

    const weekDaysContainer = document.querySelector(".d-weekly-view .d-schedule-wrapper");

    WEEK_DAYS.forEach(weekDay => {
        // create a container for the week day
        const weekDayContainer = document.createElement("div");
        weekDayContainer.classList.add(`tab-group-${weekViewTabGroup}`, "tab-content");
        const containerId = `${weekViewTabGroup}-${weekDay.toLowerCase()}`
        weekDayContainer.id = containerId;

        const scheduleContainer = document.createElement("div");
        scheduleContainer.classList.add("d-schedule-rows-container");
        weekDayContainer.appendChild(scheduleContainer);

        // add it to the document before calling the generator function
        weekDaysContainer.appendChild(weekDayContainer);

        // generate the first row 
        generateDailyScheduleRows(document.querySelector(`#${containerId} .d-schedule-rows-container`));

        // create the '+' button beneath the rows
        const addRowButtonContainer = document.createElement("div");
        addRowButtonContainer.classList.add("d-row", "d-justify-end", "d-margin-top-10");

        const addRowButton = document.createElement("button");
        addRowButton.classList.add("d-button", "d-add-schedule-point");
        addRowButton.type = "button";
        addRowButton.onclick = () => createDailyScheduleRow(document.querySelector(`#${containerId} .d-schedule-rows-container`));
        addRowButton.innerHTML = '+';

        addRowButtonContainer.appendChild(addRowButton);
        weekDayContainer.appendChild(addRowButtonContainer);
    });

    // show only the Monday tab
    showTab(`tab-group-${weekViewTabGroup}`, `${weekViewTabGroup}-mon`)
}

/**
 * Displays the popup to show the day schedule for a given month & day index
 * @param { {monthIndex: Number, dayIndex: Number} }  
 */
function showPopup({ monthIndex = -1, dayIndex = 0 }) {
    document.getElementById(POPUP_WINDOW_ID).classList.remove("d-hidden");
    const rowsContainer = document.querySelector(`#${POPUP_WINDOW_ID} .d-schedule-rows-container`);
    const storedSchedule = schedule.getDaySchedule({ monthIndex: monthIndex, dayIndex: dayIndex });
    generateDailyScheduleRows(rowsContainer, (storedSchedule && storedSchedule.length > 0) ? storedSchedule : undefined);

    let headerText;
    if (monthIndex == -1)
        headerText = `Day: ${dayIndex + 1}`;
    else
        headerText = `${monthIndexToName(monthIndex)} ${dayIndex}`;

    const editorHeader = document.querySelector(`#${POPUP_WINDOW_ID} #popup-header`);
    editorHeader.innerHTML = headerText;

    document.querySelector(`#${POPUP_WINDOW_ID} #d-popup-monthIndex`).value = monthIndex;
    document.querySelector(`#${POPUP_WINDOW_ID} #d-popup-dayIndex`).value = dayIndex;
}

/**
 * Closes the popup
 * @param { Boolean } promptConfirm Whether to ask the user for confirmation for closing
 * @returns 
 */
function closePopup(promptConfirm = true) {
    // check if the confirmation should be asked, then, if user cancels the closing, skip the closing process
    if (promptConfirm && !confirm("Close the editor without saving?"))
        return;

    document.getElementById(POPUP_WINDOW_ID).classList.add("d-hidden");
}

/**
 * Extracts the schedule from the popup, and closes it
 */
function savePopupSchedule() {
    const monthIndex = document.querySelector(`#${POPUP_WINDOW_ID} #d-popup-monthIndex`).value;
    const dayIndex = document.querySelector(`#${POPUP_WINDOW_ID} #d-popup-dayIndex`).value;

    if (saveDailySchedule(`#${POPUP_WINDOW_ID}`, monthIndex, dayIndex))
        closePopup(promptConfirm = false);
}

/**
 * Fetches the schedule from the host, saves it the `schedule` global variable, and updates the document
 */
function loadSchedule() {
    const deviceSchedule = fetchSchedule();

    if (deviceSchedule.scheduleType === 'custom') {
        document.getElementById('startTime').value = new Date(deviceSchedule.startTime * 1_000).toISOString().slice(0, 19);
        document.getElementById('loopTime').value = deviceSchedule.loopTime;
        schedule.setCustomScheduleParams(deviceSchedule.startTime, deviceSchedule.loopTime);
    }

    schedule.setScheduleType(deviceSchedule.scheduleType);
    schedule.setSchedule(deviceSchedule.schedule);

    generateDailyScheduleRows(
        document.querySelector(`.d-daily-view .d-schedule-rows-container`),
        (deviceSchedule.scheduleType === "daily") ? deviceSchedule.schedule : undefined
    );

    generateDailyScheduleRows(
        document.querySelector(`.d-custom-view .d-schedule-rows-container`),
        (deviceSchedule.scheduleType === "custom") ? deviceSchedule.schedule : undefined
    );

    if (deviceSchedule.scheduleType === 'weekly') {
        if (WEEK_DAYS.length != deviceSchedule.schedule.length) {
            throw new RangeError(`Device schedule does not have the correct amount of days for a weekly schedule type. Device days: ${deviceSchedule.schedule.length}; Required days: ${WEEK_DAYS.length}`);
        }

        WEEK_DAYS.forEach((weekDay, index) => {
            const location = document.querySelector(
                `#${weekViewTabGroup}-${weekDay.toLowerCase()} .d-schedule-rows-container`
            );
            const data = deviceSchedule.schedule[index];

            generateDailyScheduleRows(location, data);
        });
    }

    document.getElementById("schedule-loop-type").value = deviceSchedule.scheduleType;

    onScheduleTypeSelectChanged();
}

/**
 * Extracts a base schedule from the div with class `scheduleContainer`,
 * and stores it in the `schedule` via `setDaySchedule()`.
 * If store fails, updates the document to highlight the rows with incorrect values,
 * and alerts the user
 * 
 * @param { String } scheduleContainer 
 * A HTML element inside of which the desired schedule is located.
 * Should include both the rows & the rows' container.
 * Identifier should include the prefix: '.', '#' or nothing.
 * 
 * @returns { Boolean } Whether the saving was successful or not: true in case of success, false otherwise.
 */
function saveDailySchedule(scheduleContainer, monthIndex, dayIndex) {
    try {
        const userSchedule = getScheduleFromBaseRows(`${scheduleContainer} .d-schedule-row`);

        // re-generate the rows with the sorted schedule
        generateDailyScheduleRows(document.querySelector(`${scheduleContainer} .d-schedule-rows-container`), userSchedule);

        schedule.setDaySchedule({ schedule: userSchedule, monthIndex: monthIndex, dayIndex: dayIndex });

        return true;
    }
    catch (error) {
        if (!(error instanceof InvalidScheduleException))
            throw error;

        /** @type { [{ index: Number, reason: InvalidReason }] } */
        const invalidPoints = error.params.invalidPoints;
        const invalidReasons = new Set();
        const invalidIndexes = new Set();
        invalidPoints.forEach(invalidPoint => {
            invalidIndexes.add(invalidPoint.index);
            invalidReasons.add(invalidPoint.reason);
        });

        const scheduleRows = document.querySelectorAll(`${scheduleContainer} .d-schedule-row`);
        invalidIndexes.forEach((_, invalidIndex) => scheduleRows[invalidIndex].classList.add('d-border-wrong'));

        const invalidReasonsString =
            Array.from(
                invalidReasons,
                reason => `  - ${InvalidReason.toString(reason)};`
            ).join("\n");

        alert(
            'Some issues have been found with the schedule:\n' +
            `${invalidReasonsString}\n\n` +
            'Invalid items have been marked with red.'
        );

        return false;
    }
}

/**
 * Same as `saveDailySchedule()` but only for the weekly view
 */
function saveWeeklySchedule() {
    try {
        /** @type { [[SchedulePoint]] } */
        const userSchedule = [];
        WEEK_DAYS.forEach((weekDay) => {
            userSchedule.push(
                getScheduleFromBaseRows(`#weekly-schedule-${weekDay.toLowerCase()} .d-schedule-row`)
            );
        });

        // re-generate the rows with the sorted schedule
        WEEK_DAYS.forEach((weekDay, index) => {
            const location = document.querySelector(
                `#weekly-schedule-${weekDay.toLowerCase()} .d-schedule-rows-container`
            );
            const data = userSchedule[index];

            generateDailyScheduleRows(location, data);
        });

        // try to save the schedule, even if it is invalid
        schedule.setSchedule(userSchedule);

        return true;
    }
    catch (error) {
        if (!(error instanceof InvalidScheduleException))
            throw error;

        /** @type { [{dayIndex: Number, invalidPoints: [{ index: Number, reason: InvalidReason }]}] } */
        const invalidDays = error.params.invalidDays;
        const invalidReasons = new Set();
        const invalidIndexes = Array.from({ length: 7 }, () => new Set());
        invalidDays.forEach(invalidDay => {
            invalidDay.invalidPoints.forEach((invalidPoint) => {
                invalidIndexes[invalidDay.dayIndex].add(invalidPoint.index);
                invalidReasons.add(invalidPoint.reason);
            });
        });

        WEEK_DAYS.forEach((weekDay, weekDayIndex) => {
            const weekRows = document.querySelectorAll(`#weekly-schedule-${weekDay.toLowerCase()} .d-schedule-row`);
            invalidIndexes[weekDayIndex].forEach(pointIndex => {
                weekRows[pointIndex].classList.add('d-border-wrong');
            });
        });

        const weekDayTabButtons = document.querySelectorAll('.tab-group-weekly-schedule.tabs button');
        invalidIndexes.forEach((dayInvalidPoints, index) => {
            if (dayInvalidPoints.size !== 0)
                weekDayTabButtons[index].classList.add('d-border-wrong-tab');
            else
                weekDayTabButtons[index].classList.remove('d-border-wrong-tab');

        })

        const invalidReasonsString =
            Array.from(
                invalidReasons,
                reason => `  - ${InvalidReason.toString(reason)};`
            ).join("\n");

        alert(
            'Some issues have been found with the schedule:\n' +
            `${invalidReasonsString}\n\n` +
            'Invalid items have been marked with red.'
        );

        return false;
    }
}

/**
 * Extracts the schedule from the currently selected schedule, and stores it in the `schedule`.
 * If store fails, updates the document to highlight the rows with incorrect values, and alerts the user.
 */
function saveScheduleData() {
    const selectedType = getScheduleTypeSelection();
    if (schedule.scheduleType != selectedType)
        throw new EvalError(`Selected schedule and internal schedule types don't match. Selected: ${selectedType}; internal: ${schedule.scheduleType}`);

    switch (selectedType) {
        case "daily":
            return saveDailySchedule('.d-daily-view');

        case "weekly":
            return saveWeeklySchedule();

        case "custom":
            schedule.setCustomScheduleParams(
                Date.parse(`${document.getElementById('startTime').value}Z`) / 1_000,
                Number(document.getElementById('loopTime').value),
            );
            return saveDailySchedule('.d-custom-view');

        default:
            throw new TypeError(`Unsupported opperation for schedule type: ${selectedType}`);
    }
}

function uploadScheduleClick() {
    if (!saveScheduleData())
        return;

    console.log("Uploading schedule to the host...");
    console.log(schedule.serialize());
}
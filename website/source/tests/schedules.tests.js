// ---- Assume `schedule.js` is imported in the html file ----
/**
 * 
 * @param { Number } offset 
 * @param { ActionType } actionType 
 * @param { Number } duration 
 * @returns 
 */
function createPoint(offset, actionType, duration = 1) {
    return new SchedulePoint(offset, new Action(actionType), duration);
}

/**
 * @param { SchedulePoint } pointA 
 * @param { SchedulePoint } pointB 
 */
function isEqualSchedulePoints(pointA, pointB) {
    if (pointA.timeOffset !== pointB.timeOffset)
        return false;

    const actionTypeA = (pointA.action instanceof Action)
        ? pointA.action.type
        : pointA.action;
    const actionTypeB = (pointB.action instanceof Action)
        ? pointB.action.type
        : pointB.action;

    if (actionTypeA !== actionTypeB)
        return false;

    if (pointA.duration !== pointB.duration)
        return false;

    return true;
}

/**
 * 
 * @param { [SchedulePoint] } arrayA 
 * @param { [SchedulePoint] } arrayB 
 * @returns 
 */
function isEqualSchedulePointsArray(arrayA, arrayB) {
    if (!Array.isArray(arrayA) || !Array.isArray(arrayB))
        return false;

    if (arrayA.length !== arrayB.length)
        return false;

    for (let i = 0; i < arrayA.length; ++i)
        if (!isEqualSchedulePoints(arrayA[i], arrayB[i]))
            return false;

    return true;
}

// Minimal assert function for test results
function assert(actual, expected, message) {
    const stringActual = JSON.stringify(actual);
    const stringExpected = JSON.stringify(expected);

    if (stringActual !== stringExpected)
        throw new Error(
            `${message || 'Assertion failed'} -> Expected: ${stringExpected} | Actual: ${stringActual}`
        );
}

/**
 * 
 * @param { String } testName 
 * @param { CustomSchedule | DaySchedule } checkerSchedule a schedule to check with
 * @param { [SchedulePoint] } schedulePoints 
 * @param { [{index: Number, reason: InvalidReason}] } expectedResults 
 */
function assertBaseSchedule(testName, checkerSchedule, schedulePoints, expectedResults) {
    let actualResults = checkerSchedule.getIndexesOfInvalidItems(schedulePoints);
    actualResults = actualResults.sort((a, b) => a.index - b.index);
    expectedResults = expectedResults.sort((a, b) => a.index - b.index);

    const pass =
        actualResults.length === expectedResults.length &&
        expectedResults.every((expected, i) => {
            const actual = actualResults[i];
            return actual.index === expected.index && actual.reason === expected.reason;
        });

    if (!pass)
        throw new Error(`${testName} -> Failed\nExpected:\n${JSON.stringify(expectedResults, null, 2)}\nGot:\n${JSON.stringify(actualResults, null, 2)}`);
}

// --- Test runners ---
function test(name, fn) {
    try {
        console.log(`Testing ${name}...`);
        fn();
        console.log(`[+] ${name} -> Passed`);
    } catch (e) {
        console.error(`[x] ${name} -> Failed: ${e.message}`);
    } finally {
        console.log('');
    }
}

function testNoThrow(name, fn) {
    try {
        fn();
    } catch (e) {
        console.error(`[x] ${name} -> Failed: An exception has been thrown: ${e.message}`);
        throw new Error(name);
    }
}

function testForThrow(name, fn) {
    passed = true;
    try {
        fn();
        console.error(`[x] ${name} -> Failed: No exception has been thrown`);
        passed = false;
    } catch (e) {
    }

    if (!passed)
        throw new Error(name);
}

// --- Tests ---
test('Action class', () => {
    const action1 = new Action(ActionType.OPEN_RELAY);
    const action2 = new Action(ActionType.CLOSE_RELAY);

    assert(action1.type, ActionType.OPEN_RELAY);
    assert(action2.type, ActionType.CLOSE_RELAY);
    assert(action1.getInverse(), action2.type);

    testForThrow('Invalid ActionType value #1', () => { new Action(-1); });
    testForThrow('Invalid ActionType value #2', () => { new Action(2); });
    testForThrow('Invalid ActionType value #3', () => { new Action([]); });
    testForThrow('Invalid ActionType value #4', () => { new Action(); });
});

test('SchedulePoint class', () => {
    const point = createPoint(0, ActionType.CLOSE_RELAY, 1);
    createPoint(0, ActionType.CLOSE_RELAY, 1);

    assert(point.timeOffset, 0, 'Time offset assertion');
    assert(point.action.type, ActionType.CLOSE_RELAY, 'Action type assertion');
    assert(point.action.getInverse(), ActionType.OPEN_RELAY, 'Inverse action type assertion');
    assert(point.duration, 1, 'Duration assertion');

    testForThrow('Constructor timeOffset', () => { createPoint(-1, ActionType.CLOSE_RELAY, 1); });
    testForThrow('Constructor action #1', () => { createPoint(0, -1, 1); });
    testForThrow('Constructor action #2', () => { createPoint(0, new Object(), 1); });
    testForThrow('Constructor duration', () => { createPoint(0, ActionType.CLOSE_RELAY, -1); });
});

test('CustomSchedule construction', () => {
    testNoThrow('Default constructor', () => new CustomSchedule());
    testNoThrow('Constructor schedule points #1', () => new CustomSchedule(0, 1, [createPoint(0, ActionType.OPEN_RELAY, 0)]));
    testNoThrow('Constructor schedule points #2', () => new CustomSchedule(0, 1, [{ timeOffset: 0, action: ActionType.OPEN_RELAY, duration: 0 }]));

    testForThrow('Constructor startTime', () => new CustomSchedule(-1, 1, []));
    testForThrow('Constructor loopTime', () => new CustomSchedule(0, 0, []));
    testForThrow('Constructor schedule points #1', () => new CustomSchedule(0, 1, null));
    testForThrow('Constructor schedule points #2', () => new CustomSchedule(0, 1, 1));
    testForThrow('Constructor schedule points #3', () => new CustomSchedule(0, 1, [{}]));
    testForThrow('Constructor schedule points #4', () => new CustomSchedule(0, 1, [{ garbage: 3 }]));
    testForThrow('Constructor schedule points #2', () => new CustomSchedule(0, 1, [{ time_offset: 0, action_: ActionType.OPEN_RELAY, duration_: 0 }]));
});

test('DaySchedule functionality', () => {
    testNoThrow('Default constructor', () => new DaySchedule());
    testNoThrow('Constructor #1', () => new DaySchedule([]));
    testNoThrow('Constructor #2', () => new DaySchedule([{ timeOffset: 0, action: ActionType.OPEN_RELAY, duration: 0 }]));
    testNoThrow('Constructor #3', () => new DaySchedule([createPoint(0, ActionType.OPEN_RELAY, 0)]));

    const daySchedule = new DaySchedule();

    assertBaseSchedule(
        "Unordered detection",
        daySchedule,
        [
            createPoint(10, ActionType.CLOSE_RELAY, 0),
            createPoint(5, ActionType.OPEN_RELAY, 0),
            createPoint(20, ActionType.CLOSE_RELAY, 0)
        ],
        [
            { index: 0, reason: InvalidReason.UNORDERED },
        ]
    );

    assertBaseSchedule(
        "Overlap detection",
        daySchedule,
        [
            createPoint(0, ActionType.CLOSE_RELAY, 10),
            createPoint(5, ActionType.OPEN_RELAY, 3), // overlaps with previous
            createPoint(20, ActionType.CLOSE_RELAY, 0)
        ],
        [
            { index: 0, reason: InvalidReason.OVERLAP },
            { index: 1, reason: InvalidReason.OVERLAP }
        ]
    );

    assertBaseSchedule(
        "Duplicate timeOffset detection",
        daySchedule,
        [
            createPoint(10, ActionType.CLOSE_RELAY, 0),
            createPoint(10, ActionType.OPEN_RELAY, 1),
            createPoint(30, ActionType.CLOSE_RELAY, 1)
        ],
        [
            { index: 0, reason: InvalidReason.DUPLICATE },
            { index: 1, reason: InvalidReason.DUPLICATE }
        ]
    );

    assertBaseSchedule(
        "Loop overflow detection",
        daySchedule,
        [
            createPoint(0, ActionType.CLOSE_RELAY, 1),
            createPoint(87_000, ActionType.OPEN_RELAY, 1) // overflows loopTime of 60
        ],
        [
            { index: 1, reason: InvalidReason.LOOP_OVERFLOW }
        ]
    )

    assertBaseSchedule(
        "Multiple invalidities",
        daySchedule,
        [
            createPoint(20, ActionType.CLOSE_RELAY, 0), // unordered
            createPoint(10, ActionType.CLOSE_RELAY, 10),
            createPoint(15, ActionType.OPEN_RELAY, 5), // overlaps
            createPoint(15, ActionType.CLOSE_RELAY, 0), // duplicate
            createPoint(87_000, ActionType.OPEN_RELAY, 1)  // overflow
        ],
        [
            { index: 0, reason: InvalidReason.UNORDERED },
            { index: 1, reason: InvalidReason.OVERLAP },
            { index: 2, reason: InvalidReason.OVERLAP },
            { index: 2, reason: InvalidReason.DUPLICATE },
            { index: 3, reason: InvalidReason.OVERLAP },
            { index: 3, reason: InvalidReason.DUPLICATE },
            { index: 4, reason: InvalidReason.LOOP_OVERFLOW }
        ]
    )

    assertBaseSchedule(
        "Valid schedule",
        daySchedule,
        [
            createPoint(10, ActionType.CLOSE_RELAY, 3),
            createPoint(15, ActionType.OPEN_RELAY, 5),
            createPoint(25, ActionType.CLOSE_RELAY, 0),
            createPoint(70, ActionType.OPEN_RELAY, 1),
            createPoint(90, ActionType.CLOSE_RELAY, 1)
        ],
        []
    )
});

test('Weekly schedule', () => {
    testNoThrow('Constructor #1', () => {
        const schedule = new WeeklySchedule();
        const newSchedule = Array.from({ length: 7 }, (_, i) => [createPoint(i, ActionType.CLOSE_RELAY, 1)]);
        schedule.setSchedule(newSchedule);

        for (let i = 0; i < newSchedule.length; ++i) {
            if (isEqualSchedulePointsArray(schedule.getDaySchedule(i), newSchedule[i]))
                continue;

            console.error('Expected:', newSchedule[i], 'Got:', schedule.getDaySchedule(i));
            assert(true, false, `Weekly schedule day #${i}`);
        }
    });

    testNoThrow('Constructor #2', () => {
        const newSchedule = Array.from({ length: 7 }, (_, i) => [createPoint(i, ActionType.CLOSE_RELAY, 1)]);
        const schedule = new WeeklySchedule(newSchedule);

        for (let i = 0; i < newSchedule.length; ++i) {
            if (isEqualSchedulePointsArray(schedule.getDaySchedule(i), newSchedule[i]))
                continue;

            console.error('Expected:', newSchedule[i], 'Got:', schedule.getDaySchedule(i));
            assert(true, false, `Weekly schedule day #${i}`);
        }
    });

    testNoThrow('Constructor #3', () => {
        const newSchedule = Array.from({ length: 7 }, (_, i) => [{ timeOffset: i, action: ActionType.CLOSE_RELAY, duration: 1 }]);
        const schedule = new WeeklySchedule(newSchedule);

        for (let i = 0; i < newSchedule.length; ++i) {
            if (isEqualSchedulePointsArray(schedule.getDaySchedule(i), newSchedule[i]))
                continue;

            console.error('Expected:', newSchedule[i], 'Got:', schedule.getDaySchedule(i));
            assert(true, false, `Weekly schedule day #${i}`);
        }
    });
});

test('Monthly schedule constructor', () => {
    const schedule = new MonthlySchedule();
    const newSchedule = Array.from({ length: 31 }, (_, i) => [createPoint(i, ActionType.CLOSE_RELAY, 1)]);
    schedule.setSchedule(newSchedule);

    for (let i = 0; i < newSchedule.length; ++i)
        assert(schedule.getDaySchedule(i), newSchedule[i], `Monthly schedule day #${i}`);
});


function updateTime() {
    fetch('/get_time')
        .then(response => response.text())
        .then(timeString => {
            const date = new Date(timeString);
            const formattedTime = date.toLocaleTimeString([], { hour: '2-digit', minute: '2-digit', second: '2-digit' });
            document.getElementById("timeDisplay").textContent = "System Time: " + formattedTime;
        })
        .catch(error => {
            console.error("Error fetching time:", error);
            document.getElementById("timeDisplay").textContent = "System Time: Error loading time";
        });
}

function fetchSchedule() {
    return getSchedulePlacholder();
    // fetch('/get_schedule')
    //     .then(response => response.arrayBuffer())
    //     .then(data => {
    //         let schedule = decodeScheduleToJson(data);
    //         console.log(schedule);
    //         // const tableBody = document.getElementById("scheduleTable").getElementsByTagName("tbody")[0];
    //         // tableBody.innerHTML = ""; // Clear existing rows
    //         // data.schedule_items.forEach(item => {
    //         //     const hour = Math.floor(item.time / 60).toString().padStart(2, '0');
    //         //     const minute = (item.time % 60).toString().padStart(2, '0');
    //         //     addScheduleRow(`${hour}:${minute}`, item.type);
    //         // });
    //     })
    //     .catch(error => console.error('Error fetching schedule:', error));
}

function decodeScheduleToJson(rawBuffer) {
    const dataView = new DataView(rawBuffer);
    let result = {};

    let scheduleType = dataView.getUint8(0);
    switch (scheduleType) {
        case 1:
            result.scheduleType = 'daily';
            result.schedule = decodeDailySchedule(new DataView(dataView.buffer, 1));

        case 2:
            // return 'weeky';
            break;

        case 3:
            result.scheduleType = 'monthly';
            result.schedule = decodeMonthlySchedule(new DataView(dataView.buffer, 1));

        case 4:
            // return 'yearly';
            break;

        case 5:
            // return 'custom';
            break;

        default:
            return null;
    }

    return result;
}

/**
 * 
 * @param {Dataview} dataView 
 * @returns 
 */
function decodeDailySchedule(dataView) {
    let itemsCount = dataView.getUint32(0, true);

    let result = [];
    let byteOffset = 4;
    for (let index = 0; index < itemsCount; ++index) {
        result.push({
            daySecond: dataView.getUint32(byteOffset, true),
            fireAction: dataView.getUint8(byteOffset + 4)
        });
        byteOffset += 5;
    }

    return { schedule: result, bytesRead: byteOffset };
}

/**
 * @param {DataView} dataView 
 */
function decodeMonthlySchedule(dataView) {
    let result = [];
    let byteOffset = 0;

    for (let index = 0; index < 31; index++) {
        let parseResult = decodeDailySchedule(new DataView(dataView.buffer, dataView.byteOffset + byteOffset));
        result.push(parseResult.schedule);
        byteOffset += parseResult.bytesRead;
    }

    return result;
}

function getSchedulePlacholder() {
    return {
        scheduleType: "custom",
        startTime: 10,
        loopTime: 3000,
        schedule: [{
            "timeOffset": 2730,
            "action": 0,
            "duration": 0,
        }]
    }

    return {
        scheduleType: "daily",
        schedule: [{
            "timeOffset": 2730,
            "action": 0
        }]
    }

    return {
        scheduleType: "monthly",
        schedule: [
            [
                {
                    "timeOffset": 2730,
                    "action": 0
                },
                {
                    "timeOffset": 5078,
                    "action": 1
                },
                {
                    "timeOffset": 5370,
                    "action": 1
                },
                {
                    "timeOffset": 5871,
                    "action": 1
                },
                {
                    "timeOffset": 7363,
                    "action": 0
                }
            ],
            [
                {
                    "timeOffset": 1896,
                    "action": 0
                },
                {
                    "timeOffset": 3739,
                    "action": 0
                },
                {
                    "timeOffset": 5514,
                    "action": 1
                },
                {
                    "timeOffset": 7998,
                    "action": 1
                },
                {
                    "timeOffset": 9685,
                    "action": 1
                },
                {
                    "timeOffset": 12399,
                    "action": 1
                },
                {
                    "timeOffset": 13731,
                    "action": 0
                },
                {
                    "timeOffset": 14311,
                    "action": 1
                }
            ],
            [
                {
                    "timeOffset": 941,
                    "action": 1
                },
                {
                    "timeOffset": 2057,
                    "action": 1
                },
                {
                    "timeOffset": 4563,
                    "action": 0
                },
                {
                    "timeOffset": 5603,
                    "action": 0
                },
                {
                    "timeOffset": 8431,
                    "action": 0
                }
            ],
            [
                {
                    "timeOffset": 2350,
                    "action": 0
                },
                {
                    "timeOffset": 5193,
                    "action": 1
                },
                {
                    "timeOffset": 7266,
                    "action": 0
                },
                {
                    "timeOffset": 7557,
                    "action": 1
                },
                {
                    "timeOffset": 7817,
                    "action": 0
                },
                {
                    "timeOffset": 9554,
                    "action": 0
                }
            ],
            [
                {
                    "timeOffset": 2517,
                    "action": 1
                },
                {
                    "timeOffset": 4271,
                    "action": 0
                },
                {
                    "timeOffset": 4425,
                    "action": 1
                },
                {
                    "timeOffset": 7206,
                    "action": 0
                },
                {
                    "timeOffset": 7866,
                    "action": 1
                },
                {
                    "timeOffset": 9901,
                    "action": 0
                },
                {
                    "timeOffset": 12507,
                    "action": 1
                },
                {
                    "timeOffset": 15069,
                    "action": 1
                },
                {
                    "timeOffset": 16621,
                    "action": 1
                }
            ],
            [
                {
                    "timeOffset": 2290,
                    "action": 1
                },
                {
                    "timeOffset": 2808,
                    "action": 0
                },
                {
                    "timeOffset": 6269,
                    "action": 1
                },
                {
                    "timeOffset": 6276,
                    "action": 0
                },
                {
                    "timeOffset": 8730,
                    "action": 0
                }
            ],
            [
                {
                    "timeOffset": 457,
                    "action": 1
                },
                {
                    "timeOffset": 2926,
                    "action": 1
                },
                {
                    "timeOffset": 4630,
                    "action": 1
                },
                {
                    "timeOffset": 4644,
                    "action": 0
                },
                {
                    "timeOffset": 7650,
                    "action": 1
                },
                {
                    "timeOffset": 8664,
                    "action": 1
                },
                {
                    "timeOffset": 9288,
                    "action": 1
                }
            ],
            [
                {
                    "timeOffset": 1754,
                    "action": 0
                },
                {
                    "timeOffset": 3295,
                    "action": 1
                },
                {
                    "timeOffset": 5558,
                    "action": 0
                },
                {
                    "timeOffset": 8696,
                    "action": 0
                },
                {
                    "timeOffset": 12168,
                    "action": 1
                },
                {
                    "timeOffset": 13419,
                    "action": 0
                }
            ],
            [
                {
                    "timeOffset": 3153,
                    "action": 1
                },
                {
                    "timeOffset": 6164,
                    "action": 1
                },
                {
                    "timeOffset": 8733,
                    "action": 0
                },
                {
                    "timeOffset": 8997,
                    "action": 1
                },
                {
                    "timeOffset": 12424,
                    "action": 0
                },
                {
                    "timeOffset": 14070,
                    "action": 0
                },
                {
                    "timeOffset": 17346,
                    "action": 1
                },
                {
                    "timeOffset": 19368,
                    "action": 0
                },
                {
                    "timeOffset": 22606,
                    "action": 0
                },
                {
                    "timeOffset": 24665,
                    "action": 1
                }
            ],
            [
                {
                    "timeOffset": 1928,
                    "action": 1
                },
                {
                    "timeOffset": 5014,
                    "action": 1
                },
                {
                    "timeOffset": 7878,
                    "action": 1
                },
                {
                    "timeOffset": 8165,
                    "action": 1
                },
                {
                    "timeOffset": 11516,
                    "action": 1
                }
            ],
            [
                {
                    "timeOffset": 579,
                    "action": 1
                },
                {
                    "timeOffset": 3495,
                    "action": 1
                },
                {
                    "timeOffset": 6484,
                    "action": 1
                },
                {
                    "timeOffset": 6894,
                    "action": 0
                },
                {
                    "timeOffset": 8441,
                    "action": 0
                },
                {
                    "timeOffset": 9985,
                    "action": 1
                },
                {
                    "timeOffset": 10719,
                    "action": 0
                },
                {
                    "timeOffset": 12214,
                    "action": 1
                }
            ],
            [
                {
                    "timeOffset": 3175,
                    "action": 1
                },
                {
                    "timeOffset": 6221,
                    "action": 1
                },
                {
                    "timeOffset": 6572,
                    "action": 1
                },
                {
                    "timeOffset": 8045,
                    "action": 1
                },
                {
                    "timeOffset": 9621,
                    "action": 1
                },
                {
                    "timeOffset": 10416,
                    "action": 1
                },
                {
                    "timeOffset": 12273,
                    "action": 1
                }
            ],
            [
                {
                    "timeOffset": 2138,
                    "action": 1
                },
                {
                    "timeOffset": 4755,
                    "action": 1
                },
                {
                    "timeOffset": 6103,
                    "action": 0
                },
                {
                    "timeOffset": 6550,
                    "action": 0
                },
                {
                    "timeOffset": 7582,
                    "action": 0
                },
                {
                    "timeOffset": 9832,
                    "action": 1
                },
                {
                    "timeOffset": 10809,
                    "action": 0
                },
                {
                    "timeOffset": 14377,
                    "action": 0
                },
                {
                    "timeOffset": 16458,
                    "action": 0
                },
                {
                    "timeOffset": 19666,
                    "action": 0
                }
            ],
            [
                {
                    "timeOffset": 1245,
                    "action": 0
                },
                {
                    "timeOffset": 2114,
                    "action": 1
                },
                {
                    "timeOffset": 3932,
                    "action": 1
                },
                {
                    "timeOffset": 4197,
                    "action": 1
                },
                {
                    "timeOffset": 5909,
                    "action": 0
                },
                {
                    "timeOffset": 7543,
                    "action": 1
                },
                {
                    "timeOffset": 10972,
                    "action": 0
                },
                {
                    "timeOffset": 11504,
                    "action": 0
                },
                {
                    "timeOffset": 12123,
                    "action": 0
                },
                {
                    "timeOffset": 12224,
                    "action": 1
                }
            ],
            [
                {
                    "timeOffset": 226,
                    "action": 0
                },
                {
                    "timeOffset": 1307,
                    "action": 1
                },
                {
                    "timeOffset": 2406,
                    "action": 1
                },
                {
                    "timeOffset": 4961,
                    "action": 0
                },
                {
                    "timeOffset": 6780,
                    "action": 1
                },
                {
                    "timeOffset": 7204,
                    "action": 1
                },
                {
                    "timeOffset": 7739,
                    "action": 0
                }
            ],
            [
                {
                    "timeOffset": 384,
                    "action": 0
                },
                {
                    "timeOffset": 1814,
                    "action": 1
                },
                {
                    "timeOffset": 4813,
                    "action": 0
                },
                {
                    "timeOffset": 8117,
                    "action": 1
                },
                {
                    "timeOffset": 10485,
                    "action": 0
                },
                {
                    "timeOffset": 11303,
                    "action": 0
                },
                {
                    "timeOffset": 13644,
                    "action": 0
                },
                {
                    "timeOffset": 16711,
                    "action": 1
                }
            ],
            [
                {
                    "timeOffset": 1041,
                    "action": 1
                },
                {
                    "timeOffset": 1517,
                    "action": 1
                },
                {
                    "timeOffset": 3295,
                    "action": 0
                },
                {
                    "timeOffset": 5750,
                    "action": 0
                },
                {
                    "timeOffset": 7546,
                    "action": 1
                },
                {
                    "timeOffset": 11036,
                    "action": 1
                },
                {
                    "timeOffset": 12406,
                    "action": 0
                },
                {
                    "timeOffset": 15571,
                    "action": 1
                }
            ],
            [
                {
                    "timeOffset": 2558,
                    "action": 1
                },
                {
                    "timeOffset": 5481,
                    "action": 1
                },
                {
                    "timeOffset": 9064,
                    "action": 0
                },
                {
                    "timeOffset": 11978,
                    "action": 0
                },
                {
                    "timeOffset": 15010,
                    "action": 1
                },
                {
                    "timeOffset": 15925,
                    "action": 0
                },
                {
                    "timeOffset": 19022,
                    "action": 1
                },
                {
                    "timeOffset": 19826,
                    "action": 1
                }
            ],
            [
                {
                    "timeOffset": 394,
                    "action": 1
                },
                {
                    "timeOffset": 2484,
                    "action": 0
                },
                {
                    "timeOffset": 4037,
                    "action": 1
                },
                {
                    "timeOffset": 5078,
                    "action": 0
                },
                {
                    "timeOffset": 5796,
                    "action": 1
                },
                {
                    "timeOffset": 7120,
                    "action": 0
                },
                {
                    "timeOffset": 9385,
                    "action": 1
                }
            ],
            [
                {
                    "timeOffset": 3517,
                    "action": 0
                },
                {
                    "timeOffset": 4858,
                    "action": 0
                },
                {
                    "timeOffset": 7909,
                    "action": 0
                },
                {
                    "timeOffset": 8328,
                    "action": 1
                },
                {
                    "timeOffset": 10397,
                    "action": 1
                },
                {
                    "timeOffset": 11853,
                    "action": 0
                },
                {
                    "timeOffset": 14587,
                    "action": 0
                },
                {
                    "timeOffset": 14896,
                    "action": 0
                },
                {
                    "timeOffset": 15619,
                    "action": 0
                },
                {
                    "timeOffset": 15827,
                    "action": 1
                }
            ],
            [
                {
                    "timeOffset": 1244,
                    "action": 1
                },
                {
                    "timeOffset": 2136,
                    "action": 1
                },
                {
                    "timeOffset": 3859,
                    "action": 1
                },
                {
                    "timeOffset": 5635,
                    "action": 0
                },
                {
                    "timeOffset": 5762,
                    "action": 0
                }
            ],
            [
                {
                    "timeOffset": 2665,
                    "action": 1
                },
                {
                    "timeOffset": 5923,
                    "action": 0
                },
                {
                    "timeOffset": 6988,
                    "action": 0
                },
                {
                    "timeOffset": 7197,
                    "action": 1
                },
                {
                    "timeOffset": 7645,
                    "action": 1
                },
                {
                    "timeOffset": 10727,
                    "action": 1
                },
                {
                    "timeOffset": 11852,
                    "action": 0
                },
                {
                    "timeOffset": 14003,
                    "action": 1
                }
            ],
            [
                {
                    "timeOffset": 1966,
                    "action": 0
                },
                {
                    "timeOffset": 2301,
                    "action": 0
                },
                {
                    "timeOffset": 2997,
                    "action": 1
                },
                {
                    "timeOffset": 3637,
                    "action": 1
                },
                {
                    "timeOffset": 5031,
                    "action": 0
                }
            ],
            [
                {
                    "timeOffset": 2850,
                    "action": 0
                },
                {
                    "timeOffset": 3941,
                    "action": 0
                },
                {
                    "timeOffset": 4281,
                    "action": 0
                },
                {
                    "timeOffset": 7065,
                    "action": 1
                },
                {
                    "timeOffset": 8588,
                    "action": 0
                }
            ],
            [
                {
                    "timeOffset": 2191,
                    "action": 0
                },
                {
                    "timeOffset": 4190,
                    "action": 0
                },
                {
                    "timeOffset": 6896,
                    "action": 0
                },
                {
                    "timeOffset": 10376,
                    "action": 0
                },
                {
                    "timeOffset": 12294,
                    "action": 1
                },
                {
                    "timeOffset": 13098,
                    "action": 0
                },
                {
                    "timeOffset": 16105,
                    "action": 0
                },
                {
                    "timeOffset": 18978,
                    "action": 1
                },
                {
                    "timeOffset": 21961,
                    "action": 0
                }
            ],
            [
                {
                    "timeOffset": 2212,
                    "action": 1
                },
                {
                    "timeOffset": 4439,
                    "action": 1
                },
                {
                    "timeOffset": 6718,
                    "action": 1
                },
                {
                    "timeOffset": 8317,
                    "action": 0
                },
                {
                    "timeOffset": 10970,
                    "action": 1
                },
                {
                    "timeOffset": 14333,
                    "action": 0
                },
                {
                    "timeOffset": 17362,
                    "action": 1
                },
                {
                    "timeOffset": 19572,
                    "action": 1
                },
                {
                    "timeOffset": 20915,
                    "action": 0
                },
                {
                    "timeOffset": 21015,
                    "action": 0
                }
            ],
            [
                {
                    "timeOffset": 1001,
                    "action": 1
                },
                {
                    "timeOffset": 1763,
                    "action": 0
                },
                {
                    "timeOffset": 4162,
                    "action": 0
                },
                {
                    "timeOffset": 6015,
                    "action": 0
                },
                {
                    "timeOffset": 6196,
                    "action": 1
                },
                {
                    "timeOffset": 7417,
                    "action": 1
                }
            ],
            [
                {
                    "timeOffset": 3160,
                    "action": 0
                },
                {
                    "timeOffset": 4956,
                    "action": 1
                },
                {
                    "timeOffset": 6874,
                    "action": 1
                },
                {
                    "timeOffset": 8315,
                    "action": 0
                },
                {
                    "timeOffset": 9724,
                    "action": 1
                }
            ],
            [
                {
                    "timeOffset": 3380,
                    "action": 1
                },
                {
                    "timeOffset": 4975,
                    "action": 0
                },
                {
                    "timeOffset": 6309,
                    "action": 1
                },
                {
                    "timeOffset": 8021,
                    "action": 0
                },
                {
                    "timeOffset": 9734,
                    "action": 1
                },
                {
                    "timeOffset": 13223,
                    "action": 1
                },
                {
                    "timeOffset": 13371,
                    "action": 1
                },
                {
                    "timeOffset": 13476,
                    "action": 0
                },
                {
                    "timeOffset": 17017,
                    "action": 1
                }
            ],
            [
                {
                    "timeOffset": 1567,
                    "action": 1
                },
                {
                    "timeOffset": 3240,
                    "action": 1
                },
                {
                    "timeOffset": 4273,
                    "action": 1
                },
                {
                    "timeOffset": 6604,
                    "action": 1
                },
                {
                    "timeOffset": 8785,
                    "action": 1
                },
                {
                    "timeOffset": 10111,
                    "action": 1
                }
            ],
            [
                {
                    "timeOffset": 3487,
                    "action": 1
                },
                {
                    "timeOffset": 6777,
                    "action": 1
                },
                {
                    "timeOffset": 6868,
                    "action": 0
                },
                {
                    "timeOffset": 9240,
                    "action": 1
                },
                {
                    "timeOffset": 12402,
                    "action": 0
                }
            ]
        ]
    }
}

// function submitSchedule(scheduleFormId) {
//     const form = document.getElementById(scheduleFormId);
//     const times = Array.from(form.querySelectorAll("input[name='time[]']")).map(input => input.value);
//     const types = Array.from(form.querySelectorAll("select[name='type[]']")).map(select => select.value);

//     const schedule_items = times.map((time, index) => {
//         const [hour, minute] = time.split(":").map(Number);
//         return {
//             time: hour * 60 + minute,
//             type: types[index] === "single" ? 1 : types[index] === "double" ? 2 : 3
//         };
//     });

//     const jsonData = {
//         schedule_items: schedule_items
//     };

//     console.log(JSON.stringify(jsonData, null, 2));

//     fetch('/update_schedule', {
//         method: 'POST',
//         headers: {
//             'Content-Type': 'application/json'
//         },
//         body: JSON.stringify(jsonData)
//     })
//         .then(response => response.text())
//         .then(data => console.log('Server Response:', data))
//         .catch(error => console.error('Error:', error));
// }

// function fetchSchedule() {
//     fetch('/get_schedule')
//         .then(response => response.json())
//         .then(data => {
//             const tableBody = document.getElementById("scheduleTable").getElementsByTagName("tbody")[0];
//             tableBody.innerHTML = ""; // Clear existing rows
//             data.schedule_items.forEach(item => {
//                 const hour = Math.floor(item.time / 60).toString().padStart(2, '0');
//                 const minute = (item.time % 60).toString().padStart(2, '0');
//                 addScheduleRow(`${hour}:${minute}`, item.type);
//             });
//         })
//         .catch(error => console.error('Error fetching schedule:', error));
// }

// function submitSettings() {
//     if (!confirm("Wi-Fi will restart after settings are applied. Do you want to proceed?")) {
//         return;
//     }

//     const form = document.getElementById("settingsForm");
//     const jsonData = {
//         wifi_ssid: form.wifi_ssid.value,
//         wifi_password: form.wifi_password.value,
//         remote_wifi_ssid: form.remote_wifi_ssid.value,
//         remote_wifi_password: form.remote_wifi_password.value,
//         timezone: form.timezone.value
//     };

//     console.log(JSON.stringify(jsonData, null, 2));

//     fetch('/update_settings', {
//         method: 'POST',
//         headers: {
//             'Content-Type': 'application/json'
//         },
//         body: JSON.stringify(jsonData)
//     })
//         .then(response => {
//             if (!response.ok) {
//                 throw new Error('Failed to update settings');
//             }
//             return response.text();
//         })
//         .then(data => console.log('Server Response:', data))
//         .catch(error => {
//             console.error('Error:', error);
//             alert('Error updating settings: ' + error.message);
//         });
// }

// function fetchSettings() {
//     fetch('/get_settings')
//         .then(response => response.json())
//         .then(data => {
//             document.getElementById("wifi_ssid").value = data.wifi_ssid || "";
//             document.getElementById("wifi_password").value = data.wifi_password || "";
//             document.getElementById("remote_wifi_ssid").value = data.remote_wifi_ssid || "";
//             document.getElementById("remote_wifi_password").value = data.remote_wifi_password || "";
//             document.getElementById("timezone").value = data.timezone || "";
//         })
//         .catch(error => {
//             console.error('Error fetching settings:', error);
//             // alert('Error fetching settings: ' + error.message);
//         });
// }
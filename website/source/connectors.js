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
        scheduleType: "monthly",
        schedule: [
            [
                {
                    "daySecond": 2730,
                    "fireAction": 0
                },
                {
                    "daySecond": 5078,
                    "fireAction": 1
                },
                {
                    "daySecond": 5370,
                    "fireAction": 1
                },
                {
                    "daySecond": 5871,
                    "fireAction": 1
                },
                {
                    "daySecond": 7363,
                    "fireAction": 0
                }
            ],
            [
                {
                    "daySecond": 1896,
                    "fireAction": 0
                },
                {
                    "daySecond": 3739,
                    "fireAction": 0
                },
                {
                    "daySecond": 5514,
                    "fireAction": 1
                },
                {
                    "daySecond": 7998,
                    "fireAction": 1
                },
                {
                    "daySecond": 9685,
                    "fireAction": 1
                },
                {
                    "daySecond": 12399,
                    "fireAction": 1
                },
                {
                    "daySecond": 13731,
                    "fireAction": 0
                },
                {
                    "daySecond": 14311,
                    "fireAction": 1
                }
            ],
            [
                {
                    "daySecond": 941,
                    "fireAction": 1
                },
                {
                    "daySecond": 2057,
                    "fireAction": 1
                },
                {
                    "daySecond": 4563,
                    "fireAction": 0
                },
                {
                    "daySecond": 5603,
                    "fireAction": 0
                },
                {
                    "daySecond": 8431,
                    "fireAction": 0
                }
            ],
            [
                {
                    "daySecond": 2350,
                    "fireAction": 0
                },
                {
                    "daySecond": 5193,
                    "fireAction": 1
                },
                {
                    "daySecond": 7266,
                    "fireAction": 0
                },
                {
                    "daySecond": 7557,
                    "fireAction": 1
                },
                {
                    "daySecond": 7817,
                    "fireAction": 0
                },
                {
                    "daySecond": 9554,
                    "fireAction": 0
                }
            ],
            [
                {
                    "daySecond": 2517,
                    "fireAction": 1
                },
                {
                    "daySecond": 4271,
                    "fireAction": 0
                },
                {
                    "daySecond": 4425,
                    "fireAction": 1
                },
                {
                    "daySecond": 7206,
                    "fireAction": 0
                },
                {
                    "daySecond": 7866,
                    "fireAction": 1
                },
                {
                    "daySecond": 9901,
                    "fireAction": 0
                },
                {
                    "daySecond": 12507,
                    "fireAction": 1
                },
                {
                    "daySecond": 15069,
                    "fireAction": 1
                },
                {
                    "daySecond": 16621,
                    "fireAction": 1
                }
            ],
            [
                {
                    "daySecond": 2290,
                    "fireAction": 1
                },
                {
                    "daySecond": 2808,
                    "fireAction": 0
                },
                {
                    "daySecond": 6269,
                    "fireAction": 1
                },
                {
                    "daySecond": 6276,
                    "fireAction": 0
                },
                {
                    "daySecond": 8730,
                    "fireAction": 0
                }
            ],
            [
                {
                    "daySecond": 457,
                    "fireAction": 1
                },
                {
                    "daySecond": 2926,
                    "fireAction": 1
                },
                {
                    "daySecond": 4630,
                    "fireAction": 1
                },
                {
                    "daySecond": 4644,
                    "fireAction": 0
                },
                {
                    "daySecond": 7650,
                    "fireAction": 1
                },
                {
                    "daySecond": 8664,
                    "fireAction": 1
                },
                {
                    "daySecond": 9288,
                    "fireAction": 1
                }
            ],
            [
                {
                    "daySecond": 1754,
                    "fireAction": 0
                },
                {
                    "daySecond": 3295,
                    "fireAction": 1
                },
                {
                    "daySecond": 5558,
                    "fireAction": 0
                },
                {
                    "daySecond": 8696,
                    "fireAction": 0
                },
                {
                    "daySecond": 12168,
                    "fireAction": 1
                },
                {
                    "daySecond": 13419,
                    "fireAction": 0
                }
            ],
            [
                {
                    "daySecond": 3153,
                    "fireAction": 1
                },
                {
                    "daySecond": 6164,
                    "fireAction": 1
                },
                {
                    "daySecond": 8733,
                    "fireAction": 0
                },
                {
                    "daySecond": 8997,
                    "fireAction": 1
                },
                {
                    "daySecond": 12424,
                    "fireAction": 0
                },
                {
                    "daySecond": 14070,
                    "fireAction": 0
                },
                {
                    "daySecond": 17346,
                    "fireAction": 1
                },
                {
                    "daySecond": 19368,
                    "fireAction": 0
                },
                {
                    "daySecond": 22606,
                    "fireAction": 0
                },
                {
                    "daySecond": 24665,
                    "fireAction": 1
                }
            ],
            [
                {
                    "daySecond": 1928,
                    "fireAction": 1
                },
                {
                    "daySecond": 5014,
                    "fireAction": 1
                },
                {
                    "daySecond": 7878,
                    "fireAction": 1
                },
                {
                    "daySecond": 8165,
                    "fireAction": 1
                },
                {
                    "daySecond": 11516,
                    "fireAction": 1
                }
            ],
            [
                {
                    "daySecond": 579,
                    "fireAction": 1
                },
                {
                    "daySecond": 3495,
                    "fireAction": 1
                },
                {
                    "daySecond": 6484,
                    "fireAction": 1
                },
                {
                    "daySecond": 6894,
                    "fireAction": 0
                },
                {
                    "daySecond": 8441,
                    "fireAction": 0
                },
                {
                    "daySecond": 9985,
                    "fireAction": 1
                },
                {
                    "daySecond": 10719,
                    "fireAction": 0
                },
                {
                    "daySecond": 12214,
                    "fireAction": 1
                }
            ],
            [
                {
                    "daySecond": 3175,
                    "fireAction": 1
                },
                {
                    "daySecond": 6221,
                    "fireAction": 1
                },
                {
                    "daySecond": 6572,
                    "fireAction": 1
                },
                {
                    "daySecond": 8045,
                    "fireAction": 1
                },
                {
                    "daySecond": 9621,
                    "fireAction": 1
                },
                {
                    "daySecond": 10416,
                    "fireAction": 1
                },
                {
                    "daySecond": 12273,
                    "fireAction": 1
                }
            ],
            [
                {
                    "daySecond": 2138,
                    "fireAction": 1
                },
                {
                    "daySecond": 4755,
                    "fireAction": 1
                },
                {
                    "daySecond": 6103,
                    "fireAction": 0
                },
                {
                    "daySecond": 6550,
                    "fireAction": 0
                },
                {
                    "daySecond": 7582,
                    "fireAction": 0
                },
                {
                    "daySecond": 9832,
                    "fireAction": 1
                },
                {
                    "daySecond": 10809,
                    "fireAction": 0
                },
                {
                    "daySecond": 14377,
                    "fireAction": 0
                },
                {
                    "daySecond": 16458,
                    "fireAction": 0
                },
                {
                    "daySecond": 19666,
                    "fireAction": 0
                }
            ],
            [
                {
                    "daySecond": 1245,
                    "fireAction": 0
                },
                {
                    "daySecond": 2114,
                    "fireAction": 1
                },
                {
                    "daySecond": 3932,
                    "fireAction": 1
                },
                {
                    "daySecond": 4197,
                    "fireAction": 1
                },
                {
                    "daySecond": 5909,
                    "fireAction": 0
                },
                {
                    "daySecond": 7543,
                    "fireAction": 1
                },
                {
                    "daySecond": 10972,
                    "fireAction": 0
                },
                {
                    "daySecond": 11504,
                    "fireAction": 0
                },
                {
                    "daySecond": 12123,
                    "fireAction": 0
                },
                {
                    "daySecond": 12224,
                    "fireAction": 1
                }
            ],
            [
                {
                    "daySecond": 226,
                    "fireAction": 0
                },
                {
                    "daySecond": 1307,
                    "fireAction": 1
                },
                {
                    "daySecond": 2406,
                    "fireAction": 1
                },
                {
                    "daySecond": 4961,
                    "fireAction": 0
                },
                {
                    "daySecond": 6780,
                    "fireAction": 1
                },
                {
                    "daySecond": 7204,
                    "fireAction": 1
                },
                {
                    "daySecond": 7739,
                    "fireAction": 0
                }
            ],
            [
                {
                    "daySecond": 384,
                    "fireAction": 0
                },
                {
                    "daySecond": 1814,
                    "fireAction": 1
                },
                {
                    "daySecond": 4813,
                    "fireAction": 0
                },
                {
                    "daySecond": 8117,
                    "fireAction": 1
                },
                {
                    "daySecond": 10485,
                    "fireAction": 0
                },
                {
                    "daySecond": 11303,
                    "fireAction": 0
                },
                {
                    "daySecond": 13644,
                    "fireAction": 0
                },
                {
                    "daySecond": 16711,
                    "fireAction": 1
                }
            ],
            [
                {
                    "daySecond": 1041,
                    "fireAction": 1
                },
                {
                    "daySecond": 1517,
                    "fireAction": 1
                },
                {
                    "daySecond": 3295,
                    "fireAction": 0
                },
                {
                    "daySecond": 5750,
                    "fireAction": 0
                },
                {
                    "daySecond": 7546,
                    "fireAction": 1
                },
                {
                    "daySecond": 11036,
                    "fireAction": 1
                },
                {
                    "daySecond": 12406,
                    "fireAction": 0
                },
                {
                    "daySecond": 15571,
                    "fireAction": 1
                }
            ],
            [
                {
                    "daySecond": 2558,
                    "fireAction": 1
                },
                {
                    "daySecond": 5481,
                    "fireAction": 1
                },
                {
                    "daySecond": 9064,
                    "fireAction": 0
                },
                {
                    "daySecond": 11978,
                    "fireAction": 0
                },
                {
                    "daySecond": 15010,
                    "fireAction": 1
                },
                {
                    "daySecond": 15925,
                    "fireAction": 0
                },
                {
                    "daySecond": 19022,
                    "fireAction": 1
                },
                {
                    "daySecond": 19826,
                    "fireAction": 1
                }
            ],
            [
                {
                    "daySecond": 394,
                    "fireAction": 1
                },
                {
                    "daySecond": 2484,
                    "fireAction": 0
                },
                {
                    "daySecond": 4037,
                    "fireAction": 1
                },
                {
                    "daySecond": 5078,
                    "fireAction": 0
                },
                {
                    "daySecond": 5796,
                    "fireAction": 1
                },
                {
                    "daySecond": 7120,
                    "fireAction": 0
                },
                {
                    "daySecond": 9385,
                    "fireAction": 1
                }
            ],
            [
                {
                    "daySecond": 3517,
                    "fireAction": 0
                },
                {
                    "daySecond": 4858,
                    "fireAction": 0
                },
                {
                    "daySecond": 7909,
                    "fireAction": 0
                },
                {
                    "daySecond": 8328,
                    "fireAction": 1
                },
                {
                    "daySecond": 10397,
                    "fireAction": 1
                },
                {
                    "daySecond": 11853,
                    "fireAction": 0
                },
                {
                    "daySecond": 14587,
                    "fireAction": 0
                },
                {
                    "daySecond": 14896,
                    "fireAction": 0
                },
                {
                    "daySecond": 15619,
                    "fireAction": 0
                },
                {
                    "daySecond": 15827,
                    "fireAction": 1
                }
            ],
            [
                {
                    "daySecond": 1244,
                    "fireAction": 1
                },
                {
                    "daySecond": 2136,
                    "fireAction": 1
                },
                {
                    "daySecond": 3859,
                    "fireAction": 1
                },
                {
                    "daySecond": 5635,
                    "fireAction": 0
                },
                {
                    "daySecond": 5762,
                    "fireAction": 0
                }
            ],
            [
                {
                    "daySecond": 2665,
                    "fireAction": 1
                },
                {
                    "daySecond": 5923,
                    "fireAction": 0
                },
                {
                    "daySecond": 6988,
                    "fireAction": 0
                },
                {
                    "daySecond": 7197,
                    "fireAction": 1
                },
                {
                    "daySecond": 7645,
                    "fireAction": 1
                },
                {
                    "daySecond": 10727,
                    "fireAction": 1
                },
                {
                    "daySecond": 11852,
                    "fireAction": 0
                },
                {
                    "daySecond": 14003,
                    "fireAction": 1
                }
            ],
            [
                {
                    "daySecond": 1966,
                    "fireAction": 0
                },
                {
                    "daySecond": 2301,
                    "fireAction": 0
                },
                {
                    "daySecond": 2997,
                    "fireAction": 1
                },
                {
                    "daySecond": 3637,
                    "fireAction": 1
                },
                {
                    "daySecond": 5031,
                    "fireAction": 0
                }
            ],
            [
                {
                    "daySecond": 2850,
                    "fireAction": 0
                },
                {
                    "daySecond": 3941,
                    "fireAction": 0
                },
                {
                    "daySecond": 4281,
                    "fireAction": 0
                },
                {
                    "daySecond": 7065,
                    "fireAction": 1
                },
                {
                    "daySecond": 8588,
                    "fireAction": 0
                }
            ],
            [
                {
                    "daySecond": 2191,
                    "fireAction": 0
                },
                {
                    "daySecond": 4190,
                    "fireAction": 0
                },
                {
                    "daySecond": 6896,
                    "fireAction": 0
                },
                {
                    "daySecond": 10376,
                    "fireAction": 0
                },
                {
                    "daySecond": 12294,
                    "fireAction": 1
                },
                {
                    "daySecond": 13098,
                    "fireAction": 0
                },
                {
                    "daySecond": 16105,
                    "fireAction": 0
                },
                {
                    "daySecond": 18978,
                    "fireAction": 1
                },
                {
                    "daySecond": 21961,
                    "fireAction": 0
                }
            ],
            [
                {
                    "daySecond": 2212,
                    "fireAction": 1
                },
                {
                    "daySecond": 4439,
                    "fireAction": 1
                },
                {
                    "daySecond": 6718,
                    "fireAction": 1
                },
                {
                    "daySecond": 8317,
                    "fireAction": 0
                },
                {
                    "daySecond": 10970,
                    "fireAction": 1
                },
                {
                    "daySecond": 14333,
                    "fireAction": 0
                },
                {
                    "daySecond": 17362,
                    "fireAction": 1
                },
                {
                    "daySecond": 19572,
                    "fireAction": 1
                },
                {
                    "daySecond": 20915,
                    "fireAction": 0
                },
                {
                    "daySecond": 21015,
                    "fireAction": 0
                }
            ],
            [
                {
                    "daySecond": 1001,
                    "fireAction": 1
                },
                {
                    "daySecond": 1763,
                    "fireAction": 0
                },
                {
                    "daySecond": 4162,
                    "fireAction": 0
                },
                {
                    "daySecond": 6015,
                    "fireAction": 0
                },
                {
                    "daySecond": 6196,
                    "fireAction": 1
                },
                {
                    "daySecond": 7417,
                    "fireAction": 1
                }
            ],
            [
                {
                    "daySecond": 3160,
                    "fireAction": 0
                },
                {
                    "daySecond": 4956,
                    "fireAction": 1
                },
                {
                    "daySecond": 6874,
                    "fireAction": 1
                },
                {
                    "daySecond": 8315,
                    "fireAction": 0
                },
                {
                    "daySecond": 9724,
                    "fireAction": 1
                }
            ],
            [
                {
                    "daySecond": 3380,
                    "fireAction": 1
                },
                {
                    "daySecond": 4975,
                    "fireAction": 0
                },
                {
                    "daySecond": 6309,
                    "fireAction": 1
                },
                {
                    "daySecond": 8021,
                    "fireAction": 0
                },
                {
                    "daySecond": 9734,
                    "fireAction": 1
                },
                {
                    "daySecond": 13223,
                    "fireAction": 1
                },
                {
                    "daySecond": 13371,
                    "fireAction": 1
                },
                {
                    "daySecond": 13476,
                    "fireAction": 0
                },
                {
                    "daySecond": 17017,
                    "fireAction": 1
                }
            ],
            [
                {
                    "daySecond": 1567,
                    "fireAction": 1
                },
                {
                    "daySecond": 3240,
                    "fireAction": 1
                },
                {
                    "daySecond": 4273,
                    "fireAction": 1
                },
                {
                    "daySecond": 6604,
                    "fireAction": 1
                },
                {
                    "daySecond": 8785,
                    "fireAction": 1
                },
                {
                    "daySecond": 10111,
                    "fireAction": 1
                }
            ],
            [
                {
                    "daySecond": 3487,
                    "fireAction": 1
                },
                {
                    "daySecond": 6777,
                    "fireAction": 1
                },
                {
                    "daySecond": 6868,
                    "fireAction": 0
                },
                {
                    "daySecond": 9240,
                    "fireAction": 1
                },
                {
                    "daySecond": 12402,
                    "fireAction": 0
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
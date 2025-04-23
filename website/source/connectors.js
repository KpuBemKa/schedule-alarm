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
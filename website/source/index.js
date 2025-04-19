function addScheduleRow(time = "", type = "single") {
    const table = document.getElementById("scheduleTable").getElementsByTagName("tbody")[0];
    const newRow = table.insertRow();
    newRow.innerHTML = `
        <td><input type="time" name="time[]" value="${time}" required class="form-time"></td>
        <td>
            <select name="type[]" required class="form-select">
                <option value="single" ${type == 1 ? "selected" : ""}>Single</option>
                <option value="double" ${type == 2 ? "selected" : ""}>Double</option>
                <option value="triple" ${type == 3 ? "selected" : ""}>Triple</option>
            </select>
        </td>
        <td><button type="button" onclick="deleteScheduleRow(this)" class="delete-button">x</button></td>
    `;
}

function deleteScheduleRow(button) {
    const row = button.parentNode.parentNode;
    row.parentNode.removeChild(row);

}

function submitSchedule() {
    const form = document.getElementById("alarmForm");
    const times = Array.from(form.querySelectorAll("input[name='time[]']")).map(input => input.value);
    const types = Array.from(form.querySelectorAll("select[name='type[]']")).map(select => select.value);

    const schedule_items = times.map((time, index) => {
        const [hour, minute] = time.split(":").map(Number);
        return {
            time: hour * 60 + minute,
            type: types[index] === "single" ? 1 : types[index] === "double" ? 2 : 3
        };
    });

    const jsonData = {
        schedule_items: schedule_items
    };

    console.log(JSON.stringify(jsonData, null, 2));

    fetch('/update_schedule', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify(jsonData)
    })
        .then(response => response.text())
        .then(data => console.log('Server Response:', data))
        .catch(error => console.error('Error:', error));
}

function fetchSchedule() {
    fetch('/get_schedule')
        .then(response => response.json())
        .then(data => {
            const tableBody = document.getElementById("scheduleTable").getElementsByTagName("tbody")[0];
            tableBody.innerHTML = ""; // Clear existing rows
            data.schedule_items.forEach(item => {
                const hour = Math.floor(item.time / 60).toString().padStart(2, '0');
                const minute = (item.time % 60).toString().padStart(2, '0');
                addScheduleRow(`${hour}:${minute}`, item.type);
            });
        })
        .catch(error => console.error('Error fetching schedule:', error));
}

function submitSettings() {
    if (!confirm("Wi-Fi will restart after settings are applied. Do you want to proceed?")) {
        return;
    }

    const form = document.getElementById("settingsForm");
    const jsonData = {
        wifi_ssid: form.wifi_ssid.value,
        wifi_password: form.wifi_password.value,
        remote_wifi_ssid: form.remote_wifi_ssid.value,
        remote_wifi_password: form.remote_wifi_password.value,
        timezone: form.timezone.value
    };

    console.log(JSON.stringify(jsonData, null, 2));

    fetch('/update_settings', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify(jsonData)
    })
        .then(response => {
            if (!response.ok) {
                throw new Error('Failed to update settings');
            }
            return response.text();
        })
        .then(data => console.log('Server Response:', data))
        .catch(error => {
            console.error('Error:', error);
            alert('Error updating settings: ' + error.message);
        });
}

function fetchSettings() {
    fetch('/get_settings')
        .then(response => response.json())
        .then(data => {
            document.getElementById("wifi_ssid").value = data.wifi_ssid || "";
            document.getElementById("wifi_password").value = data.wifi_password || "";
            document.getElementById("remote_wifi_ssid").value = data.remote_wifi_ssid || "";
            document.getElementById("remote_wifi_password").value = data.remote_wifi_password || "";
            document.getElementById("timezone").value = data.timezone || "";
        })
        .catch(error => {
            console.error('Error fetching settings:', error);
            // alert('Error fetching settings: ' + error.message);
        });
}

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

document.addEventListener("DOMContentLoaded", fetchSettings);
document.addEventListener("DOMContentLoaded", updateTime);
document.addEventListener("DOMContentLoaded", fetchSchedule);
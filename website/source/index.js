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
document.addEventListener("DOMContentLoaded", fetchSchedule);
document.addEventListener("DOMContentLoaded", updateTime);
const calendarEl = document.getElementById('calendar');
const popupEl = document.getElementById('popup');
const popupContentEl = document.getElementById('popup-content');

const year = new Date().getFullYear();
const monthNames = ["January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"];
const weekdays = ["Mo", "Tu", "We", "Th", "Fr", "Sa", "Su"];

function createCalendar(targetClass, year) {
    const calendarContainers = document.querySelectorAll(`.${targetClass}`);

    calendarContainers.forEach(calendarEl => {
        calendarEl.innerHTML = ''; // Clear previous content if any

        for (let month = 0; month < 12; month++) {
            const monthDiv = document.createElement('div');
            monthDiv.className = 'c-month';
            monthDiv.innerHTML = `<h3>${monthNames[month]}</h3>`;

            const daysGrid = document.createElement('div');
            daysGrid.className = 'c-days';

            weekdays.forEach(d => {
                const wd = document.createElement('div');
                wd.className = 'c-weekday';
                wd.textContent = d;
                daysGrid.appendChild(wd);
            });

            const firstDay = new Date(year, month, 0).getDay();
            const numDays = new Date(year, month + 1, 0).getDate();

            // empty slots before first day
            for (let i = 0; i < firstDay; i++) {
                const empty = document.createElement('div');
                daysGrid.appendChild(empty);
            }

            for (let day = 1; day <= numDays; day++) {
                const dayEl = document.createElement('div');
                dayEl.className = 'c-day';
                dayEl.textContent = day;
                dayEl.addEventListener('click', () => showPopup(`${year}-${month + 1}-${day}`));
                daysGrid.appendChild(dayEl);
            }

            monthDiv.appendChild(daysGrid);
            calendarEl.appendChild(monthDiv);
        }
    });
}

function showPopup(dateString) {
    popupContentEl.innerHTML = `<h2>Selected Date: ${dateString}</h2>
        <p>You can add any custom UI or actions here, like forms, buttons, etc.</p>`;
    popupEl.style.display = 'block';
}

function closePopup() {
    popupEl.style.display = 'none';
}
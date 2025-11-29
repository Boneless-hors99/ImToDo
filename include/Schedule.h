#ifndef SCHEDULE
#define SCHEDULE

#include <chrono>
#include <date/date.h>

using Date = date::year_month_day;

// Generic Task class, date would be due date for task, meeting, presentation, exam, etc...
class Task{
    private:
    Date m_date;
    public:
    Task();
};

class Schedule{
    private:

    public:
    Schedule();
    bool RenderCalendar(const char* id,
                    date::year_month& currentMonth,
                    Date& selectedDate);
};

#endif // !SCHEDULE
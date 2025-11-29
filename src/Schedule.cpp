#include "Schedule.h"

#include "imgui.h"

using namespace date;

Task::Task()
{
}

Schedule::Schedule()
{
}

bool Schedule::RenderCalendar(const char* id,
                    date::year_month& currentMonth,
                    Date& selectedDate)
{
    ImGui::Begin(id);

    // ----- Header: month / year + arrows -----
    year y = currentMonth.year();
    month m = currentMonth.month();

    static const char* monthNames[] = {
        "January","February","March","April","May","June",
        "July","August","September","October","November","December"
    };

    int monthIndex = unsigned(m) - 1;

    if (ImGui::ArrowButton("##prev_month", ImGuiDir_Left)) {
        if (m == January) {
            m = December;
            y = year(int(y) - 1);
        } else {
            m = month(unsigned(m) - 1);
        }
        currentMonth = y/m;
    }
    ImGui::SameLine();

    ImGui::Text("%s %d", monthNames[monthIndex], int(y));
    ImGui::SameLine();

    if (ImGui::ArrowButton("##next_month", ImGuiDir_Right)) {
        if (m == December) {
            m = January;
            y = year(int(y) + 1);
        } else {
            m = month(unsigned(m) + 1);
        }
        currentMonth = y/m;
    }

    ImGui::Separator();

    bool dateChanged = false;

    // ---------- table should fill remaining space ----------
    // Get how much vertical space is left in this window *after* header + separator
    float availHeight = ImGui::GetContentRegionAvail().y - 20; //Extra 20 just in case
    if (availHeight < 0.0f) availHeight = 0.0f;

    // Assume up to 6 weeks of days → 6 rows
    // (the header row with Mon–Sun is separate)
    const int numCalendarRows = 6;
    float rowHeight = (numCalendarRows > 0) ? (availHeight / numCalendarRows) : 0.0f;

    ImGuiTableFlags flags =
        ImGuiTableFlags_Borders |
        ImGuiTableFlags_SizingStretchSame |
        ImGuiTableFlags_RowBg;

    // Give the table a height so it stretches vertically
    if (ImGui::BeginTable("CalendarTable", 7, flags, ImVec2(-FLT_MIN, availHeight)))
    {
        // Column headers (Mon–Sun)
        const char* weekdayLabels[] = { "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun" };
        for (int i = 0; i < 7; ++i) {
            ImGui::TableSetupColumn(weekdayLabels[i]);
        }
        ImGui::TableHeadersRow();

        // ----- Date math -----
        year_month_day firstYmd = y/m/1;
        sys_days firstDaySys = sys_days(firstYmd);
        weekday firstWday = weekday(firstDaySys);

        // Monday-first column index (0..6)
        int firstColumn = (firstWday.c_encoding() + 6) % 7;

        year_month_day lastYmd = y/m/last;
        unsigned daysInMonth = unsigned(unsigned(lastYmd.day()));

        auto todaySys = floor<days>(std::chrono::system_clock::now());
        year_month_day todayYmd(todaySys);

        int day = 1;
        int column = 0;
        int rowsUsed = 0;

        // First row (week 1)
        ImGui::TableNextRow(ImGuiTableRowFlags_None, rowHeight);
        rowsUsed++;

        // Empty leading cells
        for (int i = 0; i < firstColumn; ++i) {
            ImGui::TableSetColumnIndex(column++);
            ImGui::TextUnformatted("");
        }

        // Fill days
        while (day <= int(daysInMonth))
        {
            if (column == 7) {
                column = 0;
                ImGui::TableNextRow(ImGuiTableRowFlags_None, rowHeight);
                rowsUsed++;
            }

            ImGui::TableSetColumnIndex(column++);

            year_month_day thisYmd = y/m/day;
            bool isToday    = (thisYmd == todayYmd);
            bool isSelected = (thisYmd == selectedDate);

            ImGui::PushID(day);

            // Color override
            if (isSelected) {
                ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.2f, 0.5f, 1.0f, 0.8f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.6f, 1.0f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.1f, 0.4f, 0.9f, 1.0f));
            } else if (isToday) {
                ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.2f, 0.7f, 0.2f, 0.7f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.8f, 0.2f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.1f, 0.6f, 0.1f, 1.0f));
            } else {
                ImGui::PushStyleColor(ImGuiCol_Button,        ImGui::GetStyle().Colors[ImGuiCol_Button]);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered]);
                ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
            }

            // Full-cell button: use rowHeight as height, column width as width
            float cellWidth  = ImGui::GetColumnWidth();
            float cellHeight = rowHeight;
            char label[4];
            snprintf(label, sizeof(label), "%d", day);

            if (ImGui::Button(label, ImVec2(cellWidth - 4.0f, cellHeight - 4.0f))) {
                selectedDate = thisYmd;
                dateChanged = true;
            }

            ImGui::PopStyleColor(3);
            ImGui::PopID();

            ++day;
        }

        // If I didn’t use all 6 rows (short month), add empty rows to fill height
        while (rowsUsed < numCalendarRows) {
            ImGui::TableNextRow(ImGuiTableRowFlags_None, rowHeight);
            rowsUsed++;
            for (int i = 0; i < 7; ++i) {
                ImGui::TableSetColumnIndex(i);
                ImGui::TextUnformatted("");
            }
        }

        ImGui::EndTable();
    }

    ImGui::End();
    return dateChanged;
}
